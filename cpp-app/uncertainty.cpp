#include "uncertainty.h"

#include <cmath>

Uncertain scale(const Uncertain a, const double factor) {
    return {
        a.mean * factor,
        a.error * factor
    };
}

Uncertain add(const Uncertain a, const Uncertain b) {
    return {
        a.mean + b.mean,
        sqrt(
            (a.error * a.error) + (b.error * b.error)
        )
    };
}

double zScore(const Uncertain center, const Uncertain observation) {
    const double newError = sqrt(center.error*center.error + observation.error*observation.error);
    if (newError == 0) {
        if (observation.mean > center.mean) {
            return 1e100;
        } else if (observation.mean < center.mean) {
            return -1e100;
        } else {
            return 0;
        }
    }
    double z = (observation.mean - center.mean) / newError;
    if (z > 1e100) {
        z = 1e100;
    } else if (z < -1e100) {
        z = -1e100;
    }
    return z;
}

double exactErf(const double x) {
    double accumulator = 0;
    double nFactorial = 1;
    for (int nIndex = 0; nIndex < 1000; nIndex++) {
        const double n = nIndex;
        if (n > 1) {
            nFactorial *= n;
        }
        double erfTerm = pow(x, 2*n+1);
        erfTerm /= nFactorial;
        erfTerm /= 2*n+1;
        erfTerm *= (-1) * (((nIndex&1)<<1) - 1);
        erfTerm *= 2 / sqrt(M_PI);
        const double newAccumulator = accumulator + erfTerm;
        if (newAccumulator == accumulator || std::isinf(newAccumulator) || std::isnan(newAccumulator)) {
            break;
        }
        accumulator = newAccumulator;
    }
    return accumulator;
}

double inexactPositiveErf(const double x) {
    if (x < 3.5) {
        // erf(3)
        return 0.9999779095030014;
    }
    if (x < 4) {
        // erf(3.5)
        return 0.9999992569016276;
    }
    if (x < 4.5) {
        // erf(4)
        return 0.9999999845827421;
    }
    if (x < 5) {
        // erf(4.5)
        return 0.9999999998033839;
    }
    if (x < 5.5) {
        // erf(5)
        return 0.9999999999984626;
    }
    if (x < 6) {
        // erf(5.5)
        return 0.9999999999999927;
    }
    return 1;
}

double erf(const double x) {
    if (x > -5 && x < 5) {
        return exactErf(x);
    } else if (x > 0) {
        return inexactPositiveErf(x);
    } else {
        return -inexactPositiveErf(-x);
    }
}

double pValue(const Uncertain center, const Uncertain observation) {
    const double z = zScore(center, observation);
    const double p = 0.5 * (1 + erf(z / M_SQRT2));
    return p <= 0.5 ? 2 * p : 2 * (1-p);
}