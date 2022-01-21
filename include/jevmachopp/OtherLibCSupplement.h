#pragma once

#ifdef __cplusplus
extern "C" {
#endif

ssize_t olibc2uboot_write(int fd, const void *buf, size_t count);

int olibc2uboot_printf(const char * __restrict fmt, ...)
        __attribute__((__format__(__printf__, 1, 2)));

void olibc2uboot_panic(const char *fmt, ...)
        __attribute__((__format__(__printf__, 1, 2), noreturn));

#ifdef __cplusplus
} // extern "C"
#endif
