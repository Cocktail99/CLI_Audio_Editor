#include <iomanip>
#include <ostream>

#include "Editor.h"
#include "Spectrogram.h"

using namespace cmd;

void Editor::handleSpectrogram(const Args& args, std::ostream& out) {
    expectArgs(args, 1, 2, "SPECTROGRAM <image.bmp> [fft size]");
    const auto size = args.size() > 1 ? static_cast<std::size_t>(toNumber(args[1])) : 4096;
    spectrogram::renderBmp(requireClip(), args[0], size);
    out << "Spectrogram written to " << args[0] << '\n';
}

void Editor::handlePeakFreq(const Args& args, std::ostream& out) {
    expectArgs(args, 0, 0, "PEAKFREQ");
    out << "Dominant frequency: " << std::fixed << std::setprecision(1)
        << spectrogram::dominantFrequency(requireClip()) << " Hz\n"
        << std::defaultfloat;
}
