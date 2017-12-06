#include "bva.hpp"

#define rad2deg(x) (x) * 180.0f / CV_PI

//returns true if two vectors are similar
static bool isEqual(Vec2f a, Vec2f b) {
	float angle = fabs(rad2deg(a[1]) - rad2deg(b[1]));
	float dist = fabs(a[0] - b[0]);

	return (angle < 10.0f) && (dist < 200.0f);
}

//calculate steering angle
static float getAngle(std::vector<Vec2f> clusteredLines) {
	float sum = 0;
	if (clusteredLines.size() == 0) return 0; //no lane was detected
	for (Vec2f v : clusteredLines) {
		float deg = rad2deg(v[1]);
		if (deg < 90) {
			sum += deg;
		}
		else {
			sum -= 180 - deg;
		}
	}

	return sum / clusteredLines.size();
};

/*static*/ void clusterLines(std::vector<Vec2f>& lines, std::vector<Vec2f>& clusteredLines) {
	std::vector<int> labels;
	int classes;
	classes = cv::partition(lines, labels, isEqual);

//	for (int i = 0; i < lines.size(); i++) {
//		printf("dist: %.3f angle: %.3f Klasse: %d\n", lines.at(i)[0], rad2deg(lines.at(i)[1]), labels.at(i));
//	}

	for (int i = 0; i < classes; i++) {
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
		clusteredLines.push_back(Vec2f(sumDist / classSize, sumAngle / classSize));
	}
	printf("Clustered:\n");
	for (Vec2f v : clusteredLines) {
		printf("dist: %.3f angle: %.3f\n", v[0], rad2deg(v[1]));//
	}

	//printf("Klassen: %d\n", classes);
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
cv::Mat bva::findLinePointsNew(cv::Mat& src, int& angle)
{
	//----------------------ROI------------------------------
	//cv::Mat mask = cv::Mat::zeros(src.size(), CV_8U);
	//std::vector<std::vector<cv::Point> > maskContour;
	//createMask(mask, maskContour, cv::Point(500, 600));
	//cv::fillPoly(mask, maskContour, 255);
	//src = src & mask;

	//--------------------canny-------------------------
	cv::cuda::GpuMat image(src);
	//image.upload(src);

	cv::cuda::GpuMat contours;

	cv::Ptr<cv::cuda::CannyEdgeDetector> canny = cv::cuda::createCannyEdgeDetector(0, 10, 3, false);
	canny->detect(image, contours);

	cv::cuda::GpuMat contoursInv;
	cv::cuda::threshold(contours, contoursInv, 128, 255, THRESH_BINARY_INV);

	//--------------perspective warp------------------
	cv::Mat transform_matrix;
	cv::Point2f source_points[4];
	cv::Point2f dest_points[4];
	cv::Point2f refPoint = cv::Point(680, 650);
	source_points[0] = refPoint;
	source_points[1] = cv::Point(0, contours.rows - 1);
	source_points[2] = cv::Point(contours.cols - 1, contours.rows - 1);
	source_points[3] = cv::Point(contours.cols - 1 - refPoint.x, refPoint.y);

	dest_points[0] = cv::Point2f(0, 0);
	dest_points[1] = cv::Point2f(250, contours.rows - 1);
	dest_points[2] = cv::Point2f(contours.cols - 250, contours.rows - 1);
	dest_points[3] = cv::Point2f(contours.cols - 1, 0);

	transform_matrix = cv::getPerspectiveTransform(source_points, dest_points);
	cv::cuda::GpuMat contoursWarped;
	cv::cuda::warpPerspective(contours, contoursWarped, transform_matrix, cv::Size(contours.cols, contours.rows));

	/*
	Hough tranform for line detection with feedback
	Increase by 25 for the next frame if we found some lines.
	This is so we don't miss other lines that may crop up in the next frame
	but at the same time we don't want to start the feed back loop from scratch.
	*/
	//---------------hough transformation---------------------------
	cv::cuda::GpuMat GpuMatLines;
	vector<Vec2f> lines;
	/*if (houghVote < 1 or lines.size() > 2) { // we lost all lines. reset
	houghVote = 300;
	}
	else {
	houghVote += 25;
	}*/

	//while(lines.size() < 10 && houghVote > 0){

	cv::Ptr<cv::cuda::HoughLinesDetector> hough = cv::cuda::createHoughLinesDetector(1, CV_PI / 180, 150);

	hough->detect(contoursWarped, GpuMatLines);
	hough->downloadResults(GpuMatLines, lines);
	//houghVote -= 5;
	//}
	//std::cout << houghVote << "\n";
	cv::cuda::GpuMat result(image.size(), CV_8U, Scalar(255));
	contoursWarped.copyTo(result);

	std::vector<Vec2f> clusteredLines;
	clusterLines(lines, clusteredLines);

	// Draw the lines
	std::vector<Vec2f>::const_iterator it = lines.begin();
	cv::Mat output;
	result.download(output);

	while (it != lines.end()) {
		float rho = (*it)[0];   // first element is distance rho
		float theta = (*it)[1]; // second element is angle theta

								//if ( (theta > 0.09 && theta < 1.48) || (theta < 3.14 && theta > 1.66) || (theta > 1.5 && theta < 1.6)) { // filter to remove vertical and horizontal lines

								// point of intersection of the line with first row
		Point pt1(rho / cos(theta), 0);
		// point of intersection of the line with last row
		Point pt2((rho - result.rows*sin(theta)) / cos(theta), result.rows);
		// draw a line: Color = Scalar(R, G, B), thickness
		//cv::line(output, pt1, pt2, Scalar(255, 255, 255), 1);
		//}

		++it;
	}
	it = clusteredLines.begin();
	while (it != clusteredLines.end()) {

		float rho = (*it)[0];   // first element is distance rho
		float theta = (*it)[1]; // second element is angle theta

								//if ( (theta > 0.09 && theta < 1.48) || (theta < 3.14 && theta > 1.66) || (theta > 1.5 && theta < 1.6)) { // filter to remove vertical and horizontal lines

								// point of intersection of the line with first row
		Point pt1(rho / cos(theta), 0);
		// point of intersection of the line with last row
		Point pt2((rho - result.rows*sin(theta)) / cos(theta), result.rows);
		// draw a line: Color = Scalar(R, G, B), thickness
		cv::line(output, pt1, pt2, Scalar(255, 255, 255), 3);
		//}

		++it;
	}

	//steeringangle
	angle = getAngle(clusteredLines);

	return output;
}

cv::Mat bva::lineBinarization(cv::Mat& input_img, int hueLow,
	int hueHigh, int saturation, int value)
{
	cv::Mat hsv;
	cv::Mat out;
	//convert to HSV colorspace
	cvtColor(input_img, hsv, CV_BGR2HSV);
	
	//Filter blue color (range: ~90-120 saturation: ~120-255)
	inRange(hsv, Scalar(hueLow,
		saturation,
		value)
		, Scalar(hueHigh, 255, 255), out);

	//closing
	int kernelSize = 6;
	Mat kernel = getStructuringElement(0, Size(2 * kernelSize + 1, 2 * kernelSize + 1), Point(kernelSize, kernelSize));
	int operation = 3;
	morphologyEx(out, out, operation, kernel);

	//Gauss filter for flattening edges after closing
	cv::GaussianBlur(out, out, Size(5, 5), 0, 0);

	return out;
}
