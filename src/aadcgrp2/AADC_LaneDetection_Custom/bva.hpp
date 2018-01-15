#ifndef __BVA_HPP__
#define __BVA_HPP__

#include "stdafx.h"

namespace bva {
	void lineBinarization(cv::Mat& input_img, cv::Mat& out,
                int hueLow,	int hueHigh, int saturation, int value);

	void applyCanny(cv::Mat& src, cv::cuda::GpuMat& out,
					bool debug, cv::Mat& debugOutput);

	void applyPerspectiveWarp(cv::cuda::GpuMat& img, cv::cuda::GpuMat& out,
								bool debug, cv::Mat& debugOutput);

	void findLines(cv::cuda::GpuMat& img, cv::Mat& out, int houghThresh,
					float angleThresh, float distanceThresh, float stopThresh,
					tFloat32& angle, tFloat32& speed);
}

#endif
