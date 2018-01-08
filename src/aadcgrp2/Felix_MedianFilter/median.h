#ifndef _Median_H_
#define _Median_H_

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <algorithm>

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

    void pushValue(float value);

    float calculateMedian();
};

#endif // _Median_H_
