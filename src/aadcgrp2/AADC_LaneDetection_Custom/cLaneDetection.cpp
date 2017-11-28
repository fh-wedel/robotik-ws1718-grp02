/**
Copyright (c)
Audi Autonomous Driving Cup. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3.  All advertising materials mentioning features or use of this software must display the following acknowledgement: �This product includes software developed by the Audi AG and its contributors for Audi Autonomous Driving Cup.�
4.  Neither the name of Audi nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY AUDI AG AND CONTRIBUTORS �AS IS� AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL AUDI AG OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


**********************************************************************
* $Author:: spiesra $  $Date:: 2017-05-22 18:08:00#$ $Rev:: 63774   $
**********************************************************************/
#include "stdafx.h"
#include "cLaneDetection.h"
#include <iostream>

#define rad2deg(x) (x) * 180.0f / CV_PI


int houghVote = 200;

// define the ADTF property names to avoid errors
ADTF_FILTER_PLUGIN(ADTF_FILTER_DESC,
    OID_ADTF_FILTER_DEF,
    cLaneDetection)



    cLaneDetection::cLaneDetection(const tChar* __info) : cFilter(__info)
{

    SetPropertyInt("ROI::XOffset", 0);
    SetPropertyStr("ROI::XOffset" NSSUBPROP_DESCRIPTION, "X Offset for Region of Interest Rectangular");
    SetPropertyBool("ROI::XOffset" NSSUBPROP_ISCHANGEABLE, tTrue);

    SetPropertyInt("ROI::YOffset", 500);
    SetPropertyStr("ROI::YOffset" NSSUBPROP_DESCRIPTION, "Y Offset for Region of Interest Rectangular");
    SetPropertyBool("ROI::YOffset" NSSUBPROP_ISCHANGEABLE, tTrue);

    SetPropertyInt("ROI::Width", 1280);
    SetPropertyStr("ROI::Width" NSSUBPROP_DESCRIPTION, "Width of the Region of Interest Rectangular");
    SetPropertyBool("ROI::Width" NSSUBPROP_ISCHANGEABLE, tTrue);

    SetPropertyInt("ROI::Height", 200);
    SetPropertyStr("ROI::Height" NSSUBPROP_DESCRIPTION, "Height of the Region of Interest Rectangular");
    SetPropertyBool("ROI::Height" NSSUBPROP_ISCHANGEABLE, tTrue);

    SetPropertyInt("Algorithm::Detection Lines", 25);
    SetPropertyStr("Algorithm::Detection Lines" NSSUBPROP_DESCRIPTION, "number of detection lines searched in ROI");
    SetPropertyBool("Algorithm::Detection Lines" NSSUBPROP_ISCHANGEABLE, tTrue);
    SetPropertyBool("Algorithm::Detection Lines" NSSUBPROP_MIN, 1);

    SetPropertyInt("Algorithm::Minimum Line Width", 30);
    SetPropertyStr("Algorithm::Minimum Line Width" NSSUBPROP_DESCRIPTION, "Minimum Line Width in Pixel");
    SetPropertyBool("Algorithm::Minimum Line Width" NSSUBPROP_ISCHANGEABLE, tTrue);
    SetPropertyBool("Algorithm::Minimum Line Width" NSSUBPROP_MIN, 1);

    SetPropertyInt("Algorithm::Maximum Line Width", 100);
    SetPropertyStr("Algorithm::Maximum Line Width" NSSUBPROP_DESCRIPTION, "Maximum Line Width in Pixel");
    SetPropertyBool("Algorithm::Maximum Line Width" NSSUBPROP_ISCHANGEABLE, tTrue);
    SetPropertyBool("Algorithm::Maximum Line Width" NSSUBPROP_MIN, 1);

    SetPropertyInt("Algorithm::Minimum Line Contrast", 50);
    SetPropertyStr("Algorithm::Minimum Line Contrast" NSSUBPROP_DESCRIPTION, "Mimimum line contrast in gray Values");
    SetPropertyBool("Algorithm::Minimum Line Contrast" NSSUBPROP_ISCHANGEABLE, tTrue);
    SetPropertyInt("Algorithm::Minimum Line Contrast" NSSUBPROP_MIN, 1);
    SetPropertyInt("Algorithm::Minimum Line Contrast" NSSUBPROP_MAX, 255);

    SetPropertyInt("Algorithm::Image Binarization Threshold", 100);
    SetPropertyStr("Algorithm::Image Binarization Threshold" NSSUBPROP_DESCRIPTION, "Threshold for image binarization");
    SetPropertyBool("Algorithm::Image Binarization Threshold" NSSUBPROP_ISCHANGEABLE, tTrue);
    SetPropertyInt("Algorithm::Image Binarization Threshold" NSSUBPROP_MIN, 1);
    SetPropertyInt("Algorithm::Image Binarization Threshold" NSSUBPROP_MAX, 255);

//eigene
    SetPropertyInt("Algorithm::HueLow", 90);
    SetPropertyStr("Algorithm::HueLow" NSSUBPROP_DESCRIPTION, "q");
    SetPropertyBool("Algorithm::HueLow" NSSUBPROP_ISCHANGEABLE, tTrue);


    SetPropertyInt("Algorithm::HueHigh", 120);
    SetPropertyStr("Algorithm::HueHigh" NSSUBPROP_DESCRIPTION, "q");
    SetPropertyBool("Algorithm::HueHigh" NSSUBPROP_ISCHANGEABLE, tTrue);


    SetPropertyInt("Algorithm::Saturation", 120);
    SetPropertyStr("Algorithm::Saturation" NSSUBPROP_DESCRIPTION, "q");
    SetPropertyBool("Algorithm::Saturation" NSSUBPROP_ISCHANGEABLE, tTrue);


    SetPropertyInt("Algorithm::Value", 120);
    SetPropertyStr("Algorithm::Value" NSSUBPROP_DESCRIPTION, "q");
    SetPropertyBool("Algorithm::Value" NSSUBPROP_ISCHANGEABLE, tTrue);

}

