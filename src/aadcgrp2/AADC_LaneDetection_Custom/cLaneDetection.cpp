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
#include "bva.hpp"

#include <iostream>


// define the ADTF property names to avoid errors
ADTF_FILTER_PLUGIN(ADTF_FILTER_DESC,
	OID_ADTF_FILTER_DEF,
	cLaneDetection)


cv::Mat linePoints;
std::vector<cv::Point> rightPoints;
std::vector<cv::Point> leftPoints;

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
	SetPropertyInt("Algorithm::Minimum Line Contrast" NSSUBPROP_MAX, 1000);

	SetPropertyInt("Algorithm::Image Binarization Threshold", 100);
	SetPropertyStr("Algorithm::Image Binarization Threshold" NSSUBPROP_DESCRIPTION, "Threshold for image binarization");
	SetPropertyBool("Algorithm::Image Binarization Threshold" NSSUBPROP_ISCHANGEABLE, tTrue);
	SetPropertyInt("Algorithm::Image Binarization Threshold" NSSUBPROP_MIN, 1);
	SetPropertyInt("Algorithm::Image Binarization Threshold" NSSUBPROP_MAX, 255);

	//eigene
	SetPropertyInt("Algorithm::Hue Low", 90);
	SetPropertyStr("Algorithm::Hue Low" NSSUBPROP_DESCRIPTION, "Low threshold for hue");
	SetPropertyBool("Algorithm::Hue Low" NSSUBPROP_ISCHANGEABLE, tTrue);


	SetPropertyInt("Algorithm::Hue High", 120);
	SetPropertyStr("Algorithm::Hue High" NSSUBPROP_DESCRIPTION, "Upper threshold for hue");
	SetPropertyBool("Algorithm::Hue High" NSSUBPROP_ISCHANGEABLE, tTrue);


	SetPropertyInt("Algorithm::Saturation", 120);
	SetPropertyStr("Algorithm::Saturation" NSSUBPROP_DESCRIPTION, "Lower threshold for saturation");
	SetPropertyBool("Algorithm::Saturation" NSSUBPROP_ISCHANGEABLE, tTrue);


	SetPropertyInt("Algorithm::Value", 120);
	SetPropertyStr("Algorithm::Value" NSSUBPROP_DESCRIPTION, "Lower threshold for Value");
	SetPropertyBool("Algorithm::Value" NSSUBPROP_ISCHANGEABLE, tTrue);

  SetPropertyInt("Algorithm::Hough Threshold", 250);
	SetPropertyStr("Algorithm::Hough Threshold" NSSUBPROP_DESCRIPTION, "Threshold for hough votes");
	SetPropertyBool("Algorithm::Hough Threshold" NSSUBPROP_ISCHANGEABLE, tTrue);

	SetPropertyFloat("Algorithm::Angle Threshold", 2.0f);
	SetPropertyStr("Algorithm::Angle Threshold" NSSUBPROP_DESCRIPTION, "Threshold for angles in line clustering");
	SetPropertyBool("Algorithm::Angle Threshold" NSSUBPROP_ISCHANGEABLE, tTrue);

	SetPropertyFloat("Algorithm::Distance Threshold", 200.0f);
	SetPropertyStr("Algorithm::Distance Threshold" NSSUBPROP_DESCRIPTION, "Threshold for distance in line clustering");
	SetPropertyBool("Algorithm::Distance Threshold" NSSUBPROP_ISCHANGEABLE, tTrue);

}

cLaneDetection::~cLaneDetection() {}

tResult cLaneDetection::Start(__exception)
{

	return cFilter::Start(__exception_ptr);
}

tResult cLaneDetection::Stop(__exception)
{
	//destroyWindow("Debug");
	return cFilter::Stop(__exception_ptr);
}


tResult cLaneDetection::CreateInputPins(__exception) {
	// Video Input
	RETURN_IF_FAILED(m_oVideoInputPin.Create("Video_Input", IPin::PD_Input, static_cast<IPinEventSink*>(this)));
	RETURN_IF_FAILED(RegisterPin(&m_oVideoInputPin));

    RETURN_NOERROR;
}


