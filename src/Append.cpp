#include "Editing.h"
#include "EditingUtils.h"

namespace editing {

void append(AudioClip& clip, const AudioClip& other) {
    detail::checkSameRate(clip, other);
    AudioClip matched = detail::matchChannels(other, clip.numChannels());
    for (uint16_t c = 0; c < clip.numChannels(); ++c) {
        auto& dst = clip.channel(c);
        const auto& src = matched.channel(c);
        dst.insert(dst.end(), src.begin(), src.end());
    }
}

}  // namespace editing
