cv::Mat hoe_OpenCV(cv::Mat inputImage, float intensity) {
    cv::Mat outputImage = inputImage.clone();
    cv::circle(outputImage, cv::Point(200,200), 200, cv::Scalar(0,cv::saturate_cast<uchar>(intensity*4),0),-1);
    return outputImage;
}