tResult cLaneDetection::CreateOutputPins(__exception) {

// Debug Video Output Pins

	RETURN_IF_FAILED(m_oVideoOutputPin.Create("Video_Output_Debug", IPin::PD_Output, static_cast<IPinEventSink*>(this)));
	RETURN_IF_FAILED(RegisterPin(&m_oVideoOutputPin));


	m_oGCLOutputPin.Create("GCL", new adtf::cMediaType(MEDIA_TYPE_COMMAND, MEDIA_SUBTYPE_COMMAND_GCL), static_cast<IPinEventSink*>(this));
	RegisterPin(&m_oGCLOutputPin);

// Steering Angle Output Pin

    // create description manager
    cObjectPtr<IMediaDescriptionManager> pDescManager;
    RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER,IID_ADTF_MEDIA_DESCRIPTION_MANAGER,(tVoid**)&pDescManager,__exception_ptr));

    // get media type
    tChar const * strDescSignalValue = pDescManager->GetMediaDescription("tSignalValue");
    RETURN_IF_POINTER_NULL(strDescSignalValue);
    cObjectPtr<IMediaType> pTypeSignalValue = new cMediaType(0, 0, 0, "tSignalValue", strDescSignalValue, IMediaDescription::MDF_DDL_DEFAULT_VERSION); //TODO: Soll angeblich ein "deprecated constructor" sein !!

    // set member media description
    RETURN_IF_FAILED(pTypeSignalValue->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_SteeringOutputDescription));

    // create pin
    RETURN_IF_FAILED(m_SteeringPin.Create("steeringAngle", pTypeSignalValue, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(&m_SteeringPin));

    RETURN_NOERROR;
}

