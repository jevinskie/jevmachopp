#if __has_include(<_musl_version.h>)

#include <cassert>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <sys/auxv.h>
#include <sys/uio.h>
#include <unistd.h>

#include "jevmachopp/Common.h"
#include "jevmachopp/OtherLibCSupplement.h"

#define printf olibc2uboot_printf
#undef assert
#define assert(expr)                                                                               \
    ((void)((expr) || (olibc2uboot_assert(#expr, __FILE__, __LINE__, __PRETTY_FUNCTION__), 0)))

extern "C" {

void *__dso_handle;

const char *__embcust_argv[] = {
    "embcust",
    nullptr,
};

const char *__embcust_environ[] = {
    // "LIBUNWIND_PRINT_APIS=1",
    // "LIBUNWIND_PRINT_UNWINDING=1",
    // "LIBUNWIND_PRINT_DWARF=1",
    nullptr,
};

typedef struct {
    size_t type;
    size_t val;
} auxv_t;

auxv_t __embcust_auxv[] = {
    {.type = 0, .val = 0},
};

extern void __libc_start_embcust(void);
void (*__libc_start_embcust_dummy_import)(void) = __libc_start_embcust;

// for compiler-rt __aarch64_have_lse_atomics really
unsigned long (*__getauxval_dummy_import)(unsigned long) = getauxval;

ssize_t SYS_IMP_writev(int fd, const struct iovec *iov, int iovcnt) {
    ssize_t res = 0;
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

void *__libc_calloc(size_t num, size_t size) {
    return calloc(num, size);
}

void __libc_free(void *ptr) {
    free(ptr);
}

void *__libc_malloc(size_t size) {
    return malloc(size);
}

int posix_memalign(void **memptr, size_t alignment, size_t size) {
    if (alignment < sizeof(void *) ||
        alignment % sizeof(void *) ||
        !is_pow2(alignment))
        return EINVAL;
    auto res = memalign(alignment, size);
    if (!res)
        return ENOMEM;
    *memptr = res;
    return 0;
}

pid_t SYS_IMP_getpid(void) {
    return 1; // we be init, yo
}

pid_t SYS_IMP_set_tid_address(int *tidptr) {
    return SYS_IMP_getpid();
}

int SYS_IMP_close(int fd) {
    assert(!"close() unimp");
    return -ENOSYS;
}

int SYS_IMP_exit(int ec) {
    assert(!"exit() called");
    // on assert() path
    return -ENOSYS;
}

int SYS_IMP_exit_group(int ec) {
    // assert(!"exit_group() unimp");
    // on assert() path
    return -ENOSYS;
}

int SYS_IMP_fcntl(int fd, int cmd, long arg) {
    assert(!"fcntl() unimp");
    return -ENOSYS;
}

int SYS_IMP_fstat(int fd, struct stat *statbuf) {
    assert(!"fstat() unimp");
    return -ENOSYS;
}

void *SYS_IMP_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    assert(!"mmap() unimp");
    return nullptr;
}

int SYS_IMP_munmap(void *addr, size_t length) {
    assert(!"munmap() unimp");
    return -ENOSYS;
}

int SYS_IMP_openat(int dirfd, const char *pathname, int flags, mode_t mode) {
    assert(!"openat() unimp");
    return -ENOSYS;
}

int SYS_IMP_ppoll(struct pollfd *fds, nfds_t nfds, const struct timespec *tmo_p,
                  const sigset_t *sigmask) {
    assert(!"ppoll() unimp");
    return -ENOSYS;
}

ssize_t SYS_IMP_read(int fd, void *buf, size_t count) {
    assert(!"read() unimp");
    return -ENOSYS;
}

ssize_t SYS_IMP_readv(int fd, const struct iovec *iov, int iovcnt) {
    assert(!"readv() unimp");
    return -ENOSYS;
}

int SYS_IMP_futex(uint32_t *uaddr, int futex_op, uint32_t val, const struct timespec *timeout,
                  uint32_t *uaddr2, uint32_t val3) {
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

int SYS_IMP_rt_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact,
                         size_t sigsetsize) {
    // assert(!"rt_sigaction() unimp");
    // on assert() path
    return -ENOSYS;
}

int SYS_IMP_rt_sigprocmask(int how, const sigset_t *set, sigset_t *oldset, size_t sigsetsize) {
    // assert(!"rt_sigprocmask() unimp");
    // on assert() path
    return -ENOSYS;
}

int SYS_IMP_tkill(pid_t tid, int sig) {
    // assert(!"tkill() unimp");
    // on assert() path
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
