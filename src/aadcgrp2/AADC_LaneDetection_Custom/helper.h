#ifndef __HELPER_H__
#define __HELPER_H__

/**
 * Interface for bva helper functions.
 * @author Frauke Jörgens, Jan Ottmüller
 */

#include "stdafx.h"

namespace help {
    /**
    * Calculates the degree value for a given radian value.
    * @param  x The value to be translated to degree.
    * @return   The degree value of the given radian.
    */
    #define rad2deg(x) (x) * 180.0f / CV_PI

    /**
    * Calculates the radian value for a given degree value.
    * @param  x The value to be translated to radian.
    * @return   The radian value of the given degree.
    */
    #define deg2rad(x) (x) / 180.0f * CV_PI

    /** The screen/image size after applying the perspective transform. */
    static cv::Size screenSize (1920,1486);

    /**
     * Returns the x value of a line for a given y value.
     * @param[in]  distance The distance of the line's support vector.
     * @param[in]  angle    The angle of the line's support vector.
     * @param[in]  yValue   The given y value.
     * @return              The x value for the given y value on the line.
     */
    float xValueOfLineAt(float distance, float angle, float yValue);

    /**
     * Returns the x value of a line for a given y value.
     * @param[in]  line     The given line.
     * @param[in]  yValue   The given y value.
     * @return              The x value for the given y value on the line.
     */
    float xValueOfLineAt(cv::Vec2f& line, float yValue);

    /**
     * Returns the x value of a line for a given y value.
     * @param[in]  line     The given line.
     * @param[in]  yValue   The given y value.
     * @return              The x value for the given y value on the line.
     */
    float xValueOfLineAt(cv::Vec3f& line, float yValue);

    /**
     * Returns the y value of a line for a given x value.
     * @param[in]  distance The distance of the line's support vector.
     * @param[in]  angle    The angle of the line's support vector.
     * @param[in]  xValue   The given y value.
     * @return              The y value for the given x value on the line.
     */
    float yValueOfLineAt(float distance, float angle, float xValue);

    /**
     * Returns the y value of a line for a given x value.
     * @param[in]  line     The given line.
     * @param[in]  xValue   The given y value.
     * @return              The y value for the given x value on the line.
     */
    float yValueOfLineAt(cv::Vec2f& line, float xValue);

    /**
     * Returns the y value of a line for a given x value.
     * @param[in]  line     The given line.
     * @param[in]  xValue   The given y value.
     * @return              The y value for the given x value on the line.
     */
    float yValueOfLineAt(cv::Vec3f& line, float xValue);

    /**
     * Returns the center of two lines for a given y value.
     * @param[in]  first  The first line.
     * @param[in]  second The second line.
     * @param[in]  yValue The given y value.
     * @return            The center of the two lines at the y value.
     */
    float centerOfLinesAt(cv::Vec3f& first, cv::Vec3f& second, float yValue);

    /**
     * Returns the center of two lines at the bottom of the screen.
     * @param[in]  first  The first line.
     * @param[in]  second The second line.
     * @return            The center of the two lines at the bottom of the screen.
     */
    float centerOfLinesAtBottom(cv::Vec3f& first, cv::Vec3f& second);

    /* ----- LINE CLASSIFICATION -----  */
    /**
     * Returns if a line is to be considered horizontal.
     * @param  line The line in question.
     * @return      true, if the line is considered horizontal.
     *              false, otherwise.
     */
    bool lineIsHorizontal(cv::Vec3f& line);

    /**
     * Returns if a line is to be considered vertical.
     * @param  line The line in question.
     * @return      true, if the line is considered vertical.
     *              false, otherwise.
     */
    bool lineIsVertical(cv::Vec3f& line);

    /**
     * Returns if a line is to be considered a stop line.
     * @param  line The line in question.
     * @return      true, if the line is considered a stop line.
     *              false, otherwise.
     */
    bool lineIsStopLine(cv::Vec3f& line);

    /**
     * Returns if a line is to be considered a left line.
     * @param  line The line in question.
     * @return      true, if the line is considered a left line.
     *              false, otherwise.
     */
    bool lineIsLeftLine(cv::Vec3f& line);

    /**
     * Returns if a line is to be considered a right line.
     * @param  line The line in question.
     * @return      true, if the line is considered a right line.
     *              false, otherwise.
     */
    bool lineIsRightLine(cv::Vec3f& line);
}

#endif
