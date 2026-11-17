#include "Editing.h"
#include "EditingUtils.h"

namespace editing {

// Removes the samples between start and end.
void cut(AudioClip& clip, double start, double end) {
    detail::checkRange(clip, start, end);
    const std::size_t a = clip.secondsToFrame(start);
    const std::size_t b = clip.secondsToFrame(end);
    for (auto& ch : clip.channels()) ch.erase(ch.begin() + a, ch.begin() + b);
}

}  // namespace editing
