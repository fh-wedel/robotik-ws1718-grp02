/**
 * Implementation of main bva tasks.
 * @author Frauke Jörgens, Jan Ottmüller
 */
#include "bva.h"
#include "helper.h"

/**
 * Writes a debug RGB Image as a cv::Mat from a cv::cuda::GpuMat
 * @param[in]  out          The original output image to be copied (cv::cuda::GpuMat)
 * @param[in]  debug        The debug flag
 * @param[out]  debugOutput The output image as a cv::Mat
 */
#define WRITE_DEBUG_VIDEO(out, debug, debugOutput) \
	if (debug) { \
		out.download(debugOutput); \
		cv::cvtColor(debugOutput, debugOutput, CV_GRAY2RGB); \
	}

/** The maximum speed our car should be able to drive at the moment. */
#define MAX_SPEED  0.25f

/** The angle at which a line is considered a curve. */
#define CURVE_THRESH 12.0f

/** Threshold for the angle for hough line transform. */
static float bva_angleThresh;

/** Threshold for the distance for hough line transform. */
static float bva_distanceThresh;

/**
 * Returns true if two lines are considered equivalent.
 * @param[in]  a The first line.
 * @param[in]  b The second line.
 * @return       true,  if the lines are considered equal.
 * 			     false, otherwise.
 */
static bool isEquivalent(cv::Vec2f a, cv::Vec2f b) {
	float angle = fabs(rad2deg(a[1]) - rad2deg(b[1]));
	float dist = fabs(a[0] - b[0]);

	return (angle < bva_angleThresh) && (dist < bva_distanceThresh);
}

/**
 * Gets the weighted angle sum for all the lines in a vector.
 * @param[in]  lines The lines with which the angle is to be calculated.
 * @return           The calculated angle.
 */
static float getAngleSum(std::vector<cv::Vec3f> lines) {
	float sum = 0.0f;
	for (cv::Vec3f line : lines) {
		float angle = rad2deg(line[1]);

		// Line Classification
		bool lineIsLeftCurve          = angle < -CURVE_THRESH;
		bool lineIsRightCurve         = angle >  CURVE_THRESH;

		float factor = 1.0f;

		// Handling the curvature problem.
		if (lineIsLeftCurve) {
			angle = max(0.0f, min(help::yValueOfLineAt(line, help::screenSize.width * 0.6)
									/ help::screenSize.height * factor, 1.0f )) * angle ;
		}

		if (lineIsRightCurve) {
			angle = max(0.0f, min(help::yValueOfLineAt(line, help::screenSize.width * 0.4)
									/ help::screenSize.height * factor, 1.0f)) * angle ;
		}

		sum += angle * line[2];

	}

	return sum;
}

/**
 * Pools a vector of lines so that they become one averaged line.
 * @param[in] lines   The vector of lines to be pooled.
 * @param[out] output The output (averaged) line.
 */
static void poolLines(std::vector<cv::Vec3f>& lines, cv::Vec3f& output) {
	int amountOfLines = (int)lines.size();

	float totalWeight = 0;
	float sumAngle = 0;
	float sumDist = 0;

	for (int i = 0; i < amountOfLines; i++) {
		sumDist += lines.at(i)[0];
		sumAngle += lines.at(i)[1];
		totalWeight += lines.at(i)[2];
	}

	output[0] = sumDist / amountOfLines;
	output[1] = sumAngle / amountOfLines;
	output[2] = totalWeight;
}

/**
 * Calculates the steering angle.
 * @param[in]  rightLines        All detected right lines.
 * @param[in]  leftLines         All detected left lines.
 * @param[in]  unclassifiedLines All unclassified lines.
 * @return                       The calculated steering angle.
 */
