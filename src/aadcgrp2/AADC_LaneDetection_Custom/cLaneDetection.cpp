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
#include "bva.h"

#include <iostream>

#define SC_PROP_DEBUG_MODE "Debug Mode"

// define the ADTF property names to avoid errors
ADTF_FILTER_PLUGIN(ADTF_FILTER_DESC,
	OID_ADTF_FILTER_DEF,
	cLaneDetection)

cLaneDetection::cLaneDetection(const tChar* __info) : cStdFilter(__info)
{

	SetPropertyBool(SC_PROP_DEBUG_MODE, tFalse);
    SetPropertyStr(SC_PROP_DEBUG_MODE NSSUBPROP_DESCRIPTION, "If true debug infos are plotted to console");
	SetPropertyBool(SC_PROP_DEBUG_MODE NSSUBPROP_ISCHANGEABLE, tTrue);

	// own (custom) properties
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

	SetPropertyFloat("Algorithm::Stop Threshold", 0.4f);
	SetPropertyStr("Algorithm::Stop Threshold" NSSUBPROP_DESCRIPTION, "Threshold for stop weight");
	SetPropertyBool("Algorithm::Stop Threshold" NSSUBPROP_ISCHANGEABLE, tTrue);

}

cLaneDetection::~cLaneDetection() {}

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

	RETURN_IF_FAILED(m_oDebugBinaryVideoOutputPin.Create("Binary_Video_Output_Debug", IPin::PD_Output, static_cast<IPinEventSink*>(this)));
	RETURN_IF_FAILED(RegisterPin(&m_oDebugBinaryVideoOutputPin));

	RETURN_IF_FAILED(m_oDebugCannyVideoOutputPin.Create("Canny_Video_Output_Debug", IPin::PD_Output, static_cast<IPinEventSink*>(this)));
	RETURN_IF_FAILED(RegisterPin(&m_oDebugCannyVideoOutputPin));

	RETURN_IF_FAILED(m_oDebugPerspWarpVideoOutputPin.Create("Perpsective_Warp_Video_Output_Debug", IPin::PD_Output, static_cast<IPinEventSink*>(this)));
 	RETURN_IF_FAILED(RegisterPin(&m_oDebugPerspWarpVideoOutputPin));

	// Steering Angle and Speed Output Pin
	RETURN_IF_FAILED(registerFloatInputPin("steeringAngle", &m_SteeringPin, __exception_ptr));
	RETURN_IF_FAILED(registerFloatInputPin("speed", &m_SpeedPin, __exception_ptr));

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
			// the input format was changed, so the imageformat has to changed in this filter also
			RETURN_IF_FAILED(UpdateInputImageFormat(m_oVideoInputPin.GetFormat()));
		}
	}
	RETURN_NOERROR;
}

tResult cLaneDetection::PropertyChanged(const tChar* strName)
{
	RETURN_IF_FAILED(cFilter::PropertyChanged(strName));
	// associate the properties to the member
	if (cString::IsEqual(strName, SC_PROP_DEBUG_MODE))
		m_bDebugModeEnabled = GetPropertyBool(SC_PROP_DEBUG_MODE);

	// own (custom) properties
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
	else if (cString::IsEqual(strName, "Algorithm::Stop Threshold"))
		m_filterProperties.stopThresh = GetPropertyFloat("Algorithm::Stop Threshold");
	RETURN_NOERROR;
}

tResult cLaneDetection::transmitImage(cv::Mat &image, cVideoPin &outputVideoPin, tBitmapFormat &format)
{
	if (!image.empty() && outputVideoPin.IsConnected()) {

		UpdateOutputImageFormat(image, outputVideoPin, format);

		//create a cImage from CV Matrix (not necessary, just for demonstration)
		cImage newImage;
		newImage.Create(
			format.nWidth,
			format.nHeight,
			format.nBitsPerPixel,
			format.nBytesPerLine,
			image.data
		);

		//create the new media sample
		cObjectPtr<IMediaSample> pMediaSample;
		RETURN_IF_FAILED(AllocMediaSample((tVoid**)&pMediaSample));
		//updating media sample
		RETURN_IF_FAILED(pMediaSample->Update(_clock->GetStreamTime(), newImage.GetBitmap(), newImage.GetSize(), IMediaSample::MSF_None));
		//transmitting
		RETURN_IF_FAILED(outputVideoPin.Transmit(pMediaSample));

		image.release();
	}

	RETURN_NOERROR;
}

