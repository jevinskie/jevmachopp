#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <cxxopts.hpp>

#include <jevmachopp/MachO.h>
#include <jevmachopp/SearchFS.h>
#include <jevmachopp/SegmentCommand.h>
#include <jevmachopp/Slurp.h>

constexpr auto min_macho_sz = sizeof(MachO) + sizeof(SegmentCommand);

__attribute__((noinline)) static cxxopts::ParseResult parse_opts(int argc, const char **argv) {
    cxxopts::Options options("jev-searchfs", "SearchFS test harness");

    // clang-format off
    options.add_options()
        ("v,volume", "volume to search", cxxopts::value<std::string>())
        ("h,help", "Print usage")
    ;
    // clang-format on

    const auto args = options.parse(argc, argv);

    if (args.count("help") || !args.count("volume")) {
        printf("%s\n", options.help().data());
        exit(1);
    }

    return args;
}

__attribute__((noinline)) static std::string volume_arg(const cxxopts::ParseResult &args) {
    return args["volume"].as<std::string>();
}

// __attribute__((used, visibility("default"))) extern "C" int real_main(int argc,
//                                                                       const char *argv[]) {
int main(int argc, const char **argv) {
    const auto args = parse_opts(argc, argv);
    const auto volume_path = volume_arg(args);
    // const auto volume_path = std::string{"/"};
    const char *volume_path_cstr = volume_path.data();
    const auto search_res = SearchFS::files_larger_than(volume_path_cstr, min_macho_sz);
    printf("SearchFS::files_larger_than(\"%s\", %zu) = %s\n", volume_path_cstr, min_macho_sz,
           YESNOCStr(search_res));

    return 0;
}
