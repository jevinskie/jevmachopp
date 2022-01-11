#pragma once

#error Don't includ me

#include "sys/cdefs.h"
#undef __THROW
#define __THROW

#include "assert.h"
#include "ctype.h"
typedef __intmax_t intmax_t;
typedef __uintmax_t uintmax_t;
// typedef unsigned long ulong;
#include "inttypes.h"
#include "stddef.h"
#include "stdint.h"
// typedef unsigned long uint64_t;
#include "stdio.h"
#include "stdlib.h"
// #include "string.h"
#include "time.h"
#include "malloc.h"


typedef unsigned long ulong;

extern "C" {

#define fputc __uboot_fputc
#define putc __uboot_putc
#define puts __uboot_puts
#define fprintf __uboot_fprintf
#define fputs __uboot_fputs
#define fgetc __uboot_fgetc
#define printf __uboot_printf

#undef stdin
#undef stdout
#undef stderr

#include <linux/compiler_attributes.h>

#undef INT_MAX
#undef UINT32_MAX
#undef UINT64_MAX
#undef INT32_MAX
#undef INT64_MAX
#undef LONG_MAX
#undef LONG_LONG_MAX

#include <linux/compiler_types.h>

#undef inline


#include <log.h>


#include <linux/kernel.h>
#undef swap
#undef clamp
#undef INT_MAX
#define INT_MAX __INT_MAX__
#undef UINT32_MAX
#define UINT32_MAX __UINT32_MAX__
#undef UINT64_MAX
#define UINT64_MAX __UINT64_MAX__
#undef INT32_MAX
#define INT32_MAX __INT32_MAX__
#undef INT64_MAX
#define INT64_MAX __INT64_MAX__
#undef LONG_MAX
#define LONG_MAX __LONG_MAX__
#undef LONG_LONG_MAX
#define LONG_LONG_MAX __LONG_LONG_MAX__

#undef fputc
#undef putc
#undef puts
#undef fprintf
#undef fputs
#undef fgetc
// #undef printf
// #define printf(fmt, ...) fprintf(stdout, fmt, __VA_ARGS__)
// #define PRINT(fmt, args...) printf(fmt, args);
// #define LOG(args...) ExtendLog(__FILE_NAME__, __LINE__, __PRETTY_FUNCTION__, args);
// extern void ExtendLog(const char *file, int lineNumber, const char *functionName, NSString *format, ...) NS_FORMAT_FUNCTION(4,5);
#define printf fprintf(1, fmt, args...) __attribute__((format(2,3)))

#include <string.h>

}

extern "C" __weak inline char *strerror(int errn) {
	assert(!"strerror called");
	return nullptr;
}

extern "C" __weak inline int strcoll(const char *a, const char *b) {
	assert(!"strcoll called");
	return 0;
}

extern "C" __weak inline size_t strxfrm(char *dst, const char *src, size_t num) {
	assert(!"strcoll called");
	return 0;
}
