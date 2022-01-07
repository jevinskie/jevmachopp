#pragma once

#include "stdint.h"
#include "stdio.h"

#include "sys/cdefs.h"

extern "C" {

#define fputc __uboot_fputc
#define putc __uboot_putc
#define puts __uboot_puts
#define fprintf __uboot_fprintf
#define fputs __uboot_fputs
#define fgetc __uboot_fgetc

#include <linux/compiler_attributes.h>

#include <linux/kernel.h>
#undef swap
#undef clamp

#undef fputc
#undef putc
#undef puts
#undef fprintf
#undef fputs
#undef fgetc


}
