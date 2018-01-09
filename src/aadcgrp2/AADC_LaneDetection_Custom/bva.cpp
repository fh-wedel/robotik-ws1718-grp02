#include "bva.hpp"

#define rad2deg(x) (x) * 180.0f / CV_PI

#define deg2rad(x) (x) / 180.0f * CV_PI

#define MAX_SPEED  0.25f

static float bva_angleThresh;
static float bva_distanceThresh;

static cv::Size screenSize (1920,1486);


//MARK: - Helper Functions
// TODO: Maybe outsource helper functions into its own module -- helper._pp ?

static float xValueOfLineAt(float distance, float angle, float yValue) {
	return distance / cos(angle) + tan(angle) * yValue;
}
static float xValueOfLineAt(cv::Vec2f& line, float yValue) {
	float distance = line[0];
	float angle = line[1];

	return xValueOfLineAt(distance, angle, yValue);
}
static float xValueOfLineAt(cv::Vec3f& line, float yValue) {
	float distance = line[0];
	float angle = line[1];

	return xValueOfLineAt(distance, angle, yValue);
}


static float yValueOfLineAt(float distance, float angle, float xValue) {
	return (distance / sin(angle)) - xValue / tan(angle);
}
static float yValueOfLineAt(cv::Vec2f& line, float xValue) {
	float distance = line[0];
	float angle = line[1];

	return yValueOfLineAt(distance, angle, xValue);
}
static float yValueOfLineAt(cv::Vec3f& line, float xValue) {
	float distance = line[0];
	float angle = line[1];

	return yValueOfLineAt(distance, angle, xValue);
}


static float centerOfLinesAt(cv::Vec3f& first, cv::Vec3f& second, float yValue) {
	return (xValueOfLineAt(first, yValue) + xValueOfLineAt(second, yValue)) / 2;
}
static float centerOfLinesAtBottom(cv::Vec3f& first, cv::Vec3f& second) {
	return centerOfLinesAt(first, second, screenSize.height);
}

// returns true if two lines are similar
static bool isEqual(cv::Vec2f a, cv::Vec2f b) {
	float angle = fabs(rad2deg(a[1]) - rad2deg(b[1]));
	float dist = fabs(a[0] - b[0]);

	return (angle < bva_angleThresh) && (dist < bva_distanceThresh);
}

// gets the weighted angle sum for all the lines in a vector.
static float getAngleSum(std::vector<cv::Vec3f> lines) {
	float sum = 0.0f;
	for (cv::Vec3f line : lines) {

		#define CURVE_THRESH 20.0f

		float angle = rad2deg(line[1]);

		if ((angle > -CURVE_THRESH && angle < CURVE_THRESH)
		 || ((angle < -CURVE_THRESH // linkskurve
			&& yValueOfLineAt(line, screenSize.width) > screenSize.height * 0.9f)
		 || (angle > CURVE_THRESH // rechtskurve
	        && yValueOfLineAt(line, 0) > screenSize.height * 0.9f))) {
				// linie wird für kurveberechnung mit einbezogen
				sum += angle * line[2];

		} else {
			printf("Keine Wertung\n");
		}

	}




	return sum;
}

// return if a value lies in a range with specified tolerance
static bool isInRange(float value, float rangeMiddle, float tolerance) {
	return value > rangeMiddle - tolerance &&
				 value < rangeMiddle + tolerance;
}
//MARK: - Line Classification

static bool lineIsHorizontal(cv::Vec3f& line) {
		float angle = rad2deg(line[1]);

		//NOTE: We're dealing with the normal vector.

		return (fabs(angle) > 75.0f);
}

static bool lineIsVertical(cv::Vec3f& line) {
		float angle = rad2deg(line[1]);

		//NOTE: We're dealing with the normal vector.

		return fabs(angle) < 20.0f;
}

// returns if a line is a stop line
static bool lineIsStopLine(cv::Vec3f& line) {
	return lineIsHorizontal(line);
}

// returns if a line is a left line
static bool lineIsLeftLine(cv::Vec3f& line) {
	float dist = line[0];

	//NOTE: We're dealing with the normal vector.

	return (lineIsVertical(line) && dist < screenSize.width / 2.0f);
}

// returns if a line is a right line
static bool lineIsRightLine(cv::Vec3f& line) {
	float dist = line[0];

	//NOTE: We're dealing with the normal vector.

	return (lineIsVertical(line) && dist > screenSize.width / 2.0f);
}


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

