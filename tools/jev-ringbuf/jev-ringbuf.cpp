#include <algorithm>
#include <cassert>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include <benchmark/benchmark.h>
#include <fmt/core.h>
#include <nanorange/algorithm/is_sorted.hpp>
#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>

#include <jevmachopp/RingBuffer.h>

using namespace std::string_literals;
using namespace std::literals;

constexpr std::size_t NUM_ELEM = 0x1000;

template <typename T, std::size_t NumElem>
static void RingBuffer_single_producer_multi_consumer(MultiConsRingBuffer<T, NumElem> &rb,
                                                      const std::size_t NUM_PUSH,
                                                      std::thread *producer, std::thread *consumers,
                                                      std::vector<T> *results,
                                                      const unsigned int nthread) {
    assert(nthread >= 2);

    *producer = std::thread{[&rb, NUM_PUSH]() {
        for (std::size_t i = 1; i <= NUM_PUSH; ++i) {
            rb.push(i);
        }
        rb.finish();
    }};

    for (auto i = 0u; i < nthread - 1; ++i) {
        consumers[i] = std::thread{[i, &rb, &results]() {
            while (!(rb.is_done() && rb.empty())) {
                const auto val = rb.pop();
                if (val) {
                    results[i].emplace_back(*val);
                }
            }
        }};
    }

    producer->join();

    for (auto i = 0u; i < nthread - 1; ++i) {
        consumers[i].join();
    }
}

static void BM_RingBuffer(benchmark::State &state) {
    const auto NUM_PUSH = (std::size_t)(NUM_ELEM * 16.3);
    const auto EXPECTED_SUM = (std::size_t)((NUM_PUSH * (NUM_PUSH + 1)) / 2);
    const auto nthread = std::thread::hardware_concurrency();
    auto rb = MultiConsRingBuffer<uint32_t, NUM_ELEM>{};
    std::thread producer;
    std::thread consumers[nthread - 1];
    std::vector<uint32_t> results[nthread - 1];
    for (auto &v : results) {
        v.reserve(NUM_PUSH);
    }

    int num_run = 0;

    for (auto _ : state) {
        rb.clear();
        for (auto &v : results) {
            v.clear();
        }

        RingBuffer_single_producer_multi_consumer<uint32_t, NUM_ELEM>(rb, NUM_PUSH, &producer,
                                                                      consumers, results, nthread);

        std::size_t sz_sum = 0;
        std::size_t sum = 0;
        for (auto i = 0u; i < nthread - 1; ++i) {
            std::size_t res_sum = 0;
            for (const auto n : results[i]) {
                res_sum += n;
            }
            const auto sz = results[i].size();
            sum += res_sum;
            sz_sum += sz;
            for (std::size_t j = 1, je = results[i].size(); j < je; ++j) {
                const auto prev = results[i][j - 1];
                const auto cur = results[i][j];
                if (cur <= prev) {
                    fprintf(stderr,
                            "ordering violation thread %u idx: %zu prev: %u cur: %u diff: %u\n", i,
                            j, prev, cur, prev - cur);
                }
            }
            //            assert(ranges::is_sorted(results[i]));
        }

        assert(sz_sum == NUM_PUSH);
        assert(sum == EXPECTED_SUM);
        ++num_run;
    }
}

BENCHMARK(BM_RingBuffer);

// BENCHMARK_MAIN();

static void BM_RingBuffer_simple() {
    const auto NUM_PUSH = (std::size_t)(NUM_ELEM * 16.3);
    const auto EXPECTED_SUM = (std::size_t)((NUM_PUSH * (NUM_PUSH + 1)) / 2);
    const auto nthread = std::thread::hardware_concurrency();
    auto rb = MultiConsRingBuffer<uint32_t, NUM_ELEM>{};
    std::thread producer;
    std::thread consumers[nthread - 1];
    std::vector<uint32_t> results[nthread - 1];
    for (auto &v : results) {
        v.reserve(NUM_PUSH);
    }

    int num_run = 0;

    ankerl::nanobench::Bench().run("SPMC uint32_t", [&] {
        rb.clear();
        for (auto &v : results) {
            v.clear();
        }

        RingBuffer_single_producer_multi_consumer<uint32_t, NUM_ELEM>(rb, NUM_PUSH, &producer,
                                                                      consumers, results, nthread);

        std::size_t sz_sum = 0;
        std::size_t sum = 0;
        for (auto i = 0u; i < nthread - 1; ++i) {
            std::size_t res_sum = 0;
            for (const auto n : results[i]) {
                res_sum += n;
            }
            const auto sz = results[i].size();
            sum += res_sum;
            sz_sum += sz;
            for (std::size_t j = 1, je = results[i].size(); j < je; ++j) {
                const auto prev = results[i][j - 1];
                const auto cur = results[i][j];
                if (cur <= prev) {
                    fprintf(stderr,
                            "ordering violation thread %u idx: %zu prev: %u cur: %u diff: %u\n", i,
                            j, prev, cur, prev - cur);
                }
            }
            //            assert(ranges::is_sorted(results[i]));
        }

        assert(sz_sum == NUM_PUSH);
        assert(sum == EXPECTED_SUM);
        ++num_run;
    });
}

inline uint64_t arm_v8_get_timing(void) {
    return __builtin_arm_rsr64("CNTPCT_EL0");
}

int main() {
    const auto start_cnt = arm_v8_get_timing();
    printf("start count: %llu\n", start_cnt);
    BM_RingBuffer_simple();
    const auto end_cnt = arm_v8_get_timing();
    printf("end count: %llu\n", end_cnt);
    printf("num cyc: %llu\n", end_cnt - start_cnt);
}