cLaneDetection::~cLaneDetection()
{
}

tResult cLaneDetection::Start(__exception)
{

    return cFilter::Start(__exception_ptr);
}

tResult cLaneDetection::Stop(__exception)
{
    //destroyWindow("Debug");
    return cFilter::Stop(__exception_ptr);
}
tResult cLaneDetection::Init(tInitStage eStage, __exception)
{
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr));

    if (eStage == StageFirst)
    {
		vw.open("/home/aadc/ADTF/src/aadcgrp2/AADC_LaneDetection_Custom/recording/recording001.avi",
			CV_FOURCC('M','J','P','G'), 30, Size(640, 480));

        // Video Input
        RETURN_IF_FAILED(m_oVideoInputPin.Create("Video_Input", IPin::PD_Input, static_cast<IPinEventSink*>(this)));
        RETURN_IF_FAILED(RegisterPin(&m_oVideoInputPin));

        // Video Input
        RETURN_IF_FAILED(m_oVideoOutputPin.Create("Video_Output_Debug", IPin::PD_Output, static_cast<IPinEventSink*>(this)));
        RETURN_IF_FAILED(RegisterPin(&m_oVideoOutputPin));

        // Video Input
        m_oGCLOutputPin.Create("GCL", new adtf::cMediaType(MEDIA_TYPE_COMMAND, MEDIA_SUBTYPE_COMMAND_GCL), static_cast<IPinEventSink*>(this));
        RegisterPin(&m_oGCLOutputPin);

    }
    else if (eStage == StageNormal)
    {
    }
    else if (eStage == StageGraphReady)
    {
        // get the image format of the input video pin
        cObjectPtr<IMediaType> pType;
        RETURN_IF_FAILED(m_oVideoInputPin.GetMediaType(&pType));

        cObjectPtr<IMediaTypeVideo> pTypeVideo;
        RETURN_IF_FAILED(pType->GetInterface(IID_ADTF_MEDIA_TYPE_VIDEO, (tVoid**)&pTypeVideo));

        // set the image format of the input video pin
        if (IS_FAILED(UpdateInputImageFormat(pTypeVideo->GetFormat())))
        {
            LOG_ERROR("Invalid Input Format for this filter");
        }
    }

    RETURN_NOERROR;
}



