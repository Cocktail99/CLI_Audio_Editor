#include "Effects.h"

#include <stdexcept>

Normalize::Normalize(double targetPeak) : target_(targetPeak) {
    if (targetPeak <= 0 || targetPeak > 1) throw std::invalid_argument("target peak must be in (0, 1]");
}

void Normalize::apply(AudioClip& clip) const {
    const float peak = clip.peak();
    if (peak == 0.0f) return;  // silence: nothing to scale
    Volume(target_ / peak).apply(clip);
}
