#include <assert.h>
#include <fmt/core.h>
#include <jevmachopp/MachO.h>
#include <jevmachopp/SegmentCommand.h>
#include <jevmachopp/Slurp.h>

int main(int argc, const char *argv[]) {
    assert(argc == 2);
    std::string infile(argv[1]);
    uint64_t size;
    uint8_t *inbuf = Slurp::readfile(infile, &size);
    auto macho = (const MachO *)inbuf;
    fmt::print("macho: {}\n", (void *)macho);
    fmt::print("macho fmt: {}\n", *macho);
    fmt::print("macho->cputype: {}\n", macho->cputype);

    for (const auto &o : macho->segmentLoadCommands()) {
        fmt::print("o: {}\n", o);
    }

    fmt::print("ret: {:s}\n", type_name<decltype(macho->segments())>());

    for (const auto &o : macho->segments()) {
        fmt::print("o: {}\n", *o);
    }

    return 0;
}
