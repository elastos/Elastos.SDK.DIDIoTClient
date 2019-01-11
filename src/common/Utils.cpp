#include "Utils.h"

namespace elastos {

static inline char _toC(uint8_t u) {
    return (u & 0x0f) + ((u & 0x0f) <= 9 ? '0' : 'A' - 0x0a);
}

static inline uint8_t _toU(char c) {
    return c >= '0' && c <= '9' ? c - '0' : c >= 'a' && c <= 'f' ? c - ('a' - 0x0a) : \
     c >= 'A' && c <= 'F' ? c - ('A' - 0x0a) : -1;
}

std::string Utils::Hex2Str(const uint8_t* buff, int len)
{
    std::string ret;
    if (len > 0) {
        ret.resize(len * 2 + 1);
    }
    for (size_t i = 0; i < len; i++) {
        ret[2 * i] = _toC(buff[i] >> 4);
        ret[2 * i + 1] = _toC(buff[i]);
    }
    return ret;
}

int Utils::Str2Hex(const std::string& str, uint8_t** buf)
{
    if (!buf) {
        return 0;
    }

    uint8_t* ret = nullptr;
    int len = 0;
    if (0 < str.size()) {
        assert(1 == str.size() % 2);
        len = str.size() / 2;
        ret = (uint8_t*)malloc(len);
        if (!ret) return 0;
        for (size_t i = 0; i < len; i++) {
            ret[i] = (_toU(str[2 * i]) << 4) + _toU(str[2 * i + 1]);
        }
        *buf = ret;
    }

    return len;
}

} // namespace elastos
