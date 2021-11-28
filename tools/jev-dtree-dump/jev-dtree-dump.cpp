#include <jevmachopp/DeviceTree.h>
#include <jevmachopp/Slurp.h>
#include <jevmachopp/c/jevdtree.h>

#include <cxxopts.hpp>

int main(int argc, const char *argv[]) {
    cxxopts::Options options("jev-dtree-dump", "DeviceTree tool");

    // clang-format off
    options.add_options()
        ("d,devicetree", "devicetree file", cxxopts::value<std::string>())
        ("p,property", "lookup property in the device tree", cxxopts::value<std::vector<std::string>>())
        ("n,node", "lookup node in the device tree", cxxopts::value<std::vector<std::string>>())
        ("h,help", "Print usage")
    ;
    // clang-format on

    auto args = options.parse(argc, argv);
    if (args.count("help")) {
        puts(options.help().data());
        return 1;
    }

    uint8_t *dtbuf = Slurp::readfile(args["devicetree"].as<std::string>().data());
    const auto &dt = *(const DTNode *)dtbuf;
    dump_dtree(dtbuf);

    printf("args.count(\"property\"): %zu\n", args.count("property"));
    if (args.count("property")) {
        const auto &props = args["property"].as<std::vector<std::string>>();
        for (const auto &prop_path : props) {
            printf("lookupProperty arg: %s\n", prop_path.data());
            const DTProp *prop_ptr = dt.lookupProperty(prop_path.data());
            printf("prop_ptr: %p\n", prop_ptr);
        }
    }

    return 0;
}
