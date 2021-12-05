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

int main(void) {
    auto rb = MultiConsRingBuffer<std::string, NUM_ELEM>{};

    rb.push("one"s);
    rb.push("two"s);

    auto p0_ready_sem = std::binary_semaphore{};
    //    p0_ready_sem.acquire();
    auto p0_go_sem = std::binary_semaphore{};
    //    p0_go_sem.acquire();

    std::thread p0{[&]() {
        const auto p0_res = rb.pop(&p0_ready_sem, &p0_go_sem);
        fprintf(stderr, "p0_res: %s\n", p0_res.data());
    }};

    fprintf(stderr, "main releasing p0_ready_sem\n");
    p0_ready_sem.release();
    fprintf(stderr, "main reacquiring p0_ready_sem\n");
    p0_ready_sem.acquire();
    fprintf(stderr, "main did reacquire p0_ready_sem\n");

    const auto pop0 = rb.pop();
    assert(pop0 == "one"sv);

    p0_go_sem.release();

    p0.join();

    const auto pop1 = rb.pop();
    assert(pop1 == "two"sv);
    return 0;
}
