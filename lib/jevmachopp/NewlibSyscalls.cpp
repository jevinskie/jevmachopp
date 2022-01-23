#if __has_include(<_newlib_version.h>)

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#include "jevmachopp/OtherLibCSupplement.h"

#define printf olibc2uboot_printf

extern "C" {

// huh?
void *__dso_handle[32];

// huh?
int *__errno(void) {
    write(2, __FUNCTION__, strlen(__FUNCTION__));
    write(2, "\n", 1);
    return &errno;
}

__attribute__((noreturn)) void _exit(int status) {
    write(2, __FUNCTION__, strlen(__FUNCTION__));
    write(2, "\n", 1);
    printf("_exit(%d)\n", status);
    assert(!"exit()");
}

int _stat(const char *__restrict path, struct stat *__restrict st) {
    write(2, __FUNCTION__, strlen(__FUNCTION__));
    write(2, "\n", 1);
    printf("_stat(\"%s\", %p) called\n", path, (void *)st);
    assert(!"_stat()");
    return -ENOSYS;
}

int _unlink(const char *path) {
    write(2, __FUNCTION__, strlen(__FUNCTION__));
    write(2, "\n", 1);
    printf("_unlink(\"%s\") called\n", path);
    assert(!"_unlink()");
    return -ENOSYS;
}

int _open(const char *path, int flag, int m) {
    write(2, __FUNCTION__, strlen(__FUNCTION__));
    write(2, "\n", 1);
    printf("_open(\"%s\", %d, %d) called\n", path, flag, m);
    assert(!"_open()");
    return -ENOSYS;
}

int board_fit_config_name_match(const char *name) {
    write(2, __FUNCTION__, strlen(__FUNCTION__));
    write(2, "\n", 1);
    printf("board_fit_config_name_match(\"%s\") called\n", name);
    assert(!"board_fit_config_name_match()");
    return -1;
}

void board_fit_image_post_process(void **p_image, size_t *p_size) {
    write(2, __FUNCTION__, strlen(__FUNCTION__));
    write(2, "\n", 1);
    printf("board_fit_image_post_process(%p, %p) called\n", p_image, (void *)p_size);
    assert(!"board_fit_image_post_process()");
}

__attribute__((nothrow)) void *_malloc_r(struct _reent *r, size_t sz) {
    write(2, __FUNCTION__, strlen(__FUNCTION__));
    write(2, "\n", 1);
    return malloc(sz);
}

__attribute__((nothrow)) void *_realloc_r(struct _reent *r, void *ptr, size_t sz) {
    write(2, __FUNCTION__, strlen(__FUNCTION__));
    write(2, "\n", 1);
    return realloc(ptr, sz);
}

__attribute__((nothrow)) void *_calloc_r(struct _reent *r, size_t num, size_t sz) {
    write(2, __FUNCTION__, strlen(__FUNCTION__));
    write(2, "\n", 1);
    return calloc(num, sz);
}

__attribute__((nothrow)) void _free_r(struct _reent *r, void *ptr) {
    write(2, __FUNCTION__, strlen(__FUNCTION__));
    write(2, "\n", 1);
    return free(ptr);
}

int _isatty(int fd) {
    write(2, __FUNCTION__, strlen(__FUNCTION__));
    write(2, "\n", 1);
    // return fd == STDIN_FILENO;
    return 0;
}

__attribute__((noreturn)) void abort(void) {
    write(2, __FUNCTION__, strlen(__FUNCTION__));
    write(2, "\n", 1);
    newlib2uboot_panic("abort()! caller: %p\n", __builtin_return_address(0));
}

int _close_r(struct _reent *r, int fd) {
    // write(2, __FUNCTION__, strlen( __FUNCTION__));
    // write(2, "\n", 1);
    printf("_close_r(%p, %d)\n", r, fd);
    // assert(!"_close_r()");
    return 0;
}

ssize_t _write_r(struct _reent *r, int fd, const void *buf, size_t sz) {
    return olibc2uboot_write(fd, buf, sz);
}

ssize_t _read_r(struct _reent *r, int fd, void *buf, size_t sz) {
    write(2, __FUNCTION__, strlen(__FUNCTION__));
    write(2, "\n", 1);
    printf("_read_r(%p, %d, %p, %p)\n", r, fd, buf, (void *)sz);
    assert(!"_read_r()");
    return -1;
}

int _fstat_r(struct _reent *r, int fd, struct stat *st) {
    write(2, __FUNCTION__, strlen(__FUNCTION__));
    write(2, "\n", 1);
    // printf("_fstat_r(%p, %d, %p)\n", r, fd, (void*)st);
    // assert(!"_fstat_r()");
    return -1;
}

off_t _lseek_r(struct _reent *r, int fd, off_t off, int whence) {
    write(2, __FUNCTION__, strlen(__FUNCTION__));
    write(2, "\n", 1);
    printf("_lseek_r(%p, %d, %p, %p)\n", r, fd, (void *)off, (void *)whence);
    assert(!"_lseek_r()");
    return -1;
}

pid_t _getpid() {
    // write(2, __FUNCTION__, strlen( __FUNCTION__));
    // write(2, "\n", 1);
    // printf("_getpid()\n");
    // assert(!"_getpid()");
    return 1;
}

int _kill(pid_t pid, int signal) {
    // write(2, __FUNCTION__, strlen( __FUNCTION__));
    // write(2, "\n", 1);
    // printf("_kill(%d, %d)\n", (int)pid_t, signal);
    // assert(!"_kill()");
    return 0; // yeah sure you killed 'em, tiger
}

} // extern "C"

#endif
