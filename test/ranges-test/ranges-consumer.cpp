#include "ranges-consumer.h"
#include "ranges-producer.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

bool isVersionEven(const Version &ver) {
    return (ver.major & 1) == 0;
}

auto evenVersions(std::list<Version> &versions) {
    return ranges::views::filter(versions, isVersionEven);
}

int main(void) {
    fmt::print("foo(): {:d}\n", foo());

    auto versions = makeVersions();
    fmt::print("versions: {}\n", fmt::join(versions, ", "));
    for (const auto &ver : versions) {
        fmt::print("ver: {}\n", ver);
    }
    auto evenVer = evenVersions(versions);
    auto b = evenVer.begin();
    auto e = evenVer.end();
    fmt::print("&*b: {:p} &*e: {:p}\n", fmt::ptr(&*b), fmt::ptr(&*e));
    for (const auto &ver : evenVer) {
        fmt::print("ver: {}\n", ver);
    }
    // fmt::print("versions: {}\n", fmt::join(evenVer, ", "));

    return 0;
}
