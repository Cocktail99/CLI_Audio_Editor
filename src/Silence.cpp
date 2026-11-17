#include "Editing.h"
#include "EditingUtils.h"

namespace editing {

void insertSilence(AudioClip& clip, double at, double seconds) {
    if (seconds <= 0) throw std::invalid_argument("silence length must be positive");
    const std::size_t pos = clip.secondsToFrame(at);
    const auto n = static_cast<std::size_t>(seconds * clip.sampleRate());
    for (auto& ch : clip.channels()) ch.insert(ch.begin() + pos, n, 0.0f);
}

}  // namespace editing