static tFloat32 getAngle(std::vector<cv::Vec3f> rightLines,
						 std::vector<cv::Vec3f> leftLines,
					 	 std::vector<cv::Vec3f> unclassifiedLines) {

  	int linesSize = rightLines.size() + leftLines.size() + unclassifiedLines.size();
	if (linesSize == 0) return 0; // no lane was detected

	tFloat32 sum = getAngleSum(rightLines);
	sum += getAngleSum(leftLines);
	sum += getAngleSum(unclassifiedLines);
	tFloat32 steeringAngle = sum / (linesSize);

	// Lane keeping
	if (rightLines.size() > 0 || leftLines.size() > 0) {
		cv::Vec3f rightLine;
		cv::Vec3f leftLine;

		// We only want one line per side
		poolLines(rightLines, rightLine);
		poolLines(leftLines, leftLine);

		float rightDistance = (rightLines.size() > 0)
							? help::screenSize.width - help::xValueOfLineAt(rightLine, help::screenSize.height)
						    : -1;
		float leftDistance  = (leftLines.size() > 0)
							? -help::xValueOfLineAt(leftLine, help::screenSize.height)
						    :  1;

		float deviation = (rightDistance + leftDistance) / 2.0f;

		#define A_MAX deg2rad(25.0f)
		#define X_MAX (0.15f * help::screenSize.width)

		/**
	 	 *          	         A_MAX			              deviation
	 	 *  ------------------------------------------- * - -------------
		 * 		    /       / |deviation|  \        \        |deviation|
		 *   1 + e^( -14 * (---------------) - 0.5  )
	 	 *		   \	   \	 X_MAX    /		   /
	 	 *
	 	 */

		float laneKeepingAngle =
			(A_MAX/(1.0f + expf(-14.0f * (fabs(deviation) / X_MAX - 0.5f))))
			* -(deviation / fabs(deviation))
		;

		return (steeringAngle + rad2deg(laneKeepingAngle)) / 2.0f;
	}
	return steeringAngle;
}

/**
 * Classifies a line into one of these categories:
 * - right line
 * - left line
 * - stop line
 *
 * based off its angle and distance.
 * If no category matches, the line is unclassified.
 * @param[in] lines              The lines to be classified.
 * @param[out] rightLines        The detected right lines.
 * @param[out] leftLines     	 The detected left lines.
 * @param[out] stopLines      	 The detected stop lines.
 * @param[out] unclassifiedLines The unclassified lines lines.
 */
static void classifyLines(std::vector<cv::Vec3f>& lines,
	std::vector<cv::Vec3f>& rightLines,
	std::vector<cv::Vec3f>& leftLines,
	std::vector<cv::Vec3f>& stopLines,
	std::vector<cv::Vec3f>& unclassifiedLines) {

	for (cv::Vec3f& line : lines) {
		if (help::lineIsStopLine(line)) {
			stopLines.push_back(line);

		} else if (help::lineIsLeftLine(line)) {
			leftLines.push_back(line);

		} else if (help::lineIsRightLine(line)) {
			rightLines.push_back(line);

		} else {
			unclassifiedLines.push_back(line);
		}
	}
}

/**
 * Draws the detected and clustered lines in the specified color.
 *
 * As cv::line doesn't support GPU computation, we do this on the CPU.
 *
 * @param[in, out] out   The image to be drawn on.
 * @param[in] lines      The lines to be drawn.
 * @param[in] color      The color to draw the lines with.
 */
static void drawLines(cv::Mat& out, std::vector<cv::Vec3f>& lines, cv::Scalar color) {

	std::vector<cv::Vec3f>::const_iterator it = lines.begin();
	while (it != lines.end()) {

		float rho = (*it)[0];    // first element is distance rho
		float theta = (*it)[1];  // second element is angle theta
		float weight = (*it)[2]; // third element is the weight of the line


        // point of intersection of the line with first row
		cv::Point pt1(rho / cos(theta), 0);
		// point of intersection of the line with last row
		cv::Point pt2((rho - help::screenSize.height*sin(theta)) / cos(theta), help::screenSize.height);
		// draw a line: Color = Scalar(R, G, B), thickness
		cv::line(out, pt1, pt2, color, weight * 50);

		++it;
	}
}

/**
 * Gets the speed percentage by the stop lines and stop threshold.
 * @param[in] stopLines  The detected stop lines.
 * @param[in] stopThresh The stop line threshold.
 * @return               The speed percentage.
 */
