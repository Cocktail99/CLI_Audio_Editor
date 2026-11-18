#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct Rgb {
    uint8_t r, g, b;
};

// pixels[y][x], y = 0 is the top row.
using Image = std::vector<std::vector<Rgb>>;

// Writes an image as an uncompressed 24-bit BMP file.
void writeBmp(const std::string& path, const Image& pixels);

// Maps t in [0, 1] to a "heat" colour: black -> purple -> red -> yellow -> white.
Rgb heatColor(double t);