tResult cLaneDetection::Shutdown(tInitStage eStage, ucom::IException** __exception_ptr)
{
    if (eStage == StageGraphReady)
    {
    }

    return cFilter::Shutdown(eStage, __exception_ptr);
}

tResult cLaneDetection::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample)
{

    RETURN_IF_POINTER_NULL(pMediaSample);
    RETURN_IF_POINTER_NULL(pSource);
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived)
    {
        if (pSource == &m_oVideoInputPin)
        {
            if (m_sInputFormat.nPixelFormat == IImage::PF_UNKNOWN)
            {
                RETURN_IF_FAILED(UpdateInputImageFormat(m_oVideoInputPin.GetFormat()));
            }

            ProcessVideo(pMediaSample);
        }
    }
    else if (nEventCode == IPinEventSink::PE_MediaTypeChanged)
    {
        if (pSource == &m_oVideoInputPin)
        {
            //the input format was changed, so the imageformat has to changed in this filter also
            RETURN_IF_FAILED(UpdateInputImageFormat(m_oVideoInputPin.GetFormat()));
        }
    }
    RETURN_NOERROR;
}

tResult cLaneDetection::PropertyChanged(const tChar* strName)
{
    RETURN_IF_FAILED(cFilter::PropertyChanged(strName));
    //associate the properties to the member
    if (cString::IsEqual(strName, "ROI::Width"))
        m_filterProperties.ROIWidth = GetPropertyInt("ROI::Width");
    else if (cString::IsEqual(strName, "ROI::Height"))
        m_filterProperties.ROIHeight = GetPropertyInt("ROI::Height");
    else if (cString::IsEqual(strName, "ROI::XOffset"))
        m_filterProperties.ROIOffsetX = GetPropertyInt("ROI::XOffset");
    else if (cString::IsEqual(strName, "ROI::YOffset"))
        m_filterProperties.ROIOffsetY = GetPropertyInt("ROI::YOffset");
    else if (cString::IsEqual(strName, "Algorithm::Detection Lines"))
        m_filterProperties.detectionLines = GetPropertyInt("Algorithm::Detection Lines");
    else if (cString::IsEqual(strName, "Algorithm::Maximum Line Width"))
        m_filterProperties.maxLineWidth = GetPropertyInt("Algorithm::Maximum Line Width");
    else if (cString::IsEqual(strName, "Algorithm::Minimum Line Width"))
        m_filterProperties.minLineWidth = GetPropertyInt("Algorithm::Minimum Line Width");
    else if (cString::IsEqual(strName, "Algorithm::Minimum Line Contrast"))
        m_filterProperties.minLineContrast = GetPropertyInt("Algorithm::Minimum Line Contrast");
    else if (cString::IsEqual(strName, "Algorithm::Image Binarization Threshold"))
        m_filterProperties.thresholdImageBinarization = GetPropertyInt("Algorithm::Image Binarization Threshold");

        //eigene properties
    else if (cString::IsEqual(strName, "Algorithm::HueLow"))
        m_filterProperties.HueLow = GetPropertyInt("Algorithm::HueLow");
    else if (cString::IsEqual(strName, "Algorithm::HueHigh"))
        m_filterProperties.HueHigh = GetPropertyInt("Algorithm::HueHigh");
    else if (cString::IsEqual(strName, "Algorithm::Saturation"))
        m_filterProperties.Saturation = GetPropertyInt("Algorithm::Saturation");
    else if (cString::IsEqual(strName, "Algorithm::Value"))
        m_filterProperties.Value = GetPropertyInt("Algorithm::Value");
    RETURN_NOERROR;
}


