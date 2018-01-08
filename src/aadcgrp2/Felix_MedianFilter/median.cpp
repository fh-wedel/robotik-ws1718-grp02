#include "median.h"

cMedian::cMedian(int windowSize) {
    this->calculatedMedian = 0;
    this->medianIsUpToDate = false;
    this->windowSize = windowSize;
}

void cMedian::pushValue(float newValue) {
    // erase oldest values if window size has been reached
    int amountOfOverflownElements = storedValues.size() - windowSize;
    if (amountOfOverflownElements > 0) {
        storedValues.erase(
            storedValues.begin(),
            storedValues.begin() + amountOfOverflownElements
        );
    }

    // append new value
    storedValues.push_back(newValue);
    medianIsUpToDate = false;
}

float cMedian::calculateMedian() {

    if (!medianIsUpToDate) {
        std::vector<float> values = storedValues;

        // sort values ascending
        std::sort(values.begin(), values.end());

        // 0 / 2 = 0    -> 0
        // 1 / 2 = 0.5  -> 0
        // 2 / 2 = 1    -> 1
        // 3 / 2 = 1.5  -> 1
        // 4 / 2 = 2    -> 2
        // 5 / 2 = 2.5  -> 2
        int medianIndex = values.size() / 2;

        calculatedMedian = values[medianIndex];
    }

    return calculatedMedian;
}
