#pragma once

#include "jevmachopp/Common.h"
#include "jevmachopp/c/jevxnuboot.h"

class DTNode;

namespace XNUBoot {

constexpr auto BOOT_LINE_LENGTH = 608;

extern "C" void xnu_jump_stub(uint64_t new_base_plus_bootargs_off, uint64_t image_addr,
                              uint64_t new_base, uint64_t image_size, uint64_t image_entry);
extern "C" char _xnu_jump_stub_end;

using generic_func = uint64_t (*)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

struct vector_args {
    generic_func *entry;
    uint64_t args[5];
    bool restore_logo;
};

extern "C" struct vector_args next_stage;

} // namespace XNUBoot

class XNUBootArgsVideo {
public:
    void *baseAddr;    // Base address of video memory
    uint64_t display;  // Display Code (if Applicable)
    uint64_t rowBytes; // Number of bytes per pixel row
    uint64_t width;    // Width
    uint64_t height;   // Height
    uint64_t depth;    // Pixel Depth and other parameters
};

static_assert_size_is(XNUBootArgsVideo, 48);

class XNUBootArgs {
public:
    uint16_t revision;         // Revision of boot_args structure
    uint16_t version;          // Version of boot_args structure
    uint64_t virtBase;         // Virtual base of memory
    uint64_t physBase;         // Physical base of memory
    uint64_t memSize;          // Size of memory
    uint64_t topOfKernelData;  // Highest physical address used in kernel data area
    XNUBootArgsVideo video;    // Video Information
    uint32_t machineType;      // Machine Type
    DTNode *deviceTree;        // Base of flattened device tree
    uint32_t deviceTreeLength; // Length of flattened tree
    char commandLine[XNUBoot::BOOT_LINE_LENGTH]; // Passed in command line
    uint64_t bootFlags;                          // Additional flags specified by the bootloader
    uint64_t memSizeActual;                      // Actual size of memory
};

static_assert_size_is(XNUBootArgs, 736);
