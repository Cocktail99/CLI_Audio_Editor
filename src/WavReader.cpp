#include <algorithm>
#include <cstring>
#include <fstream>
#include <iterator>
#include <stdexcept>

#include "AudioClip.h"
#include "ByteIO.h"

using byteio::readLE16;
using byteio::readLE32;

namespace {

constexpr uint16_t kFormatPcm = 1;
constexpr uint16_t kFormatFloat = 3;
constexpr uint16_t kFormatExtensible = 0xFFFE;

// Decodes a single sample starting at p into a float in [-1, 1].
float decodeSample(const uint8_t* p, uint16_t format, uint16_t bits) {
    if (format == kFormatPcm) {
        switch (bits) {
            case 8:
                return (static_cast<int>(p[0]) - 128) / 128.0f;
            case 16:
                return static_cast<int16_t>(readLE16(p)) / 32768.0f;
            case 24: {
                int32_t v = p[0] | (p[1] << 8) | (p[2] << 16);
                if (v & 0x800000) v |= ~0xFFFFFF;  // sign-extend
                return v / 8388608.0f;
            }
            case 32:
                return static_cast<float>(static_cast<int32_t>(readLE32(p)) / 2147483648.0);
        }
    } else if (format == kFormatFloat) {
        if (bits == 32) {
            uint32_t raw = readLE32(p);
            float f;
            std::memcpy(&f, &raw, sizeof f);
            return f;
        }
        if (bits == 64) {
            uint64_t raw = static_cast<uint64_t>(readLE32(p)) |
                           (static_cast<uint64_t>(readLE32(p + 4)) << 32);
            double d;
            std::memcpy(&d, &raw, sizeof d);
            return static_cast<float>(d);
        }
    }
    throw std::runtime_error("unsupported sample format (" + std::to_string(bits) + "-bit)");
}

}  // namespace

AudioClip AudioClip::loadWav(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) throw std::runtime_error("cannot open file '" + path + "'");
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

    if (data.size() < 12 || std::memcmp(data.data(), "RIFF", 4) != 0 ||
        std::memcmp(data.data() + 8, "WAVE", 4) != 0) {
        throw std::runtime_error("'" + path + "' is not a WAV file");
    }

    bool haveFmt = false;
    uint16_t format = 0, numChannels = 0, bits = 0, blockAlign = 0;
    uint32_t sampleRate = 0;
    const uint8_t* samples = nullptr;
    std::size_t samplesSize = 0;

    // Walk the RIFF chunks; we only care about "fmt " and "data".
    std::size_t pos = 12;
    while (pos + 8 <= data.size()) {
        const uint8_t* id = data.data() + pos;
        std::size_t size = readLE32(data.data() + pos + 4);
        std::size_t body = pos + 8;
        std::size_t available = std::min(size, data.size() - body);

        if (std::memcmp(id, "fmt ", 4) == 0) {
            if (available < 16) throw std::runtime_error("corrupt fmt chunk");
            const uint8_t* f = data.data() + body;
            format = readLE16(f);
            numChannels = readLE16(f + 2);
            sampleRate = readLE32(f + 4);
            blockAlign = readLE16(f + 12);
            bits = readLE16(f + 14);
            if (format == kFormatExtensible) {
                if (available < 26) throw std::runtime_error("corrupt extensible fmt chunk");
                format = readLE16(f + 24);  // first two bytes of the SubFormat GUID
            }
            haveFmt = true;
        } else if (std::memcmp(id, "data", 4) == 0) {
            samples = data.data() + body;
            samplesSize = available;  // tolerate truncated files
        }
        pos = body + size + (size & 1);  // chunks are padded to an even size
    }

    if (!haveFmt) throw std::runtime_error("WAV file has no fmt chunk");
    if (!samples) throw std::runtime_error("WAV file has no data chunk");
    if (numChannels == 0 || sampleRate == 0 || bits == 0) throw std::runtime_error("invalid WAV header");
    if (format != kFormatPcm && format != kFormatFloat)
        throw std::runtime_error("only PCM and float WAV files are supported");

    const std::size_t bytesPerSample = bits / 8;
    if (blockAlign == 0) blockAlign = static_cast<uint16_t>(bytesPerSample * numChannels);
    const std::size_t frames = samplesSize / blockAlign;

    AudioClip clip(sampleRate, numChannels, frames);
    for (std::size_t i = 0; i < frames; ++i) {
        const uint8_t* frame = samples + i * blockAlign;
        for (uint16_t c = 0; c < numChannels; ++c) {
            clip.channels_[c][i] = decodeSample(frame + c * bytesPerSample, format, bits);
        }
    }
    return clip;
}
