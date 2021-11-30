TARGETS := libjevmachopp.a

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

AR ?= aarch64-none-elf-gcc-ar
CC ?= aarch64-none-elf-gcc
CXX ?= aarch64-none-elf-g++
LD ?= aarch64-none-elf-ld

BOOST_DEFINE_FLAGS := -DBOOST_STATIC_STRING_STANDALONE=1 -DBOOST_NO_EXCEPTIONS=1
FMT_DEFINE_FLAGS := -DFMT_EXCEPTIONS=0 -DFMT_LOCALE -DFMT_USE_DOUBLE=0 -DFMT_USE_FLOAT=0 -DFMT_USE_INT128=0 -DFMT_USE_LONG_DOUBLE=0 -DFMT_USE_NONTYPE_TEMPLATE_PARAMETERS=1 -DFMT_USE_USER_DEFINED_LITERALS=1
FMT_DEFINE_FLAGS += -DUSE_FMT=0
DEFINE_FLAGS := $(BOOST_DEFINE_FLAGS) $(FMT_DEFINE_FLAGS)
INCLUDE_FLAGS := -I $(ROOT_DIR)/include -I $(ROOT_DIR)/3rdparty/fmt/include -I $(ROOT_DIR)/3rdparty/hedley -I $(ROOT_DIR)/3rdparty/callable_traits/include -I $(ROOT_DIR)/3rdparty/static_string/include -I $(ROOT_DIR)/3rdparty/static_vector/include -I $(ROOT_DIR)/3rdparty/enum.hpp/headers -I $(ROOT_DIR)/3rdparty/nanorange/include -I $(ROOT_DIR)/3rdparty/uleb128/include -I $(ROOT_DIR)/3rdparty/visit/include

# C_CXX_FLAGS := -Os
C_CXX_FLAGS := $(C_CXX_FLAGS) -fno-rtti -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables
C_CXX_FLAGS += -Wno-unknown-pragmas
# C_CXX_FLAGS += $(C_CXX_FLAGS) -flto -fuse-linker-plugin -ffat-lto-objects
JEV_CFLAGS := $(CFLAGS)
JEV_CFLAGS := $(filter-out -ffreestanding,$(JEV_CFLAGS))
JEV_CFLAGS := $(filter-out -nostdinc,$(JEV_CFLAGS))
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

all: $(TARGETS)

.PHONY: clean

clean:
	rm -f $(TARGETS) $(LIBJEVMACHOPP_OBJS)

%.o: $(ROOT_DIR)/lib/jevmachopp/%.cpp
	$(CXX) $(JEV_CXXFLAGS) -c -o $@ $^

%.o: $(ROOT_DIR)/lib/jevmachopp/%.S
	$(CXX) $(JEV_CXXFLAGS) -c -o $@ $^

libjevmachopp.a: $(LIBJEVMACHOPP_OBJS)
	$(AR) rc $@ $^
