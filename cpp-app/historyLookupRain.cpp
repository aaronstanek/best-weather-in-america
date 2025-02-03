#include "historyLookup.h"

#include <cmath>

#include "rain.bin.h"

char getRainPixelAtRaw(const char* const rainPixels, const long mapIndex, const long x, const long y) {
    if (mapIndex < 0 || mapIndex >= RAIN_MAP_COUNT || x < 0 || x >= RAIN_MAP_WIDTH || y < 0 || y >= RAIN_MAP_HEIGHT) {
        return 100;
    }
    return rainPixels[
        mapIndex + (x * RAIN_MAP_COUNT) + (y * RAIN_MAP_COUNT * RAIN_MAP_WIDTH)
    ];
}

double decodeRainPixel(const char pixel) {
    switch (pixel) {
        case 0:
            return 0.005;
        case 1:
            return 0.055;
        case 2:
            return 0.175;
        case 3:
            return 0.375;
        case 4:
            return 0.625;
        case 5:
            return 0.875;
        case 6:
            return 1.25;
        case 7:
            return 1.75;
        case 8:
            return 2.5;
        case 9:
            return 3.5;
        case 10:
            return 4.5;
        case 11:
            return 5.5;
        case 12:
            return 7.0;
        case 13:
            return 9.0;
        case 14:
            return 11.0;
        case 15:
            return 13.0;
        default:
            return NAN;
    }
}

void reduceRain(double& accumulator, int& count, const char* const rainPixels, const long mapIndex, const long x, const long y) {
    const double rain = decodeRainPixel(getRainPixelAtRaw(rainPixels, mapIndex, x, y));
    if (std::isnan(rain)) return;
    accumulator += rain;
    count++;
}

double getRainPixelAtStable(const char* const rainPixels, const long mapIndex, const long x, const long y) {
    double accumulator = 0;
    int count = 0;
    reduceRain(accumulator, count, rainPixels, mapIndex, x, y);
    if (count) {
        return accumulator;
    }
    for (long delta = 1; true; delta++) {
        for (long xPrime = x - delta; xPrime <= x + delta; xPrime++) {
            reduceRain(accumulator, count, rainPixels, mapIndex, xPrime, y - delta);
            reduceRain(accumulator, count, rainPixels, mapIndex, xPrime, y + delta);
        }
        for (long yPrime = y - delta + 1; yPrime <= y + delta - 1; yPrime++) {
            reduceRain(accumulator, count, rainPixels, mapIndex, x - delta, yPrime);
            reduceRain(accumulator, count, rainPixels, mapIndex, x + delta, yPrime);
        }
        if (count) {
            return accumulator / ((double)count);
        }
    }
}

double getHistoricalRainAt(const char* const rainPixels, const std::vector<double>& normalizedWeights, const long x, const long y) {
    double accumulator = 0;
    for (long mapIndex = 0; mapIndex < RAIN_MAP_COUNT; mapIndex++) {
        accumulator += getRainPixelAtStable(rainPixels, mapIndex, x, y) * normalizedWeights[mapIndex];
    }
    return accumulator;
}