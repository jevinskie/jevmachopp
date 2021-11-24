#include "jevmachopp/NVRAM.h"
#include "jevmachopp/Hex.h"
#include "jevmachopp/c/jevnvram.h"

#pragma mark Environment Variable Codings

std::size_t escapeDataToData(const std::span<const uint8_t> &value, std::span<uint8_t> &&decoded) {
    const auto dec_buf_sz = decoded.size_bytes();
    const auto enc_buf_sz = value.size_bytes();
    assert(dec_buf_sz >= enc_buf_sz);

    const uint8_t *startPtr;
    const uint8_t *endPtr;
    const uint8_t *wherePtr;
    std::size_t outOff = 0;
    uint8_t byte;
    bool ok = true;

    wherePtr = (const uint8_t *)value.data();
    endPtr = wherePtr + value.size_bytes();

    while (wherePtr < endPtr) {
        startPtr = wherePtr;
        byte = *wherePtr++;
        if ((byte == 0x00) || (byte == 0xFF)) {
            for (; ((wherePtr - startPtr) < 0x7F) && (wherePtr < endPtr) && (byte == *wherePtr);
                 wherePtr++) {}
            decoded[outOff++] = 0xff;
            byte = (byte & 0x80) | ((uint8_t)(wherePtr - startPtr));
        }
        decoded[outOff++] = byte;
    }
    decoded[outOff++] = 0;

    assert(outOff <= decoded.size_bytes());

    return outOff;
}

#pragma mark C

void dump_nvram(const void *nvram_buf) {
    printf("nvram @ %p\n", nvram_buf);

#if USE_FMT

    // const auto buf_span = std::span<const uint8_t>{(const uint8_t*)nvram_buf, 32};
    const auto buf_span = std::span<const uint8_t>{(const uint8_t *)nvram_buf, 423};
    std::experimental::fixed_capacity_vector<uint8_t, 512> out_vec{};
    fmt::print("buf_span: size: {:d} size_bytes: {:d}\n", buf_span.size(), buf_span.size_bytes());
    fmt::print("buf_span: extent: {:d}\n", buf_span.extent);
    fmt::print("buf_span: Extent: {:d}\n", decltype(buf_span)::extent);
    // std::span<uint8_t> out_vec_span{out_vec.begin(), out_vec.end()};
    // auto decoded_sz = escapeDataToData(buf_span, out_vec_span);
    auto decoded_sz = escapeDataToData(
        buf_span, std::span<uint8_t>{out_vec.begin(), out_vec.begin() + out_vec.capacity()});
    fmt::print("decoded_sz: {:d} out_vec.size(): {:d}\n", decoded_sz, out_vec.size());

    auto hexstr = buf2hexstr<512>(out_vec.data(), decoded_sz);
    fmt::print("hexstr: {:s}\n", hexstr);

    fmt::print("nvram: {:p}\n", nvram_buf);

#endif
}
