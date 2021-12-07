#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <thread>

#include <catch2/catch_test_macros.hpp>
#include <fmt/core.h>

#include <jevmachopp/RingBuffer.h>

using namespace std::string_literals;
using namespace std::literals;

constexpr std::size_t NUM_ELEM = 0x1000;

TEST_CASE("ctor", "[ringbuffer]") {
    auto rb = RingBuffer<uint32_t, NUM_ELEM>{};

    SECTION("mirror [0]") {
        rb.m_buf[0] = 0xdeadbeef;
        REQUIRE(rb.m_buf_mirror[0] == rb.m_buf[0]);
    }
}

TEST_CASE("push 'n pop", "[ringbuffer]") {
    auto rb = RingBuffer<uint32_t, NUM_ELEM>{};

    SECTION("push") {
        rb.push(0xdeadbeef);
        REQUIRE(**rb.peek() == 0xdeadbeef);
    }

    SECTION("pop") {
        rb.push(0xdeadbeef);
        REQUIRE(*rb.pop() == 0xdeadbeef);
    }

    SECTION("push/pop 2x") {
        rb.push(1);
        rb.emplace((decltype(rb)::value_type)2);

        REQUIRE(*rb.pop() == 1);
        REQUIRE(*rb.pop() == 2);
    }

    SECTION("2x size") {
        for (std::size_t i = 0; i < 1.5 * 2 * NUM_ELEM; ++i) {
            rb.push(i);
            REQUIRE(*rb.pop() == i);
        }
    }
}

TEST_CASE("no overflow", "[ringbuffer]") {
    auto rb = RingBuffer<uint32_t, NUM_ELEM>{};

    for (std::size_t i = 0; i < rb.static_size; ++i) {
        rb.push(i);
    }
    SUCCEED("no overflow point");
}

#if 0
TEST_CASE("overflow by one", "[ringbuffer]") {
    auto rb = RingBuffer<uint32_t, NUM_ELEM>{};

    for (std::size_t i = 0; i < rb.static_size; ++i) {
        rb.push(i);
    }
    SUCCEED("no overflow point");
    rb.push(0xdeadbeef);
    REQUIRE_FALSE(true);
}
#endif

TEST_CASE("MultiCons push 'n pop trivial", "[ringbuffer]") {
    auto rb = MultiConsRingBuffer<uint32_t, NUM_ELEM>{};

    rb.push(1);
    rb.push(2);

    REQUIRE(rb.pop() == 1);
    REQUIRE(rb.pop() == 2);
}

TEST_CASE("MultiCons push 'n pop std::string simple", "[ringbuffer]") {
    auto rb = MultiConsRingBuffer<std::string_view, NUM_ELEM>{};

    rb.push("one"sv);
    rb.push("two"sv);

    REQUIRE(rb.pop() == "one"s);
    REQUIRE(rb.pop() == "two"s);
}

// FIXME: switch from semaphore to condvar
#if 0
TEST_CASE("MultiCons push 'n pop std::string sem", "[ringbuffer]") {
    auto rb = MultiConsRingBuffer<std::string_view, NUM_ELEM>{};

    rb.push("one"sv);
    rb.push("two"sv);

    auto p0_ready_sem = std::binary_semaphore{};
    p0_ready_sem.acquire();
    auto p0_go_sem = std::binary_semaphore{};
    p0_go_sem.acquire();

    std::thread p0{[&]() {
        const auto p0_res = rb.pop(&p0_ready_sem, &p0_go_sem);
        fprintf(stderr, "p0_res: %s\n", p0_res.data());
    }};

    fprintf(stderr, "main releasing p0_ready_sem\n");
    p0_ready_sem.release();
    fprintf(stderr, "main reacquiring p0_ready_sem\n");
    p0_ready_sem.acquire();
    fprintf(stderr, "main did reacquire p0_ready_sem\n");

    REQUIRE(rb.pop() == "one"s);

    p0_go_sem.release();

    p0.join();

    REQUIRE(rb.pop() == "two"s);
}
#endif

TEST_CASE("load int 128", "[ringbuffer]") {
    auto rb = MultiConsRingBuffer<std::string_view, NUM_ELEM>{};
    rb.lol.store((uint128_t)0xdeadbeef'00000000ull << 64 | 0xbaadc0de'00000000ull);
    const auto res = rb.lol.load();
    const auto *pp = (std::pair<uint64_t, uint64_t> *)&res;
    fprintf(stderr, "pp first: 0x%llx second: 0x%llx\n", pp->first, pp->second);
}

TEST_CASE("load real pair u64", "[ringbuffer]") {
    auto rb = MultiConsRingBuffer<std::string_view, NUM_ELEM>{};
    rb.rofl.store({0xfacefeed, 0x00070007});
    const auto res = rb.rofl.load();
    // const auto *pp = (std::pair<uint64_t, uint64_t> *)&res;
    fprintf(stderr, "pp first: 0x%llx second: 0x%llx\n", res.first, res.second);
}
