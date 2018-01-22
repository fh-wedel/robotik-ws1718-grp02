#ifndef __BVA_H__
#define __BVA_H__
/**
 * Interface of main bva tasks.
 * @author Frauke Jörgens, Jan Ottmüller
 */

#include "stdafx.h"

namespace bva {
	/**
	 * Detects the blue lines in an image using the HSV model and given parameters.
	 *
	 * As cv::inRange doesn't support GPU computation, we do this on the CPU.
	 *
	 * @param input_img[in]  The BGR input image.
	 * @param out[out]       The binary outut image.
	 * @param hueLow[in]     The low hue threshold.
	 * @param hueHigh[in]    The upper hue threshold.
	 * @param saturation[in] The low saturation threshold.
	 * @param value[in]      The low value threshold.
	 */
	void lineBinarization(cv::Mat& input_img, cv::Mat& out,
                int hueLow,	int hueHigh, int saturation, int value);

	/**
	 * Applies the canny algorithm to a cv::Mat binary image and gives the output
	 * as a cv::cuda::GpuMat.
	 * @param src[in]          The binary inout image.
	 * @param out[out]         The output image after applying Canny.
	 * @param debug[in]        Flag, indicating whether debug mode is enabled.
	 * @param debugOutput[out] The debug output image (The out image but as cv::Mat).
	 */
	void applyCanny(cv::Mat& src, cv::cuda::GpuMat& out,
					bool debug, cv::Mat& debugOutput);

	/**
	 * Applies the perspective warp to an image.
	 * @param img[in]          The input cv::cuda::GpuMat image.
	 * @param out[out]         The output cv::cuda::GpuMat image.
	 * @param debug[in]        Flag, indicating whether debug mode is enabled.
	 * @param debugOutput[out] The debug output cv::Mat image.
	 */
	void applyPerspectiveWarp(cv::cuda::GpuMat& img, cv::cuda::GpuMat& out,
								bool debug, cv::Mat& debugOutput);
								
	/**
	 * Looks for left lines, right lines, stop lines in an image and gives the
	 * steering angle and speed as output parameters.
	 *
	 * @param img[in]            The input cv::cuda::GpuMat image after applying
	 * 							 Canny and perspective warp.
	 * @param out[out]           The output cv::Mat image.
	 * @param houghThresh[in]    The hough threshold.
	 * @param angleThresh[in]    The angle threshold for line classification.
	 * @param distanceThresh[in] The distance threshold for line classification.
	 * @param stopThresh[in]     The stop threshold for stop lines.
	 * @param angle[out]         The calculated steering angle.
	 * @param speed[out]         The calculated speed.
	 */
	void findLines(cv::cuda::GpuMat& img, cv::Mat& out, int houghThresh,
					float angleThresh, float distanceThresh, float stopThresh,
					tFloat32& angle, tFloat32& speed);
}

#endif
