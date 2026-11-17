#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

// An in-memory audio clip. Samples are stored as 32-bit floats in the
// range [-1, 1], one vector per channel (non-interleaved).
class AudioClip {
public:
    AudioClip() = default;
    AudioClip(uint32_t sampleRate, uint16_t numChannels, std::size_t numFrames);

    // WAV I/O (implemented from scratch, no external libraries).
    // Reading supports PCM 8/16/24/32-bit and IEEE float 32/64-bit.
    // Writing always produces 16-bit PCM.
    static AudioClip loadWav(const std::string& path);
    void saveWav(const std::string& path) const;

    uint32_t sampleRate() const { return sampleRate_; }
    uint16_t numChannels() const { return static_cast<uint16_t>(channels_.size()); }
    std::size_t numFrames() const { return channels_.empty() ? 0 : channels_[0].size(); }
    double duration() const;
    bool empty() const { return numFrames() == 0; }

    std::vector<float>& channel(std::size_t c) { return channels_.at(c); }
    const std::vector<float>& channel(std::size_t c) const { return channels_.at(c); }
    std::vector<std::vector<float>>& channels() { return channels_; }
    const std::vector<std::vector<float>>& channels() const { return channels_; }

    void setSampleRate(uint32_t sr) { sampleRate_ = sr; }
    void resize(std::size_t numFrames);

    // Converts a time in seconds to a frame index, clamped to [0, numFrames].
    std::size_t secondsToFrame(double seconds) const;

    // Largest absolute sample value over all channels.
    float peak() const;

    // Average of all channels.
    std::vector<float> mixToMono() const;

private:
    uint32_t sampleRate_ = 44100;
    std::vector<std::vector<float>> channels_;
};
