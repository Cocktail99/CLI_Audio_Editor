#include "Effects.h"

#include <cmath>
#include <stdexcept>

namespace {
constexpr double kPi = 3.14159265358979323846;
}

BiquadFilter::BiquadFilter(Type type, double cutoffHz, double q) : type_(type), cutoff_(cutoffHz), q_(q) {
    if (cutoffHz <= 0) throw std::invalid_argument("cutoff frequency must be positive");
    if (q <= 0) throw std::invalid_argument("Q must be positive");
}

std::string BiquadFilter::name() const {
    return type_ == Type::LowPass ? "low-pass filter" : "high-pass filter";
}

void BiquadFilter::apply(AudioClip& clip) const {
    const double fs = clip.sampleRate();
    if (cutoff_ >= fs / 2) throw std::invalid_argument("cutoff must be below " + std::to_string(fs / 2) + " Hz");

    const double w0 = 2 * kPi * cutoff_ / fs;
    const double cosw = std::cos(w0);
    const double alpha = std::sin(w0) / (2 * q_);

    double b0, b1, b2;
    if (type_ == Type::LowPass) {
        b0 = (1 - cosw) / 2;
        b1 = 1 - cosw;
        b2 = (1 - cosw) / 2;
    } else {
        b0 = (1 + cosw) / 2;
        b1 = -(1 + cosw);
        b2 = (1 + cosw) / 2;
    }
    const double a0 = 1 + alpha;
    const double a1 = -2 * cosw;
    const double a2 = 1 - alpha;

    // Normalize so a0 == 1.
    b0 /= a0; b1 /= a0; b2 /= a0;
    const double na1 = a1 / a0, na2 = a2 / a0;

    for (auto& ch : clip.channels()) {
        double x1 = 0, x2 = 0, y1 = 0, y2 = 0;  // filter state (Direct Form I)
        for (float& s : ch) {
            const double x0 = s;
            const double y0 = b0 * x0 + b1 * x1 + b2 * x2 - na1 * y1 - na2 * y2;
            x2 = x1; x1 = x0;
            y2 = y1; y1 = y0;
            s = static_cast<float>(y0);
        }
    }
}
