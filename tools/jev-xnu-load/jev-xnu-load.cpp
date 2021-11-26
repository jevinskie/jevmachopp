#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <cxxopts.hpp>

#include <jevmachopp/Slurp.h>
#include <jevmachopp/c/jevxnuboot.h>

// clang-format off
constexpr uintptr_t virt_base     = 0xffff'fe00'19cb'c000;
constexpr uintptr_t phys_base     = 0x0000'0008'01cb'c000;
constexpr uintptr_t virt_phys_off = 0xffff'fdf8'1800'0000;

constexpr uintptr_t dt_arg_base   = 0xffff'fe00'1bc6'4000;
constexpr uintptr_t dt_base       = 0x0000'0008'03c6'4000;
constexpr uintptr_t dt_end        = 0x0000'0008'03cb'c000;

constexpr uintptr_t tc_base       = 0x0000'0008'03cb'c000;
constexpr uintptr_t tc_end        = 0x0000'0008'03d1'c000;

constexpr uintptr_t m1n1_base     = 0x0000'0008'03d1'c000;

constexpr uintptr_t kc_base       = 0x0000'0008'03d1'c000;
constexpr uintptr_t entry_point   = 0x0000'0008'0463'8530;
constexpr uintptr_t kc_end        = 0x0000'0008'092c'c000;

constexpr uintptr_t sepfw_base    = 0x0000'0008'081b'4000;
constexpr uintptr_t sepfw_end     = 0x0000'0008'088d'c000;

constexpr uintptr_t ba_base       = 0x0000'0008'088d'c000;
constexpr uintptr_t ba_end        = 0x0000'0008'088e'0000;

constexpr uintptr_t mmap_base     = 0x0000'0008'0000'0000;
constexpr uintptr_t mmap_size     = 256 * 1024 * 1024;
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
    // memcpy((char *)kc_base, )

    size_t sepfw_bin_sz = 0;
    const auto *sepfw_bin =
        Slurp::readfile(args["sepfw"].as<std::string>().data(), &sepfw_bin_sz, false, nullptr);
    memcpy((char *)sepfw_base, sepfw_bin, sepfw_bin_sz);

    size_t bootargs_bin_sz = 0;
    const auto *bootargs_bin = Slurp::readfile(args["boot-args"].as<std::string>().data(),
                                               &bootargs_bin_sz, false, nullptr);
    memcpy((char *)ba_base, bootargs_bin, bootargs_bin_sz);

    size_t devicetree_bin_sz = 0;
    const auto *devicetree_bin = Slurp::readfile(args["devicetree"].as<std::string>().data(),
                                                 &devicetree_bin_sz, false, nullptr);
    memcpy((char *)dt_base, devicetree_bin, devicetree_bin_sz);

    size_t trustcache_bin_sz = 0;
    const auto *trustcache_bin = Slurp::readfile(args["trustcache"].as<std::string>().data(),
                                                 &trustcache_bin_sz, false, nullptr);
    memcpy((char *)tc_base, trustcache_bin, trustcache_bin_sz);

    const void *calculated_entry_point = XNUBoot::load_and_prep_xnu_kernelcache();
    printf("calculated_entry_point: %p\n", calculated_entry_point);

    return 0;
}
