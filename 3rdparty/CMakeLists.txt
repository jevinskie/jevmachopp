# Boost
add_subdirectory(callable_traits)

add_subdirectory(cxxopts)
add_subdirectory(enum.hpp)
add_subdirectory(fmt)

option(BUILD_TESTING "Build testing." OFF)
option(ENABLE_BENCHMARKS "Enable benchmarks." OFF)
add_subdirectory(nanorange)

option(BENCHMARK_ENABLE_TESTING "Enable google benchmark testing." OFF)
add_subdirectory(visit)
