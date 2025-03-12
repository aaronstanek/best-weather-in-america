#pragma once

struct Coordinate {
    double latitude;
    double longitude;
};

struct Line {
    Coordinate startInclusive;
    Coordinate endExclusive;
};

struct ThreeVec {
    double x;
    double y;
    double z;
};