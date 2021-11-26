#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <cxxopts.hpp>

#include <jevmachopp/Slurp.h>
#include <jevmachopp/c/jevxnuboot.h>

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
    if (args.count("help")) {
        printf("%s\n", options.help().data());
        exit(0);
    }

    const void *entry_point = XNUBoot::load_and_prep_xnu_kernelcache();
    printf("entry_point: %p\n", entry_point);

    return 0;
}
