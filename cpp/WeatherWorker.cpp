#include "WeatherWorker.h"

#include "randomGenerator.h"
#include "boundaries.h"
#include "randomPointInUs.h"
#include <math.h>
#include <algorithm>

#define AS_RADIANS(x) ((((double)M_PI)/((double)180)) * (x))
#define AS_DEGREES(x) ((((double)180)/((double)M_PI)) * (x))

ThreeVec computeThreeVec(Coordinate coordinate) {
    const double latitudeRadians = AS_RADIANS(coordinate.latitude);
    const double longitudeRadians = AS_RADIANS(coordinate.longitude);
    const double cosLatitudeRadians = cos(latitudeRadians);
    return {
        cosLatitudeRadians * sin(longitudeRadians),
        cosLatitudeRadians * cos(longitudeRadians),
        sin(latitudeRadians)
    };
}

double computeAngularDistance(ThreeVec a, ThreeVec b) {
    double aDotB = (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
    if (aDotB > 1) {
        aDotB = 1;
    } else if (aDotB < -1) {
        aDotB = -1;
    }
    return AS_DEGREES(acos(aDotB));
}

double computeBadnessScore(double temperature, double humidity, double wind, double rain) {
    double score = 0.0002753038612347609 * pow(temperature - 70, 2);
    score += 0.00010046751836904414 * pow(humidity - 50, 2);
    score += 0.00014421437280512476 * pow(wind - 5, 2);
    score += 0.07129086834186059 * pow(rain, 0.3739738949262469);
    return score;
}

size_t computeLongitudeBucket(const double longitude) {
    if (longitude < -97.3938893834258) {
        if (longitude < -112.3180496729619) {
            return 0;
        } else {
            return 1;
        }
    } else {
        if (longitude < -84.82014756760478) {
            return 2;
        } else {
            return 3;
        }
    }
}

WeatherWorker::WeatherWorker() {
    for (unsigned char i = 0; i < 4; i++) {
        buckets[i] = 0;
    }
    coordinateRegister = {0, 0};
}

void WeatherWorker::setLocationFar() {
    while (true) {
        Coordinate candidate = randomPointInUs();
        const size_t bucketId = computeLongitudeBucket(candidate.longitude);
        if (buckets[bucketId] >= 4) {
            continue;
        }
        coordinateRegister = candidate;
        return;
    }
}

#define NEARBY_ARRAY_LENGTH (2)

void WeatherWorker::setLocationNearby() {
    size_t bestArrayLength = 0;
    size_t bestArray[NEARBY_ARRAY_LENGTH];
    for (size_t h = 0; h < history.size(); h++) {
        bool addedValue = false;
        if (bestArrayLength < NEARBY_ARRAY_LENGTH) {
            bestArray[bestArrayLength] = h;
            bestArrayLength++;
            addedValue = true;
        } else if (history.at(h).badnessScore < history.at(bestArray[NEARBY_ARRAY_LENGTH-1]).badnessScore) {
            bestArray[NEARBY_ARRAY_LENGTH-1] = h;
            addedValue = true;
        }
        if (!addedValue) {
            continue;
        }
        for (size_t a = bestArrayLength-1; a > 0; a--) {
            if (history.at(bestArray[a-1]).badnessScore <= history.at(bestArray[a]).badnessScore) {
                break;
            }
            const size_t t = bestArray[a-1];
            bestArray[a-1] = bestArray[a];
            bestArray[a] = t;
        }
    }
    for (unsigned short i = 0; i < 10000; i++) {
        Coordinate candidate = weightedRandomPointInNorthAmerica();
        ThreeVec candidateThreeVec = computeThreeVec(candidate);
        bool isClosestSet = false;
        size_t closestIndex = 0;
        double largestDotProduct;
        for (size_t h = 0; h < history.size(); h++) {
            const double dotProduct = (candidateThreeVec.x * history.at(h).threeVec.x) + (candidateThreeVec.y * history.at(h).threeVec.y) + (candidateThreeVec.z * history.at(h).threeVec.z);
            if (!isClosestSet || dotProduct > largestDotProduct) {
                isClosestSet = true;
                closestIndex = h;
                largestDotProduct = dotProduct;
            }
        }
        for (size_t a = 0; a < bestArrayLength; a++) {
            if (closestIndex == bestArray[a]) {
                if (isInUS(candidate)) {
                    coordinateRegister = candidate;
                    return;
                }
            }
        }
    }
    coordinateRegister = randomPointInUs();
}

void WeatherWorker::setLocation() {
    if (history.size() < 16) {
        setLocationFar();
    } else {
        setLocationNearby();
    }
}

void WeatherWorker::addToHistory(double latitude, double longitude, double temperature, double humidity, double wind, double rain) {
    HistoryElement newElement;
    newElement.coordinate = {latitude, longitude};
    newElement.threeVec = computeThreeVec(newElement.coordinate);
    newElement.badnessScore = computeBadnessScore(
        temperature,
        humidity,
        wind,
        rain
    );
    history.push_back(newElement);
    buckets[computeLongitudeBucket(longitude)]++;
}

size_t WeatherWorker::getBestIndex() {
    size_t bestIndexSoFar = 0;
    for (size_t i = 1; i < history.size(); i++) {
        if (history.at(i).badnessScore < history.at(bestIndexSoFar).badnessScore) {
            bestIndexSoFar = i;
        }
    }
    return bestIndexSoFar;
}
