#include "temperature.bin.h"

#include <fstream>

char* loadTemperaturePixels() {
    char* temperaturePixels = new char[TEMPERATURE_MAP_COUNT * TEMPERATURE_MAP_WIDTH * TEMPERATURE_MAP_HEIGHT];
    std::ifstream file("temperature.bin", std::ios::binary);
    file.read(temperaturePixels, TEMPERATURE_MAP_COUNT * TEMPERATURE_MAP_WIDTH * TEMPERATURE_MAP_HEIGHT);
    return temperaturePixels;
}