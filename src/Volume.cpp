#include "Effects.h"

#include <stdexcept>

Volume::Volume(double factor) : factor_(factor) {
    if (factor < 0) throw std::invalid_argument("volume factor must be non-negative");
}

void Volume::apply(AudioClip& clip) const {
    for (auto& ch : clip.channels())
        for (float& s : ch) s = static_cast<float>(s * factor_);
}
