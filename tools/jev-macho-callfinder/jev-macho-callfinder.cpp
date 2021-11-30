#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <cxxopts.hpp>

#include <jevmachopp/Common.h>
#include <jevmachopp/MachO.h>
#include <jevmachopp/Slurp.h>

int main(int argc, const char *argv[]) {
    cxxopts::Options options("jev-xnu-load",
                             "Mock iBoot prep of KC and other data then mock chainloading xnu");

    // clang-format off
    options.add_options()
        ("m,macho", "macho file", cxxopts::value<std::string>())
        ("s,symbol", "symbol to find calls to", cxxopts::value<std::string>())
        ("h,help", "Print usage")
    ;
    // clang-format on

    auto args = options.parse(argc, argv);
    if (args.count("help") || !args.count("macho") || !args.count("symbol")) {
        printf("%s\n", options.help().data());
        return 1;
    }

    size_t macho_sz = 0;
    const auto *macho_buf = Slurp::readfile(args["macho"].as<std::string>().data(), &macho_sz,
                                            false, (const void *)0x400000000);
    const auto &macho = *(const MachO *)macho_buf;

    fmt::print("macho: {}\n", macho);

    return 0;
}
