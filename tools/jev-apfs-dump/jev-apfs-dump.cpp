#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <jevmachopp/Common.h>
#include <jevmachopp/Slurp.h>

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
    if (if_type != to_underlying_int(if_type::IF_TYPE_NVME) &&
        if_type != to_underlying_int(if_type::IF_TYPE_HOST) &&
        devnum != 0) {
        return -ENODEV;
    }
    if (devp) {
        switch (if_type) {
        case to_underlying_int(if_type::IF_TYPE_NVME):
            *devp = &fake_m1_nvme;
            break;
        case to_underlying_int(if_type::IF_TYPE_HOST):
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

} // extern "C"

unsigned long fake_block_read(struct blk_desc *block_dev, lbaint_t start, lbaint_t blkcnt,
                              void *buffer) {
    if (start + blkcnt > block_dev->lba) {
        return -1;
    }
    memcpy(buffer, block_dev->__buf + (start * block_dev->blksz), blkcnt * block_dev->blksz);
    return blkcnt;
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

    return 0;
}
