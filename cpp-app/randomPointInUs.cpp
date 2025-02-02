#include "randomGenerator.h"
#include "boundaries.h"
#include <cmath>

double roundTo4Places(double x) {
    return round(x*1e4) * 1e-4;
}

Point weightedRandomPointInNorthAmerica() {
    Point candidate;
    while (true) {
        candidate.latitude = 24.4 + randomGenerator.getRandom() * (49.4 - 24.4);
        if (randomGenerator.getRandom() > cos( candidate.latitude * (((double)M_PI)/((double)180)) ) ) {
            continue;
        }
        candidate.longitude = -124.8 + randomGenerator.getRandom() * (124.8 - 66.8);
        candidate.latitude = roundTo4Places(candidate.latitude);
        candidate.longitude = roundTo4Places(candidate.longitude);
        return candidate;
    }
}

Point randomPointInUs() {
    Point candidate;
    while (true) {
        candidate = weightedRandomPointInNorthAmerica();
        if (isInUS(candidate)) {
            return candidate;
        }
    }
}
