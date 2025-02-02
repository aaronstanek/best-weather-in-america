#include "updateWeights.h"

#include "historyLookup.h"

#include "rain.bin.h"
#include "temperature.bin.h"

#include <cmath>

void updateRainWeights(const char* const rainPixels, double* const rainWeights, const long x, const long y, const double observation) {
    const double standardDeviation = getHistoricalRainAt(rainPixels, rainWeights, x, y).error * sqrt(RAIN_MAP_COUNT-1);
    const Uncertain center = {observation, standardDeviation};
    double total = 0;
    for (long mapIndex = 0; mapIndex < RAIN_MAP_COUNT; mapIndex++) {
        const Uncertain pixel = getRainPixelAtStable(rainPixels, mapIndex, x, y);
        double updatedWeight = rainWeights[mapIndex] * pValue(center, pixel);
        if (updatedWeight < 1e-100) {
            updatedWeight = 1e-100;
        }
        rainWeights[mapIndex] = updatedWeight;
        total += updatedWeight;
    }
    for (long mapIndex = 0; mapIndex < RAIN_MAP_COUNT; mapIndex++) {
        rainWeights[mapIndex] /= total;
    }
}

void updateTemperatureWeights(const char* const temperaturePixels, double* const temperatureWeights, const long x, const long y, const double observation) {
    const double standardDeviation = getHistoricalTemperatureAt(temperaturePixels, temperatureWeights, x, y).error * sqrt(TEMPERATURE_MAP_COUNT-1);
    const Uncertain center = {observation, standardDeviation};
    double total = 0;
    for (long mapIndex = 0; mapIndex < TEMPERATURE_MAP_COUNT; mapIndex++) {
        const Uncertain pixel = getTemperaturePixelAtStable(temperaturePixels, mapIndex, x, y);
        double updatedWeight = temperatureWeights[mapIndex] * pValue(center, pixel);
        if (updatedWeight < 1e-100) {
            updatedWeight = 1e-100;
        }
        temperatureWeights[mapIndex] = updatedWeight;
        total += updatedWeight;
    }
    for (long mapIndex = 0; mapIndex < TEMPERATURE_MAP_COUNT; mapIndex++) {
        temperatureWeights[mapIndex] /= total;
    }
}