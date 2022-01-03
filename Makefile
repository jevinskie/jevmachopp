TARGETS := build/jevmachopp/libjevmachopp.a build/jevmachopp/apfs/libapfs.a

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

JEV_AR ?= aarch64-none-elf-gcc-ar
JEV_CC ?= aarch64-none-elf-gcc
JEV_CXX ?= aarch64-none-elf-g++
JEV_LD ?= aarch64-none-elf-ld

BOOST_DEFINE_FLAGS := -DBOOST_STATIC_STRING_STANDALONE=1 -DBOOST_NO_EXCEPTIONS=1
FMT_DEFINE_FLAGS := -DFMT_EXCEPTIONS=0 -DFMT_LOCALE -DFMT_USE_DOUBLE=0 -DFMT_USE_FLOAT=0 -DFMT_USE_INT128=0 -DFMT_USE_LONG_DOUBLE=0 -DFMT_USE_NONTYPE_TEMPLATE_PARAMETERS=1 -DFMT_USE_USER_DEFINED_LITERALS=1
FMT_DEFINE_FLAGS += -DUSE_FMT=0
NANO_DEFINE_FLAGS += -DNANORANGE_NO_STD_FORWARD_DECLARATIONS
DEFINE_FLAGS := -DM1N1=1 $(BOOST_DEFINE_FLAGS) $(FMT_DEFINE_FLAGS) $(NANO_DEFINE_FLAGS)
INCLUDE_FLAGS := -I $(ROOT_DIR)/include -I $(ROOT_DIR)/3rdparty/fmt/include -I $(ROOT_DIR)/3rdparty/hedley -I $(ROOT_DIR)/3rdparty/callable_traits/include -I $(ROOT_DIR)/3rdparty/static_string/include -I $(ROOT_DIR)/3rdparty/static_vector/include -I $(ROOT_DIR)/3rdparty/enum.hpp/headers -I $(ROOT_DIR)/3rdparty/nanorange/include -I $(ROOT_DIR)/3rdparty/uleb128/include -I $(ROOT_DIR)/3rdparty/visit/include

DEFINE_FLAGS += -DKZ_EXCEPTIONS=0
INCLUDE_FLAGS += -I $(ROOT_DIR)/3rdparty/apfs-fuse-embedded/3rdparty/expected/src

# C_CXX_FLAGS := -Os
C_CXX_FLAGS := $(C_CXX_FLAGS) -fno-rtti -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables
C_CXX_FLAGS += -Wno-unknown-pragmas
# C_CXX_FLAGS += $(C_CXX_FLAGS) -flto -fuse-linker-plugin -ffat-lto-objects
JEV_CFLAGS := $(CFLAGS)
JEV_CFLAGS := $(filter-out -ffreestanding,$(JEV_CFLAGS))
JEV_CFLAGS := $(filter-out -nostdinc,$(JEV_CFLAGS))
JEV_CFLAGS := $(filter-out -isystem $(shell $(CC) -print-file-name=include),$(JEV_CFLAGS))
JEV_CFLAGS := $(filter-out -isystem sysinc,$(JEV_CFLAGS))
JEV_CFLAGS := $(JEV_CFLAGS) $(C_CXX_FLAGS) -std=gnu11 $(DEFINE_FLAGS) $(INCLUDE_FLAGS)
JEV_CXXFLAGS := $(JEV_CFLAGS) $(CXXFLAGS) $(C_CXX_FLAGS) -std=gnu++2b $(DEFINE_FLAGS) $(INCLUDE_FLAGS)
JEV_CXXFLAGS := $(filter-out -Werror=strict-prototypes,$(JEV_CXXFLAGS))
JEV_CXXFLAGS := $(filter-out -Werror=implicit-function-declaration,$(JEV_CXXFLAGS))
JEV_CXXFLAGS := $(filter-out -Werror=implicit-int,$(JEV_CXXFLAGS))
JEV_CXXFLAGS := $(filter-out -std=gnu11,$(JEV_CXXFLAGS))
JEV_CXXFLAGS += -fconcepts-diagnostics-depth=6

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
	$(JEV_CXX) $(JEV_CXXFLAGS) -c -o $@ $^

build/jevmachopp/%.o: $(ROOT_DIR)/lib/jevmachopp/%.S
	@mkdir -p "$(dir $@)"
	$(JEV_CXX) $(JEV_CXXFLAGS) -c -o $@ $^

build/jevmachopp/libjevmachopp.a: $(LIBJEVMACHOPP_OBJS)
	@mkdir -p "$(dir $@)"
	$(JEV_AR) rc $@ $^

build/jevmachopp/apfs/%.o: $(ROOT_DIR)/3rdparty/apfs-fuse-embedded/ApfsLib/%.cpp
	@mkdir -p "$(dir $@)"
	$(JEV_CXX) $(JEV_CXXFLAGS) -D__LITTLE_ENDIAN__ -c -o $@ $^

build/jevmachopp/apfs/libapfs.a: $(LIBAPFSCXX_OBJS)
	@mkdir -p "$(dir $@)"
	$(JEV_AR) rc $@ $^
