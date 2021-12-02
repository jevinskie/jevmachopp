#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <cxxopts.hpp>

#include <jevmachopp/CallFinder.h>
#include <jevmachopp/Common.h>
#include <jevmachopp/MachO.h>
#include <jevmachopp/Slurp.h>

int main(int argc, const char *argv[]) {
    cxxopts::Options options("jev-macho-callfinder",
                             "Find calls in MachO files and their probable arguments");

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
    const auto &symbol_name = args["symbol"].as<std::string>();
    const auto find_res = CallFinder::findCallsTo(macho, symbol_name);
    fmt::print("findCallsTo(\"{:s}\"): {}\n", symbol_name, find_res);

    return 0;
}
