#include "jevmachopp/LEB128.h"

namespace LEB128 {

std::size_t DecodeUleb128Backwards(std::span<const uint8_t> prev_data, uint64_t *const val) {
    const auto sz = prev_data.size_bytes();
    if (!sz || !val) {
        return 0;
    }
    if (sz == 1) {
        assert(!(prev_data[0] & 0x80));
        *val = prev_data[0];
        return 1;
    }
    const uint8_t *buf_start = &prev_data.front();
    const uint8_t *enc_val_start = &prev_data.back();
    for (; enc_val_start >= buf_start + 1; --enc_val_start) {
        if (!(enc_val_start[-1] & 0x80)) {
            break;
        }
    }
    if (enc_val_start == buf_start + 1 && (buf_start[0] & 0x80)) {
        enc_val_start = buf_start;
    }
    return bfs::DecodeUleb128({enc_val_start, &*prev_data.end()}, val);
}

} // namespace LEB128
