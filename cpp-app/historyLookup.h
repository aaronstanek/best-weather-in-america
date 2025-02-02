#pragma once

double getRainPixelAtStable(const char* const rainPixels, const long mapIndex, const long x, const long y);

double getHistoricalRainAt(const char* const rainPixels, const double* const normalizedWeights, const long x, const long y);

double getTemperaturePixelAtStable(const char* const temperaturePixels, const long mapIndex, const long x, const long y);

double getHistoricalTemperatureAt(const char* const temperaturePixels, const double* const normalizedWeights, const long x, const long y);
