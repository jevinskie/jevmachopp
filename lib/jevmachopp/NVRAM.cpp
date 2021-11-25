#include "jevmachopp/NVRAM.h"
#include "jevmachopp/Hex.h"
#include "jevmachopp/c/jevnvram.h"

#include <experimental/fixed_capacity_vector>

#pragma mark CHRPNVRAMHeader

#pragma mark CHRPNVRAMHeader - Accessors

uint16_t CHRPNVRAMHeader::size_blocks() const {
    return len;
}

uint32_t CHRPNVRAMHeader::size_bytes() const {
    return size_blocks() * 0x10;
}

const char *CHRPNVRAMHeader::name() const {
    return name_buf;
}

const uint8_t *CHRPNVRAMHeader::data() const {
    return (const uint8_t *)(this + 1);
}

#pragma mark CHRPNVRAMHeader - environment variables

packed_cstr_eterm_range CHRPNVRAMHeader::vars() const {
    return {vars_cbegin(), vars_cend()};
}

PackedCStrIteratorEmtpyTerm CHRPNVRAMHeader::vars_cbegin() const {
    return {(const char *)data()};
}

PackedCStrIteratorEmtpyTerm CHRPNVRAMHeader::vars_cend() const {
    return {};
}

const char *CHRPNVRAMHeader::varNamed(const char *name) const {
    return find_if_or_nullptr(vars(), [=](const char &varEqValStr) {
        return NVRAM::varName(&varEqValStr) == name;
    });
}

#pragma mark CHRPNVRAMHeader - fmt

#if USE_FMT
fmt::appender &CHRPNVRAMHeader::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<CHRPNVRAMHeader @ {:p} name: \"{:s}\" size blocks: {:#x} size: {:#x}>"_cf,
                   (void *)this, name(), size_blocks(), size_bytes());
    return out;
}
#endif

#pragma mark AppleNVRAMHeader

#if USE_FMT
fmt::appender &AppleNVRAMHeader::format_to(fmt::appender &out) const {
    fmt::format_to(out,
                   "<AppleNVRAMHeader @ {:p} chrp_hdr: {} adler: {:#010x} generation: {:#x}>"_cf,
                   (void *)this, chrp_hdr, adler, generation);
    return out;
}
#endif

#pragma mark Environment Variable Codings

namespace NVRAM {

const std::string_view varName(const char *varEqValStr) {
    const char *eq_chr_ptr = std::strchr(varEqValStr, '=');
    if (!eq_chr_ptr) {
        return {};
    }
    return {varEqValStr, (uintptr_t)eq_chr_ptr - (uintptr_t)varEqValStr};
}

const char *varValue(const char *varEqValStr) {
    const auto len = std::strlen(varEqValStr);
    const char *eq_chr_ptr = (const char *)std::memchr(varEqValStr, '=', len);
    if (!eq_chr_ptr) {
        return nullptr;
    }
    return eq_chr_ptr + 1;
}

NVRAMProxyData extractProxyData(const void *nvram_proxy_data_buf) {
    const auto apple_hdr_ptr = (const AppleNVRAMHeader *)nvram_proxy_data_buf;
    const auto &apple_hdr = *apple_hdr_ptr;
    const auto common_hdr_ptr = (const CHRPNVRAMHeader *)(apple_hdr_ptr + 1);
    const auto &common_hdr = *common_hdr_ptr;
    const auto system_hdr_ptr =
        (const CHRPNVRAMHeader *)((uintptr_t)common_hdr_ptr + common_hdr.size_bytes());
    const auto &system_hdr = *system_hdr_ptr;
    return {apple_hdr, common_hdr, system_hdr};
}

// dangerous (bad bounds checking)
uint32_t decodedDataLen(const std::span<const uint8_t> &escaped) {
    uint32_t totalLength = 0;
    uint32_t offset, offset2;
    uint8_t byte;
    bool ok;

    // Calculate the actual length of the data.
    ok = true;
    totalLength = 0;
    for (offset = 0; offset < escaped.size_bytes();) {
        byte = escaped[offset++];
        if (byte == 0xFF) {
            byte = escaped[offset++];
            if (byte == 0x00) {
                ok = false;
                break;
            }
            offset2 = byte & 0x7F;
        } else {
            offset2 = 1;
        }
        totalLength += offset2;
    }

    return ok ? totalLength : 0;
}

// dangerous (bad bounds checking), probably broken
uint32_t unescapeBytesToData(const std::span<const uint8_t> &escaped,
                             std::span<uint8_t> &&decoded) {
    const auto enc_buf_sz = escaped.size_bytes();
    const auto dec_buf_sz = decoded.size_bytes();
    assert(dec_buf_sz >= enc_buf_sz);

    uint32_t totalLength = decodedDataLen(escaped);
    uint32_t offset, offset2;
    uint8_t byte;
    bool ok;
    const auto length = decoded.size_bytes();
    uint32_t outOff = 0;

    if (totalLength) {
        assert(totalLength <= length);
        for (offset = 0; offset < length;) {
            byte = escaped[offset++];
            if (byte == 0xFF) {
                byte = escaped[offset++];
                offset2 = byte & 0x7F;
                byte = (byte & 0x80) ? 0xFF : 0x00;
            } else {
                offset2 = 1;
            }
            for (auto i = 0; i < offset2; ++i) {
                decoded[outOff++] = byte;
            }
        }
    }

    return totalLength;
}

// dangerous (bad bounds checking), probably broken
uint32_t escapeDataToData(const std::span<const uint8_t> &decoded, std::span<uint8_t> &&encoded) {
    const auto dec_buf_sz = decoded.size_bytes();
    const auto enc_buf_sz = encoded.size_bytes();
    assert(enc_buf_sz >= dec_buf_sz);

    const uint8_t *startPtr;
    const uint8_t *endPtr;
    const uint8_t *wherePtr;
    uint32_t outOff = 0;
    uint8_t byte;

    wherePtr = (const uint8_t *)decoded.data();
    endPtr = wherePtr + decoded.size_bytes();

    while (wherePtr < endPtr) {
        startPtr = wherePtr;
        byte = *wherePtr++;
        if ((byte == 0x00) || (byte == 0xFF)) {
            for (; ((wherePtr - startPtr) < 0x7F) && (wherePtr < endPtr) && (byte == *wherePtr);
                 wherePtr++) {}
            encoded[outOff++] = 0xff;
            byte = (byte & 0x80) | ((uint8_t)(wherePtr - startPtr));
        }
        encoded[outOff++] = byte;
    }
    encoded[outOff++] = 0;

    assert(outOff <= encoded.size_bytes());

    return outOff;
}

} // namespace NVRAM

