#pragma once

#include <cstdint>
#include <ostream>

// Little-endian helpers shared by the WAV and BMP code.
namespace byteio {

inline uint16_t readLE16(const uint8_t* p) {
    return static_cast<uint16_t>(p[0] | (p[1] << 8));
}

inline uint32_t readLE32(const uint8_t* p) {
    return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) |
           (static_cast<uint32_t>(p[2]) << 16) | (static_cast<uint32_t>(p[3]) << 24);
}

inline void writeLE16(std::ostream& out, uint16_t v) {
    out.put(static_cast<char>(v & 0xFF));
    out.put(static_cast<char>((v >> 8) & 0xFF));
}

inline void writeLE32(std::ostream& out, uint32_t v) {
    for (int i = 0; i < 4; ++i) out.put(static_cast<char>((v >> (8 * i)) & 0xFF));
}

}  // namespace byteio
