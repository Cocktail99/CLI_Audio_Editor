#include "Spectrogram.h"

#include <algorithm>
#include <cmath>
#include <complex>
#include <stdexcept>

#include "FFT.h"
#include "Image.h"

namespace {

constexpr double kDynamicRangeDb = 90.0;
constexpr double kMinFrequency = 40.0;     // bottom of the log-frequency axis
constexpr std::size_t kImageHeight = 600;  // pixels

}  // namespace

namespace spectrogram {

std::vector<std::vector<double>> compute(const AudioClip& clip, std::size_t fftSize, std::size_t maxFrames) {
    if (!fft::isPowerOfTwo(fftSize)) throw std::invalid_argument("FFT size must be a power of two");
    if (maxFrames == 0) throw std::invalid_argument("maxFrames must be positive");

    const std::vector<float> mono = clip.mixToMono();
    const std::vector<double> window = fft::hannWindow(fftSize);

    // Choose the hop size so the number of frames stays below maxFrames.
    std::size_t hop = std::max<std::size_t>(fftSize / 16, 1);
    if (mono.size() > fftSize) {
        const std::size_t needed = (mono.size() - fftSize + maxFrames - 1) / maxFrames;
        hop = std::max(hop, needed);
    }
    const std::size_t frames = mono.size() > fftSize ? (mono.size() - fftSize) / hop + 1 : 1;

    std::vector<std::vector<double>> result(frames, std::vector<double>(fftSize / 2));
    std::vector<std::complex<double>> buffer(fftSize);
    for (std::size_t f = 0; f < frames; ++f) {
        const std::size_t start = f * hop;
        for (std::size_t i = 0; i < fftSize; ++i) {
            const double s = start + i < mono.size() ? mono[start + i] : 0.0;
            buffer[i] = s * window[i];
        }
        fft::transform(buffer);
        for (std::size_t k = 0; k < fftSize / 2; ++k) {
            const double magnitude = std::abs(buffer[k]) / (fftSize / 2);
            result[f][k] = 20 * std::log10(magnitude + 1e-12);
        }
    }
    return result;
}

void renderBmp(const AudioClip& clip, const std::string& path, std::size_t fftSize, std::size_t maxWidth) {
    if (clip.empty()) throw std::invalid_argument("clip is empty");
    const auto spec = compute(clip, fftSize, maxWidth);
    const std::size_t width = spec.size();
    const std::size_t bins = fftSize / 2;
    const std::size_t height = kImageHeight;

    // Normalize relative to the loudest point so quiet files still show detail.
    double maxDb = -1e9;
    for (const auto& frame : spec)
        for (double v : frame) maxDb = std::max(maxDb, v);

    // The y axis is logarithmic (like human hearing): each row maps to a
    // frequency between kMinFrequency and Nyquist, then to a fractional FFT bin.
    const double nyquist = clip.sampleRate() / 2.0;
    const double minFreq = std::min(kMinFrequency, nyquist / 2);
    std::vector<double> rowBin(height);
    for (std::size_t row = 0; row < height; ++row) {
        const double freq = minFreq * std::pow(nyquist / minFreq, static_cast<double>(row) / (height - 1));
        rowBin[row] = std::min(freq / nyquist * bins, static_cast<double>(bins - 1));
    }

    Image pixels(height, std::vector<Rgb>(width));
    for (std::size_t x = 0; x < width; ++x) {
        for (std::size_t row = 0; row < height; ++row) {
            const auto k = static_cast<std::size_t>(rowBin[row]);
            const double frac = rowBin[row] - k;
            const double next = spec[x][std::min(k + 1, bins - 1)];
            const double db = spec[x][k] + (next - spec[x][k]) * frac;  // interpolate between bins
            const double t = (db - (maxDb - kDynamicRangeDb)) / kDynamicRangeDb;
            pixels[height - 1 - row][x] = heatColor(t);  // low frequencies at the bottom
        }
    }
    writeBmp(path, pixels);
}

double dominantFrequency(const AudioClip& clip, std::size_t fftSize) {
    if (clip.empty()) throw std::invalid_argument("clip is empty");
    const auto spec = compute(clip, fftSize, 200);

    // Sum the linear power of every bin across all frames.
    std::vector<double> power(fftSize / 2, 0.0);
    for (const auto& frame : spec)
        for (std::size_t k = 0; k < frame.size(); ++k) power[k] += std::pow(10.0, frame[k] / 10);

    const auto best = std::max_element(power.begin() + 1, power.end()) - power.begin();  // skip DC
    return static_cast<double>(best) * clip.sampleRate() / fftSize;
}

}  // namespace spectrogram
