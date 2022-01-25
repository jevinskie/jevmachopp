#include "jevmachopp/UBootAPFS.h"

#include <cassert>
#include <cerrno>
#include <cfloat>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>

#include <ApfsLib/ApfsContainer.h>
#include <ApfsLib/ApfsDir.h>
#include <ApfsLib/ApfsVolume.h>
#include <ApfsLib/BlockDumper.h>
#include <ApfsLib/Device.h>
#include <ApfsLib/DeviceUBoot.h>
#include <ApfsLib/GptPartitionMap.h>

#include <jevmachopp/Common.h>

#include <nanorange/views/drop.hpp>
#include <nanorange/views/join.hpp>
#include <visit.hpp>

typedef long long int loff_t;
typedef unsigned long int ulong;
typedef unsigned char uchar;

/*
 * Directory entry types, matches the subset of DT_x in posix readdir()
 * which apply to u-boot.
 */
#define FS_DT_DIR 4  /* directory */
#define FS_DT_REG 8  /* regular file */
#define FS_DT_LNK 10 /* symbolic link */

/*
 * A directory entry, returned by fs_readdir().  Returns information
 * about the file/directory at the current directory entry position.
 */
struct fs_dirent {
    unsigned int type; /* one of FS_DT_x (not a mask) */
    loff_t size;       /* size in bytes */
    char name[256];
};

struct fs_dir_stream {
    struct blk_desc *desc;
    int part;
};

#define UUID_STR_LEN 36
#define PART_NAME_LEN 32
#define PART_TYPE_LEN 32

#define CONFIG_PARTITION_UUIDS
#define CONFIG_PARTITION_TYPE_GUID

struct disk_partition {
    lbaint_t start;            /* # of first block in partition    */
    lbaint_t size;             /* number of blocks in partition    */
    ulong blksz;               /* block size in bytes          */
    uchar name[PART_NAME_LEN]; /* partition name           */
    uchar type[PART_TYPE_LEN]; /* string type description      */
    /*
     * The bootable is a bitmask with the following fields:
     *
     * PART_BOOTABLE        the MBR bootable flag is set
     * PART_EFI_SYSTEM_PARTITION    the partition is an EFI system partition
     */
    int bootable;
#ifdef CONFIG_PARTITION_UUIDS
    char uuid[UUID_STR_LEN + 1]; /* filesystem UUID as string, if exists */
#endif
#ifdef CONFIG_PARTITION_TYPE_GUID
    char type_guid[UUID_STR_LEN + 1]; /* type GUID as string, if exists   */
#endif
};

namespace UBootAPFS {

class DirRec : public ApfsDir::DirRec {
public:
    uint8_t drec_type() const {
        return flags & dir_rec_flags::DREC_TYPE_MASK;
    }
    bool is_dir() const {
        return drec_type() == DT_DIR;
    }
    bool is_file() const {
        return drec_type() == DT_REG;
    }
    bool is_link() const {
        return drec_type() == DT_LNK;
    }
};

struct APFSPath {
    APFSPath(const std::string &full_path) {
        // cases:
        // (empty) -> /
        // /
        // foo -> /foo
        // /foo
        // /foo/bar
        // /foo/bar/
        std::string fp = full_path;
        if (full_path.empty() || fp[0] != '/')
            fp = '/' + fp;
        auto sv = stringSplitViewDelimitedBy(fp, '/') | views::drop(1);
        if (const auto vol = ranges::get(sv, 0))
            volume = *vol;
        path = ranges::join(sv | views::drop(1), std::string{"/"});
        std::cout << "APFSPath(\"" << fp << "\") vol: " << volume << " path: " << path << "\n";
    }
    std::string volume;
    std::string path;
};

class APFSNode {
public:
    APFSNode(ApfsContainer *container) : m_container(container) {
        assert(m_container);
    }
    APFSNode(ApfsContainer *container, std::shared_ptr<ApfsVolume> volume)
        : m_container(container), m_volume(volume) {
        assert(m_container);
        assert(m_volume);
    }
    APFSNode(ApfsContainer *container, std::shared_ptr<ApfsVolume> volume,
             std::shared_ptr<DirRec> dirrec)
        : m_container(container), m_volume(volume), m_dirrec(dirrec) {
        assert(m_container);
        assert(m_volume);
        assert(m_dirrec);
    }

