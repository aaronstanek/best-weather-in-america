#pragma once

#include <vector>

double getRainPixelAtStable(const std::vector<char>& rainPixels, const long mapIndex, const long x, const long y);

double getHistoricalRainAt(const std::vector<char>& rainPixels, const std::vector<double>& normalizedWeights, const long x, const long y);

double getTemperaturePixelAtStable(const std::vector<char>& temperaturePixels, const long mapIndex, const long x, const long y);

double getHistoricalTemperatureAt(const std::vector<char>& temperaturePixels, const std::vector<double>& normalizedWeights, const long x, const long y);