tResult cLaneDetection::ProcessVideo(IMediaSample* pSample)
{

    RETURN_IF_POINTER_NULL(pSample);
    // new image for result
    cv::Mat outputImage;
    // here we store the pixel lines in the image where we search for lanes
    vector<tInt> detectionLines;

    // here we have all the detected line points
    vector<cPoint> detectedLinePoints;

    const tVoid* l_pSrcBuffer;

    //receiving data from input sample, and saving to TheInputImage
    if (IS_OK(pSample->Lock(&l_pSrcBuffer)))
    {
        //convert to mat, be sure to select the right pixelformat
        if (tInt32(m_inputImage.total() * m_inputImage.elemSize()) == m_sInputFormat.nSize)
        {
            m_inputImage.data = (uchar*)(l_pSrcBuffer);

            Size size(640, 480);
            Mat resizedImage;
            resize(m_inputImage, resizedImage, size);
            vw.write(resizedImage);

            //cv::Mat rgb[3];
            //cv::split(m_inputImage,rgb);
            //threshold(rgb[2], outputImage, m_filterProperties.thresholdImageBinarization, 255, THRESH_BINARY_INV);// Generate Binary Image

			//filter Red pixels

			Mat hsv;
			Mat out;
			cvtColor(m_inputImage,hsv,CV_BGR2HSV);
			inRange(hsv,Scalar(m_filterProperties.HueLow,
								m_filterProperties.Saturation,
								m_filterProperties.Value)
								,Scalar(m_filterProperties.HueHigh,255,255),out);//detects blue; farbbereich: 90-120; Saettigung 120

			//mit gauss (nicht so gut)
			//cv::GaussianBlur(out, outputImage, Size( 5, 5 ), 0, 0 );
			//mit median
			cv::medianBlur(out, outputImage, 3);


            //calculate the detectionlines in image
          //  getDetectionLines(detectionLines);

            //findLinePoints(detectionLines, outputImage, detectedLinePoints);
			outputImage = findLinePointsNew(outputImage);
        }
        pSample->Unlock(l_pSrcBuffer);
    }

    if (!outputImage.empty() && m_oVideoOutputPin.IsConnected())
    {
        UpdateOutputImageFormat(outputImage);

        //create a cImage from CV Matrix (not necessary, just for demonstration9
        cImage newImage;
        newImage.Create(m_sOutputFormat.nWidth, m_sOutputFormat.nHeight, m_sOutputFormat.nBitsPerPixel, m_sOutputFormat.nBytesPerLine, outputImage.data);

        //create the new media sample
        cObjectPtr<IMediaSample> pMediaSample;
        RETURN_IF_FAILED(AllocMediaSample((tVoid**)&pMediaSample));
        //updating media sample
        RETURN_IF_FAILED(pMediaSample->Update(_clock->GetStreamTime(), newImage.GetBitmap(), newImage.GetSize(), IMediaSample::MSF_None));
        //transmitting
        RETURN_IF_FAILED(m_oVideoOutputPin.Transmit(pMediaSample));

        outputImage.release();
    }

    if (m_oGCLOutputPin.IsConnected()) transmitGCL(detectionLines, detectedLinePoints);

    RETURN_NOERROR;
}

static bool isEqual(Vec2f a, Vec2f b) {
  float angle = abs(a[1] - b[1]);
  float dist = abs(a[0] - b[0]);

  return rad2deg(angle) < 10.0f && dist < 50.0f;
}

static void clusterLines(std::vector<Vec2f>& lines, std::vector<Vec2f>& clusteredLines) {
  std::vector<int> labels;
  int classes;
  classes = cv::partition(lines, labels, isEqual);

  for (int i = 0; i < classes; i++) {
    float sumAngle = 0;
    float sumDist = 0;
    int classSize = 0;
    for (int label = 0; label < (int)labels.size(); label++) {
      if (labels.at(label) == i) {
	classSize++;
        sumDist  += lines.at(i)[0];
        sumAngle += lines.at(i)[1];
      }
    }
    Vec3f mean = Vec3f();
    clusteredLines.push_back(Vec2f(sumDist / classSize, sumAngle / classSize));
  }

  printf("Klassen: %d\n", classes);
}

