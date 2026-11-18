#include <algorithm>
#include <array>

#include "Image.h"

Rgb heatColor(double t) {
    static const std::array<std::array<double, 3>, 5> stops = {{
        {0, 0, 0}, {80, 18, 123}, {220, 50, 50}, {250, 200, 40}, {255, 255, 255},
    }};
    t = std::clamp(t, 0.0, 1.0) * (stops.size() - 1);
    const auto i = std::min(static_cast<std::size_t>(t), stops.size() - 2);
    const double f = t - i;
    auto mix = [&](int c) { return static_cast<uint8_t>(stops[i][c] + (stops[i + 1][c] - stops[i][c]) * f); };
    return {mix(0), mix(1), mix(2)};
}
