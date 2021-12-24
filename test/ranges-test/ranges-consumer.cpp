#include "ranges-consumer.h"
#include "ranges-producer.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

int main(void) {
    fmt::print("foo(): {:d}\n", foo());

    auto versions = makeVersions();
    fmt::print("versions: {}\n", fmt::join(versions, ", "));
    auto evenVer = evenVersions(versions);
    fmt::print("evenVer: {}\n", fmt::join(evenVer, ", "));
    // auto evenVerLam = evenVersionsLambda(versions);
    // fmt::print("evenVerLam: {}\n", fmt::join(evenVerLam, ", "));

    return 0;
}
