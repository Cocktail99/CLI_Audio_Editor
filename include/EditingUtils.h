#pragma once

#include <stdexcept>

#include "AudioClip.h"

// Small helpers shared by the editing operations.
namespace editing::detail {

inline void checkRange(const AudioClip& clip, double start, double end) {
    if (start < 0 || end <= start) throw std::invalid_argument("invalid time range");
    if (start >= clip.duration()) throw std::invalid_argument("start is past the end of the clip");
}

inline void checkSameRate(const AudioClip& a, const AudioClip& b) {
    if (a.sampleRate() != b.sampleRate())
        throw std::invalid_argument("sample rates differ (" + std::to_string(a.sampleRate()) + " vs " +
                                    std::to_string(b.sampleRate()) + " Hz)");
}

// Returns a copy of `other` with `channels` channels (mono <-> stereo).
inline AudioClip matchChannels(const AudioClip& other, uint16_t channels) {
    if (other.numChannels() == channels) return other;
    AudioClip result(other.sampleRate(), channels, other.numFrames());
    std::vector<float> mono = other.mixToMono();
    for (uint16_t c = 0; c < channels; ++c) result.channel(c) = mono;
    return result;
}

}  // namespace editing::detail
