#ifndef _Median_H_
#define _Median_H_

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <algorithm>


/**
 * An implementation of a median filter.
 *
 * @param windowSize The number of values which should be considered.
 *                   This can be changed at any time.
 */
class cMedian {

public:
    cMedian(int windowSize);

    /*! the number of values to consider */
    int windowSize;

    /*! Flag indicating whether the median has been calculated for the current input set. */
    bool medianIsUpToDate;

    /*! The most recent median value. */
    float calculatedMedian;

    /*! the most up to date values */
    std::vector<float> storedValues;

    /**
     * Adds a new value to the median filter.
     * If the number of values exceeds the windowSize, the oldest value is discarded.
     *
     * @param value the value to add
     */
    void pushValue(float value);

    /**
     * Calculates the current median on the stored values.
     *
     * @return the median value.
     */
    float calculateMedian();
};

#endif // _Median_H_
