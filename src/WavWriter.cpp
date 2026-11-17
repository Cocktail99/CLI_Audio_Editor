#include <algorithm>
#include <cmath>
#include <fstream>
#include <stdexcept>

#include "AudioClip.h"
#include "ByteIO.h"

using byteio::writeLE16;
using byteio::writeLE32;

// Saves the clip as a 16-bit PCM WAV file.
void AudioClip::saveWav(const std::string& path) const {
    std::ofstream out(path, std::ios::binary);
    if (!out) throw std::runtime_error("cannot write file '" + path + "'");

    const uint16_t channels = numChannels();
    const uint16_t bits = 16;
    const uint16_t blockAlign = static_cast<uint16_t>(channels * bits / 8);
    const uint32_t dataSize = static_cast<uint32_t>(numFrames() * blockAlign);

    out.write("RIFF", 4);
    writeLE32(out, 36 + dataSize);
    out.write("WAVE", 4);

    out.write("fmt ", 4);
    writeLE32(out, 16);
    writeLE16(out, 1);  // PCM
    writeLE16(out, channels);
    writeLE32(out, sampleRate_);
    writeLE32(out, sampleRate_ * blockAlign);
    writeLE16(out, blockAlign);
    writeLE16(out, bits);

    out.write("data", 4);
    writeLE32(out, dataSize);

    std::vector<char> buffer(dataSize);
    std::size_t k = 0;
    for (std::size_t i = 0; i < numFrames(); ++i) {
        for (uint16_t c = 0; c < channels; ++c) {
            float s = std::clamp(channels_[c][i], -1.0f, 1.0f);
            auto v = static_cast<int16_t>(std::lround(s * 32767.0f));
            buffer[k++] = static_cast<char>(v & 0xFF);
            buffer[k++] = static_cast<char>((v >> 8) & 0xFF);
        }
    }
    out.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    if (!out) throw std::runtime_error("error while writing '" + path + "'");
}
