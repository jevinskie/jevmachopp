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
    char _data[255];
    bool valid;

public:
    string() : valid(false) {
        memset(_data, 0, sizeof(_data));
        fprintf(stderr, "string() this: %p\n", (void *)this);
        setValid(false);
    }

    string(const char *p) : valid(false) {
        memset(_data, 0, sizeof(_data));
        fprintf(stderr, "string(const char* p) this: %p p: %s\n", (void *)this, p);
        size_t size = std::strlen(p) + 1;
        // _data = new char[size];
        std::memcpy(_data, p, size);
        setValid(true);
    }

    ~string() = default;

    // ~string() {
    //     fprintf(stderr, "~string() this: %p _data: %s valid: %s\n", (void *)this, _data,
    //             YESNOCStr(valid));
    //     assert(valid);
    //     valid = false;
    //     // delete[] _data;
    //     _data = nullptr;
    // }

    string(const string &that) : valid(false) {
        memset(_data, 0, sizeof(_data));
        fprintf(stderr, "string(const string& that) this: %p that: %s\n", (void *)this, that._data);
        // _data = new char[size];
        std::memcpy(_data, that._data, sizeof(_data));
        setValid(true);
    }

    string(string &&that)
        : valid(false) // string&& is an rvalue reference to a string
    {
        memset(_data, 0, sizeof(_data));
        fprintf(stderr, "string(string&& that) this: %p %s that: %s\n", (void *)this, _data,
                that._data);
        memcpy(_data, that._data, sizeof(_data));
        const auto that_valid = that.valid;
        that.valid = false;
        memset(that._data, 0, sizeof(that._data));
        setValid(that_valid);
    }

    string &operator=(string that) {
        fprintf(stderr, "operator= this: %p %s that: %s\n", (void *)this, _data, that._data);
        //        std::swap(valid, that.valid);
        const auto that_valid = that.valid;
        const auto our_valid = valid;
        that.setValid(our_valid);
        setValid(that_valid);
        std::swap(_data, that._data);
        return *this;
    }

    char *data() {
        assert(valid);
        return &_data[0];
    }

    const char *data() const {
        assert(valid);
        return &_data[0];
    }

    operator std::string_view() const {
        assert(valid);
        return {_data, std::strlen(_data)};
    }

    void setValid(bool v) {
        valid = v;
    }
};

auto rb = MultiConsRingBuffer<string, NUM_ELEM>{};
auto p0_ready_prom = std::promise<void>{};
auto p0_ready_fut = p0_ready_prom.get_future();
auto p0_go_prom = std::promise<void>{};
auto p0_go_fut = p0_go_prom.get_future();
auto p0_runner_done_prom = std::promise<void>{};
auto p0_joiner_done_prom = std::promise<void>{};

string p0_res;

std::unique_ptr<std::thread> p0;
std::unique_ptr<std::thread> p0_joiner;

void p0_runner_func(void) {
    fprintf(stderr, "p0 thread start\n");
    p0_res = rb.pop(&p0_ready_fut, &p0_go_fut);
    fprintf(stderr, "p0_res: %s\n", p0_res.data());
    p0_runner_done_prom.set_value();
    printf("p0_runner_func done\n");
}

void p0_joiner_func(void) {
    fprintf(stderr, "p0_joiner thread start\n");
    p0->detach();
    fprintf(stderr, "p0 detached\n");

    fprintf(stderr, "p0_joiner releasing p0_ready_cv\n");
    p0_ready_prom.set_value();
    // fprintf(stderr, "main reacquiring p0_ready_cv\n");
    // p0_ready_cv.acquire();
    // fprintf(stderr, "main did reacquire p0_ready_cv\n");

    const auto p1_res = rb.pop();
    fprintf(stderr, "p1_res: %s\n", p1_res.data());
    assert(p1_res == "one"sv);

    fprintf(stderr, "p0 releasing p0_go_cv\n");
    p0_go_prom.set_value();
    fprintf(stderr, "p0 releasing done_cv\n");
    p0_joiner_done_prom.set_value();
}

int main(void) {

    fprintf(stderr, "pushing 1 from main\n");
    rb.push(string{"one"});
    fprintf(stderr, "pushing 2 from main\n");
    rb.push(string{"two"});

    //    p0_ready_cv.acquire();
    //    p0_go_cv.acquire();

    p0 = std::make_unique<std::thread>(p0_runner_func);

    p0_joiner = std::make_unique<std::thread>(p0_joiner_func);

    fprintf(stderr, "main joining p0_joiner\n");
    p0_joiner->detach();

    fprintf(stderr, "main acquiring p0_joiner_done_cv\n");
    p0_joiner_done_prom.get_future().wait();

    fprintf(stderr, "main acquiring p0_runner_done_cv\n");
    p0_runner_done_prom.get_future().wait();

    fprintf(stderr, "main p0_res: %s\n", p0_res.data());
    //    assert(p0_res == "two"sv);
    return 0;
}
