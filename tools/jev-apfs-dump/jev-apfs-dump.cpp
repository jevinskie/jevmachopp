#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <jevmachopp/Common.h>
#include <jevmachopp/Slurp.h>

#include <nanorange/views/drop.hpp>

#include <ApfsLib/ApfsContainer.h>
#include <ApfsLib/ApfsDir.h>
#include <ApfsLib/ApfsVolume.h>
#include <ApfsLib/BlockDumper.h>
#include <ApfsLib/Device.h>
#include <ApfsLib/DeviceUBoot.h>
#include <ApfsLib/GptPartitionMap.h>

unsigned long fake_block_read(struct blk_desc *block_dev, lbaint_t start, lbaint_t blkcnt,
                              void *buffer);

extern "C" {

struct udevice fake_m1_nvme = {
    .name = "nvme:0",
};

struct udevice fake_m1_host = {
    .name = "host:0",
};

struct blk_desc fake_m1_nvme_blk_desc = {
    .blksz       = 4096,
    .block_read  = &fake_block_read,
    .block_write = nullptr,
    .block_erase = nullptr,
};

int blk_get_device(int if_type, int devnum, struct udevice **devp) {
    if (if_type != to_underlying_int(intf_type::IF_TYPE_NVME) &&
        if_type != to_underlying_int(intf_type::IF_TYPE_HOST) && devnum != 0) {
        return -ENODEV;
    }
    if (devp) {
        switch (if_type) {
        case to_underlying_int(intf_type::IF_TYPE_NVME):
            *devp = &fake_m1_nvme;
            break;
        case to_underlying_int(intf_type::IF_TYPE_HOST):
            *devp = &fake_m1_host;
            break;
        default:
            assert(!"unsupported interface type");
            break;
        }
    }
    return 0;
}

struct blk_desc *blk_get_by_device(struct udevice *dev) {
    if (dev != &fake_m1_nvme && dev != &fake_m1_host) {
        return nullptr;
    }
    return &fake_m1_nvme_blk_desc;
}

void *dev_get_uclass_plat(const struct udevice *dev) {
    if (dev != &fake_m1_nvme && dev != &fake_m1_host) {
        return nullptr;
    }
    return &fake_m1_nvme_blk_desc;
}


} // extern "C"

unsigned long fake_block_read(struct blk_desc *block_dev, lbaint_t start, lbaint_t blkcnt,
                              void *buffer) {
    if (start + blkcnt > block_dev->lba) {
        return -1;
    }
    memcpy(buffer, block_dev->__buf + (start * block_dev->blksz), blkcnt * block_dev->blksz);
    return blkcnt;
}

std::unique_ptr<ApfsDir::DirRec> childDirNamed(ApfsDir *apfsDir, ApfsDir::DirRec *parentDir, std::string_view childDirName) {
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
        fmt::print("looking up childName: \"{:s}\"\n", childName);
        res = std::unique_ptr<ApfsDir::DirRec>{childDirNamed(apfsDir, res.get(), childName)};
        if (!res) {
            return nullptr;
        }
    }
    return res;
}

int main(int argc, const char **argv) {
    if (argc != 4) {
        fprintf(stderr, "usage: jev-apfs-dump <disk image> <Volume Group UUID (vuid)> <Next Stage "
                        "Image4 Hash (nsih)>\n");
        return 1;
    }
    const auto disk_img_path_cstr = argv[1];
    const auto vuid_cstr          = argv[2];
    const auto nsih_cstr          = argv[3];
    std::size_t disk_buf_len      = 0;
    fake_m1_nvme_blk_desc.__buf = Slurp::readfile(disk_img_path_cstr, &disk_buf_len, false /* rw */,
                                                  (const void *)0x400000000ull);
    assert(disk_buf_len % fake_m1_nvme_blk_desc.blksz == 0);
    fake_m1_nvme_blk_desc.lba = disk_buf_len / fake_m1_nvme_blk_desc.blksz;

    auto dev = Device::OpenDevice("nvme:0");

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
    fprintf(stderr, "# volumes in container: %u\n", nvol);

    ApfsVolume *preboot_vol = nullptr;
    for (unsigned int volidx = 0; volidx < nvol; ++volidx) {
        apfs_superblock_t sb;
        assert(container->GetVolumeInfo(volidx, sb));
        fprintf(stderr, "role: 0x%04hx\n", sb.apfs_role);
        if (sb.apfs_role == APFS_VOL_ROLE_PREBOOT) {
            preboot_vol = container->GetVolume(volidx);
            break;
        }
    }
    assert(preboot_vol);
    fprintf(stderr, "preboot name: %s\n", preboot_vol->name());

    std::string_view kc_dir{"/D8961206-5EAC-4D35-94A3-5412F17E6B3B/boot/CBE5168B59E7B0104701733E3A617B82BC6F895B88CACFCE327725CB5532929C19244CFCEF709E3D0F8337A4866F608C/System/Library/Caches/com.apple.kernelcaches"};
    std::string_view kc_path{"/D8961206-5EAC-4D35-94A3-5412F17E6B3B/boot/CBE5168B59E7B0104701733E3A617B82BC6F895B88CACFCE327725CB5532929C19244CFCEF709E3D0F8337A4866F608C/System/Library/Caches/com.apple.kernelcaches/kernelcache"};

    ApfsDir apfsDir(*preboot_vol);
    ApfsDir::DirRec res;
    assert(apfsDir.LookupName(res, ROOT_DIR_INO_NUM, "D8961206-5EAC-4D35-94A3-5412F17E6B3B"));

    assert(apfsDir.LookupName(res, ROOT_DIR_PARENT, "root"));

    const auto kc_dir_res = lookupDir(&apfsDir, kc_dir);
    if (kc_dir_res) {
        fmt::print("found dir named: {:s}\n", kc_dir_res->name);
    } else {
        fmt::print("failed to find dir\n");
    }

    return 0;
}
