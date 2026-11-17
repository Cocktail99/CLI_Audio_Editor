#include "Effects.h"

#include <algorithm>

void Reverse::apply(AudioClip& clip) const {
    for (auto& ch : clip.channels()) std::reverse(ch.begin(), ch.end());
}
