#pragma once

#include <random>

class RandomGenerator {
public:
    // Constructor
    RandomGenerator() : generator_(std::random_device()()) {}

    // Method to generate a random double in the range [0, 1)
    double getRandom() {
        std::uniform_real_distribution<double> distribution(0.0, 1.0);
        return distribution(generator_);
    }

private:
    std::mt19937 generator_; // Mersenne Twister engine
};

extern RandomGenerator randomGenerator;
