#include <assert.h>

#include <jevmachopp/Slurp.h>
#include <jevmachopp/c/jevmacho.h>

int main(int argc, const char *argv[]) {
    assert(argc == 2);
    uint8_t *inbuf = Slurp::readfile(argv[1]);

    dump_macho(inbuf);

    return 0;
}
