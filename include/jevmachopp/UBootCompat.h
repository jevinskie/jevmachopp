#pragma once

#include "sys/cdefs.h"
#undef __THROW
#define __THROW

#include "assert.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"


extern "C" {

#define fputc __uboot_fputc
#define putc __uboot_putc
#define puts __uboot_puts
#define fprintf __uboot_fprintf
#define fputs __uboot_fputs
#define fgetc __uboot_fgetc

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

#define __REAL_ASSERT assert
#undef assert
#include <log.h>
#undef assert
#define assert __REAL_ASSERT
#undef __REAL_ASSERT



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


}
