#include "FFT.h"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace {
constexpr double kPi = 3.14159265358979323846;
}

namespace fft {

bool isPowerOfTwo(std::size_t n) {
    return n != 0 && (n & (n - 1)) == 0;
}

void transform(std::vector<std::complex<double>>& a, bool inverse) {
    const std::size_t n = a.size();
    if (!isPowerOfTwo(n)) throw std::invalid_argument("FFT size must be a power of two");

    // 1. Reorder the input into bit-reversed index order.
    for (std::size_t i = 1, j = 0; i < n; ++i) {
        std::size_t bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) std::swap(a[i], a[j]);
    }

    // 2. Combine sub-transforms of size len/2 into size len ("butterflies").
    for (std::size_t len = 2; len <= n; len <<= 1) {
        const double angle = 2 * kPi / len * (inverse ? 1 : -1);
        const std::complex<double> wLen(std::cos(angle), std::sin(angle));
        for (std::size_t i = 0; i < n; i += len) {
            std::complex<double> w(1);
            for (std::size_t k = 0; k < len / 2; ++k) {
                const std::complex<double> even = a[i + k];
                const std::complex<double> odd = a[i + k + len / 2] * w;
                a[i + k] = even + odd;
                a[i + k + len / 2] = even - odd;
                w *= wLen;
            }
        }
    }

    if (inverse)
        for (auto& x : a) x /= static_cast<double>(n);
}

std::vector<double> hannWindow(std::size_t n) {
    std::vector<double> w(n);
    if (n == 1) { w[0] = 1; return w; }
    for (std::size_t i = 0; i < n; ++i) w[i] = 0.5 * (1 - std::cos(2 * kPi * i / (n - 1)));
    return w;
}

}  // namespace fft
