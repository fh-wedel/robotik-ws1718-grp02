#ifndef __HELPER_H__
#define __HELPER_H__

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

  float xValueOfLineAt(float distance, float angle, float yValue);

  float xValueOfLineAt(cv::Vec2f& line, float yValue);

  float xValueOfLineAt(cv::Vec3f& line, float yValue);

  float yValueOfLineAt(float distance, float angle, float xValue);

  float yValueOfLineAt(cv::Vec2f& line, float xValue);

  float yValueOfLineAt(cv::Vec3f& line, float xValue);

  float centerOfLinesAt(cv::Vec3f& first, cv::Vec3f& second, float yValue);

  float centerOfLinesAtBottom(cv::Vec3f& first, cv::Vec3f& second);

  /* ----- LINE CLASSIFICATION -----  */
  bool lineIsHorizontal(cv::Vec3f& line);

  bool lineIsVertical(cv::Vec3f& line);

  bool lineIsStopLine(cv::Vec3f& line);

  bool lineIsLeftLine(cv::Vec3f& line);

  bool lineIsRightLine(cv::Vec3f& line);
}

#endif
