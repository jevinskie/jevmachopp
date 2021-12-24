#include <jevmachopp/Common.h>

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
    const auto path = "/chosen/internal-use-only-unit";
    for (const auto &foo : stringSplitViewDelimitedBy(path, '/')) {
        std::cout << foo << std::endl;
        printf("foo: %.*s\n", (int)foo.size(), foo.data());
    }
    // for (const auto word : nano::views::split(path, delim)) {
    //     std::cout << std::string_view{word.begin(), word.end()} << ' ';
    // }
    return 0;
}
