#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <cxxopts.hpp>

#include <jevmachopp/Slurp.h>
#include <jevmachopp/c/jevxnuboot.h>

#if __has_include(<ptrauth.h>)
#include <ptrauth.h>
#endif

// clang-format off
constexpr uintptr_t virt_base       = 0xffff'fe00'0dcb'c000;
constexpr uintptr_t phys_base       = 0x0000'0008'01cb'c000;
constexpr uintptr_t virt_phys_off   = 0xffff'fdf8'0c00'0000;
static_assert_cond(virt_base - phys_base == virt_phys_off);

constexpr uintptr_t dt_base         = 0x0000'0008'040c'c000;

constexpr uintptr_t tc_base         = 0x0000'0008'0412'4000;

constexpr uintptr_t sepfw_base      = 0x0000'0008'0c61'c000;

constexpr uintptr_t ba_base         = 0x0000'0008'0cd4'4000;

constexpr uintptr_t mmap_base       = 0x0000'0008'0000'0000;
constexpr uintptr_t mmap_size       = 512 * 1024 * 1024;

constexpr uintptr_t kc_payload_base = 0x0000'0008'0461'c000;
// clang-format on

int main(int argc, const char *argv[]) {
    cxxopts::Options options("jev-xnu-load",
                             "Mock iBoot prep of KC and other data then mock chainloading xnu");

    // clang-format off
    options.add_options()
        ("k,kernelcache", "kernelcache file", cxxopts::value<std::string>())
        ("s,sepfw", "SEPFW file", cxxopts::value<std::string>())
        ("b,boot-args", "BootArgs file", cxxopts::value<std::string>())
        ("d,devicetree", "DeviceTree file", cxxopts::value<std::string>())
        ("t,trustcache", "TrustCache file", cxxopts::value<std::string>())
        ("h,help", "Print usage")
    ;
    // clang-format on

    auto args = options.parse(argc, argv);
    if (args.count("help") || !args.count("kernelcache") || !args.count("sepfw") ||
        !args.count("boot-args") || !args.count("devicetree") || !args.count("trustcache")) {
        printf("%s\n", options.help().data());
        return 1;
    }

    auto *mem = Slurp::mapMemory(mmap_size, (const void *)mmap_base);
    printf("mem: %p\n", mem);
    assert(mem && (uintptr_t)mem == mmap_base);

    size_t kc_bin_sz = 0;
    const auto *kc_bin =
        Slurp::readfile(args["kernelcache"].as<std::string>().data(), &kc_bin_sz, false, nullptr);
    memcpy((char *)kc_payload_base, kc_bin, kc_bin_sz);

    size_t sepfw_bin_sz = 0;
    const auto *sepfw_bin =
        Slurp::readfile(args["sepfw"].as<std::string>().data(), &sepfw_bin_sz, false, nullptr);
    memcpy((char *)sepfw_base, sepfw_bin, sepfw_bin_sz);

    size_t bootargs_bin_sz   = 0;
    const auto *bootargs_bin = Slurp::readfile(args["boot-args"].as<std::string>().data(),
                                               &bootargs_bin_sz, false, nullptr);
    memcpy((char *)ba_base, bootargs_bin, bootargs_bin_sz);

    size_t devicetree_bin_sz   = 0;
    const auto *devicetree_bin = Slurp::readfile(args["devicetree"].as<std::string>().data(),
                                                 &devicetree_bin_sz, false, nullptr);
    memcpy((char *)dt_base, devicetree_bin, devicetree_bin_sz);

    size_t trustcache_bin_sz   = 0;
    const auto *trustcache_bin = Slurp::readfile(args["trustcache"].as<std::string>().data(),
                                                 &trustcache_bin_sz, false, nullptr);
    memcpy((char *)tc_base, trustcache_bin, trustcache_bin_sz);

    XNUBoot::load_and_prep_xnu_kernelcache((const void *)ba_base);
    printf("XNUBoot::load_and_prep_xnu_kernelcache completed\n");

    return 0;
}