// calculates steering angle
static tFloat32 getAngle(std::vector<cv::Vec3f> rightLines,
						 std::vector<cv::Vec3f> leftLines,
					 	 std::vector<cv::Vec3f> unclassifiedLines) {

  	int linesSize = rightLines.size() + leftLines.size() + unclassifiedLines.size();
	if (linesSize == 0) return 0; //no lane was detected
	// TODO: not distinguishable to 'straight' -> maybe a struct is the desired return type
	// Car: "Just go straight and full speed :]"

	tFloat32 sum = getAngleSum(rightLines);
	sum += getAngleSum(leftLines);
	sum += getAngleSum(unclassifiedLines);
	tFloat32 steeringAngle = sum / (linesSize);

	// Lane keeping
	// NOTE: Use first 30 frames or so for optimal line placement detection?

	if (rightLines.size() > 0 || leftLines.size() > 0) {
		cv::Vec3f rightLine;
		cv::Vec3f leftLine;

		poolLines(rightLines, rightLine);
		poolLines(leftLines, leftLine);

		float rightDistance = (rightLines.size() > 0) ? screenSize.width - xValueOfLineAt(rightLine, screenSize.height)
													  : -1;
		float leftDistance  = (leftLines.size() > 0)  ? -xValueOfLineAt(leftLine, screenSize.height)
													  :  1;

		float deviation = (rightDistance + leftDistance) / 2.0f;

		#define A_MAX deg2rad(25.0f)
		#define X_MAX (0.15f * screenSize.width)

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

// classifies a line into one of these categories:
// - right line
// - left line
// - stop line
//
// based off its angle and distance.
// If no category matches, the line is unclassified.
static void classifyLines(std::vector<cv::Vec3f>& lines,
	std::vector<cv::Vec3f>& rightLines,
	std::vector<cv::Vec3f>& leftLines,
	std::vector<cv::Vec3f>& stopLines,
	std::vector<cv::Vec3f>& unclassifiedLines) {

	for (cv::Vec3f& line : lines) {
		if (lineIsStopLine(line)) {
			stopLines.push_back(line);

		} else if (lineIsLeftLine(line)) {
			leftLines.push_back(line);

		} else if (lineIsRightLine(line)) {
			rightLines.push_back(line);

		} else {
			unclassifiedLines.push_back(line);
		}
	}
}

// draws the detected and clustered lines in the specified color
static void drawLines(cv::Mat& out, std::vector<cv::Vec3f>& lines, cv::Scalar color) {

	std::vector<cv::Vec3f>::const_iterator it = lines.begin();
	while (it != lines.end()) {

		float rho = (*it)[0];    // first element is distance rho
		float theta = (*it)[1];  // second element is angle theta
		float weight = (*it)[2]; // third element is the weight of the line


    // point of intersection of the line with first row
		cv::Point pt1(rho / cos(theta), 0);
		// point of intersection of the line with last row
		cv::Point pt2((rho - screenSize.height*sin(theta)) / cos(theta), screenSize.height);
		// draw a line: Color = Scalar(R, G, B), thickness
		cv::line(out, pt1, pt2, color, weight * 50);

		++it;
	}
}

//MARK: - Distance Approximation

// TODO not yet in use
static float convertPixelToMM(float pixel) {
	//TODO: Constant based on perspective transform.
	//TODO: Should be the same for vertical and horizontal (-> keep aspect ratio).
	float ratio = 0.5f; //dummy value -> 1080 pixel equal approx. 55cm

	return ratio * pixel;
}


static float distanceFromStopLine(cv::Vec3f& line) {
	return convertPixelToMM(
		yValueOfLineAt(line, screenSize.width / 2)
	);
}


static tFloat32 getSpeedPercentage(std::vector<cv::Vec3f> stopLines){

	if(stopLines.size() > 0) {
		cv::Vec3f thickestLine = stopLines.at(0);

		// We look for the thickest (most sure) stop line
		for(cv::Vec3f& line : stopLines){
			if (line[2] > thickestLine[2]) {
		 		thickestLine = line;
			}
		}
	 float dist = yValueOfLineAt(thickestLine, screenSize.width / 2);

	 #define BREAK_DISTANCE 0.1f
	 float lowerPart = screenSize.height * BREAK_DISTANCE;
	 float upperPart = screenSize.height - lowerPart;

	 return 1 / (upperPart) * (screenSize.height-dist);

	 // TODO: The slowing down process seems to be too slow -- sometimes we
	 // drive over the stop line

	 // TODO: Calculation is based off percentage of stop line in frame.
	 // Maybe use a different function?
	}

	return 1;
}


//MARK: - Clustering and Detection

// Clusters lines according to a similarity measure (isEqual())
// weight saved in clusteredLines[2]
static void clusterLines(std::vector<cv::Vec2f>& lines, std::vector<cv::Vec3f>& clusteredLines) {
	std::vector<int> labels;
	int amountOfClasses = cv::partition(lines, labels, isEqual);

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
	/*
	printf("Clustered:\n");
	for (cv::Vec3f v : clusteredLines) {
		printf("dist: %.3f angle: %.3f weight: %.3f\n", v[0], rad2deg(v[1]), v[2]);
	}
	*/
}


// own implementation of line detection
void bva::findLines(cv::Mat& src, cv::Mat& out, int houghThresh,
						float angleThresh, float distanceThresh, float stopThresh,
						tFloat32& angle, tFloat32& speed)
{

	//--------------------canny-------------------------
	cv::cuda::GpuMat image(src);

	cv::cuda::GpuMat contours;

	cv::Ptr<cv::cuda::CannyEdgeDetector> canny = cv::cuda::createCannyEdgeDetector(0, 10, 3, false);
	canny->detect(image, contours);

	//--------------perspective warp------------------
	cv::Mat transform_matrix;
	cv::Point2f source_points[4];
	cv::Point2f dest_points[4];

	// Parameters
	cv::Point2f refPoint = cv::Point(230, 270);
	int bottomCornerInset = 550;

	/*	TODO: refPoint.x < imageSize.width / 2
	 * 	-> ansonsten wird das bild gespiegelt?!
	 *
	 * 	vielleicht hier lieber eine eingabe in prozent verwenden?
	 * 	(länge der oberen kante des trapezes beträgt x% der bildbreite)
	 */

	source_points[0] = refPoint;
	source_points[1] = cv::Point(0, contours.rows - 1); // bottom left corner
	source_points[2] = cv::Point(contours.cols - 1, contours.rows - 1); // bottom right corner
	source_points[3] = cv::Point(contours.cols - 1 - refPoint.x, refPoint.y);

	dest_points[0] = cv::Point(0, 0);
	dest_points[1] = cv::Point(bottomCornerInset, 1486 - 1);
	dest_points[2] = cv::Point(contours.cols - bottomCornerInset, 1486 - 1);
	dest_points[3] = cv::Point(contours.cols - 1, 0);

	transform_matrix = cv::getPerspectiveTransform(source_points, dest_points);
	cv::cuda::GpuMat contoursWarped;
	cv::cuda::warpPerspective(
		contours,
		contoursWarped,
		transform_matrix,
		cv::Size(1920,1486)
	);

	//---------------hough transformation---------------------------
	cv::cuda::GpuMat GpuMatLines;
	std::vector<cv::Vec2f> lines;

	cv::Ptr<cv::cuda::HoughLinesDetector> hough = cv::cuda::createHoughLinesDetector(1, CV_PI / 180, houghThresh);

	hough->detect(contoursWarped, GpuMatLines);
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
		// angle:    -90  .. 90
		// distance: -inf .. inf
		if (angle > 90 && angle < 270) {
			line[0] = -dist;
			line[1] = deg2rad(angle - 180.0f);

		} else if (angle < 0 || angle > 270) {
			printf("ALARM! %.2f\n", angle);
		}
	}

	// Create our final mat on GPU and write the contours to it.
	cv::cuda::GpuMat result(image.size(), CV_8U);
	contoursWarped.copyTo(result);

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
	float tempSpeed = MAX_SPEED * getSpeedPercentage(stopLines);

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

void bva::lineBinarization(cv::Mat& input_img, cv::Mat& out,
          int hueLow, int hueHigh, int saturation, int value)
{
	cv::Mat hsv;
	//convert to HSV colorspace
	cv::cvtColor(input_img, hsv, CV_BGR2HSV);

	//Filter blue color (range: ~90-120 saturation: ~120-255)
	cv::inRange(hsv,
				cv::Scalar(hueLow, saturation, value),
				cv::Scalar(hueHigh, 255, 255),
				out
	);

	//closing
	int kernelSize = 6;
	cv::Mat kernel = cv::getStructuringElement(0, cv::Size(2 * kernelSize + 1, 2 * kernelSize + 1), cv::Point(kernelSize, kernelSize));
	int operation = 3;
	cv::morphologyEx(out, out, operation, kernel);

	//Gauss filter for flattening edges after closing
	cv::GaussianBlur(out, out, cv::Size(5, 5), 0, 0);
}
