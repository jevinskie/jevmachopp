#if __has_include(<_newlib_version.h>)

#include <cassert>
#include <cerrno>
#include <cstdlib>
#include <cstdio>

#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/unistd.h>

extern "C" {

// huh?
void *__dso_handle[32];

// huh?
int *__errno(void) {
    return &errno;
}

int board_fit_config_name_match(const char *name) {
    printf("board_fit_config_name_match(\"%s\") called\n", name);
    assert(!"board_fit_config_name_match()");
    return -1;
}

void board_fit_image_post_process(void **p_image, size_t *p_size) {
    printf("board_fit_image_post_process(%p, %p) called\n", p_image, (void*)p_size);
    assert(!"board_fit_image_post_process()");
}

__attribute__((nothrow))
void *_malloc_r(struct _reent *r, size_t sz) {
    return malloc(sz);
}

__attribute__((nothrow))
void *_realloc_r(struct _reent *r, void *ptr, size_t sz) {
    return realloc(ptr, sz);
}

__attribute__((nothrow))
void *_calloc_r(struct _reent *r, size_t num, size_t sz) {
    return calloc(num, sz);
}

__attribute__((nothrow))
void _free_r(struct _reent *r, void *ptr) {
    return free(ptr);
}

int _isatty(int fd) {
    // return fd == STDIN_FILENO;
    return 0;
}

__attribute__((noreturn))
void abort(void) {
    assert(!"abort()");
}

void _close_r(struct _reent *r, int fd) {
    printf("_close_r(%p, %d)\n", r, fd);
    assert(!"_close_r()");
}

ssize_t _write_r(struct _reent *r, int fd, const void *buf, size_t sz) {
    // printf("_write_r(%p, %d, %p, %p)\n", r, fd, buf, (void*)sz);
    // assert(!"_write_r()");
    // return -1;
    return write(fd, buf, sz);
}

ssize_t _read_r(struct _reent *r, int fd, void *buf, size_t sz) {
    printf("_read_r(%p, %d, %p, %p)\n", r, fd, buf, (void*)sz);
    assert(!"_read_r()");
    return -1;
}


int _fstat_r(struct _reent *r, int fd, struct stat *st) {
    printf("_fstat_r(%p, %d, %p)\n", r, fd, (void*)st);
    assert(!"_fstat_r()");
    return -1;
}

off_t _lseek_r(struct _reent *r, int fd, off_t off, int whence) {
    printf("_lseek_r(%p, %d, %p, %p)\n", r, fd, (void*)off, (void*)whence);
    assert(!"_lseek_r()");
    return -1;
}

} // extern "C"

#endif
