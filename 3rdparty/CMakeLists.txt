# Boost
add_subdirectory(callable_traits)

add_subdirectory(apfs-fuse-embedded)

add_subdirectory(arch-arm64/disassembler)

set(BENCHMARK_ENABLE_TESTING false)
add_subdirectory(benchmark)

add_subdirectory(Catch2)
add_subdirectory(cxxopts)
add_subdirectory(enum.hpp)
add_subdirectory(fmt)
add_subdirectory(nanobench)

set(BUILD_TESTING OFF)
set(ENABLE_BENCHMARKS OFF)
add_subdirectory(nanorange)

if (APPLE)
	add_subdirectory(perf-macos)
endif()

add_subdirectory(uleb128)

option(BENCHMARK_ENABLE_TESTING "Enable google benchmark testing." OFF)
add_subdirectory(visit)
