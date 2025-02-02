#pragma once

void updateRainWeights(const char* const rainPixels, double* const rainWeights, const long x, const long y, const double observation);

void updateTemperatureWeights(const char* const temperaturePixels, double* const temperatureWeights, const long x, const long y, const double observation);