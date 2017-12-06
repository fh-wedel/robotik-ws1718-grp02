#include "bva.hpp"

#define rad2deg(x) (x) * 180.0f / CV_PI

static bool isEqual(Vec2f a, Vec2f b) {
  float angle = fabs(rad2deg(a[1]) - rad2deg(b[1]));
  float dist  = fabs(a[0] - b[0]);

  return (angle < 20.0f);// && (dist < 50.0f);
}

/*static*/ void clusterLines(std::vector<Vec2f>& lines, std::vector<Vec2f>& clusteredLines) {
  std::vector<int> labels;
  int classes;
  classes = cv::partition(lines, labels, isEqual);

  for (int i = 0; i < lines.size(); i++) {
    printf("dist: %.3f angle: %.3f Klasse: %d\n", lines.at(i)[0], rad2deg(lines.at(i)[1]), labels.at(i));
  }

  for (int i = 0; i < classes; i++) {
    float sumAngle = 0;
    float sumDist = 0;
    int classSize = 0;
    for (int label = 0; label < (int)labels.size(); label++) {
      if (labels.at(label) == i) {
		    classSize++;
        sumDist  += lines.at(label)[0];
        sumAngle += lines.at(label)[1];
      }
    }
    printf("sumDist: %.1f sumAngle: %.1f\n", sumDist, sumAngle);
    clusteredLines.push_back(Vec2f(sumDist / classSize, sumAngle / classSize));
  }
  printf("Clustered:\n");
  for (Vec2f v : clusteredLines) {
    printf("dist: %.3f angle: %.3f\n", v[0], rad2deg(v[1]));//
  }

  printf("Klassen: %d\n", classes);
}

static void createMask(cv::Mat& mask, std::vector<std::vector<cv::Point> >& contours,
                      cv::Point refPoint) {
    std::vector<cv::Point> contour;
    contour.push_back(refPoint);
    contour.push_back(cv::Point(0, mask.rows - 1));
    contour.push_back(cv::Point(mask.cols - 1, mask.rows - 1));
    contour.push_back(cv::Point(mask.cols - 1 - refPoint.x, refPoint.y));

    contours.push_back(contour);
}


