#include "historyLookup.h"

#include "rain.bin.h"

char getRainPixelAtRaw(const char* const rainPixels, const long mapIndex, const long x, const long y) {
    if (mapIndex < 0 || mapIndex >= RAIN_MAP_COUNT || x < 0 || x >= RAIN_MAP_WIDTH || y < 0 || y >= RAIN_MAP_HEIGHT) {
        return 100;
    }
    return rainPixels[
        mapIndex + (x * RAIN_MAP_COUNT) + (y * RAIN_MAP_COUNT * RAIN_MAP_WIDTH)
    ];
}

bool decodeRainPixel(Uncertain& output, const char pixel) {
    switch (pixel) {
        case 0:
            output = {0.005, 0.005};
            return true;
        case 1:
            output = {0.055, 0.045};
            return true;
        case 2:
            output = {0.175, 0.075};
            return true;
        case 3:
            output = {0.375, 0.125};
            return true;
        case 4:
            output = {0.625, 0.125};
            return true;
        case 5:
            output = {0.875, 0.125};
            return true;
        case 6:
            output = {1.25, 0.25};
            return true;
        case 7:
            output = {1.75, 0.25};
            return true;
        case 8:
            output = {2.5, 0.5};
            return true;
        case 9:
            output = {3.5, 0.5};
            return true;
        case 10:
            output = {4.5, 0.5};
            return true;
        case 11:
            output = {5.5, 0.5};
            return true;
        case 12:
            output = {7.0, 1.0};
            return true;
        case 13:
            output = {9.0, 1.0};
            return true;
        case 14:
            output = {11.0, 1.0};
            return true;
        case 15:
            output = {13.0, 1.0};
            return true;
        default:
            return false;
    }
}

Uncertain getRainPixelAtStable(const char* const rainPixels, const long mapIndex, const long x, const long y) {
    Uncertain t;
    if (decodeRainPixel(t, getRainPixelAtRaw(rainPixels, mapIndex, x, y))) {
        return t;
    }
    for (long delta = 1; true; delta++) {
        Uncertain accumulator = {0, 0};
        int count = 0;
        for (long xPrime = x - delta; xPrime <= x + delta; xPrime++) {
            if (decodeRainPixel(t, getRainPixelAtRaw(rainPixels, mapIndex, xPrime, y - delta))) {
                accumulator = add(accumulator, t);
                count++;
            }
            if (decodeRainPixel(t, getRainPixelAtRaw(rainPixels, mapIndex, xPrime, y + delta))) {
                accumulator = add(accumulator, t);
                count++;
            }
        }
        for (long yPrime = y - delta + 1; yPrime <= y + delta - 1; yPrime++) {
            if (decodeRainPixel(t, getRainPixelAtRaw(rainPixels, mapIndex, x - delta, yPrime))) {
                accumulator = add(accumulator, t);
                count++;
            }
            if (decodeRainPixel(t, getRainPixelAtRaw(rainPixels, mapIndex, x + delta, yPrime))) {
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

Uncertain getHistoricalRainAt(const char* const rainPixels, const double* const normalizedWeights, const long x, const long y) {
    Uncertain accumulator = {0, 0};
    for (long mapIndex = 0; mapIndex < RAIN_MAP_COUNT; mapIndex++) {
        accumulator = add(
            accumulator,
            scale(
                getRainPixelAtStable(rainPixels, mapIndex, x, y),
                normalizedWeights[mapIndex]
                )
            );
    }
    return accumulator;
}