//own implementation of line detection
cv::Mat cLaneDetection::findLinePointsNew(cv::Mat& src)
{
		cv::cuda::GpuMat image;
		image.upload(src);

		cv::cuda::GpuMat contours;

		cv::Ptr<cv::cuda::CannyEdgeDetector> canny = cv::cuda::createCannyEdgeDetector(100, 150, 3, false);
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
        if (houghVote < 1 or lines.size() > 2) { // we lost all lines. reset
            houghVote = 300;
        }
<<<<<<< HEAD
        else{ houghVote += 25;}
        while(lines.size() < 4 && houghVote > 0){
          cv::Ptr<cv::cuda::HoughLinesDetector> hough = cv::cuda::createHoughLinesDetector(1, 3.1415/180, houghVote);
          hough->detect(contours, tmpLines);
          hough->downloadResults(tmpLines, lines);
            //cv::cuda::HoughLines(contours,lines,1,3.1415/180, houghVote);
            houghVote -= 5;
            
            
=======
        else {
          houghVote += 25;
        }

        while(lines.size() < 10 && houghVote > 0){

    			cv::Ptr<cv::cuda::HoughLinesDetector> hough = cv::cuda::createHoughLinesDetector(1, CV_PI/180, houghVote);

    			hough->detect(contours, GpuMatLines);
    			hough->downloadResults(GpuMatLines, lines);
          houghVote -= 5;
>>>>>>> 21762758738efacbc1190d9aa8a7a63526da9e73
        }
        std::cout << houghVote << "\n";
        cv::cuda::GpuMat result(image.size(),CV_8U,Scalar(255));
        contours.copyTo(result);

        std::vector<Vec2f> clusteredLines;
        clusterLines(lines, clusteredLines);

        // Draw the lines
        std::vector<Vec2f>::const_iterator it = clusteredLines.begin();
        cv::Mat output;
        result.download(output);

        while (it!=clusteredLines.end()) {

            float rho= (*it)[0];   // first element is distance rho
            float theta= (*it)[1]; // second element is angle theta

            if ( (theta > 0.09 && theta < 1.48) || (theta < 3.14 && theta > 1.66) || (theta > 1.5 && theta < 1.6)) { // filter to remove vertical and horizontal lines

                // point of intersection of the line with first row
                Point pt1(rho/cos(theta),0);
                // point of intersection of the line with last row
                Point pt2((rho-result.rows*sin(theta))/cos(theta),result.rows);
                // draw a line: Color = Scalar(R, G, B), thickness
                cv::line( output, pt1, pt2, Scalar(255,255,255), 1);
            }

            ++it;
        }



	return output;
}


tResult cLaneDetection::findLinePoints(const vector<tInt>& detectionLines, const cv::Mat& image, vector<cPoint>& detectedLinePoints)
{
    //iterate through the calculated horizontal lines
    for (vector<tInt>::const_iterator nline = detectionLines.begin(); nline != detectionLines.end(); nline++)
    {
        uchar ucLastVal = 0;

        // create vector with line data
        const uchar* p = image.ptr<uchar>(*nline, m_filterProperties.ROIOffsetX);
        std::vector<uchar> lineData(p, p + m_filterProperties.ROIWidth);

        tBool detectedStartCornerLine = tFalse;
        tInt columnStartCornerLine = 0;

        for (std::vector<uchar>::iterator lineIterator = lineData.begin(); lineIterator != lineData.end(); lineIterator++)
        {
            uchar ucCurrentVal = *lineIterator;
            tInt currentIndex = tInt(std::distance(lineData.begin(), lineIterator));
            //look for transition from dark to bright -> start of line corner
            if ((ucCurrentVal - ucLastVal) > m_filterProperties.minLineContrast)
            {
                detectedStartCornerLine = tTrue;
                columnStartCornerLine = currentIndex;
            }//look for transition from bright to dark -> end of line
            else if ((ucLastVal - ucCurrentVal) > m_filterProperties.minLineContrast && detectedStartCornerLine)
            {
                //we already have the start corner of line, so check the width of detected line
                if ((abs(columnStartCornerLine - currentIndex) > m_filterProperties.minLineWidth)
                    && (abs(columnStartCornerLine - currentIndex) < m_filterProperties.maxLineWidth))
                {
                    detectedLinePoints.push_back(cPoint(tInt(currentIndex - abs(columnStartCornerLine - currentIndex) / 2 +
                        m_filterProperties.ROIOffsetX), *nline));

                    detectedStartCornerLine = tFalse;
                    columnStartCornerLine = 0;
                }
            }
            //we reached maximum line width limit, stop looking for end of line
            if (detectedStartCornerLine &&
                abs(columnStartCornerLine - currentIndex) > m_filterProperties.maxLineWidth)
            {
                detectedStartCornerLine = tFalse;
                columnStartCornerLine = 0;
            }
            ucLastVal = ucCurrentVal;
        }
    }

    RETURN_NOERROR;
}


tResult cLaneDetection::getDetectionLines(vector<tInt>& detectionLines)
{
    tInt distanceBetweenDetectionLines = m_filterProperties.ROIHeight / (m_filterProperties.detectionLines + 1);

    for (int i = 1; i <= m_filterProperties.detectionLines; i++)
    {
        detectionLines.push_back(m_filterProperties.ROIOffsetY + i * distanceBetweenDetectionLines);
    }
    RETURN_NOERROR;
}


tResult cLaneDetection::UpdateInputImageFormat(const tBitmapFormat* pFormat)
{
    if (pFormat != NULL)
    {
        //update member variable
        m_sInputFormat = (*pFormat);
        LOG_INFO(adtf_util::cString::Format("Input: Size %d x %d ; BPL %d ; Size %d , PixelFormat; %d", m_sInputFormat.nWidth, m_sInputFormat.nHeight, m_sInputFormat.nBytesPerLine, m_sInputFormat.nSize, m_sInputFormat.nPixelFormat));
        //create the input matrix
        RETURN_IF_FAILED(BmpFormat2Mat(m_sInputFormat, m_inputImage));
    }
    RETURN_NOERROR;
}

tResult cLaneDetection::transmitGCL(const vector<tInt>& detectionLines, const vector<cPoint>& detectedLinePoints)
{

    IDynamicMemoryBlock* pGCLCmdDebugInfo;

    cGCLWriter::GetDynamicMemoryBlock(pGCLCmdDebugInfo);

    //set color
    cGCLWriter::StoreCommand(pGCLCmdDebugInfo, GCL_CMD_FGCOL, cColor::Red.GetRGBA());
    //show roi
    cGCLWriter::StoreCommand(pGCLCmdDebugInfo, GCL_CMD_DRAWRECT, m_filterProperties.ROIOffsetX, m_filterProperties.ROIOffsetY,
        m_filterProperties.ROIOffsetX + m_filterProperties.ROIWidth, m_filterProperties.ROIOffsetY + m_filterProperties.ROIHeight);


    //show detection lines
    cGCLWriter::StoreCommand(pGCLCmdDebugInfo, GCL_CMD_FGCOL, cColor::Yellow.GetRGBA());
    for (vector<tInt>::const_iterator it = detectionLines.begin(); it != detectionLines.end(); it++)
    {
        cGCLWriter::StoreCommand(pGCLCmdDebugInfo, GCL_CMD_DRAWLINE, m_filterProperties.ROIOffsetX, *it, m_filterProperties.ROIOffsetX + m_filterProperties.ROIWidth, *it);
    }

    //show detected lane points
    cGCLWriter::StoreCommand(pGCLCmdDebugInfo, GCL_CMD_FGCOL, cColor::Green.GetRGBA());
    for (vector<cPoint>::const_iterator LinePointsIter = detectedLinePoints.begin(); LinePointsIter != detectedLinePoints.end(); LinePointsIter++)
    {
        cGCLWriter::StoreCommand(pGCLCmdDebugInfo, GCL_CMD_FILLCIRCLE, LinePointsIter->GetX(), LinePointsIter->GetY(), 20);
    }

    cGCLWriter::StoreCommand(pGCLCmdDebugInfo, GCL_CMD_FGCOL, cColor::Blue.GetRGBA());
    //show minimum and maximum line width
    cGCLWriter::StoreCommand(pGCLCmdDebugInfo, GCL_CMD_DRAWRECT, m_filterProperties.ROIOffsetX, m_filterProperties.ROIOffsetY,
        m_filterProperties.ROIOffsetX + m_filterProperties.maxLineWidth, m_filterProperties.ROIOffsetY - 20);
    cGCLWriter::StoreCommand(pGCLCmdDebugInfo, GCL_CMD_DRAWRECT, m_filterProperties.ROIOffsetX + m_filterProperties.ROIWidth - m_filterProperties.maxLineWidth, m_filterProperties.ROIOffsetY,
        m_filterProperties.ROIOffsetX + m_filterProperties.ROIWidth, m_filterProperties.ROIOffsetY - 20);


    //show minimum and maximum line width at left and right roi
    cGCLWriter::StoreCommand(pGCLCmdDebugInfo, GCL_CMD_DRAWRECT, m_filterProperties.ROIOffsetX + m_filterProperties.ROIWidth - m_filterProperties.minLineWidth, m_filterProperties.ROIOffsetY - 20,
        m_filterProperties.ROIOffsetX + m_filterProperties.ROIWidth, m_filterProperties.ROIOffsetY - 40);
    cGCLWriter::StoreCommand(pGCLCmdDebugInfo, GCL_CMD_DRAWRECT, m_filterProperties.ROIOffsetX, m_filterProperties.ROIOffsetY - 20,
        m_filterProperties.ROIOffsetX + m_filterProperties.minLineWidth, m_filterProperties.ROIOffsetY - 40);


    cGCLWriter::StoreCommand(pGCLCmdDebugInfo, GCL_CMD_END);

    //alloc media sample and transmit it over output pin
    cObjectPtr<IMediaSample> pSample;
    RETURN_IF_FAILED(AllocMediaSample((tVoid**)&pSample));
    RETURN_IF_FAILED(pSample->Update(_clock->GetStreamTime(),
        pGCLCmdDebugInfo->GetPtr(), (tInt)pGCLCmdDebugInfo->GetSize(), IMediaSample::MSF_None));
    RETURN_IF_FAILED(m_oGCLOutputPin.Transmit(pSample));

    cGCLWriter::FreeDynamicMemoryBlock(pGCLCmdDebugInfo);




    RETURN_NOERROR;
}

tResult cLaneDetection::UpdateOutputImageFormat(const cv::Mat& outputImage)
{
    //check if pixelformat or size has changed
    if (tInt32(outputImage.total() * outputImage.elemSize()) != m_sOutputFormat.nSize)
    {
        Mat2BmpFormat(outputImage, m_sOutputFormat);

        LOG_INFO(adtf_util::cString::Format("Output: Size %d x %d ; BPL %d ; Size %d , PixelFormat; %d", m_sOutputFormat.nWidth, m_sOutputFormat.nHeight, m_sOutputFormat.nBytesPerLine, m_sOutputFormat.nSize, m_sOutputFormat.nPixelFormat));
        //set output format for output pin
        m_oVideoOutputPin.SetFormat(&m_sOutputFormat, NULL);
    }
    RETURN_NOERROR;
}
