#include "jevmachopp/LEB128.h"

namespace LEB128 {

std::size_t DecodeUleb128Backwards(std::span<const uint8_t> prev_data, uint64_t *const val) {
    if (!prev_data.size_bytes()) {
        return 0;
    }
    if (prev_data.back() == 0) {
        return 0;
    }
    const uint8_t *buf_start = &prev_data.front();
    const uint8_t *enc_val_start = &prev_data.back() - 1;
    for (; enc_val_start >= buf_start; --enc_val_start) {
        if (!(*enc_val_start & 0x80)) {
            break;
        }
    }
    if (!(*enc_val_start & 0x80)) {
        ++enc_val_start;
    }
    return bfs::DecodeUleb128({enc_val_start, &*prev_data.end()}, val);
}

} // namespace LEB128
