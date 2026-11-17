#include "Effects.h"

#include <stdexcept>

FadeIn::FadeIn(double seconds) : seconds_(seconds) {
    if (seconds <= 0) throw std::invalid_argument("fade length must be positive");
}

void FadeIn::apply(AudioClip& clip) const {
    const std::size_t n = clip.secondsToFrame(seconds_);
    if (n == 0) return;
    for (auto& ch : clip.channels())
        for (std::size_t i = 0; i < n; ++i) ch[i] *= static_cast<float>(i) / n;
}

FadeOut::FadeOut(double seconds) : seconds_(seconds) {
    if (seconds <= 0) throw std::invalid_argument("fade length must be positive");
}

void FadeOut::apply(AudioClip& clip) const {
    const std::size_t total = clip.numFrames();
    const std::size_t n = clip.secondsToFrame(seconds_);
    if (n == 0) return;
    for (auto& ch : clip.channels())
        for (std::size_t i = 0; i < n; ++i) ch[total - 1 - i] *= static_cast<float>(i) / n;
}
