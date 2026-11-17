#include "Editing.h"
#include "EditingUtils.h"

namespace editing {

// Overlays `other` on top of the clip, starting at `at` seconds.
void mix(AudioClip& clip, const AudioClip& other, double at) {
    detail::checkSameRate(clip, other);
    if (at < 0) throw std::invalid_argument("position must be non-negative");
    AudioClip matched = detail::matchChannels(other, clip.numChannels());
    const auto offset = static_cast<std::size_t>(at * clip.sampleRate());
    if (offset + matched.numFrames() > clip.numFrames()) clip.resize(offset + matched.numFrames());
    for (uint16_t c = 0; c < clip.numChannels(); ++c) {
        auto& dst = clip.channel(c);
        const auto& src = matched.channel(c);
        for (std::size_t i = 0; i < src.size(); ++i) dst[offset + i] += src[i];
    }
}

}  // namespace editing
