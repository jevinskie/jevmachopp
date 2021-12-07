#include <algorithm>
#include <cassert>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include <fmt/core.h>

#include <jevmachopp/RingBuffer.h>

using namespace std::string_literals;
using namespace std::literals;

constexpr std::size_t NUM_ELEM = 0x1000;

constexpr auto NUM_PUSH = (std::size_t)(NUM_ELEM * 0.3);
// constexpr auto NUM_PUSH = NUM_ELEM * 16.3;

constexpr auto EXPECTED_SUM = (std::size_t)((NUM_PUSH * (NUM_PUSH + 1)) / 2);

int main(void) {
    //    const auto nthread = std::thread::hardware_concurrency();
    const unsigned nthread = 2;
    assert(nthread >= 2);

    printf("NUM_ELEM: %zu NUM_PUSH: %zu EXPECTED_SUM: %zu\n", NUM_ELEM, NUM_PUSH, EXPECTED_SUM);

    auto rb = MultiConsRingBuffer<uint32_t, NUM_ELEM>{};

    std::thread producer{[&rb]() {
        for (std::size_t i = 1; i <= NUM_PUSH; ++i) {
            rb.push(i);
        }
        rb.finish();
        fprintf(stderr, "producer finished\n");
    }};

    std::thread consumers[nthread - 1];
    std::vector<uint32_t> results[nthread - 1];

    for (auto &v : results) {
        v.reserve(NUM_PUSH);
    }

    producer.join();

    for (auto i = 0u; i < nthread - 1; ++i) {
        consumers[i] = std::thread{[i, &rb, &results]() {
            fprintf(stderr, "consumer: %u\n", i);
            while (!rb.is_done()) {
                results[i].emplace_back(rb.pop());
            }
            while (!rb.empty()) {
                results[i].emplace_back(rb.pop());
            }
        }};
    }

    for (auto &c : consumers) {
        c.join();
    }

    std::size_t sz_sum = 0;
    std::size_t sum = 0;
    for (auto i = 0u; i < nthread - 1; ++i) {
        std::size_t res_sum = 0;
        for (const auto n : results[i]) {
            res_sum += n;
        }
        const auto sz = results[i].size();
        printf("thread # %u results sz: %zu sum: %zu\n", i, sz, res_sum);
        sum += res_sum;
        sz_sum += sz;
    }
    printf("sz_sum: %zu sum: %zu\n", sz_sum, sum);

    printf("rb idx rd: %zu wr: %zu\n", rb.rd_idx_raw.load(), rb.wr_idx_raw);

    return 0;
}
