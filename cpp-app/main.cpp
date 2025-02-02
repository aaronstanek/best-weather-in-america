#include "rain.bin.h"
#include "temperature.bin.h"

#include "randomPointInUs.h"
#include "predictImageCoordinates.h"

#include "historyLookup.h"
#include "updateWeights.h"

#include <iostream>

double* initializeRainWeights() {
    double* const rainWeights = new double[RAIN_MAP_COUNT];
    for (unsigned int i = 0; i < RAIN_MAP_COUNT; i++) {
        rainWeights[i] = (((double)1)/((double)RAIN_MAP_COUNT));
    }
    return rainWeights;
}

double* initializeTemperatureWeights() {
    double* const temperatureWeights = new double[TEMPERATURE_MAP_COUNT];
    for (unsigned int i = 0; i < TEMPERATURE_MAP_COUNT; i++) {
        temperatureWeights[i] = (((double)1)/((double)TEMPERATURE_MAP_COUNT));
    }
    return temperatureWeights;
}

#include "randomGenerator.h"

double weatherGoodnessScore(Uncertain rain, Uncertain temperature) {
    // MOCK!!!
    return randomGenerator.getRandom();
}

Point generateAGoodPoint(const char* const rainPixels, const char* const temperaturePixels, const double* const rainWeights, const double* const temperatureWeights) {
    Point bestPoint;
    double bestScore;
    for (int i = 0; i < 1000; i++) {
        const Point candidate = randomPointInUs();
        const ImageCoordinates rainCoordinates = predictRainImageCoordinates(candidate.latitude, candidate.longitude);
        const ImageCoordinates temperatureCoordiantes = predictTemperatureImageCoordinates(candidate.latitude, candidate.longitude);
        const Uncertain rain = getHistoricalRainAt(rainPixels, rainWeights, rainCoordinates.x, rainCoordinates.y);
        const Uncertain temperature = getHistoricalTemperatureAt(temperaturePixels, temperatureWeights, temperatureCoordiantes.x, temperatureCoordiantes.y);
        const double score = weatherGoodnessScore(rain, temperature);
        if (i == 0 || score > bestScore) {
            bestPoint = candidate;
            bestScore = score;
        }
    }
    return bestPoint;
}

void sendDouble(const double* const d) {
    for (std::size_t i = 0; i < sizeof(double); i++) {
        const char c = ((char*)d)[i];
        switch (c) {
            case 0:
                std::cout << ((char)9) << ((char)8);
                break;
            case 8:
                std::cout << ((char)8) << ((char)8);
                break;
            case 9:
                std::cout << ((char)9) << ((char)9);
                break;
            case 10:
                std::cout << ((char)8) << ((char)9);
                break;
            default:
                std::cout << c;
        }
    }
}

void sendPoint(Point p) {
    sendDouble(&p.latitude);
    sendDouble(&p.longitude);
    std::cout << '\n' << std::flush;
}

double receiveDouble() {
    char bytes[sizeof(double)];
    for (std::size_t i = 0; i < sizeof(double); i++) {
        std::cin.get(bytes[i]);
    }
    return *((double*)(&bytes[0]));
}

void receiveUpdate(const char* const rainPixels, const char* const temperaturePixels, double* const rainWeights, double* const temperatureWeights) {
    const double latitude = receiveDouble();
    const double longitude = receiveDouble();
    // rain
    const double rainObservation = receiveDouble();
    const ImageCoordinates rainCoordinates = predictRainImageCoordinates(latitude, longitude);
    updateRainWeights(rainPixels, rainWeights, rainCoordinates.x, rainCoordinates.y, rainObservation);
    // temperature
    const double temperatureObservation = receiveDouble();
    const ImageCoordinates temperatureCoordiantes = predictTemperatureImageCoordinates(latitude, longitude);
    updateTemperatureWeights(temperaturePixels, temperatureWeights, temperatureCoordiantes.x, temperatureCoordiantes.y, temperatureObservation);
}

int main() {
    const char* const rainPixels = loadRainPixels();
    const char* const temperaturePixels = loadTemperaturePixels();
    double* const rainWeights = initializeRainWeights();
    double* const temperatureWeights = initializeTemperatureWeights();
    while (true) {
        char command;
        std::cin.get(command);
        switch (command) {
            case 'G':
                sendPoint(generateAGoodPoint(rainPixels, temperaturePixels, rainWeights, temperatureWeights));
                break;
            case 'U':
                receiveUpdate(rainPixels, temperaturePixels, rainWeights, temperatureWeights);
                break;
            default:
                break;
        }
    }
    return 0;
}