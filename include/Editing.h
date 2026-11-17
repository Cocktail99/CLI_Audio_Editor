#pragma once

#include "AudioClip.h"

// Structural edits: operations that change which samples are in the clip.
namespace editing {

// Keeps only the part between start and end (in seconds).
void crop(AudioClip& clip, double start, double end);

// Removes the part between start and end (in seconds).
void cut(AudioClip& clip, double start, double end);

}  // namespace editing
