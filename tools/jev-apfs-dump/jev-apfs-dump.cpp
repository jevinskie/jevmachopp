#include <cassert>
#include <cstdint>
#include <cstdio>

#include <jevmachopp/Slurp.h>

#include <ApfsLib/Device.h>
#include <ApfsLib/ApfsContainer.h>
#include <ApfsLib/ApfsVolume.h>
#include <ApfsLib/BlockDumper.h>
#include <ApfsLib/ApfsDir.h>
#include <ApfsLib/GptPartitionMap.h>

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
