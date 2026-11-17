#include "Effects.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

Speed::Speed(double factor) : factor_(factor) {
    if (factor <= 0) throw std::invalid_argument("speed factor must be positive");
}

void Speed::apply(AudioClip& clip) const {
    const std::size_t oldLen = clip.numFrames();
    if (oldLen == 0) return;
    const auto newLen = static_cast<std::size_t>(std::floor(oldLen / factor_));

    for (auto& ch : clip.channels()) {
        std::vector<float> out(newLen);
        for (std::size_t i = 0; i < newLen; ++i) {
            const double src = i * factor_;
            const auto j = static_cast<std::size_t>(src);
            const double frac = src - j;
            const float a = ch[std::min(j, oldLen - 1)];
            const float b = ch[std::min(j + 1, oldLen - 1)];
            out[i] = static_cast<float>(a + (b - a) * frac);
        }
        ch = std::move(out);
    }
}