static tFloat32 getSpeedPercentage(std::vector<cv::Vec3f> stopLines,
									float stopThresh) {

	if(stopLines.size() > 0) {
		cv::Vec3f thickestLine = stopLines.at(0);

		// We look for the thickest (most sure) stop line
		for(cv::Vec3f& line : stopLines) {
			if (line[2] > thickestLine[2]) {
		 		thickestLine = line;
			}
		}
		if (thickestLine[2] < stopThresh) return 1;

		float dist = help::yValueOfLineAt(thickestLine, help::screenSize.width / 2);

		#define BREAK_DISTANCE 0.1f
		float lowerPart = help::screenSize.height * BREAK_DISTANCE;
		float upperPart = help::screenSize.height - lowerPart;

		return 1 / (upperPart) * (help::screenSize.height - dist);
	}

	return 1;
}

void bva::lineBinarization(cv::Mat& input_img, cv::Mat& out,
          int hueLow, int hueHigh, int saturation, int value) {

	cv::Mat hsv;
	// convert to HSV colorspace
	cv::cvtColor(input_img, hsv, CV_BGR2HSV);

	// Filter blue color (range: ~90-120 saturation: ~120-255)
	cv::inRange(hsv,
				cv::Scalar(hueLow, saturation, value),
				cv::Scalar(hueHigh, 255, 255),
				out
	);

	// closing
	int kernelSize = 6;
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE,
										cv::Size(2 * kernelSize + 1, 2 * kernelSize + 1));
	cv::morphologyEx(out, out, cv::MORPH_CLOSE, kernel);

	// Gaussian filter for flattening edges after closing
	cv::GaussianBlur(out, out, cv::Size(5, 5), 0, 0);
}

void bva::applyCanny(cv::Mat& src, cv::cuda::GpuMat& out,
						bool debug, cv::Mat& debugOutput) {
	cv::cuda::GpuMat image(src);

	cv::Ptr<cv::cuda::CannyEdgeDetector> canny = cv::cuda::createCannyEdgeDetector(0, 10, 3, false);
	canny->detect(image, out);

	WRITE_DEBUG_VIDEO(out, debug, debugOutput)
}

void bva::applyPerspectiveWarp(cv::cuda::GpuMat& img, cv::cuda::GpuMat& out,
							bool debug, cv::Mat& debugOutput) {
	cv::Mat transform_matrix;
	cv::Point2f source_points[4];
	cv::Point2f dest_points[4];

	// Parameters
	cv::Point2f refPoint = cv::Point(230, 270);
	int bottomCornerInset = 550;

	source_points[0] = refPoint;
	source_points[1] = cv::Point(0, img.rows - 1); // bottom left corner
	source_points[2] = cv::Point(img.cols - 1, img.rows - 1); // bottom right corner
	source_points[3] = cv::Point(img.cols - 1 - refPoint.x, refPoint.y);

	dest_points[0] = cv::Point(0, 0);
	dest_points[1] = cv::Point(bottomCornerInset, help::screenSize.width - 1);
	dest_points[2] = cv::Point(img.cols - bottomCornerInset, help::screenSize.width - 1);
	dest_points[3] = cv::Point(img.cols - 1, 0);

	transform_matrix = cv::getPerspectiveTransform(source_points, dest_points);

	cv::cuda::warpPerspective(
		img,
		out,
		transform_matrix,
		help::screenSize
	);

	WRITE_DEBUG_VIDEO(out, debug, debugOutput)
}

//MARK: - Clustering and Detection

/**
 * Clusters lines according to a similarity measure (isEquivalent()).
 * Weight saved in clusteredLines[2].
 * @param lines[in]           The lines to be clustered.
 * @param clusteredLines[out] The clustered lines.
 */
static void clusterLines(std::vector<cv::Vec2f>& lines, std::vector<cv::Vec3f>& clusteredLines) {
	std::vector<int> labels;
	int amountOfClasses = cv::partition(lines, labels, isEquivalent);

	for (int i = 0; i < amountOfClasses; i++) {
		float sumAngle = 0;
		float sumDist = 0;
		int classSize = 0;
		for (int label = 0; label < (int)labels.size(); label++) {
			if (labels.at(label) == i) {
				classSize++;
				sumDist += lines.at(label)[0];
				sumAngle += lines.at(label)[1];
			}
		}
		clusteredLines.push_back(cv::Vec3f(sumDist / classSize,
									 	   sumAngle / classSize,
										   (float) classSize / lines.size()));
	}
}

