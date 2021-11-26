#include <assert.h>

#include <jevmachopp/Slurp.h>
#include <jevmachopp/c/jevxnuboot.h>

int main(int argc, const char *argv[]) {
    assert(argc == 2);
    uint64_t size;
    uint8_t *inbuf = Slurp::readfile(argv[1], &size);

    const void *entry_point = XNUBoot::load_and_prep_xnu_kernelcache();
    printf("entry_point: %p\n", entry_point);

    return 0;
}
