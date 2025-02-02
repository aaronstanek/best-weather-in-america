#include "predictImageCoordinates.h"
#include <cmath>

ImageCoordinates predictImageCoordinates(
    const double latitude, const double longitude,
    const double NX, const double NY, const double S, const double R,
    const double lat0, const double lat1, const double lat2) {
        const double correctedLatitude = lat0 + lat1 * latitude + lat2 * latitude * latitude;
        const double pixelDistanceFromPole = (90 - correctedLatitude) * S;
        const double radianAngleFromDown = (longitude + R) * (((double)M_PI)/((double)180));
        const double unitCircleX = sin(radianAngleFromDown);
        const double unitCircleY = cos(radianAngleFromDown);
        return {
            (long)round(NX + unitCircleX * pixelDistanceFromPole),
            (long)round(NY + unitCircleY * pixelDistanceFromPole)
        };
}

ImageCoordinates predictRainImageCoordinates(const double latitude, const double longitude) {
    return predictImageCoordinates(
        latitude, longitude,
        150.51279808083137, -253.99945748328085, 3.1666154278610636, 99.995290126437,
        -147.13388596796284, 3.5709933172935293, -0.012141193812205082
    );
}

ImageCoordinates predictTemperatureImageCoordinates(const double latitude, const double longitude) {
    return predictImageCoordinates(
        latitude, longitude,
        151.62289183353775, -252.83894805515092, 3.8557063183193443, 99.95384670002451,
        -103.80827326350413, 2.896012697373892, -0.00955657365342634
    );
}