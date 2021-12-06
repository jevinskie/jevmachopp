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

constexpr auto NUM_PUSH = NUM_ELEM * 0.3;
// constexpr auto NUM_PUSH = NUM_ELEM * 16.3;

constexpr auto EXPECTED_SUM = (NUM_PUSH * (NUM_PUSH + 1)) / 2;

int main(void) {
    const auto nthread = std::thread::hardware_concurrency();
    assert(nthread >= 3);

    auto rb = MultiConsRingBuffer<uint32_t, NUM_ELEM>{};

    std::thread producer{[&rb]() {
        for (std::size_t i = 1; i <= NUM_PUSH; ++i) {
            rb.push(i);
        }
        rb.finish();
    }};

    std::thread consumers[nthread - 1];
    std::vector<uint32_t> results[nthread - 1];

    for (auto i = 0u; i < nthread - 1; ++i) {
        consumers[i] = std::thread{[i, &rb]() {
            fprintf(stderr, "consumer: %u\n", i);
            while (!rb.is_done() || rb.size()) {
                const uint32_t v = rb.pop();
            }
        }};
    }

    producer.join();
    for (auto &c : consumers) {
        c.join();
    }

    return 0;
}