tResult cLaneDetection::Init(tInitStage eStage, __exception)
{
	RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr));

	switch (eStage) {
	case StageFirst:
		RETURN_IF_FAILED(CreateInputPins(__exception_ptr));
		RETURN_IF_FAILED(CreateOutputPins(__exception_ptr));
		break;

	case StageNormal:
		break;

	case StageGraphReady:
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
		break;
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
	else if (cString::IsEqual(strName, "Algorithm::Hue Low"))
		m_filterProperties.hueLow = GetPropertyInt("Algorithm::Hue Low");
	else if (cString::IsEqual(strName, "Algorithm::Hue High"))
		m_filterProperties.hueHigh = GetPropertyInt("Algorithm::Hue High");
	else if (cString::IsEqual(strName, "Algorithm::Saturation"))
		m_filterProperties.saturation = GetPropertyInt("Algorithm::Saturation");
	else if (cString::IsEqual(strName, "Algorithm::Value"))
		m_filterProperties.value = GetPropertyInt("Algorithm::Value");
	else if (cString::IsEqual(strName, "Algorithm::Hough Threshold"))
		m_filterProperties.houghThresh = GetPropertyInt("Algorithm::Hough Threshold");
	else if (cString::IsEqual(strName, "Algorithm::Angle Threshold"))
		m_filterProperties.angleThresh = GetPropertyInt("Algorithm::Angle Threshold");
	else if (cString::IsEqual(strName, "Algorithm::Distance Threshold"))
		m_filterProperties.distanceThresh = GetPropertyInt("Algorithm::Distance Threshold");
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

			/*cv::Mat transform_matrix;
			cv::Point2f source_points[4];
			cv::Point2f dest_points[4];
			int bottomCornerInset = m_filterProperties.minLineContrast; //300
			cv::cuda::GpuMat image(m_inputImage);

			// Schachbrettmuster: 24.7 / 14.3
			cv::Point2f refPoint = cv::Point(m_filterProperties.minLineWidth, m_filterProperties.maxLineWidth); //200, 850
			source_points[0] = refPoint;
			source_points[1] = cv::Point(0, image.rows - 1); // bottom left corner
			source_points[2] = cv::Point(image.cols - 1, image.rows - 1); // bottom right corner
			source_points[3] = cv::Point(image.cols - 1 - refPoint.x, refPoint.y);

			dest_points[0] = cv::Point2f(0, 0);
			dest_points[1] = cv::Point2f(bottomCornerInset, image.rows - 1);
			dest_points[2] = cv::Point2f(image.cols - bottomCornerInset, image.rows - 1);
			dest_points[3] = cv::Point2f(image.cols - 1, 0);

			transform_matrix = cv::getPerspectiveTransform(source_points, dest_points);
			cv::cuda::GpuMat imageWarped;
			cv::cuda::warpPerspective(
				image,
				imageWarped,
				transform_matrix,
				image.size()
			);
			imageWarped.download(outputImage);*/


			// Binarization of specified range
			bva::lineBinarization(m_inputImage, outputImage,
				m_filterProperties.hueLow, m_filterProperties.hueHigh,
				m_filterProperties.saturation, m_filterProperties.value);



			//find the lines in image and calculate the desired steering angle
			tFloat32 angle = -1;
			angle = bva::findLines(outputImage, outputImage, m_filterProperties.houghThresh,
								m_filterProperties.angleThresh, m_filterProperties.distanceThresh);

			printf("Winkel %f\n", angle);
			transmitValue(angle);
		}
		pSample->Unlock(l_pSrcBuffer);
	}

	if (!outputImage.empty() && m_oVideoOutputPin.IsConnected())
	{
		UpdateOutputImageFormat(outputImage);

		//create a cImage from CV Matrix (not necessary, just for demonstration)
		cImage newImage;
		newImage.Create(
			m_sOutputFormat.nWidth,
			m_sOutputFormat.nHeight,
			m_sOutputFormat.nBitsPerPixel,
			m_sOutputFormat.nBytesPerLine,
			outputImage.data
		);

		//create the new media sample
		cObjectPtr<IMediaSample> pMediaSample;
		RETURN_IF_FAILED(AllocMediaSample((tVoid**)&pMediaSample));
		//updating media sample
		RETURN_IF_FAILED(pMediaSample->Update(_clock->GetStreamTime(), newImage.GetBitmap(), newImage.GetSize(), IMediaSample::MSF_None));
		//transmitting
		RETURN_IF_FAILED(m_oVideoOutputPin.Transmit(pMediaSample));

		outputImage.release();
	}

	if (m_oGCLOutputPin.IsConnected()) {
		transmitGCL(detectionLines, detectedLinePoints);
	}

	if (m_SteeringPin.IsConnected()) {
		// TODO hier transmitValue(angle) ?
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
			int y = *nline ;
			int x = tInt(currentIndex - abs(columnStartCornerLine - currentIndex) / 2 +
				m_filterProperties.ROIOffsetX);

			printf("x: %d, y: %d\n", x, y);
			//linePoints.at<uchar>(y, x) = 255;
			cv::circle(linePoints, cv::Point(x,y),20, cv::Scalar(255, 255, 255), -1);

			if(x < image.cols / 2){
				leftPoints.push_back(cv::Point(x,y));
			}else{
				rightPoints.push_back(cv::Point(x,y));
			}

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

tResult cLaneDetection::transmitValue(tFloat32 value) {

    cObjectPtr<IMediaSample> pMediaSample = initMediaSample(m_SteeringOutputDescription);
    {
        // focus for sample write lock
        // read data from the media sample with the coder of the descriptor
        __adtf_sample_write_lock_mediadescription(m_SteeringOutputDescription, pMediaSample, pCoder);

        /*! indicates of bufferIDs were set */
        static tBool m_SteeringOutputDescriptionIsInitialized = false;
        /*! the id for the f32value of the media description for input pin for the set speed */
        static tBufferID m_SteeringOutputDescriptionID;
        /*! the id for the arduino time stamp of the media description for input pin for the set speed */
        static tBufferID m_OutputValueTimestampID;

        if(!m_SteeringOutputDescriptionIsInitialized) {
            pCoder->GetID("f32Value", m_SteeringOutputDescriptionID);
            pCoder->GetID("ui32ArduinoTimestamp", m_OutputValueTimestampID);
            m_SteeringOutputDescriptionIsInitialized = tTrue;
        }

        //write values to media sample
        pCoder->Set(m_SteeringOutputDescriptionID, (tVoid*)&value);
    }

    //transmit media sample over output pin
    RETURN_IF_FAILED(pMediaSample->SetTime(_clock->GetStreamTime()));
    RETURN_IF_FAILED(m_SteeringPin.Transmit(pMediaSample));

    RETURN_NOERROR;
}

cObjectPtr<IMediaSample> cLaneDetection::initMediaSample(cObjectPtr<IMediaTypeDescription> typeDescription) {

    // determine size in memory using the type descriptor
    cObjectPtr<IMediaSerializer> pSerializer;
    typeDescription->GetMediaSampleSerializer(&pSerializer);
    tInt nSize = pSerializer->GetDeserializedSize();

    // create new media sample
    cObjectPtr<IMediaSample> pMediaSample;
    AllocMediaSample((tVoid**)&pMediaSample);
    pMediaSample->AllocBuffer(nSize);

    return pMediaSample;
}
