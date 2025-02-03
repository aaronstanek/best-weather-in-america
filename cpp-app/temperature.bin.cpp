#include "temperature.bin.h"

#include <fstream>

#define TEMPERATURE_PIXEL_COUNT (TEMPERATURE_MAP_COUNT * TEMPERATURE_MAP_WIDTH * TEMPERATURE_MAP_HEIGHT)

void loadTemperaturePixels(std::vector<char>& temperaturePixels) {
    temperaturePixels.resize(TEMPERATURE_PIXEL_COUNT);
    std::ifstream file("temperature.bin", std::ios::binary);
    file.read(temperaturePixels.data(), TEMPERATURE_PIXEL_COUNT);
}