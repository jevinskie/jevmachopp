#include <algorithm>
#include <cassert>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <thread>

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

    std::thread producer{[&]() {
        for (std::size_t i = 1; i <= NUM_PUSH; ++i) {
            rb.push(i);
        }
    }};

    std::thread consumers[nthread - 1];
    for (std::thread *t = &consumers[0], *te = &consumers[nthread]; t != te; ++t) {
        *t = std::thread{[]() {}};
    }

    producer.join();

    for (auto &t : consumers) {
        t.join();
    }

    return 0;
}
