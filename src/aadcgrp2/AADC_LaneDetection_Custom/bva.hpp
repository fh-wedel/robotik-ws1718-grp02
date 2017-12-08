#ifndef __BVA_HPP__
#define __BVA_HPP__

#include "stdafx.h"

namespace bva {
	tFloat32 findLines(cv::Mat& src, cv::Mst& out, int houghThresh );

	void lineBinarization(cv::Mat& input_img, cv::Mat& out,
                int hueLow,	int hueHigh, int saturation, int value);
}

#endif
