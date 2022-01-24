#include "jevmachopp/UBootAPFS.h"

#include <cassert>
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

struct fs_dirent;
struct fs_dir_stream;
typedef long long int loff_t;
typedef unsigned long int ulong;
typedef unsigned char uchar;

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

// /foo
// /foo/bar
// /foo/bar/

struct APFSPath {
    APFSPath(const std::string &full_path) : valid(false) {
        if (full_path.empty() || full_path[0] != '/')
            return;
        auto sv = stringSplitViewDelimitedBy(full_path, '/') | views::drop(1);
        if (const auto vol = ranges::get(sv, 0))
            volume = *vol;
        path = ranges::join(sv | views::drop(1), std::string{"/"});
        std::cout << "APFSPath(\"" << full_path << "\") vol: " << volume << " path: " << path
                  << "\n";
        valid = true;
    }
    std::string volume;
    std::string path;
    bool valid;
};

std::unique_ptr<ApfsDir::DirRec> childDirNamed(ApfsDir *apfsDir, ApfsDir::DirRec *parentDir,
                                               std::string_view childDirName) {
    assert(parentDir);
    auto res = std::make_unique<ApfsDir::DirRec>();
    if (!apfsDir->LookupName(*res, parentDir->file_id, std::string{childDirName}.c_str())) {
        return nullptr;
    }
    return res;
}

std::unique_ptr<ApfsDir::DirRec> lookupDir(ApfsDir *apfsDir, std::string_view dirPath) {
    if (!dirPath.size() || dirPath[0] != '/') {
        return nullptr;
    }
    std::unique_ptr<ApfsDir::DirRec> res = std::make_unique<ApfsDir::DirRec>();
    assert(apfsDir->LookupName(*res, ROOT_DIR_PARENT, "root"));
    for (const auto childName : stringSplitViewDelimitedBy(dirPath, '/') | views::drop(1)) {
        printf("looking up childName: \"%*s\"\n", SV2PF(childName));
        res = std::unique_ptr<ApfsDir::DirRec>{childDirNamed(apfsDir, res.get(), childName)};
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
    bool open(struct blk_desc *fs_dev_desc, struct disk_partition *fs_partition) {
        if (!m_dev.Open(fs_dev_desc))
            return false;
        m_container = new (reinterpret_cast<ApfsContainer *>(&m_container_storage))
            ApfsContainer(&m_dev, fs_partition->start, fs_partition->size);
        m_opened = true;
        return true;
    }
    void close() {
        m_opened = false;
        std::destroy_at(std::launder(m_container));
        m_container = nullptr;
        m_dev.Close();
    }
    const ApfsContainer *container() const {
        if (!m_opened)
            return nullptr;
        return const_cast<const ApfsContainer *>(m_container);
    }
    ApfsContainer *container() {
        return const_cast<ApfsContainer *>(static_cast<const APFSCtx &>(*this).container());
    }
    bool opened() const {
        return m_opened;
    }

private:
    DeviceUBoot m_dev;
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
    // assert(!"apfs_probe");
    struct disk_partition *p = fs_partition;
    printf("apfs_probe() part start: 0x%lx sz: 0x%lx blksz: %ld name: %s type: %s bootable: %d "
           "uuid: %s guid: %s\n",
           p->start, p->size, p->blksz, p->name, p->type, p->bootable, p->uuid, p->type_guid);
    bool good = apfs_ctx.open(fs_dev_desc, fs_partition);
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
    return -1;
}

int apfs_readdir(struct fs_dir_stream *dirs, struct fs_dirent **dentp) {
    assert(!"apfs_readdir");
    return -1;
}

void apfs_closedir(struct fs_dir_stream *dirs) {
    assert(!"apfs_closedir");
}

void uboot_apfs_doit(void) {
    setvbuf(stdout, nullptr, _IONBF, 0);

    printf("entering uboot_apfs_doit()\n");

    // g_debug = 0xff;

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
    ApfsDir::DirRec res;
    printf("res: %p\n", (void *)&res);
    // assert(apfsDir.LookupName(res, ROOT_DIR_INO_NUM, "D8961206-5EAC-4D35-94A3-5412F17E6B3B"));
    // printf("lookup of root dir D8961206-5EAC-4D35-94A3-5412F17E6B3B worked\n");

    std::vector<ApfsDir::DirRec> list_res;
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

    return;
}

} // extern "C"
