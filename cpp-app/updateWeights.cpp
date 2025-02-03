#include "updateWeights.h"

#include "historyLookup.h"

#include <cmath>
#include <algorithm>

struct SortArrayElement {
    long mapIndex;
    double error;
};

bool isABeforeB(SortArrayElement& a, SortArrayElement& b) {
    return a.error < b.error;
}

void updateFromSortArray(std::vector<SortArrayElement>& sortArray, std::vector<double>& weights) {
    std::sort(sortArray.begin(), sortArray.end(), isABeforeB);
    // sortArray is sorted from small error to large error
    const double exponentDenominator = sortArray.size() - 1;
    double totalWeight = 0;
    for (long i = 0; i < sortArray.size(); i++) {
        const long mapIndex = sortArray[i].mapIndex;
        double newWeight = weights[mapIndex] * pow(0.25, ((double)i)/exponentDenominator);
        newWeight = std::max(newWeight, 1e-100);
        weights[mapIndex] = newWeight;
        totalWeight += newWeight;
    }
    for (long mapIndex = 0; mapIndex < weights.size(); mapIndex++) {
        weights[mapIndex] /= totalWeight;
    }
}

void updateRainWeights(const std::vector<char>& rainPixels, std::vector<double>& rainWeights, const long x, const long y, const double observation) {
    std::vector<SortArrayElement> sortArray;
    sortArray.reserve(rainWeights.size());
    for (long mapIndex = 0; mapIndex < rainWeights.size(); mapIndex++) {
        double distance = getRainPixelAtStable(rainPixels, mapIndex, x, y) - observation;
        sortArray.push_back({mapIndex, distance*distance});
    }
    updateFromSortArray(sortArray, rainWeights);
}

void updateTemperatureWeights(const std::vector<char>& temperaturePixels, std::vector<double>& temperatureWeights, const long x, const long y, const double observation) {
    std::vector<SortArrayElement> sortArray;
    sortArray.reserve(temperatureWeights.size());
    for (long mapIndex = 0; mapIndex < temperatureWeights.size(); mapIndex++) {
        double distance = getTemperaturePixelAtStable(temperaturePixels, mapIndex, x, y) - observation;
        sortArray.push_back({mapIndex, distance*distance});
    }
    updateFromSortArray(sortArray, temperatureWeights);
}