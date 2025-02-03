#pragma once

#include <vector>

void updateRainWeights(const std::vector<char>& rainPixels, std::vector<double>& rainWeights, const long x, const long y, const double observation);

void updateTemperatureWeights(const std::vector<char>& temperaturePixels, std::vector<double>& temperatureWeights, const long x, const long y, const double observation);