void bva::findLines(cv::cuda::GpuMat& img, cv::Mat& out, int houghThresh,
						float angleThresh, float distanceThresh, float stopThresh,
						tFloat32& angle, tFloat32& speed) {
	//---------------hough transformation---------------------------
	cv::cuda::GpuMat GpuMatLines;
	std::vector<cv::Vec2f> lines;

	cv::Ptr<cv::cuda::HoughLinesDetector> hough = cv::cuda::createHoughLinesDetector(1, CV_PI / 180, houghThresh);

	hough->detect(img, GpuMatLines);
	hough->downloadResults(GpuMatLines, lines);

	// Apply Normalization for better recognition of vertical lines (angle doesn't jump from 0 to 180)
	// -> By remapping (91 to 180) to (-90 to 0) the distance needs to be inverted.
	//
	//   O----------------> x
	//   |`-_              |
	//   |   `-_   angle   )
	//   |      `-_       /
	//   |		     `-_   /
	//   |			      `-_
	// y V
	//
	//NOTE: We're dealing with the normal vector.

	for (cv::Vec2f& line : lines) {
		float angle = rad2deg(line[1]);
		float dist = line[0];

		// Normalizing angle and distance so that they are in the following range:
		// angle:    -90 .. 90
		// distance: -inf .. inf
		if (angle > 90 && angle < 270) {
			line[0] = -dist;
			line[1] = deg2rad(angle - 180.0f);

		} else if (angle < 0 || angle > 270) {
			printf("ALARM! %.2f\n", angle);
		}
	}

	// Create our final mat on GPU and write the contours to it.
	cv::cuda::GpuMat result(img.size(), CV_8U);
	img.copyTo(result);

	// Cluster the detected hough lines and draw them onto the mat
	//
	// Unfortunately drawing can't be done using GPU (yet), therefore we download
	// the intermediary result and use the CPU.
	result.download(out);
	cv::cvtColor(out, out, CV_GRAY2RGB);

	bva_angleThresh = angleThresh;
	bva_distanceThresh = distanceThresh;

	std::vector<cv::Vec3f> clusteredLines;
	clusterLines(lines, clusteredLines);

	std::vector<cv::Vec3f> rightLines;
	std::vector<cv::Vec3f> leftLines;
	std::vector<cv::Vec3f> stopLines;
	std::vector<cv::Vec3f> unclassifiedLines;
	classifyLines(clusteredLines, rightLines, leftLines, stopLines, unclassifiedLines);

	// Draw the lines
	drawLines(out, rightLines, cv::Scalar(255, 0, 0)); // blue right lines
	drawLines(out, leftLines, cv::Scalar(0, 255, 0));  // green left lines
	drawLines(out, stopLines, cv::Scalar(0, 0, 255));  // red stop lines
	drawLines(out, unclassifiedLines, cv::Scalar(0, 255, 255)); // yellow unclassified

	// steering angle
	angle = getAngle(rightLines, leftLines, unclassifiedLines);

	// speed
	float tempSpeed = MAX_SPEED * getSpeedPercentage(stopLines, stopThresh);

	// using an epsilon to make the car halt at very low speeds.
	speed = tempSpeed > 0.1f ? tempSpeed : 0;

	// Write current angle to screen for visualization
	std::string textAngle = std::to_string(angle);
	std::string textSpeed = std::to_string(speed);
	int fontFace = cv::FONT_HERSHEY_SCRIPT_SIMPLEX;
	double fontScale = 2;
	int thickness = 3;
	cv::Point textOrgAngle(10, 130);
	cv::Point textOrgSpeed(10, 200);
	cv::putText(out, textAngle, textOrgAngle, fontFace, fontScale, cv::Scalar::all(255), thickness, 8);
	cv::putText(out, textSpeed, textOrgSpeed, fontFace, fontScale, cv::Scalar::all(255), thickness, 8);
}
