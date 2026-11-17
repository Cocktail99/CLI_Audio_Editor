#include "Editing.h"

namespace editing {

void toMono(AudioClip& clip) {
    std::vector<float> mono = clip.mixToMono();
    clip.channels() = {std::move(mono)};
}

}  // namespace editing
