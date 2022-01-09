#include "jevmachopp/UBootAPFS.h"

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>



#include <ApfsLib/ApfsContainer.h>
#include <ApfsLib/ApfsDir.h>
#include <ApfsLib/ApfsVolume.h>
#include <ApfsLib/BlockDumper.h>
#include <ApfsLib/Device.h>
#include <ApfsLib/DeviceUBoot.h>
#include <ApfsLib/GptPartitionMap.h>


#include <jevmachopp/Common.h>

#include <nanorange/views/drop.hpp>

namespace UBootAPFS {


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
        printf("looking up childName: \"%*s\"\n", SV2PF(childName));
        res = std::unique_ptr<ApfsDir::DirRec>{childDirNamed(apfsDir, res.get(), childName)};
        if (!res) {
            return nullptr;
        }
    }
    return res;
}

void uboot_apfs_doit(void) {
    auto dev = Device::OpenDevice("host:0");
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

    std::string_view kc_dir{"/D8961206-5EAC-4D35-94A3-5412F17E6B3B/boot/CBE5168B59E7B0104701733E3A617B82BC6F895B88CACFCE327725CB5532929C19244CFCEF709E3D0F8337A4866F608C/System/Library/Caches/com.apple.kernelcaches"};
    std::string_view kc_path{"/D8961206-5EAC-4D35-94A3-5412F17E6B3B/boot/CBE5168B59E7B0104701733E3A617B82BC6F895B88CACFCE327725CB5532929C19244CFCEF709E3D0F8337A4866F608C/System/Library/Caches/com.apple.kernelcaches/kernelcache"};

    ApfsDir apfsDir(*preboot_vol);
    ApfsDir::DirRec res;
    assert(apfsDir.LookupName(res, ROOT_DIR_INO_NUM, "D8961206-5EAC-4D35-94A3-5412F17E6B3B"));

    assert(apfsDir.LookupName(res, ROOT_DIR_PARENT, "root"));

    const auto kc_dir_res = lookupDir(&apfsDir, kc_dir);
    if (kc_dir_res) {
        printf("found dir named: %s\n", kc_dir_res->name.c_str());
    } else {
        printf("failed to find dir\n");
    }

    return;
}

} // namespace UBootAPFS
