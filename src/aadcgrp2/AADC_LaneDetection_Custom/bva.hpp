#ifndef __BVA_HPP__
#define __BVA_HPP__

#include "stdafx.h"

namespace bva {
	/*tFloat32 rightLineAngle;

	tFloat32 leftLineAngle;*/

	void findLines(cv::Mat& src, cv::Mat& out, int houghThresh,
								float angleThresh, float distanceThresh, float stopThresh,
								tFloat32& angle, tFloat32& speed);

	void lineBinarization(cv::Mat& input_img, cv::Mat& out,
                int hueLow,	int hueHigh, int saturation, int value);
}

#endif
