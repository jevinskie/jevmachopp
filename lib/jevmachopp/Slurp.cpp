#include "jevmachopp/Slurp.h"

#include <cassert>
#include <stdio.h>
#include <string>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

uint8_t *Slurp::readfile(const char *filename, uint64_t *len, bool rw, void *preferred_addr) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        assert(!"couldnt open");
    }

    struct stat st {};
    if (fstat(fd, &st) != 0) {
        assert(!"couldnt stat");
    }

    auto *buf = (uint8_t *)mmap(preferred_addr, st.st_size, rw ? PROT_READ | PROT_WRITE : PROT_READ,
                                MAP_PRIVATE | (preferred_addr ? MAP_FIXED : 0), fd, 0);
    if (buf == NULL) {
        assert(!"couldnt mmap");
    }
    close(fd);

    if (len) {
        *len = (uint64_t)st.st_size;
    }

    return buf;
}

void Slurp::writefile(const char *filename, uint8_t *buf, uint64_t len) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0755);
    assert(fd >= 0);
    auto wrote_len = write(fd, buf, len);
    assert(wrote_len == (ssize_t)len);
    close(fd);
}

uint8_t *Slurp::mapMemory(std::size_t size, void *preferred_addr) {
    auto *buf =
        (uint8_t *)mmap(preferred_addr, size, PROT_READ | PROT_WRITE,
                        MAP_ANONYMOUS | MAP_PRIVATE | (preferred_addr ? MAP_FIXED : 0), -1, 0);
    if (buf == NULL) {
        assert(!"couldnt mmap");
    }
    return buf;
}
