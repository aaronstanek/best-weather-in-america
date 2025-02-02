#pragma once

struct Uncertain {
    double mean;
    double error;
};

Uncertain scale(const Uncertain a, const double factor);

Uncertain add(const Uncertain a, const Uncertain b);

double pValue(const Uncertain center, const Uncertain observation);