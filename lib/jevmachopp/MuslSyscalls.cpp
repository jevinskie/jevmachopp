#if __has_include(<_musl_version.h>)

#include <cassert>
#include <cerrno>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstring>

#include <signal.h>
#include <sys/uio.h>
#include <unistd.h>

#include "jevmachopp/OtherLibCSupplement.h"

#define printf olibc2uboot_printf


extern "C" {

const char *__embcust_argv[] = {
    "embcust",
    nullptr,
};

const char *__embcust_environ[] = {
    nullptr,
};

int SYS_IMP_writev(int fd, const struct iovec *iov, int iovcnt) {
    int res = 0;
    for (int i = 0; i < iovcnt; ++i) {
        int sub_res = olibc2uboot_write(fd, iov[i].iov_base, iov[i].iov_len);
        res += iov[i].iov_len;
        if (sub_res < iov[i].iov_len) {
            if (sub_res >= 0) {
                return res;
            } else {
                return sub_res;
            }
        }
    }
    return res;
}

int posix_memalign(void **memptr, size_t alignment, size_t size) {
    return -1;
}


int SYS_IMP_close(int fd) {
    assert(!"close() unimp");
    return -ENOSYS;
}

int SYS_IMP_exit(int ec) {
    assert(!"exit() unimp");
    return -ENOSYS;
}

int SYS_IMP_exit_group(int ec) {
    assert(!"exit_group() unimp");
    return -ENOSYS;
}

int SYS_IMP_futex(uint32_t *uaddr, int futex_op, uint32_t val, const struct timespec *timeout, uint32_t *uaddr2, uint32_t val3) {
    assert(!"futex() unimp");
    return -ENOSYS;
}

int SYS_IMP_ioctl(int fd, unsigned long request, ...) {
    // assert(!"ioctl() unimp");
    return -ENOSYS;
}

int SYS_IMP_lseek(int fd, off_t offset, int whence) {
    assert(!"lseek() unimp");
    return -ENOSYS;
}

int SYS_IMP_rt_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact, size_t sigsetsize) {
    assert(!"rt_sigaction() unimp");
    return -ENOSYS;
}

int SYS_IMP_rt_sigprocmask(int how, const sigset_t *set, sigset_t *oldset, size_t sigsetsize) {
    // assert(!"rt_sigprocmask() unimp");
    return -ENOSYS;
}

int SYS_IMP_tkill(pid_t tid, int sig) {
    assert(!"rt_sigprocmask() unimp");
    return -ENOSYS;   
}

// void _fini(void) {
//     assert(!"_fini() unimp");
// }

int board_fit_config_name_match(const char *name) {
    assert(!"board_fit_config_name_match()");
    return -1;
}

void board_fit_image_post_process(void **p_image, size_t *p_size) {
    assert(!"board_fit_image_post_process()");
}

} // extern "C"

#endif
