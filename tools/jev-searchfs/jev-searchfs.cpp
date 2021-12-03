#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <cxxopts.hpp>

#include <jevmachopp/MachO.h>
#include <jevmachopp/SearchFS.h>
#include <jevmachopp/SegmentCommand.h>
#include <jevmachopp/Slurp.h>

constexpr auto min_macho_sz = sizeof(MachO) + sizeof(SegmentCommand);

int main(int argc, const char *argv[]) {
    cxxopts::Options options("jev-searchfs", "SearchFS test harness");

    // clang-format off
    options.add_options()
        ("v,volume", "volume to search", cxxopts::value<std::string>())
        ("h,help", "Print usage")
    ;
    // clang-format on

    auto args = options.parse(argc, argv);
    if (args.count("help") || !args.count("volume")) {
        printf("%s\n", options.help().data());
        return 1;
    }

    const auto volume_path = args["volume"].as<std::string>();
    const char *volume_path_cstr = volume_path.data();
    const auto search_res = SearchFS::files_larger_than(volume_path_cstr, min_macho_sz);
    printf("SearchFS::files_larger_than(\"%s\", %zu) = %s\n", volume_path_cstr, min_macho_sz,
           YESNOCStr(search_res));

    return 0;
}
