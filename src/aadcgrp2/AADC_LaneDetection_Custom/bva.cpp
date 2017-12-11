#include "bva.hpp"

#define rad2deg(x) (x) * 180.0f / CV_PI

//returns true if two vectors are similar
static bool isEqual(cv::Vec2f a, cv::Vec2f b) {
	float angle = fabs(rad2deg(a[1]) - rad2deg(b[1]));
	float dist = fabs(a[0] - b[0]);

	return (angle < 10.0f) && (dist < 200.0f);
}

//calculate steering angle
static tFloat32 getAngle(std::vector<cv::Vec3f> clusteredLines) {
	tFloat32 sum = 0;
	if (clusteredLines.size() == 0) return 0; //no lane was detected
	// TODO: not distinguishable to 'straight' -> maybe a struct is the desired return type
	// Car: "Just go straight and full speed :]"
	int lines = 0;

	for (cv::Vec3f v : clusteredLines) {
			lines += v[2];
	}
	for (cv::Vec3f v : clusteredLines) {
		float angle = rad2deg(v[1]);
		printf("CalculatedAngle; %.2f\n", angle);
		if (angle > 90) angle -= 180;
		sum += angle * (v[2] / lines);

	}

	return sum / clusteredLines.size();
};

//weight saved in clusteredLines[2]
/*static*/ void clusterLines(std::vector<cv::Vec2f>& lines, std::vector<cv::Vec3f>& clusteredLines) {
	std::vector<int> labels;
	int amountOfClasses = cv::partition(lines, labels, isEqual);

//	for (int i = 0; i < lines.size(); i++) {
//		printf("dist: %.3f angle: %.3f Klasse: %d\n", lines.at(i)[0], rad2deg(lines.at(i)[1]), labels.at(i));
//	}

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
		//printf("sumDist: %.1f sumAngle: %.1f\n", sumDist, sumAngle);
		clusteredLines.push_back(cv::Vec3f(sumDist / classSize, sumAngle / classSize, (float) classSize));
	}
	printf("Clustered:\n");
	for (cv::Vec3f v : clusteredLines) {
		printf("dist: %.3f angle: %.3f weight: %.1f\n", v[0], rad2deg(v[1]), v[2]);
	}

	//printf("Klassen: %d\n", amountOfClasses);
}

//static void createMask(cv::Mat& mask, std::vector<std::vector<cv::Point> >& contours,
//	cv::Point refPoint) {
//	std::vector<cv::Point> contour;
//	contour.push_back(refPoint);
//	contour.push_back(cv::Point(0, mask.rows - 1));
//	contour.push_back(cv::Point(mask.cols - 1, mask.rows - 1));
//	contour.push_back(cv::Point(mask.cols - 1 - refPoint.x, refPoint.y));
//	contours.push_back(contour);
//}

//own implementation of line detection
tFloat32 bva::findLines(cv::Mat& src, cv::Mat& out, int houghThresh)
{
	//----------------------ROI------------------------------
	//cv::Mat mask = cv::Mat::zeros(src.size(), CV_8U);
	//std::vector<std::vector<cv::Point> > maskContour;
	//createMask(mask, maskContour, cv::Point(500, 600));
	//cv::fillPoly(mask, maskContour, 255);
	//src = src & mask;

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
	cv::Point2f refPoint = cv::Point(680, 650);
	int bottomCornerInset = 250;

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

	dest_points[0] = cv::Point2f(0, 0);
	dest_points[1] = cv::Point2f(bottomCornerInset, contours.rows - 1);
	dest_points[2] = cv::Point2f(contours.cols - bottomCornerInset, contours.rows - 1);
	dest_points[3] = cv::Point2f(contours.cols - 1, 0);

	transform_matrix = cv::getPerspectiveTransform(source_points, dest_points);
	cv::cuda::GpuMat contoursWarped;
	cv::cuda::warpPerspective(
		contours,
		contoursWarped,
		transform_matrix,
		contours.size()
	);

	//---------------hough transformation---------------------------
	cv::cuda::GpuMat GpuMatLines;
	std::vector<cv::Vec2f> lines;

	cv::Ptr<cv::cuda::HoughLinesDetector> hough = cv::cuda::createHoughLinesDetector(1, CV_PI / 180, houghThresh);

	hough->detect(contoursWarped, GpuMatLines);
	hough->downloadResults(GpuMatLines, lines);

	// Create our final mat on GPU and write the contours to it.
	cv::cuda::GpuMat result(image.size(), CV_8U);
	contoursWarped.copyTo(result);

	// Cluster the detected hough lines and draw them onto the mat
	//
	// Unfortunately drawing can't be done using GPU (yet), therefore we download
	// the intermediary result and use the CPU.
	result.download(out);

	std::vector<cv::Vec3f> clusteredLines;
	clusterLines(lines, clusteredLines);

	// Draw the lines
	std::vector<cv::Vec3f>::const_iterator it = clusteredLines.begin();
	while (it != clusteredLines.end()) {

		float rho = (*it)[0];   // first element is distance rho
		float theta = (*it)[1]; // second element is angle theta


    // point of intersection of the line with first row
		cv::Point pt1(rho / cos(theta), 0);
		// point of intersection of the line with last row
		cv::Point pt2((rho - result.rows*sin(theta)) / cos(theta), result.rows);
		// draw a line: Color = Scalar(R, G, B), thickness
		cv::line(out, pt1, pt2, cv::Scalar(255, 255, 255), 3);

		++it;
	}

	//steeringangle
	tFloat32 angle = getAngle(clusteredLines);

	// Write current angle to screen for visualization
	string text = std::to_string(angle);
  int fontFace = cv::FONT_HERSHEY_SCRIPT_SIMPLEX;
  double fontScale = 2;
  int thickness = 3;
  cv::Point textOrg(10, 130);
  cv::putText(out, text, textOrg, fontFace, fontScale, cv::Scalar::all(255), thickness,8);

	return angle;
}

void bva::lineBinarization(cv::Mat& input_img, cv::Mat& out,
          int hueLow, int hueHigh, int saturation, int value)
{
	cv::Mat hsv;
	//convert to HSV colorspace
	cv::cvtColor(input_img, hsv, CV_BGR2HSV);

	//Filter blue color (range: ~90-120 saturation: ~120-255)
	cv::inRange(hsv, cv::Scalar(hueLow,
		saturation,
		value)
		, cv::Scalar(hueHigh, 255, 255), out);

	//closing
	int kernelSize = 6;
	cv::Mat kernel = cv::getStructuringElement(0, cv::Size(2 * kernelSize + 1, 2 * kernelSize + 1), cv::Point(kernelSize, kernelSize));
	int operation = 3;
	cv::morphologyEx(out, out, operation, kernel);

	//Gauss filter for flattening edges after closing
	cv::GaussianBlur(out, out, cv::Size(5, 5), 0, 0);
}