tResult cLaneDetection::ProcessVideo(IMediaSample* pSample)
{

	RETURN_IF_POINTER_NULL(pSample);
	// images for each bva step
	cv::Mat binary;         // binary image of street
	cv::cuda::GpuMat canny; // canny image of street
	cv::cuda::GpuMat persp; // image after applying perspective transform

	// debug images without using the GPU
	cv::Mat dbgCanny;
	cv::Mat dbgPersp;

	// final output image -- colored and weighted lines from hough line transform
	cv::Mat outputImage;

	const tVoid* l_pSrcBuffer;

	tFloat32 angle = -1;
	tFloat32 speed = 0;

	//receiving data from input sample, and saving to TheInputImage
	if (IS_OK(pSample->Lock(&l_pSrcBuffer)))
	{
		//convert to mat, be sure to select the right pixelformat
		if (tInt32(m_inputImage.total() * m_inputImage.elemSize()) == m_sInputFormat.nSize)
		{
			m_inputImage.data = (uchar*)(l_pSrcBuffer);

			// Binarization of specified range
			bva::lineBinarization(m_inputImage, binary,
				m_filterProperties.hueLow, m_filterProperties.hueHigh,
				m_filterProperties.saturation, m_filterProperties.value);

			// Applying Canny Edge Detection
			bva::applyCanny(binary, canny, m_bDebugModeEnabled, dbgCanny);

			// Applying perspective warp of street image
			bva::applyPerspectiveWarp(canny, persp, m_bDebugModeEnabled, dbgPersp);

			// find the lines in image and calculate the desired steering angle as well as speed
			bva::findLines(persp, outputImage, m_filterProperties.houghThresh,
								m_filterProperties.angleThresh, m_filterProperties.distanceThresh,
								m_filterProperties.stopThresh, angle, speed);


		}
		pSample->Unlock(l_pSrcBuffer);
	}

	if (m_bDebugModeEnabled) {
		transmitImage(binary, m_oDebugBinaryVideoOutputPin, m_sOutputFormatDebugBinary);
		transmitImage(dbgCanny, m_oDebugCannyVideoOutputPin, m_sOutputFormatDebugCanny);
		transmitImage(dbgPersp, m_oDebugPerspWarpVideoOutputPin, m_sOutputFormatDebugPerspWarp);
	}

	transmitImage(outputImage, m_oVideoOutputPin, m_sOutputFormat);

	if (m_bDebugModeEnabled) printf("Winkel %f\n", angle);
	RETURN_IF_FAILED(transmitFloatValue(angle, &m_SteeringPin));

	if (m_bDebugModeEnabled) printf("Speed %f\n", speed);
	RETURN_IF_FAILED(transmitFloatValue(speed, &m_SpeedPin));

	RETURN_NOERROR;
}

tResult cLaneDetection::UpdateInputImageFormat(const tBitmapFormat* pFormat)
{
	if (pFormat != NULL)
	{
		//update member variable
		m_sInputFormat = (*pFormat);
		if (m_bDebugModeEnabled) {LOG_INFO(adtf_util::cString::Format("Input: Size %d x %d ; BPL %d ; Size %d , PixelFormat; %d", m_sInputFormat.nWidth, m_sInputFormat.nHeight, m_sInputFormat.nBytesPerLine, m_sInputFormat.nSize, m_sInputFormat.nPixelFormat));}
		//create the input matrix
		RETURN_IF_FAILED(BmpFormat2Mat(m_sInputFormat, m_inputImage));
	}
	RETURN_NOERROR;
}

tResult cLaneDetection::UpdateOutputImageFormat(const cv::Mat& outputImage,
					cVideoPin &outputVideoPin, tBitmapFormat &format)
{
	//check if pixelformat or size has changed
	if (tInt32(outputImage.total() * outputImage.elemSize()) != format.nSize)
	{
		Mat2BmpFormat(outputImage, format);

		if (m_bDebugModeEnabled) { LOG_INFO(adtf_util::cString::Format("Output: Size %d x %d ; BPL %d ; Size %d , PixelFormat; %d", format.nWidth, format.nHeight, format.nBytesPerLine, format.nSize, format.nPixelFormat)); }
		//set output format for output pin
		outputVideoPin.SetFormat(&format, NULL);
	}
	RETURN_NOERROR;
}
