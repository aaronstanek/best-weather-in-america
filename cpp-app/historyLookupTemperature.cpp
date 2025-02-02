#include "historyLookup.h"

#include <cmath>

#include "temperature.bin.h"

char getTemperaturePixelAtRaw(const char* const temperaturePixels, const long mapIndex, const long x, const long y) {
    if (mapIndex < 0 || mapIndex >= TEMPERATURE_MAP_COUNT || x < 0 || x >= TEMPERATURE_MAP_WIDTH || y < 0 || y >= TEMPERATURE_MAP_HEIGHT) {
        return 100;
    }
    return temperaturePixels[
        mapIndex + (x * TEMPERATURE_MAP_COUNT) + (y * TEMPERATURE_MAP_COUNT * TEMPERATURE_MAP_WIDTH)
    ];
}

double decodeTemperaturePixel(const char pixel) {
    switch (pixel) {
        case 0:
            return -5.0;
        case 1:
            return 5.0;
        case 2:
            return 15.0;
        case 3:
            return 25.0;
        case 4:
            return 35.0;
        case 5:
            return 45.0;
        case 6:
            return 55.0;
        case 7:
            return 65.0;
        case 8:
            return 75.0;
        case 9:
            return 85.0;
        case 10:
            return 95.0;
        case 11:
            return 105.0;
        default:
            return NAN;
    }
}

void reduceTemperature(double& accumulator, int& count, const char* const rainPixels, const long mapIndex, const long x, const long y) {
    const double rain = decodeTemperaturePixel(getTemperaturePixelAtRaw(rainPixels, mapIndex, x, y));
    if (std::isnan(rain)) return;
    accumulator += rain;
    count++;
}

double getTemperaturePixelAtStable(const char* const temperaturePixels, const long mapIndex, const long x, const long y) {
    double accumulator = 0;
    int count = 0;
    reduceTemperature(accumulator, count, temperaturePixels, mapIndex, x, y);
    if (count) {
        return accumulator;
    }
    for (long delta = 1; true; delta++) {
        for (long xPrime = x - delta; xPrime <= x + delta; xPrime++) {
            reduceTemperature(accumulator, count, temperaturePixels, mapIndex, xPrime, y - delta);
            reduceTemperature(accumulator, count, temperaturePixels, mapIndex, xPrime, y + delta);
        }
        for (long yPrime = y - delta + 1; yPrime <= y + delta - 1; yPrime++) {
            reduceTemperature(accumulator, count, temperaturePixels, mapIndex, x - delta, yPrime);
            reduceTemperature(accumulator, count, temperaturePixels, mapIndex, x + delta, yPrime);
        }
        if (count) {
            return accumulator / ((double)count);
        }
    }
}

double getHistoricalTemperatureAt(const char* const temperaturePixels, const double* const normalizedWeights, const long x, const long y) {
    double accumulator = 0;
    for (long mapIndex = 0; mapIndex < TEMPERATURE_MAP_COUNT; mapIndex++) {
        accumulator += getTemperaturePixelAtStable(temperaturePixels, mapIndex, x, y) * normalizedWeights[mapIndex];
    }
    return accumulator;
}