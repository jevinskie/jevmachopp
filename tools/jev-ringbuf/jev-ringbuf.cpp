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

class string {
    char *data;

public:
    string(const char *p) {
        fprintf(stderr, "string(const char* p) p: %s\n", p);
        size_t size = std::strlen(p) + 1;
        data = new char[size];
        std::memcpy(data, p, size);
    }

    ~string() {
        delete[] data;
    }

    string(const string &that) {
        fprintf(stderr, "string(const string& that) that: %s\n", that.data);
        size_t size = std::strlen(that.data) + 1;
        data = new char[size];
        std::memcpy(data, that.data, size);
    }

    string(string &&that) // string&& is an rvalue reference to a string
    {
        fprintf(stderr, "string(string&& that) this: %s that: %s\n", data, that.data);
        data = that.data;
        that.data = nullptr;
    }

    string &operator=(string that) {
        fprintf(stderr, "operator= this: %s that: %s\n", data, that.data);
        std::swap(data, that.data);
        return *this;
    }
};

int main(void) {
    auto rb = MultiConsRingBuffer<std::string, NUM_ELEM>{};
    std::string p0_res;

    rb.push("one"s);
    rb.push("two"s);

    auto p0_ready_sem = std::binary_semaphore{0};
    //    p0_ready_sem.acquire();
    auto p0_go_sem = std::binary_semaphore{0};
    auto done_sem = std::binary_semaphore{0};
    //    p0_go_sem.acquire();

    std::thread p0{[&]() {
        fprintf(stderr, "p0 thread start\n");
        p0_res = rb.pop(&p0_ready_sem, &p0_go_sem);
        fprintf(stderr, "p0_res: %s\n", p0_res.data());
    }};

    std::thread p0_joiner{[&]() {
        fprintf(stderr, "p0 joiner thread start\n");
        p0.detach();
        fprintf(stderr, "p0 detached\n");

        fprintf(stderr, "p0 releasing p0_ready_sem\n");
        p0_ready_sem.release();
        // fprintf(stderr, "main reacquiring p0_ready_sem\n");
        // p0_ready_sem.acquire();
        // fprintf(stderr, "main did reacquire p0_ready_sem\n");

        const auto p1_res = rb.pop();
        fprintf(stderr, "p1_res: %s\n", p1_res.data());
        assert(p1_res == "one"sv);

        fprintf(stderr, "p0 releasing p0_go_sem\n");
        p0_go_sem.release();
        fprintf(stderr, "p0 releasing done_sem\n");
        done_sem.release();
    }};

    fprintf(stderr, "main joining p0_joiner\n");
    p0_joiner.detach();

    fprintf(stderr, "main acquiring done_sem\n");
    done_sem.acquire();

    fprintf(stderr, "main p0_res: %s\n", p0_res.data());
    //    assert(p0_res == "two"sv);
    return 0;
}