    std::string name() const {
        if (m_dirrec)
            return m_dirrec->name;
        else if (m_volume)
            return m_volume->name();
        else
            return "/";
    }

    bool isContainer() const {
        return !m_volume && !m_dirrec;
    }

    bool isVolume() const {
        return m_volume && !m_dirrec;
    }

    bool isPath() const {
        return !!m_dirrec;
    }

    bool isDir() const {
        return isPath() && m_dirrec->is_dir();
    }

    bool isDirUboot() const {
        return isContainer() || isVolume() || isDir();
    }

    bool isFile() const {
        return isPath() && m_dirrec->is_file();
    }

    bool isFileUBoot() const {
        return isPath() && !(isDir() || isLink());
    }

    bool isLink() const {
        return isPath() && m_dirrec->is_link();
    }

    unsigned int uboot_type() const {
        if (isDirUboot())
            return FS_DT_DIR;
        if (isLink())
            return FS_DT_LNK;
        assert(isFileUBoot());
        return FS_DT_REG;
    }

private:
    ApfsContainer *m_container;
    std::shared_ptr<ApfsVolume> m_volume;
    std::shared_ptr<DirRec> m_dirrec;
};

struct APFSDirStream {
    struct fs_dir_stream dir_str;
    std::vector<std::shared_ptr<APFSNode>> *nodes;
    size_t idx;
};

static_assert_cond(std::is_trivial_v<APFSDirStream> &&std::is_standard_layout_v<APFSDirStream>);

std::shared_ptr<ApfsVolume> lookupVolume(const std::string &volName, ApfsContainer *container) {
    const auto nvol = container->GetVolumeCnt();
    std::shared_ptr<ApfsVolume> vol;
    for (unsigned int volidx = 0; volidx < nvol; ++volidx) {
        apfs_superblock_t sb;
        assert(container->GetVolumeInfo(volidx, sb));
        if (volName == (const char *)sb.apfs_volname) {
            vol.reset(container->GetVolume(volidx));
            assert(vol);
            break;
        }
    }
    return vol;
}

std::shared_ptr<DirRec> lookupDirRec(const std::string &full_path,
                                     std::shared_ptr<ApfsVolume> volume) {
    return {};
}

std::shared_ptr<APFSNode> lookup(const APFSPath &path, ApfsContainer *container) {
    const bool vempty = path.volume.empty();
    const bool pempty = path.path.empty();
    if (vempty && pempty)
        return std::make_shared<APFSNode>(container);
    auto vol = lookupVolume(path.volume, container);
    if (!vempty && pempty)
        return std::make_shared<APFSNode>(container, vol);
    if (!vempty && !pempty)
        return std::make_shared<APFSNode>(container, vol, lookupDirRec(path.path, vol));
    assert(!"lookup() unhandled case");
}

std::vector<std::shared_ptr<ApfsVolume>> listContainer(ApfsContainer *container) {
    const auto nvol = container->GetVolumeCnt();
    std::vector<std::shared_ptr<ApfsVolume>> vols;
    for (unsigned int volidx = 0; volidx < nvol; ++volidx) {
        auto vol = container->GetVolume(volidx);
        assert(vol);
        vols.emplace_back(vol);
    }
    return vols;
}

std::vector<std::shared_ptr<APFSNode>> listContainerAsNodes(ApfsContainer *container) {
    std::vector<std::shared_ptr<APFSNode>> vols;
    for (auto vol : listContainer(container)) {
        vols.emplace_back(std::make_shared<APFSNode>(container, vol));
    }
    return vols;
}

std::vector<std::shared_ptr<DirRec>>
listPath(ApfsContainer *container, std::shared_ptr<ApfsVolume> volume, const std::string &path) {
    std::vector<std::shared_ptr<DirRec>> subpaths;
    return subpaths;
}

std::vector<std::shared_ptr<APFSNode>> listPathAsNodes(ApfsContainer *container,
                                                       std::shared_ptr<ApfsVolume> volume,
                                                       const std::string &path) {
    std::vector<std::shared_ptr<APFSNode>> paths;
    for (auto dirrec : listPath(container, volume, path)) {
        paths.emplace_back(std::make_shared<APFSNode>(container, volume, dirrec));
    }
    return paths;
}

std::vector<std::shared_ptr<APFSNode>> list(const APFSPath &path, ApfsContainer *container) {
    const bool vempty = path.volume.empty();
    const bool pempty = path.path.empty();
    if (vempty && pempty)
        return listContainerAsNodes(container);
    auto vol = lookupVolume(path.volume, container);
    assert(vol);
    if (!vempty && pempty)
        return listPathAsNodes(container, vol, "/");
    if (!vempty && !pempty)
        return listPathAsNodes(container, vol, path.path);
    assert(!"list() unhandled case");
}

std::unique_ptr<DirRec> childDirNamed(ApfsDir *apfsDir, DirRec *parentDir,
                                      std::string_view childDirName) {
    assert(parentDir);
    auto res = std::make_unique<DirRec>();
    if (!apfsDir->LookupName(*res, parentDir->file_id, std::string{childDirName}.c_str())) {
        return nullptr;
    }
    return res;
}

std::unique_ptr<DirRec> lookupDir(ApfsDir *apfsDir, std::string_view dirPath) {
    if (!dirPath.size() || dirPath[0] != '/') {
        return nullptr;
    }
    std::unique_ptr<DirRec> res = std::make_unique<DirRec>();
    assert(apfsDir->LookupName(*res, ROOT_DIR_PARENT, "root"));
    for (const auto childName : stringSplitViewDelimitedBy(dirPath, '/') | views::drop(1)) {
        printf("looking up childName: \"%*s\"\n", SV2PF(childName));
        res = std::unique_ptr<DirRec>{childDirNamed(apfsDir, res.get(), childName)};
        if (!res) {
            return nullptr;
        }
    }
    return res;
}

class APFSCtx {
public:
    APFSCtx() = default;
    ~APFSCtx() {
        if (m_opened)
            close();
    }
    bool open(const char *name) {
        auto sv         = stringSplitViewDelimitedBy(name, ':');
        const auto type = ranges::get(sv, 0);
        assert(type);
        const auto dev_num_str = ranges::get(sv, 1);
        assert(dev_num_str);
        const auto part_num_str = ranges::get(sv, 2);
        auto part_num           = 0;
        if (part_num_str) {
            fmt::print("setting part num to {:s} - 1\n", *part_num_str);
            const auto part_num = std::stoi(std::string{*part_num_str}, nullptr, 10) - 1;
        }
        assert(part_num >= 0);

        std::string dev_open_name = std::string{*type} + ":" + std::string{*dev_num_str};
        if (!m_dev.Open(dev_open_name.c_str()))
            return false;

        GptPartitionMap gpt;
        assert(gpt.LoadAndVerify(m_dev));
        fmt::print("gpt.size: {:d}\n", gpt.size());
        assert(gpt.GetPartitionOffsetAndSize((uint32_t)part_num, m_blk_off_blks, m_blk_sz_bytes));
        fmt::print("blk_off_blks: 0x{:x} blk_sz_bytes: 0x{:x}\n", m_blk_off_blks, m_blk_sz_bytes);
        return open_common();
    }
    bool open(struct blk_desc *fs_dev_desc, struct disk_partition *fs_partition) {
        if (!m_dev.Open(fs_dev_desc))
            return false;
        m_blk_off_blks = fs_partition->start * fs_partition->blksz;
        m_blk_sz_bytes = fs_partition->size * fs_partition->blksz;
        return open_common();
    }
    void close() {
        m_opened = false;
        std::destroy_at(std::launder(m_container));
        m_container = nullptr;
        m_dev.Close();
    }
    const ApfsContainer *container() const {
        assert(m_opened && m_container);
        return const_cast<const ApfsContainer *>(m_container);
    }
    ApfsContainer *container() {
        return const_cast<ApfsContainer *>(static_cast<const APFSCtx &>(*this).container());
    }
    bool opened() const {
        return m_opened;
    }

private:
    bool open_common() {
        m_container = new (reinterpret_cast<ApfsContainer *>(&m_container_storage))
            ApfsContainer(&m_dev, m_blk_off_blks, m_blk_sz_bytes);
        if (!m_container->Init(0, false)) {
            printf("shit\n");
            return false;
        }
        m_opened = true;
        return true;
    }

