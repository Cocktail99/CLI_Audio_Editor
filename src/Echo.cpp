#include "Effects.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

Echo::Echo(double delaySeconds, double decay) : delay_(delaySeconds), decay_(decay) {
    if (delaySeconds <= 0) throw std::invalid_argument("echo delay must be positive");
    if (decay <= 0 || decay >= 1) throw std::invalid_argument("echo decay must be in (0, 1)");
}

void Echo::apply(AudioClip& clip) const {
    const auto d = static_cast<std::size_t>(delay_ * clip.sampleRate());
    if (d == 0) return;

    // Number of repeats until the echo is quieter than -60 dB (capped).
    const int repeats = std::min(20, static_cast<int>(std::ceil(std::log(0.001) / std::log(decay_))));
    clip.resize(clip.numFrames() + d * repeats);

    for (auto& ch : clip.channels())
        for (std::size_t i = d; i < ch.size(); ++i) ch[i] += static_cast<float>(decay_ * ch[i - d]);
}
