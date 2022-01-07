#pragma once

#include "stdint.h"
#include "stdio.h"

#include "sys/cdefs.h"

extern "C" {

#define fputc __uboot_fputc

#include <linux/compiler_attributes.h>

#include <linux/kernel.h>
#undef swap
#undef clamp

#undef fputc


}
