#pragma once

#include <cstddef>
#include <vector>
#include "geometry.h"

struct HistoryElement {
    Coordinate coordinate;
    ThreeVec threeVec;
    double badnessScore;
};

class WeatherWorker {
    private:
    std::vector<HistoryElement> history;
    unsigned char buckets[4];
    void setLocationFar();
    void setLocationNearby();
    public:
    Coordinate coordinateRegister;
    WeatherWorker();
    void setLocation();
    void addToHistory(double latitude, double longitude, double temperature, double humidity, double wind, double rain);
    size_t getBestIndex();
};
