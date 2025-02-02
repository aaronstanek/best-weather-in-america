#include "geoBase.h"

bool isIntersection(Point candidate, Line segment) {
    const double parametrizedDistanceToYIntercept = (candidate.longitude - segment.startInclusive.longitude) / (segment.endExclusive.longitude - segment.startInclusive.longitude);
    if (parametrizedDistanceToYIntercept < 0 || parametrizedDistanceToYIntercept >= 1) return false;
    const double yIntercept = segment.startInclusive.latitude + parametrizedDistanceToYIntercept * (segment.endExclusive.latitude - segment.startInclusive.latitude);
    return (yIntercept >= candidate.latitude);
}