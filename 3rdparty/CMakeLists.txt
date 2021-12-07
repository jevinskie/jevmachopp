# Boost
add_subdirectory(callable_traits)

add_subdirectory(arch-arm64/disassembler)

set(BENCHMARK_ENABLE_TESTING false)
add_subdirectory(benchmark)

add_subdirectory(Catch2)
add_subdirectory(cxxopts)
add_subdirectory(enum.hpp)
add_subdirectory(fmt)
add_subdirectory(nanobench)

option(BUILD_TESTING "Build testing." OFF)
option(ENABLE_BENCHMARKS "Enable benchmarks." OFF)
add_subdirectory(nanorange)

add_subdirectory(uleb128)

option(BENCHMARK_ENABLE_TESTING "Enable google benchmark testing." OFF)
add_subdirectory(visit)
