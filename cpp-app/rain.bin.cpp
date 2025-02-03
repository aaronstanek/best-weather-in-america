#include "rain.bin.h"

#include <fstream>

#define RAIN_PIXEL_COUNT (RAIN_MAP_COUNT * RAIN_MAP_WIDTH * RAIN_MAP_HEIGHT)

void loadRainPixels(std::vector<char>& rainPixels) {
    rainPixels.resize(RAIN_PIXEL_COUNT);
    std::ifstream file("rain.bin", std::ios::binary);
    file.read(rainPixels.data(), RAIN_PIXEL_COUNT);
}