#include "randomGenerator.h"
#include "boundaries.h"
#include <cmath>

double roundTo4Places(double x) {
    return round(x * 1e4) / 1e4;
}

Coordinate weightedRandomPointInNorthAmerica() {
    Coordinate candidate;
    while (true) {
        candidate.latitude = roundTo4Places( 24.4 + randomGenerator.getRandom() * (49.4 - 24.4) );
        if (randomGenerator.getRandom() > 1.098 * cos( candidate.latitude * (((double)M_PI)/((double)180)) ) ) {
            continue;
        }
        candidate.longitude = roundTo4Places( -124.8 + randomGenerator.getRandom() * (124.8 - 66.8) );
        return candidate;
    }
}

Coordinate randomPointInUs() {
    Coordinate candidate;
    while (true) {
        candidate = weightedRandomPointInNorthAmerica();
        if (isInUS(candidate)) {
            return candidate;
        }
    }
}

