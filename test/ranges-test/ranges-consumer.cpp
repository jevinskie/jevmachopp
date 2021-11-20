#include "ranges-consumer.h"
#include "ranges-producer.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

int main(void) {
    fmt::print("foo(): {:d}\n", foo());

    auto versions = makeVersions();
    fmt::print("versions: {}\n", fmt::join(versions, ", "));

    return 0;
}