//own implementation of line detection
cv::Mat bva::findLinePointsNew(cv::Mat& src)
{
    cv::Mat mask = cv::Mat::zeros(src.size(), CV_8U);
    std::vector<std::vector<cv::Point> > maskContour;
    createMask(mask, maskContour, cv::Point(500, 600));
    cv::fillPoly(mask, maskContour, 255);

    src = src & mask;
	
        /* cv::Mat dst;
	cv::Mat transform_matrix;
	cv::Point2f source_points[4];
	cv::Point2f dest_points[4];

	source_points[0] = *new cv::Point2f(opParam.opFloat.at(2), opParam.opFloat.at(3));
	source_points[1] = *new cv::Point2f(opParam.opFloat.at(4), opParam.opFloat.at(5));
	source_points[2] = *new cv::Point2f(opParam.opFloat.at(6), opParam.opFloat.at(7));
	source_points[3] = *new cv::Point2f(opParam.opFloat.at(8), opParam.opFloat.at(9));

	dest_points[0] = *new cv::Point2f(0, 0);
	dest_points[1] = *new cv::Point2f(width, 0);
	dest_points[2] = *new cv::Point2f(width, height);
	dest_points[3] = *new cv::Point2f(0, height);
 
	transform_matrix = cv::getPerspectiveTransform(source_points, dest_points);

	cv::warpPerspective(imageStack.top().clone(), dst, transform_matrix, cv::Size(width, height));*/
	
	
    cv::cuda::GpuMat image(src);
		//image.upload(src);

		cv::cuda::GpuMat contours;

		cv::Ptr<cv::cuda::CannyEdgeDetector> canny = cv::cuda::createCannyEdgeDetector(0, 10, 3, false);
		canny->detect(image, contours);

		cv::cuda::GpuMat contoursInv;
        cv::cuda::threshold(contours,contoursInv,128,255,THRESH_BINARY_INV);

        /*
         Hough tranform for line detection with feedback
         Increase by 25 for the next frame if we found some lines.
         This is so we don't miss other lines that may crop up in the next frame
         but at the same time we don't want to start the feed back loop from scratch.
         */
        cv::cuda::GpuMat GpuMatLines;
        vector<Vec2f> lines;
        /*if (houghVote < 1 or lines.size() > 2) { // we lost all lines. reset
            houghVote = 300;
        }
        else {
          houghVote += 25;
        }*/

        //while(lines.size() < 10 && houghVote > 0){

    			cv::Ptr<cv::cuda::HoughLinesDetector> hough = cv::cuda::createHoughLinesDetector(1, CV_PI/180, 100);

    			hough->detect(contours, GpuMatLines);
    			hough->downloadResults(GpuMatLines, lines);
          //houghVote -= 5;
        //}
        //std::cout << houghVote << "\n";
        cv::cuda::GpuMat result(image.size(),CV_8U,Scalar(255));
        contours.copyTo(result);

        std::vector<Vec2f> clusteredLines;
        clusterLines(lines, clusteredLines);

        // Draw the lines
        std::vector<Vec2f>::const_iterator it = lines.begin();
        cv::Mat output;
        result.download(output);

        while (it!=lines.end()) {

            float rho= (*it)[0];   // first element is distance rho
            float theta= (*it)[1]; // second element is angle theta

            //if ( (theta > 0.09 && theta < 1.48) || (theta < 3.14 && theta > 1.66) || (theta > 1.5 && theta < 1.6)) { // filter to remove vertical and horizontal lines

                // point of intersection of the line with first row
                Point pt1(rho/cos(theta),0);
                // point of intersection of the line with last row
                Point pt2((rho-result.rows*sin(theta))/cos(theta),result.rows);
                // draw a line: Color = Scalar(R, G, B), thickness
                cv::line( output, pt1, pt2, Scalar(255,255,255), 1);
            //}

            ++it;
        }
        it = clusteredLines.begin();
        while (it!=clusteredLines.end()) {

            float rho= (*it)[0];   // first element is distance rho
            float theta= (*it)[1]; // second element is angle theta

            //if ( (theta > 0.09 && theta < 1.48) || (theta < 3.14 && theta > 1.66) || (theta > 1.5 && theta < 1.6)) { // filter to remove vertical and horizontal lines

                // point of intersection of the line with first row
                Point pt1(rho/cos(theta),0);
                // point of intersection of the line with last row
                Point pt2((rho-result.rows*sin(theta))/cos(theta),result.rows);
                // draw a line: Color = Scalar(R, G, B), thickness
                cv::line( output, pt1, pt2, Scalar(255,255,255), 3);
            //}

            ++it;
        }



	return output;
}

cv::Mat bva::lineBinarization(cv::Mat& input_img, int hueLow,
								int hueHigh, int saturation, int value)
{
	/*Size size(640, 480);
	Mat resizedImage;
	resize(m_inputImage, resizedImage, size);
	vw.write(resizedImage);*/

	//cv::Mat rgb[3];
	//cv::split(m_inputImage,rgb);
	//threshold(rgb[2], outputImage, m_filterProperties.thresholdImageBinarization, 255, THRESH_BINARY_INV);// Generate Binary Image

	//filter Red pixels

	cv::Mat hsv;
	cv::Mat out;
	cvtColor(input_img,hsv,CV_BGR2HSV);
	inRange(hsv,Scalar(hueLow,
						saturation,
						value)
						,Scalar(hueHigh,255,255),out);//detects blue; farbbereich: 90-120; Saettigung 120

	/* cv::Rect(0, out.rows / 2, out.cols, out.rows / 2); TODO ROI-Maske */

	//mit median
	//cv::medianBlur(out, out, 3);
	//closing
	//https://docs.opencv.org/2.4/doc/tutorials/imgproc/opening_closing_hats/opening_closing_hats.html
	int morph_size = 6; //kernelsize
	Mat element = getStructuringElement( 0, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );
	int operation = 3;
	morphologyEx(out, out, operation, element);
	//mit gauss (nicht so gut)
	cv::GaussianBlur(out, out, Size( 5, 5 ), 0, 0 );

	return out;
}
