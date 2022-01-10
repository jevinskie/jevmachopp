# Boost
add_subdirectory(callable_traits)

set(HAS_UBOOT_STUBS ON)
add_subdirectory(apfs-fuse-embedded)

add_subdirectory(arch-arm64/disassembler)

if (NOT CMAKE_CROSSCOMPILING)
set(BENCHMARK_ENABLE_TESTING false)
add_subdirectory(benchmark)

add_subdirectory(Catch2)
endif()

add_subdirectory(cxxopts)
add_subdirectory(enum.hpp)
add_subdirectory(fmt)

if (NOT CMAKE_CROSSCOMPILING)
add_subdirectory(nanobench)
endif()

set(BUILD_TESTING OFF)
set(ENABLE_BENCHMARKS OFF)
add_subdirectory(nanorange)

if (APPLE)
	add_subdirectory(perf-macos)
endif()

add_subdirectory(uleb128)

set(ENABLE_BENCHMARKS OFF)
add_subdirectory(visit)
