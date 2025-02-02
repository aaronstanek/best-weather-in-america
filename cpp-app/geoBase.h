#pragma once

struct Point {
    double latitude;
    double longitude;
};

struct Line {
    Point startInclusive;
    Point endExclusive;
};

bool isIntersection(Point candidate, Line segment);