    DeviceUBoot m_dev;
    uint64_t m_blk_off_blks;
    uint64_t m_blk_sz_bytes;
    std::aligned_storage_t<sizeof(ApfsContainer), alignof(ApfsContainer)> m_container_storage;
    ApfsContainer *m_container;
    bool m_opened;
};

APFSCtx apfs_ctx;

} // namespace UBootAPFS

using namespace UBootAPFS;

extern "C" {

// do_ls()
//     fs_set_blk_dev()
//         probe()
//     fs_ls()
//         ls()
//         fs_close()

int apfs_probe(struct blk_desc *fs_dev_desc, struct disk_partition *fs_partition) {
    struct disk_partition *p = fs_partition;
    bool good                = apfs_ctx.open(fs_dev_desc, fs_partition);
    return good ? 0 : -1;
}

int apfs_exists(const char *filename) {
    printf("apfs_exists(\"%s\")\n", filename);
    APFSPath p{filename};
    return 0;
}

int apfs_size(const char *filename, loff_t *size) {
    assert(!"apfs_size");
    return -1;
}

int apfs_read(const char *filename, void *buf, loff_t offset, loff_t len, loff_t *actread) {
    assert(!"apfs_read");
    return -1;
}

void apfs_close(void) {
    assert(apfs_ctx.opened());
    apfs_ctx.close();
}

int apfs_uuid(char *uuid_str) {
    assert(!"apfs_uuid");
    return -1;
}

int apfs_opendir(const char *filename, struct fs_dir_stream **dirsp) {
    printf("apfs_opendir(\"%s\", %p)\n", filename, dirsp);
    APFSPath path{filename};
    auto dir = new APFSDirStream{};
    assert(dir);
    dir->idx   = 0;
    dir->nodes = new std::vector<std::shared_ptr<APFSNode>>(list(path, apfs_ctx.container()));
    assert(dir->nodes);
    assert(dirsp);
    *dirsp = (struct fs_dir_stream *)dir;
    return 0;
}

int apfs_readdir(struct fs_dir_stream *dirs, struct fs_dirent **dentp) {
    assert(dirs);
    auto dir = (APFSDirStream *)dirs;
    if (dir->idx >= dir->nodes->size())
        return -ENOENT;
    auto node  = (*dir->nodes)[dir->idx];
    auto dent  = new fs_dirent{};
    dent->type = node->uboot_type();
    strncpy(dent->name, node->name().c_str(), sizeof(dent->name));
    assert(dentp);
    *dentp = dent;
    ++dir->idx;
    return 0;
}

void apfs_closedir(struct fs_dir_stream *dirs) {
    assert(dirs);
    auto dir = (APFSDirStream *)dirs;
    delete dir->nodes;
    delete dir;
}

void uboot_apfs_doit(void) {
    printf("entering uboot_apfs_doit()\n");

    // g_debug = 0xff;

#ifdef JEV_BAREMETAL
    assert(apfs_ctx.open("virtio:0:3"));
#else
    assert(apfs_ctx.open("host:0"));
#endif

    const auto ls_root_res = list(APFSPath{"/"}, apfs_ctx.container());
    for (const auto &n : ls_root_res) {
        fmt::print("node: {:s}\n", n->name());
    }

#if 0
#ifdef JEV_BAREMETAL
    auto dev = Device::OpenDevice("virtio:0");
#else
    auto dev = Device::OpenDevice("host:0");
#endif
    assert(dev);

    GptPartitionMap gpt;
    assert(gpt.LoadAndVerify(*dev));
    gpt.ListEntries();

    const auto partition_id = gpt.FindFirstAPFSPartition();
    uint64_t main_offset    = 0;
    uint64_t main_size      = 0;
    gpt.GetPartitionOffsetAndSize(partition_id, main_offset, main_size);
    auto container = new ApfsContainer(dev, main_offset, main_size, nullptr, 0, 0);
    assert(container->Init(0, false));

    const auto nvol = container->GetVolumeCnt();
    printf("# volumes in container: %u\n", nvol);

    ApfsVolume *preboot_vol = nullptr;
    for (unsigned int volidx = 0; volidx < nvol; ++volidx) {
        apfs_superblock_t sb;
        assert(container->GetVolumeInfo(volidx, sb));
        printf("role: 0x%04hx\n", sb.apfs_role);
        if (sb.apfs_role == APFS_VOL_ROLE_PREBOOT) {
            preboot_vol = container->GetVolume(volidx);
            break;
        }
    }
    assert(preboot_vol);
    printf("preboot name: %s\n", preboot_vol->name());

    std::string_view kc_dir{
        "/D8961206-5EAC-4D35-94A3-5412F17E6B3B/boot/"
        "CBE5168B59E7B0104701733E3A617B82BC6F895B88CACFCE327725CB5532929C19244CFCEF709E3D0F8337A486"
        "6F608C/System/Library/Caches/com.apple.kernelcaches"};
    std::string_view kc_path{
        "/D8961206-5EAC-4D35-94A3-5412F17E6B3B/boot/"
        "CBE5168B59E7B0104701733E3A617B82BC6F895B88CACFCE327725CB5532929C19244CFCEF709E3D0F8337A486"
        "6F608C/System/Library/Caches/com.apple.kernelcaches/kernelcache"};

    // for (const auto sv : stringSplitViewDelimitedBy(kc_path, '/')) {
    // printf("path part for show: %.*s\n", SV2PF(sv));
    // }

    ApfsDir apfsDir(*preboot_vol);
    // printf("apfsDir: %p\n", (void*)&apfsDir);
    DirRec res;
    printf("res: %p\n", (void *)&res);
    // assert(apfsDir.LookupName(res, ROOT_DIR_INO_NUM, "D8961206-5EAC-4D35-94A3-5412F17E6B3B"));
    // printf("lookup of root dir D8961206-5EAC-4D35-94A3-5412F17E6B3B worked\n");

    std::vector<DirRec> list_res;
    const bool list_root_res = apfsDir.ListDirectory(list_res, ROOT_DIR_PARENT);
    printf("list_root_res: %d len: %d\n", list_root_res, (int)list_res.size());
    for (const auto &dir : list_res) {
        printf("dir: %s parent_id: 0x%lx file_id: 0x%lx\n", dir.name.c_str(), dir.parent_id,
               dir.file_id);
    }

    const bool lookup_root_res = apfsDir.LookupName(res, ROOT_DIR_PARENT, "root");
    assert(lookup_root_res);
    printf("lookup of superroot dir root worked\n");

    const auto kc_dir_res = lookupDir(&apfsDir, kc_dir);
    if (kc_dir_res) {
        printf("found dir named: %s\n", kc_dir_res->name.c_str());
    } else {
        printf("failed to find dir\n");
    }
#endif

    return;
}

} // extern "C"
