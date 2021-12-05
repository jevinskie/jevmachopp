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
    char *_data;
    bool valid;

public:
    string() : _data(nullptr), valid(false) {
        fprintf(stderr, "string() this: %p\n", (void *)this);
    }

    string(const char *p) : _data(nullptr), valid(false) {
        fprintf(stderr, "string(const char* p) this: %p p: %s\n", (void *)this, p);
        size_t size = std::strlen(p) + 1;
        _data = new char[size];
        std::memcpy(_data, p, size);
        valid = true;
    }

    ~string() {
        fprintf(stderr, "~string() this: %p _data: %s valid: %s\n", (void *)this, _data,
                YESNOCStr(valid));
        assert(valid);
        valid = false;
        delete[] _data;
        _data = nullptr;
    }

    string(const string &that) : _data(nullptr), valid(false) {
        fprintf(stderr, "string(const string& that) this: %p that: %s\n", (void *)this, that._data);
        size_t size = std::strlen(that._data) + 1;
        _data = new char[size];
        std::memcpy(_data, that._data, size);
        valid = true;
    }

    string(string &&that)
        : _data(nullptr), valid(false) // string&& is an rvalue reference to a string
    {
        fprintf(stderr, "string(string&& that) this: %p %s that: %s\n", (void *)this, _data,
                that._data);
        _data = that._data;
        that.valid = false;
        that._data = nullptr;
    }

    string &operator=(string that) {
        fprintf(stderr, "operator= this: %p %s that: %s\n", (void *)this, _data, that._data);
        std::swap(valid, that.valid);
        std::swap(_data, that._data);
        return *this;
    }

    char *data() const {
        assert(valid);
        return _data;
    }

    operator std::string_view() const {
        return {_data, std::strlen(_data)};
    }
};

int main(void) {
    auto rb = MultiConsRingBuffer<string, NUM_ELEM>{};
    string p0_res;

    rb.push(string{"one"});
    rb.push(string{"two"});

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
