#include <fstream>
#include <stdexcept>

#include "ByteIO.h"
#include "Image.h"

using byteio::writeLE16;
using byteio::writeLE32;

void writeBmp(const std::string& path, const Image& pixels) {
    const auto height = static_cast<uint32_t>(pixels.size());
    const auto width = static_cast<uint32_t>(pixels.empty() ? 0 : pixels[0].size());
    const uint32_t rowSize = (width * 3 + 3) & ~3u;  // rows are padded to 4 bytes
    const uint32_t dataSize = rowSize * height;

    std::ofstream out(path, std::ios::binary);
    if (!out) throw std::runtime_error("cannot write file '" + path + "'");

    // BITMAPFILEHEADER
    out.write("BM", 2);
    writeLE32(out, 54 + dataSize);
    writeLE32(out, 0);
    writeLE32(out, 54);
    // BITMAPINFOHEADER
    writeLE32(out, 40);
    writeLE32(out, width);
    writeLE32(out, height);
    writeLE16(out, 1);   // colour planes
    writeLE16(out, 24);  // bits per pixel
    writeLE32(out, 0);   // no compression
    writeLE32(out, dataSize);
    writeLE32(out, 2835);  // 72 DPI
    writeLE32(out, 2835);
    writeLE32(out, 0);
    writeLE32(out, 0);

    // BMP stores rows bottom-up, pixels as BGR.
    std::vector<char> row(rowSize, 0);
    for (uint32_t y = height; y-- > 0;) {
        for (uint32_t x = 0; x < width; ++x) {
            row[x * 3 + 0] = static_cast<char>(pixels[y][x].b);
            row[x * 3 + 1] = static_cast<char>(pixels[y][x].g);
            row[x * 3 + 2] = static_cast<char>(pixels[y][x].r);
        }
        out.write(row.data(), rowSize);
    }
}
