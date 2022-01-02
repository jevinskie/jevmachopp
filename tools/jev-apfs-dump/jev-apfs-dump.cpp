#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <jevmachopp/Common.h>
#include <jevmachopp/Slurp.h>

#include <ApfsLib/Device.h>
#include <ApfsLib/DeviceUBoot.h>
#include <ApfsLib/ApfsContainer.h>
#include <ApfsLib/ApfsVolume.h>
#include <ApfsLib/BlockDumper.h>
#include <ApfsLib/ApfsDir.h>
#include <ApfsLib/GptPartitionMap.h>


extern "C" {

struct udevice {
    const char *name;
};

struct udevice fake_m1_nvme = {
    .name = "nvme:0",
};

int blk_get_device(int if_type, int devnum, struct udevice **devp) {
    if (if_type != to_underlying_int(if_type::IF_TYPE_NVME)) {
        return -ENODEV;
    }
    if (devp) {
        *devp = &fake_m1_nvme;
    }
    return 0;
}

struct blk_desc *blk_get_by_device(struct udevice *dev) {
    return nullptr;
}


} // extern "C"



int main(int argc, const char **argv) {
    if (argc != 4) {
        fprintf(stderr, "usage: jev-apfs-dump <disk image> <Volume Group UUID (vuid)> <Next Stage Image4 Hash (nsih)>\n");
        return 1;
    }
    const auto disk_img_path_cstr = argv[1];
    const auto vuid_cstr = argv[2];
    const auto nsih_cstr = argv[3];
    std::size_t disk_buf_len = 0;
    const auto disk_buf = Slurp::readfile(disk_img_path_cstr, &disk_buf_len, false /* rw */, (const void *)0x400000000ull);

    return 0;
}
