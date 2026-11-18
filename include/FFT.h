#pragma once

#include <complex>
#include <vector>

namespace fft {

// In-place iterative radix-2 Cooley-Tukey FFT.
// The input size must be a power of two.
void transform(std::vector<std::complex<double>>& a, bool inverse = false);

bool isPowerOfTwo(std::size_t n);

// Hann window of length n, used to reduce spectral leakage.
std::vector<double> hannWindow(std::size_t n);

}  // namespace fft
