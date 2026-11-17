#pragma once

#include "AudioClip.h"

// Structural edits: operations that change which samples are in the clip.
namespace editing {

// Keeps only the part between start and end (in seconds).
void crop(AudioClip& clip, double start, double end);

// Removes the part between start and end (in seconds).
void cut(AudioClip& clip, double start, double end);

// Appends another clip at the end. The other clip must have the same
// sample rate; channel counts are matched (mono <-> stereo).
void append(AudioClip& clip, const AudioClip& other);

// Overlays another clip starting at `at` seconds (samples are summed).
void mix(AudioClip& clip, const AudioClip& other, double at);

// Inserts `seconds` of silence at time `at`.
void insertSilence(AudioClip& clip, double at, double seconds);

// Converts the clip to a single channel by averaging.
void toMono(AudioClip& clip);

}  // namespace editing
