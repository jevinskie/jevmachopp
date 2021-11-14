#include "jevmachopp/CpuTypeMeta.h"

#include "jevmachopp/Common.h"

uint64_t CpuTypeMeta::size() const {
    return sizeof(*this);
}

uint64_t CpuTypeMeta::pack(uint8_t *buf, uint8_t *base) const {
    int32_t *cputype_buf = (int32_t *)buf;
    cputype_buf[0] = (int32_t)type;
    cputype_buf[1] = (int32_t)subtype;
    return size();
}

uint64_t CpuTypeMeta::unpack(uint8_t *buf, uint8_t *base) {
    int32_t *cputype_buf = (int32_t *)buf;
    type = (CpuType)cputype_buf[0];
    subtype = (CpuSubType)cputype_buf[1];
    return size();
}