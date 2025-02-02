#include "historyLookup.h"

#include "temperature.bin.h"

char getTemperaturePixelAtRaw(const char* const temperaturePixels, const long mapIndex, const long x, const long y) {
    if (mapIndex < 0 || mapIndex >= TEMPERATURE_MAP_COUNT || x < 0 || x >= TEMPERATURE_MAP_WIDTH || y < 0 || y >= TEMPERATURE_MAP_HEIGHT) {
        return 100;
    }
    return temperaturePixels[
        mapIndex + (x * TEMPERATURE_MAP_COUNT) + (y * TEMPERATURE_MAP_COUNT * TEMPERATURE_MAP_WIDTH)
    ];
}

bool decodeTemperaturePixel(Uncertain& output, const char pixel) {
    switch (pixel) {
        case 0:
            output = {-5.0, 5.0};
            return true;
        case 1:
            output = {5.0, 5.0};
            return true;
        case 2:
            output = {15.0, 5.0};
            return true;
        case 3:
            output = {25.0, 5.0};
            return true;
        case 4:
            output = {35.0, 5.0};
            return true;
        case 5:
            output = {45.0, 5.0};
            return true;
        case 6:
            output = {55.0, 5.0};
            return true;
        case 7:
            output = {65.0, 5.0};
            return true;
        case 8:
            output = {75.0, 5.0};
            return true;
        case 9:
            output = {85.0, 5.0};
            return true;
        case 10:
            output = {95.0, 5.0};
            return true;
        case 11:
            output = {105.0, 5.0};
            return true;
        default:
            return false;
    }
}

Uncertain getTemperaturePixelAtStable(const char* const temperaturePixels, const long mapIndex, const long x, const long y) {
    Uncertain t;
    if (decodeTemperaturePixel(t, getTemperaturePixelAtRaw(temperaturePixels, mapIndex, x, y))) {
        return t;
    }
    for (long delta = 1; true; delta++) {
        Uncertain accumulator = {0, 0};
        int count = 0;
        for (long xPrime = x - delta; xPrime <= x + delta; xPrime++) {
            if (decodeTemperaturePixel(t, getTemperaturePixelAtRaw(temperaturePixels, mapIndex, xPrime, y - delta))) {
                accumulator = add(accumulator, t);
                count++;
            }
            if (decodeTemperaturePixel(t, getTemperaturePixelAtRaw(temperaturePixels, mapIndex, xPrime, y + delta))) {
                accumulator = add(accumulator, t);
                count++;
            }
        }
        for (long yPrime = y - delta + 1; yPrime <= y + delta - 1; yPrime++) {
            if (decodeTemperaturePixel(t, getTemperaturePixelAtRaw(temperaturePixels, mapIndex, x - delta, yPrime))) {
                accumulator = add(accumulator, t);
                count++;
            }
            if (decodeTemperaturePixel(t, getTemperaturePixelAtRaw(temperaturePixels, mapIndex, x + delta, yPrime))) {
                accumulator = add(accumulator, t);
                count++;
            }
        }
        if (count < 1) {
            continue;
        }
        return scale(accumulator, ((double)1) / ((double)count) );
    }
}

Uncertain getHistoricalTemperatureAt(const char* const temperaturePixels, const double* const normalizedWeights, const long x, const long y) {
    Uncertain accumulator = {0, 0};
    for (long mapIndex = 0; mapIndex < TEMPERATURE_MAP_COUNT; mapIndex++) {
        accumulator = add(
            accumulator,
            scale(
                getTemperaturePixelAtStable(temperaturePixels, mapIndex, x, y),
                normalizedWeights[mapIndex]
                )
            );
    }
    return accumulator;
}