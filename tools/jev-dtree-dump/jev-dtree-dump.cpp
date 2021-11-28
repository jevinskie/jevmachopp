#include <jevmachopp/Slurp.h>
#include <jevmachopp/c/jevdtree.h>

#include <cxxopts.hpp>

int main(int argc, const char *argv[]) {
    cxxopts::Options options("jev-dtree-dump", "DeviceTree tool");

    // clang-format off
    options.add_options()
        ("d,devicetree", "devicetree file", cxxopts::value<std::string>())
        ("l,lookup", "lookup path in the device tree", cxxopts::value<std::vector<std::string>>())
        ("h,help", "Print usage")
    ;
    // clang-format on

    auto args = options.parse(argc, argv);
    if (args.count("help")) {
        puts(options.help().data());
        return 1;
    }

    uint8_t *dtbuf = Slurp::readfile(args["devicetree"].as<std::string>().data());

    dump_dtree(dtbuf);

    printf("args.count(\"lookup\"): %zu\n", args.count("lookup"));
    if (args.count("lookup")) {
        const auto &lookups = args["lookup"].as<std::vector<std::string>>();
        for (const auto &lookup : lookups) {
            printf("lookup arg: %s\n", lookup.data());
        }
    }

    return 0;
}
