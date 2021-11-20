#include "jevmachopp/Slurp.h"

#include <cassert>
#include <stdio.h>
#include <string>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

uint8_t *Slurp::readfile(const std::string &filename, uint64_t *len, void *preferred_addr) {
    int fd;
    struct stat st;
    uint8_t *buf;

    fd = open(filename.c_str(), O_RDONLY);
    if (fd == -1)
        assert(!"couldnt open");
    if (fstat(fd, &st) != 0)
        assert(!"couldnt stat");

    *len = (uint64_t)st.st_size;

    buf = (uint8_t *)mmap(preferred_addr, st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (buf == NULL)
        assert(!"couldnt mmap");
    close(fd);

    return buf;
}

void Slurp::writefile(const std::string &filename, uint8_t *buf, uint64_t len) {
    int fd;

    fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0755);
    write(fd, buf, len);
    close(fd);
}