#include "jevmachopp/Version.h"

#pragma mark Version

Version::Version(uint32_t ver) : patch(ver & 0xff), minor((ver >> 8) & 0xff), major(ver >> 16) {}

Version::Version(uint16_t major, uint8_t minor, uint8_t patch)
    : patch(patch), minor(minor), major(major) {}

#if USE_FMT
fmt::appender &Version::format_to(fmt::appender &out) const {
    fmt::format_to(out, "{:d}.{:d}.{:d}"_cf, major, minor, patch);
    return out;
}
#endif

#pragma mark SourceVersion

SourceVersion::SourceVersion(uint64_t version) : version(version) {}

// A.B.C.D.E packed as a24.b10.c10.d10.e10

uint32_t SourceVersion::a() const {
    return (version >> 40) & ((2 << 24) - 1);
}

uint16_t SourceVersion::b() const {
    return (version >> 30) & ((2 << 10) - 1);
}

uint16_t SourceVersion::c() const {
    return (version >> 20) & ((2 << 10) - 1);
}

uint16_t SourceVersion::d() const {
    return (version >> 10) & ((2 << 10) - 1);
}

uint16_t SourceVersion::e() const {
    return (version >> 0) & ((2 << 10) - 1);
}

#if USE_FMT
fmt::appender &SourceVersion::format_to(fmt::appender &out) const {
    fmt::format_to(out, "{:d}.{:d}.{:d}.{:d}.{:d}"_cf, a(), b(), c(), d(), e());
    return out;
}
#endif
