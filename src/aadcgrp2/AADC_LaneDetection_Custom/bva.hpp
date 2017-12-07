#ifndef __BVA_HPP__
#define __BVA_HPP__

#include "stdafx.h"

namespace bva {
	cv::Mat findLinePointsNew(cv::Mat& src, tFloat32& angle);

	cv::Mat lineBinarization(cv::Mat& input_img, int hueLow,
								int hueHigh, int saturation, int value);
}

#endif
