#pragma once

struct ImageCoordinates {
    long x;
    long y;
};

ImageCoordinates predictRainImageCoordinates(const double latitude, const double longitude);

ImageCoordinates predictTemperatureImageCoordinates(const double latitude, const double longitude);
