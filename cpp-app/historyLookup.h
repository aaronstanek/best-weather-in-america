#pragma once

#include "uncertainty.h"

Uncertain getRainPixelAtStable(const char* const rainPixels, const long mapIndex, const long x, const long y);

Uncertain getHistoricalRainAt(const char* const rainPixels, const double* const normalizedWeights, const long x, const long y);

Uncertain getTemperaturePixelAtStable(const char* const temperaturePixels, const long mapIndex, const long x, const long y);

Uncertain getHistoricalTemperatureAt(const char* const temperaturePixels, const double* const normalizedWeights, const long x, const long y);