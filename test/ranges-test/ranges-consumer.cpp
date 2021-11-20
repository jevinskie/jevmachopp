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

auto evenVersionsLambda(std::list<Version> &versions) {
    return ranges::views::filter(versions, [](const Version &ver) {
        return (ver.major & 1) == 0;
    });
}

int main(void) {
    fmt::print("foo(): {:d}\n", foo());

    auto versions = makeVersions();
    fmt::print("versions: {}\n", fmt::join(versions, ", "));
    auto evenVer = evenVersions(versions);
    fmt::print("evenVer: {}\n", fmt::join(evenVer, ", "));
    auto evenVerLam = evenVersionsLambda(versions);
    fmt::print("evenVerLam: {}\n", fmt::join(evenVerLam, ", "));

    return 0;
}
