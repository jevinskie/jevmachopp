# Boost
add_subdirectory(callable_traits)

set(BN_API_PATH ${CMAKE_CURRENT_SOURCE_DIR}/binaryninja-api)
add_subdirectory(arch-arm64)

add_subdirectory(Catch2)
add_subdirectory(cxxopts)
add_subdirectory(enum.hpp)
add_subdirectory(fmt)

option(BUILD_TESTING "Build testing." OFF)
option(ENABLE_BENCHMARKS "Enable benchmarks." OFF)
add_subdirectory(nanorange)

option(BENCHMARK_ENABLE_TESTING "Enable google benchmark testing." OFF)
add_subdirectory(visit)
