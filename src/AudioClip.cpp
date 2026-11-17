#include "AudioClip.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

AudioClip::AudioClip(uint32_t sampleRate, uint16_t numChannels, std::size_t numFrames)
    : sampleRate_(sampleRate), channels_(numChannels, std::vector<float>(numFrames, 0.0f)) {
    if (sampleRate == 0) throw std::invalid_argument("sample rate must be positive");
    if (numChannels == 0) throw std::invalid_argument("clip needs at least one channel");
}

double AudioClip::duration() const {
    return static_cast<double>(numFrames()) / sampleRate_;
}

void AudioClip::resize(std::size_t numFrames) {
    for (auto& ch : channels_) ch.resize(numFrames, 0.0f);
}

std::size_t AudioClip::secondsToFrame(double seconds) const {
    if (seconds <= 0) return 0;
    auto frame = static_cast<std::size_t>(std::llround(seconds * sampleRate_));
    return std::min(frame, numFrames());
}

float AudioClip::peak() const {
    float p = 0.0f;
    for (const auto& ch : channels_)
        for (float s : ch) p = std::max(p, std::fabs(s));
    return p;
}

std::vector<float> AudioClip::mixToMono() const {
    std::vector<float> mono(numFrames(), 0.0f);
    if (channels_.empty()) return mono;
    for (const auto& ch : channels_)
        for (std::size_t i = 0; i < mono.size(); ++i) mono[i] += ch[i];
    const float scale = 1.0f / channels_.size();
    for (float& s : mono) s *= scale;
    return mono;
}
