#pragma once

#include <string>
#include <vector>

#include "AudioClip.h"

namespace spectrogram {

// Magnitudes in dB: result[frame][bin], bins from 0 Hz up to Nyquist.
std::vector<std::vector<double>> compute(const AudioClip& clip, std::size_t fftSize, std::size_t maxFrames);

// Renders a spectrogram (time on x, frequency on y) to a 24-bit BMP image.
void renderBmp(const AudioClip& clip, const std::string& path, std::size_t fftSize = 4096,
               std::size_t maxWidth = 1600);

// Returns the frequency (Hz) with the most energy in the whole clip.
double dominantFrequency(const AudioClip& clip, std::size_t fftSize = 8192);

}  // namespace spectrogram
