#pragma once

#include <list>

#include <fmt/core.h>

#include <nanorange/views/common.hpp>
#include <nanorange/views/filter.hpp>
#include <nanorange/views/subrange.hpp>
using namespace nano;

class Version {
public:
    Version(int major = 0, int minor = 0, int patch = 0)
        : major(major), minor(minor), patch(patch){};
    int major;
    int minor;
    int patch;
};

template <> struct fmt::formatter<Version> {
    constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
        return ctx.begin();
    }

    auto format(Version const &ver, format_context &ctx) -> decltype(ctx.out()) {
        return fmt::format_to(ctx.out(), "{:d}.{:d}.{:d}", ver.major, ver.minor, ver.patch);
    }
};

std::list<Version> makeVersions();

int foo(void);
