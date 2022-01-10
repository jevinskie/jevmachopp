TARGETS := \
	build/jevmachopp/libjevmachopp.a \
	build/jevmachopp/apfs/libapfs.a \
	build/jevmachopp/apfs/miniz/libminiz.a \
	build/jevmachopp/apfs/lzfse/liblzfse.a \
	build/jevmachopp/apfs/bzip2/libbz2.a

ROOT_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

ifeq ($(ROOT_DIR), $(PWD))
	$(error Must build out of tree!)
endif

LIBJEVMACHOPP_CXX_SRCS := $(wildcard $(ROOT_DIR)/lib/jevmachopp/*.cpp)
LIBJEVMACHOPP_CXX_OBJS := $(notdir $(LIBJEVMACHOPP_CXX_SRCS:.cpp=.o))

LIBJEVMACHOPP_ASM_SRCS := $(wildcard $(ROOT_DIR)/lib/jevmachopp/*.S)
LIBJEVMACHOPP_ASM_OBJS := $(notdir $(LIBJEVMACHOPP_ASM_SRCS:.S=.o))

LIBJEVMACHOPP_OBJS := $(LIBJEVMACHOPP_CXX_OBJS) $(LIBJEVMACHOPP_ASM_OBJS)
# no slurp/mmap on baremetal
LIBJEVMACHOPP_OBJS := $(filter-out Slurp.o,$(LIBJEVMACHOPP_OBJS))
LIBJEVMACHOPP_OBJS := $(filter-out SearchFS.o,$(LIBJEVMACHOPP_OBJS))
LIBJEVMACHOPP_OBJS := $(filter-out PlatformizeHelper.o,$(LIBJEVMACHOPP_OBJS))
LIBJEVMACHOPP_OBJS := $(addprefix build/jevmachopp/,$(LIBJEVMACHOPP_OBJS))

LIBAPFSCXX_SRCS := $(wildcard $(ROOT_DIR)/3rdparty/apfs-fuse-embedded/ApfsLib/*.cpp)
LIBAPFSCXX_OBJS := $(notdir $(LIBAPFSCXX_SRCS:.cpp=.o))
LIBAPFSCXX_OBJS := $(filter-out DeviceVDI.o,$(LIBAPFSCXX_OBJS))
LIBAPFSCXX_OBJS := $(addprefix build/jevmachopp/apfs/,$(LIBAPFSCXX_OBJS))

LIBMINIZC_SRCS := $(wildcard $(ROOT_DIR)/3rdparty/apfs-fuse-embedded/3rdparty/miniz/*.c)
LIBMINIZC_OBJS := $(notdir $(LIBMINIZC_SRCS:.c=.o))
LIBMINIZC_OBJS := $(addprefix build/jevmachopp/apfs/miniz/,$(LIBMINIZC_OBJS))

LIBLZFSEC_SRCS := $(wildcard $(ROOT_DIR)/3rdparty/apfs-fuse-embedded/3rdparty/lzfse/src/*.c)
LIBLZFSEC_OBJS := $(notdir $(LIBLZFSEC_SRCS:.c=.o))
LIBLZFSEC_OBJS := $(filter-out lzfse_main.o,$(LIBLZFSEC_OBJS))
LIBLZFSEC_OBJS := $(addprefix build/jevmachopp/apfs/lzfse/,$(LIBLZFSEC_OBJS))

LIBBZ2C_SRCS := blocksort.c huffman.c crctable.c randtable.c compress.c decompress.c bzlib.c
LIBBZ2C_SRCS := $(addprefix $(ROOT_DIR)/3rdparty/apfs-fuse-embedded/3rdparty/bzip2/,$(LIBBZ2C_SRCS))
LIBBZ2C_OBJS := $(notdir $(LIBBZ2C_SRCS:.c=.o))
LIBBZ2C_OBJS := $(addprefix build/jevmachopp/apfs/bzip2/,$(LIBBZ2C_OBJS))


ifndef UBOOTRELEASE
JEV_AR ?= aarch64-none-elf-gcc-ar
JEV_CC ?= aarch64-none-elf-gcc
JEV_CXX ?= aarch64-none-elf-g++
JEV_LD ?= aarch64-none-elf-ld
else
JEV_AR := $(AR)
JEV_CC := $(CC)
JEV_CXX := $(CXX)
JEV_LD := $(LD)
endif

BOOST_DEFINE_FLAGS := -DBOOST_STATIC_STRING_STANDALONE=1 -DBOOST_NO_EXCEPTIONS=1
FMT_DEFINE_FLAGS := -DFMT_EXCEPTIONS=0 -DFMT_LOCALE -DFMT_USE_DOUBLE=0 -DFMT_USE_FLOAT=0 -DFMT_USE_INT128=0 -DFMT_USE_LONG_DOUBLE=0 -DFMT_USE_NONTYPE_TEMPLATE_PARAMETERS=1 -DFMT_USE_USER_DEFINED_LITERALS=1
FMT_DEFINE_FLAGS += -DUSE_FMT=0
NANO_DEFINE_FLAGS += -DNANORANGE_NO_STD_FORWARD_DECLARATIONS
NEWLIB_LOCALE_HELL_DEFINE_FLAGS += -D_POSIX_C_SOURCE=200809 -D_XOPEN_SOURCE=700 -D_BSD_SOURCE -D_DEFAULT_SOURCE -D_GNU_SOURCE
DEFINE_FLAGS := -DJEV_BAREMETAL $(BOOST_DEFINE_FLAGS) $(FMT_DEFINE_FLAGS) $(NANO_DEFINE_FLAGS) $(NEWLIB_LOCALE_HELL_DEFINE_FLAGS)
INCLUDE_FLAGS := \
	-I $(ROOT_DIR)/include \
	-I $(ROOT_DIR)/3rdparty/fmt/include \
	-I $(ROOT_DIR)/3rdparty/callable_traits/include \
	-I $(ROOT_DIR)/3rdparty/static_string/include \
	-I $(ROOT_DIR)/3rdparty/static_vector/include \
	-I $(ROOT_DIR)/3rdparty/enum.hpp/headers \
	-I $(ROOT_DIR)/3rdparty/nanorange/include \
	-I $(ROOT_DIR)/3rdparty/uleb128/include \
	-I $(ROOT_DIR)/3rdparty/visit/include

DEFINE_FLAGS += -DKZ_EXCEPTIONS=0

ifdef UBOOTRELEASE
DEFINE_FLAGS += -D__UBOOT__
endif

ifeq ($(NAME),m1n1)
DEFINE_FLAGS += -DM1N1
endif()

INCLUDE_FLAGS += \
	-I $(ROOT_DIR)/3rdparty/apfs-fuse-embedded \
	-I $(ROOT_DIR)/3rdparty/apfs-fuse-embedded/3rdparty/expected/src \
	-I $(ROOT_DIR)/3rdparty/apfs-fuse-embedded/3rdparty/miniz \
	-I build/jevmachopp/apfs/miniz \
	-I $(ROOT_DIR)/3rdparty/apfs-fuse-embedded/3rdparty/lzfse/src \
	-I $(ROOT_DIR)/3rdparty/apfs-fuse-embedded/3rdparty/bzip2 \
	-I build/jevmachopp/apfs/bzip2

ifdef UBOOTRELEASE
# INCLUDE_FLAGS += \
# 	-I $(ROOT_DIR)/../../include
endif

C_CXX_FLAGS := -g -O0
# C_CXX_FLAGS += -Os
C_CXX_FLAGS += $(C_CXX_FLAGS) -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables
C_CXX_FLAGS += -Wno-unknown-pragmas

# C_CXX_FLAGS += $(C_CXX_FLAGS) -flto -fuse-linker-plugin -ffat-lto-objects
JEV_CFLAGS := $(CFLAGS)
JEV_CFLAGS := $(filter-out -ffreestanding,$(JEV_CFLAGS))
JEV_CFLAGS := $(filter-out -nostdinc,$(JEV_CFLAGS))
JEV_CFLAGS := $(filter-out -isystem $(shell $(CC) -print-file-name=include),$(JEV_CFLAGS))
JEV_CFLAGS := $(filter-out -isystem sysinc,$(JEV_CFLAGS))
JEV_CFLAGS := $(filter-out -Werror=strict-prototypes,$(JEV_CFLAGS))
JEV_CFLAGS := $(JEV_CFLAGS) $(C_CXX_FLAGS) -std=gnu11 $(DEFINE_FLAGS) $(INCLUDE_FLAGS)
JEV_CXXFLAGS := $(JEV_CFLAGS) $(CXXFLAGS) $(C_CXX_FLAGS) -std=gnu++2b -fno-rtti $(DEFINE_FLAGS) $(INCLUDE_FLAGS)
JEV_CXXFLAGS := $(filter-out -Werror=strict-prototypes,$(JEV_CXXFLAGS))
JEV_CXXFLAGS := $(filter-out -Werror=implicit-function-declaration,$(JEV_CXXFLAGS))
JEV_CXXFLAGS := $(filter-out -Werror=implicit-int,$(JEV_CXXFLAGS))
JEV_CXXFLAGS := $(filter-out -std=gnu11,$(JEV_CXXFLAGS))
ifndef UBOOTRELEASE
JEV_CXXFLAGS += -fconcepts-diagnostics-depth=6
else ifeq ($(cc-name),gcc)
JEV_CXXFLAGS += -fconcepts-diagnostics-depth=6
endif

JEV_MINIZ_CFLAGS := $(JEV_CFLAGS) -DMINIZ_NO_TIME
JEV_BZ2_CFLAGS := $(JEV_CFLAGS) -DBZ2_DISABLE_FP -DBZ_DEBUG=0

# make print-LIBJEVMACHOPP_OBJS
print-%:
	@echo $* = $($*)

all-default: $(TARGETS)

.PHONY: clean-default

clean-default:
	rm -rf build/*

%: %-default
	@true

build/jevmachopp/%.o: $(ROOT_DIR)/lib/jevmachopp/%.cpp
	@mkdir -p "$(dir $@)"
	$(JEV_CXX) $(JEV_CXXFLAGS) -c -o $@ $<

build/jevmachopp/%.o: $(ROOT_DIR)/lib/jevmachopp/%.S
	@mkdir -p "$(dir $@)"
	$(JEV_CXX) $(JEV_CXXFLAGS) -c -o $@ $<

build/jevmachopp/libjevmachopp.a: $(LIBJEVMACHOPP_OBJS)
	@mkdir -p "$(dir $@)"
	$(JEV_AR) rc $@ $^

build/jevmachopp/apfs/%.o: $(ROOT_DIR)/3rdparty/apfs-fuse-embedded/ApfsLib/%.cpp build/jevmachopp/apfs/miniz/miniz_export.h build/jevmachopp/apfs/bzip2/bz_version.h
	@mkdir -p "$(dir $@)"
	$(JEV_CXX) $(JEV_CXXFLAGS) -D__LITTLE_ENDIAN__ -c -o $@ $<

build/jevmachopp/apfs/libapfs.a: $(LIBAPFSCXX_OBJS)
	@mkdir -p "$(dir $@)"
	$(JEV_AR) rc $@ $^

build/jevmachopp/apfs/miniz/miniz_export.h:
	@mkdir -p "$(dir $@)"
	echo "#ifndef MINIZ_EXPORT\n#define MINIZ_EXPORT\n#endif" > $@

build/jevmachopp/apfs/miniz/%.o: $(ROOT_DIR)/3rdparty/apfs-fuse-embedded/3rdparty/miniz/%.c build/jevmachopp/apfs/miniz/miniz_export.h
	@mkdir -p "$(dir $@)"
	$(JEV_CC) $(JEV_MINIZ_CFLAGS) -c -o $@ $<

build/jevmachopp/apfs/miniz/libminiz.a: $(LIBMINIZC_OBJS)
	@mkdir -p "$(dir $@)"
	$(JEV_AR) rc $@ $^

build/jevmachopp/apfs/lzfse/%.o: $(ROOT_DIR)/3rdparty/apfs-fuse-embedded/3rdparty/lzfse/src/%.c
	@mkdir -p "$(dir $@)"
	$(JEV_CC) $(JEV_CFLAGS) -c -o $@ $<

build/jevmachopp/apfs/lzfse/liblzfse.a: $(LIBLZFSEC_OBJS)
	@mkdir -p "$(dir $@)"
	$(JEV_AR) rc $@ $^

build/jevmachopp/apfs/bzip2/bz_version.h:
	@mkdir -p "$(dir $@)"
	echo "#define BZ_VERSION \"jev-embedded-no-fp\"" > $@

build/jevmachopp/apfs/bzip2/%.o: $(ROOT_DIR)/3rdparty/apfs-fuse-embedded/3rdparty/bzip2/%.c build/jevmachopp/apfs/bzip2/bz_version.h
	@mkdir -p "$(dir $@)"
	$(JEV_CC) $(JEV_BZ2_CFLAGS) -c -o $@ $<

build/jevmachopp/apfs/bzip2/libbz2.a: $(LIBBZ2C_OBJS)
	@mkdir -p "$(dir $@)"
	$(JEV_AR) rc $@ $^
