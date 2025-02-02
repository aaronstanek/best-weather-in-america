#include "rain.bin.h"

#include <fstream>

char* loadRainPixels() {
    char* rainPixels = new char[RAIN_MAP_COUNT * RAIN_MAP_WIDTH * RAIN_MAP_HEIGHT];
    std::ifstream file("rain.bin", std::ios::binary);
    file.read(rainPixels, RAIN_MAP_COUNT * RAIN_MAP_WIDTH * RAIN_MAP_HEIGHT);
    return rainPixels;
}