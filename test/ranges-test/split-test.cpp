#include <cstdio>
#include <iomanip>
#include <iostream>
#include <string_view>

#include <nanorange/views/all.hpp>
#include <nanorange/views/common.hpp>
#include <nanorange/views/split.hpp>
#include <nanorange/views/transform.hpp>
// using namespace nano;
using namespace std::literals::string_view_literals;

int main(void) {
    const auto path = "/chosen/internal-use-only-unit"sv;
    const auto delim = "/"sv;
    std::cout << path << std::endl;
    auto sv = path | nano::views::split('/') | nano::views::common;
    auto svb = sv.begin();
    auto svbd = *svb;
    auto sve = sv.end();
    auto sved = *sve;
    for (const auto &foo : sv) {
        auto b = foo.begin();
        auto bd = *b;
        auto bdp = &bd;
        auto e = foo.end();
        const auto *foo_chrp = &*foo.begin();
        const auto foo_sz = static_cast<int>(nano::ranges::distance(foo));
        auto fsv = std::string_view{&*foo_chrp, (size_t)foo_sz};
        // std::cout << std::string_view{foo.begin(), foo.end()} << std::endl;
        printf("foo: %.*s\n", foo_sz, foo_chrp);
    }
    // for (const auto word : nano::views::split(path, delim)) {
    //     std::cout << std::string_view{word.begin(), word.end()} << ' ';
    // }
    return 0;
}