#pragma mark C

void dump_nvram(const void *nvram_buf) {
    printf("nvram @ %p\n", nvram_buf);

    const auto proxyData = NVRAM::extractProxyData(nvram_buf);

#if USE_FMT

    fmt::print("nvram_hdr: {}\n", proxyData.nvram_hdr);
    fmt::print("common_hdr: {}\n", proxyData.common_hdr);

    for (const char &varEqValStr : proxyData.common_hdr.vars()) {
        const auto varName = NVRAM::varName(&varEqValStr);
        fmt::print("common varName: {:s}\n", varName);
    }

    fmt::print("system_hdr: {}\n", proxyData.system_hdr);

    for (const char &varEqValStr : proxyData.system_hdr.vars()) {
        const auto varName = NVRAM::varName(&varEqValStr);
        fmt::print("system varName: {:s}\n", varName);
    }

    const char *bootArgsVarEqValStr = proxyData.system_hdr.varNamed("boot-args");
    if (bootArgsVarEqValStr) {
        const char *bootArgs = NVRAM::varValue(bootArgsVarEqValStr);
        fmt::print("nvram boot-args: {:s}\n", bootArgs);
    } else {
        fmt::print("boot-args missing\n");
    }

#if 0
    // const auto buf_span = std::span<const uint8_t>{(const uint8_t*)nvram_buf, 32};
    const auto buf_span = std::span<const uint8_t>{(const uint8_t *)nvram_buf, 423};
    std::experimental::fixed_capacity_vector<uint8_t, 512> out_vec{};
    fmt::print("buf_span: size: {:d} size_bytes: {:d}\n", buf_span.size(), buf_span.size_bytes());
    fmt::print("buf_span: extent: {:d}\n", buf_span.extent);
    fmt::print("buf_span: Extent: {:d}\n", decltype(buf_span)::extent);
    // std::span<uint8_t> NVRAM::out_vec_span{out_vec.begin(), out_vec.end()};
    // auto decoded_sz = NVRAM::escapeDataToData(buf_span, out_vec_span);
    // auto decoded_sz = NVRAM::escapeDataToData(
    //     buf_span, std::span<uint8_t>{out_vec.begin(), out_vec.begin() + out_vec.capacity()});
    auto decoded_sz = NVRAM::unescapeBytesToData(
        buf_span, std::span<uint8_t>{out_vec.begin(), out_vec.begin() + out_vec.capacity()});
    fmt::print("decoded_sz: {:d} out_vec.size(): {:d}\n", decoded_sz, out_vec.size());

    auto hexstr = buf2hexstr<512>(out_vec.data(), decoded_sz);
    fmt::print("hexstr: {:s}\n", hexstr);
#endif
    fmt::print("nvram: {:p}\n", nvram_buf);

#endif
}
