#include <cxxopts.hpp>

#include <jevmachopp/DeviceTree.h>
#include <jevmachopp/Slurp.h>
#include <jevmachopp/c/jevdtree.h>

int main(int argc, const char *argv[]) {
    cxxopts::Options options("jev-dtree-dump", "DeviceTree tool");

    // clang-format off
    options.add_options()
        ("d,devicetree", "devicetree file", cxxopts::value<std::string>())
        ("p,property", "lookup property in the device tree", cxxopts::value<std::vector<std::string>>())
        ("P,patch", "patch property in the device tree", cxxopts::value<std::vector<std::string>>())
        ("m,multipatch", "patch multiple properties in the device tree", cxxopts::value<std::string>())
        ("v,nvram", "apply patches from nvram-proxy-data")
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
    auto &dt_nc    = (DTNode &)dt;
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

    if (args.count("node")) {
        const auto &nodes = args["node"].as<std::vector<std::string>>();
        for (const auto &node_path : nodes) {
            printf("lookupNode arg: %s\n", node_path.data());
            const DTNode *node_ptr = dt.lookupNode(node_path);
            printf("node_ptr: %p\n", node_ptr);
        }
    }

    if (args.count("patch")) {
        const auto &patches = args["patch"].as<std::vector<std::string>>();
        for (const auto &patch_spec : patches) {
            printf("patch_spec: %s\n", patch_spec.data());
            const auto patch_res = DT::processPatch(dt_nc, patch_spec);
            printf("patch_res: %s\n", patch_res ? "GOOD" : "BAD");
        }
    }

    if (args.count("multipatch")) {
        const auto &multipatch = args["multipatch"].as<std::string>();
        printf("multipatch: \"%s\"\n", multipatch.data());
        const auto patches_res = DT::processPatches(dt_nc, multipatch);
        printf("patches_res: %s\n", patches_res ? "GOOD" : "BAD");
    }

    if (args.count("nvram") && args["nvram"].as<bool>()) {
        printf("applying patches from nvram\n");
        const auto patches_res = DT::processPatches(dt_nc);
        printf("patches_res: %s\n", patches_res ? "GOOD" : "BAD");
    }

    return 0;
}
