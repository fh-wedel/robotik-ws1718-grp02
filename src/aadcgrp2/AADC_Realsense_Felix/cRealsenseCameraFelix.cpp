/**********************************************************************
Copyright (c)
Audi Autonomous Driving Cup. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3.  All advertising materials mentioning features or use of this software must display the following acknowledgement: “This product includes software developed by the Audi AG and its contributors for Audi Autonomous Driving Cup.”
4.  Neither the name of Audi nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY AUDI AG AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL AUDI AG OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


**********************************************************************
* $Author:: spie#$  $Date:: 2017-05-16 10:06:17#$ $Rev:: 63289   $
**********************************************************************/

#include "cRealsenseCameraFelix.h"


const cString cRealsenseCameraFelix::PropEnableDebugName = "Enable Debug Logging";
const tBool cRealsenseCameraFelix::PropEnableDebugDefault = false;

const cString cRealsenseCameraFelix::PropFPSDepthName = "DepthStream::Depth Stream FPS";
const cString cRealsenseCameraFelix::PropFPSColourName = "ColourStream::Colour Stream FPS";
const int cRealsenseCameraFelix::PropFPSDefault = 30;

const cString cRealsenseCameraFelix::PropResolutionDepthName = "DepthStream::Depth Stream Resolution";
const cString cRealsenseCameraFelix::PropResolutionColourName = "ColourStream::Colour Stream Resolution";
const cString cRealsenseCameraFelix::PropResolutionDefault = "640x480";

const cString cRealsenseCameraFelix::PropEnableColourName = "ColourStream::Enable Colour Stream";
const cString cRealsenseCameraFelix::PropEnableDepthName = "DepthStream::Enable Depth Stream";
const tBool cRealsenseCameraFelix::PropEnableStreamDefault = true;


/// Create filter shell
ADTF_FILTER_PLUGIN("AADC Realsense Camera Felix", OID_ADTF_FELIX_REALSENSE_FILTER, cRealsenseCameraFelix);

tResult cRealsenseCameraFelix::PropertyChanged(const tChar* strName) {
    RETURN_IF_FAILED(cFilter::PropertyChanged(strName));
    //associate the properties to the member
    if (cString::IsEqual(strName, "Algorithm::minDepth"))
        m_filterProperties.minDepth = GetPropertyInt("Algorithm::minDepth");
    else if (cString::IsEqual(strName, "Algorithm::maxDepth"))
        m_filterProperties.maxDepth = GetPropertyInt("Algorithm::maxDepth");
    else if (cString::IsEqual(strName, "Algorithm::blurRadius"))
        m_filterProperties.blurRadius = GetPropertyInt("Algorithm::blurRadius");
	
	RETURN_NOERROR;
}


cRealsenseCameraFelix::cRealsenseCameraFelix(const tChar* __info) :cFilter(__info), m_cameraFound(tFalse) {
    // Setting Property for Debug Output
    SetPropertyBool(PropEnableDebugName, PropEnableDebugDefault);
    // Setting Properties for Framerates of Depth and Colour Stream
    SetPropertyInt(PropFPSDepthName, PropFPSDefault);
    SetPropertyStr(PropFPSDepthName + NSSUBPROP_VALUELIST, "30@30|60@60");
    SetPropertyInt(PropFPSColourName, PropFPSDefault);
    SetPropertyStr(PropFPSColourName + NSSUBPROP_VALUELIST, "30@30|60@60");
    // Setting Properties for Resolution of Depth and Colour Stream
    SetPropertyStr(PropResolutionDepthName, PropResolutionDefault);
    SetPropertyStr(PropResolutionDepthName + NSSUBPROP_VALUELIST, "640x480@640x480|480x360@480x360|320x240@320x240");
    SetPropertyStr(PropResolutionColourName, PropResolutionDefault);
    SetPropertyStr(PropResolutionColourName + NSSUBPROP_VALUELIST, "1920x1080@1920x1080|640x480@640x480|320x240@320x240");
    // Setting Properties for Enabling Depth, Colour, Infrared1/2 Streams
    SetPropertyBool(PropEnableColourName, PropEnableStreamDefault);
    SetPropertyBool(PropEnableDepthName, PropEnableStreamDefault);
    
    //Min/Max for clipping inputs of raw depth image
    SetPropertyInt("Algorithm::minDepth", 0);
    SetPropertyStr("Algorithm::minDepth" NSSUBPROP_DESCRIPTION, "q");
    SetPropertyBool("Algorithm::minDepth" NSSUBPROP_ISCHANGEABLE, tTrue);

    
    SetPropertyInt("Algorithm::maxDepth", 4000);
    SetPropertyStr("Algorithm::maxDepth" NSSUBPROP_DESCRIPTION, "q");
    SetPropertyBool("Algorithm::maxDepth" NSSUBPROP_ISCHANGEABLE, tTrue);

	
    SetPropertyInt("Algorithm::blurRadius", 5);
    SetPropertyStr("Algorithm::blurRadius" NSSUBPROP_DESCRIPTION, "q");
    SetPropertyBool("Algorithm::blurRadius" NSSUBPROP_ISCHANGEABLE, tTrue);

    //Checking if device is connected
    try
    {
        // member variable to save the Camera in
        m_ctx = new rs::context();
        m_dev = m_ctx->get_device(0);
        // Automatically Creating Properties from CameraProperties of Librealsense
        for (int j = 0; j < NUMBER_AVAILABLE_PROPERTIES; j++)
        {
            //Checking if Camera Supports Current Property
            if (m_dev->supports_option(static_cast<rs::option>(j)))
            {
                CreateProperty(j);
            }
        }
    }
    catch (const rs::error & e)
    {
        LOG_WARNING("No Realsense device found. Plugin will not be available");
    }
    catch (const std::exception & e)
    {
        LOG_WARNING("No Realsense device found. Plugin will not be available");
    }

}

cRealsenseCameraFelix::~cRealsenseCameraFelix()
{

}
/*
void cRealsenseCameraFelix::initStaticPins() {

    // Video Pin for RGB Video
    m_outputVideoRGB.Create("outputRGB", IPin::PD_Output, static_cast<IPinEventSink*>(this));
    RegisterPin(&m_outputVideoRGB);
    // Pin for Raw Depth Data
    m_outputDepthRaw.Create("outputDepthRaw", new adtf::cMediaType(MEDIA_TYPE_STRUCTURED_DATA, MEDIA_SUBTYPE_STRUCT_STRUCTURED), static_cast<IPinEventSink*>(this));
    RegisterPin(&m_outputDepthRaw);

    // Video Pin for Visualized Depth Video
    m_outputVideoDepthVis.Create("outputDepthVisualization", IPin::PD_Output, static_cast<IPinEventSink*>(this));
    RegisterPin(&m_outputVideoDepthVis);

}


void cRealsenseCameraFelix::initDynamicPins() {

    // In this stage you would do further initialisation and/or create your dynamic pins.
    // Please take a look at the demo_dynamicpin example for further reference.
    m_filterProperties.enableDebugOutput = GetPropertyBool(PropEnableDebugName);

    // Sets Member Variables with the Values of the Properties
    // Framerate of Depth and Colour Stream
    m_filterProperties.DepthFPS = GetPropertyInt(PropFPSDepthName);
    m_filterProperties.ColourFPS = GetPropertyInt(PropFPSColourName);
    // Resolution of Depth and Colour Stream
    m_filterProperties.DepthResolution = GetPropertyStr(PropResolutionDepthName);
    m_filterProperties.ColourResolution = GetPropertyStr(PropResolutionColourName);
    // Enabling Depth, Colour and Infrared1/2 Stream
    m_filterProperties.Depth = GetPropertyBool(PropEnableDepthName);
    m_filterProperties.Colour = GetPropertyBool(PropEnableColourName);

    try {
        if (m_ctx->get_device_count() > 0) {
            m_dev = m_ctx->get_device(0);
        } else {
            THROW_ERROR_DESC(ERR_NOT_CONNECTED, "Propably no Realsense Camera connected. Connect camera and restart ADTF");
        }

        // Sets the Supported Options of the Camera with the Values of the Properties
        for (int i = 0; i < NUMBER_AVAILABLE_PROPERTIES; i++) {
            if (m_dev->supports_option(static_cast<rs::option>(i))) {
                m_dev->set_option(static_cast<rs::option>(i), (double)GetPropertyFloat(OptionName(i)));
            }
        }

    } catch (const rs::error & e) {
        std::stringstream errorDesc;
        errorDesc << "Propably no Realsense Camera connected: " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n " << e.what();
        THROW_ERROR_DESC(ERR_NOT_CONNECTED, errorDesc.str().c_str());
        m_cameraFound = tFalse;
    } catch (const std::exception & e) {
        std::stringstream errorDesc;
        errorDesc << "Propably no Realsense Camera connected:" << e.what();
        THROW_ERROR_DESC(ERR_NOT_CONNECTED, errorDesc.str().c_str());
        m_cameraFound = tFalse;
    }

    if (m_cameraFound) {
        //Bitmat Format for the RBG Ouput VideoPin
        m_BitmapFormatRGBOut.nWidth = WidthOutOfResolution(m_filterProperties.ColourResolution);
        m_BitmapFormatRGBOut.nHeight = HeightOutOfResolution(m_filterProperties.ColourResolution);
        m_BitmapFormatRGBOut.nBitsPerPixel = 24;
        m_BitmapFormatRGBOut.nPixelFormat = IImage::PF_RGB_888;
        m_BitmapFormatRGBOut.nBytesPerLine = m_BitmapFormatRGBOut.nWidth * m_BitmapFormatRGBOut.nBitsPerPixel / 8;
        m_BitmapFormatRGBOut.nSize = m_BitmapFormatRGBOut.nBytesPerLine * m_BitmapFormatRGBOut.nHeight;
        m_BitmapFormatRGBOut.nPaletteSize = 0;
        //Bitmap Format for the Visualized Depth Output VideoPin
        m_BitmapFormatDepthOut.nWidth = WidthOutOfResolution(m_filterProperties.DepthResolution);
        m_BitmapFormatDepthOut.nHeight = HeightOutOfResolution(m_filterProperties.DepthResolution);
        m_BitmapFormatDepthOut.nBitsPerPixel = 16;
        m_BitmapFormatDepthOut.nPixelFormat = IImage::PF_GREYSCALE_16;
        m_BitmapFormatDepthOut.nBytesPerLine = m_BitmapFormatDepthOut.nWidth * m_BitmapFormatDepthOut.nBitsPerPixel / 8;
        m_BitmapFormatDepthOut.nSize = m_BitmapFormatDepthOut.nBytesPerLine * m_BitmapFormatDepthOut.nHeight;
        m_BitmapFormatDepthOut.nPaletteSize = 0;

        // Setting the Bitmap Formats to the fitting Pins
        m_outputVideoRGB.SetFormat(&m_BitmapFormatRGBOut, NULL);
        m_outputVideoDepthVis.SetFormat(&m_BitmapFormatDepthOut, NULL);

        std::stringstream loginfo;
        // Enabling the Stream in the Camera if set in the Properties
        if (m_filterProperties.Colour) {
            m_dev->enable_stream(rs::stream::color, WidthOutOfResolution(m_filterProperties.ColourResolution), HeightOutOfResolution(m_filterProperties.ColourResolution), rs::format::bgr8, m_filterProperties.ColourFPS);
        }
        if (m_filterProperties.Depth) {
            m_dev->enable_stream(rs::stream::depth, WidthOutOfResolution(m_filterProperties.DepthResolution), HeightOutOfResolution(m_filterProperties.DepthResolution), rs::format::z16, m_filterProperties.DepthFPS);
        }

        // Setting Emitter Enabled for better Depth Stream in near Range
        m_dev->set_option(rs::option::r200_emitter_enabled, 1);
    }
}


tResult cRealsenseCameraFelix::Init(tInitStage eStage, __exception) {
    // never miss calling the parent implementation!!
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr))

        switch (eStage) {
            case StageFirst: // here we create and register our static pins.
                initStaticPins();
                break;

            case StageNormal:
                initDynamicPins();
                break;

            case StageGraphReady:

                // All pin connections have been established in this stage so you can query your pins
                // about their media types and additional meta data.
                // Please take a look at the demo_imageproc example for further reference.
                m_Thread.Create(cKernelThread::TF_Suspended, static_cast<IKernelThreadFunc*>(this), NULL, 0);
                break;
        }

    RETURN_NOERROR;
}*/



tResult cRealsenseCameraFelix::Init(tInitStage eStage, __exception)
{
    // never miss calling the parent implementation!!
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr))

        // in StageFirst you can create and register your static pins.
        if (eStage == StageFirst)
        {
            // Video Pin for RGB Video
            m_outputVideoRGB.Create("outputRGB", IPin::PD_Output, static_cast<IPinEventSink*>(this));
            RegisterPin(&m_outputVideoRGB);
            // Pin for Raw Depth Data
            m_outputDepthRaw.Create("outputDepthRaw", new adtf::cMediaType(MEDIA_TYPE_STRUCTURED_DATA, MEDIA_SUBTYPE_STRUCT_STRUCTURED), static_cast<IPinEventSink*>(this));
            RegisterPin(&m_outputDepthRaw);
            // Video Pin for Visualized Depth Video
            m_outputVideoDepthVis.Create("outputDepthVisualization", IPin::PD_Output, static_cast<IPinEventSink*>(this));
            RegisterPin(&m_outputVideoDepthVis);


        }
        else if (eStage == StageNormal)
        {

            // In this stage you would do further initialisation and/or create your dynamic pins.
        // Please take a look at the demo_dynamicpin example for further reference.
            m_filterProperties.enableDebugOutput = GetPropertyBool(PropEnableDebugName);

            // Sets Member Variables with the Values of the Properties
            // Framerate of Depth and Colour Stream
            m_filterProperties.DepthFPS = GetPropertyInt(PropFPSDepthName);
            m_filterProperties.ColourFPS = GetPropertyInt(PropFPSColourName);
            // Resolution of Depth and Colour Stream
            m_filterProperties.DepthResolution = GetPropertyStr(PropResolutionDepthName);
            m_filterProperties.ColourResolution = GetPropertyStr(PropResolutionColourName);
            // Enabling Depth, Colour and Infrared1/2 Stream
            m_filterProperties.Depth = GetPropertyBool(PropEnableDepthName);
            m_filterProperties.Colour = GetPropertyBool(PropEnableColourName);

            try
            {
                if (m_ctx->get_device_count() == 0)
                {
                    THROW_ERROR_DESC(ERR_NOT_CONNECTED, "Propably no Realsense Camera connected. Connect camera and restart ADTF");
                }
                else
                {
                    m_dev = m_ctx->get_device(0);
                }

                //print the available devices and their details to console
                for (int i = 0; i < m_ctx->get_device_count(); i++)
                {
                    m_cameraFound = tTrue;
                    rs::device * dev = m_ctx->get_device(i);
                    std::stringstream deviceDescription;
                    deviceDescription << "Realsense Device: " << i << " - " << dev->get_name() << "\n";
                    deviceDescription << "Serial number: " << dev->get_serial() << "\n";
                    deviceDescription << "Firmware Version: " << dev->get_firmware_version();
                    // in debug mode we also print the available propertier
                    if (m_filterProperties.enableDebugOutput)
                    {
                        deviceDescription << "\n";
                        for (int j = 0; j < NUMBER_AVAILABLE_PROPERTIES; j++)
                        {
                            if (dev->supports_option(static_cast<rs::option>(j)))
                            {
                                double min, max, step, def;
                                deviceDescription << dev->get_option_description(static_cast<rs::option>(j));
                                deviceDescription << ", ";
                                dev->get_option_range(static_cast<rs::option>(j), min, max, step, def);
                                deviceDescription << min;
                                deviceDescription << ", ";
                                deviceDescription << max;
                                deviceDescription << ", ";
                                deviceDescription << step;
                                deviceDescription << ", ";
                                deviceDescription << def;
                                deviceDescription << "\n";
                            }
                        }
                    }
                    LOG_INFO(deviceDescription.str().c_str());
                }
                // Sets the Supported Options of the Camera with the Values of the Properties
                for (int i = 0; i < NUMBER_AVAILABLE_PROPERTIES; i++)
                {
                    if (m_dev->supports_option(static_cast<rs::option>(i)))
                    {
                        m_dev->set_option(static_cast<rs::option>(i), (double)GetPropertyFloat(OptionName(i)));
                    }
                }
            }
            catch (const rs::error & e)
            {
                std::stringstream errorDesc;
                errorDesc << "Propably no Realsense Camera connected: " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n " << e.what();
                THROW_ERROR_DESC(ERR_NOT_CONNECTED, errorDesc.str().c_str());
                m_cameraFound = tFalse;
            }
            catch (const std::exception & e)
            {
                std::stringstream errorDesc;
                errorDesc << "Propably no Realsense Camera connected:" << e.what();
                THROW_ERROR_DESC(ERR_NOT_CONNECTED, errorDesc.str().c_str());
                m_cameraFound = tFalse;
            }

            if (m_cameraFound)
            {
                //Bitmat Format for the RBG Ouput VideoPin
                m_BitmapFormatRGBOut.nWidth = WidthOutOfResolution(m_filterProperties.ColourResolution);
                m_BitmapFormatRGBOut.nHeight = HeightOutOfResolution(m_filterProperties.ColourResolution);
                m_BitmapFormatRGBOut.nBitsPerPixel = 24;
                m_BitmapFormatRGBOut.nPixelFormat = IImage::PF_RGB_888;
                m_BitmapFormatRGBOut.nBytesPerLine = m_BitmapFormatRGBOut.nWidth * m_BitmapFormatRGBOut.nBitsPerPixel / 8;
                m_BitmapFormatRGBOut.nSize = m_BitmapFormatRGBOut.nBytesPerLine * m_BitmapFormatRGBOut.nHeight;
                m_BitmapFormatRGBOut.nPaletteSize = 0;
                //Bitmap Format for the Visualized Depth Output VideoPin
                m_BitmapFormatDepthOut.nWidth = WidthOutOfResolution(m_filterProperties.DepthResolution);
                m_BitmapFormatDepthOut.nHeight = HeightOutOfResolution(m_filterProperties.DepthResolution);
                m_BitmapFormatDepthOut.nBitsPerPixel = 16;
                m_BitmapFormatDepthOut.nPixelFormat = IImage::PF_GREYSCALE_16;
                m_BitmapFormatDepthOut.nBytesPerLine = m_BitmapFormatDepthOut.nWidth * m_BitmapFormatDepthOut.nBitsPerPixel / 8;
                m_BitmapFormatDepthOut.nSize = m_BitmapFormatDepthOut.nBytesPerLine * m_BitmapFormatDepthOut.nHeight;
                m_BitmapFormatDepthOut.nPaletteSize = 0;

                // Setting the Bitmap Formats to the fitting Pins
                m_outputVideoRGB.SetFormat(&m_BitmapFormatRGBOut, NULL);
                m_outputVideoDepthVis.SetFormat(&m_BitmapFormatDepthOut, NULL);

                std::stringstream loginfo;
                // Enabling the Stream in the Camera if set in the Properties
                if (m_filterProperties.Depth)
                {
                    m_dev->enable_stream(rs::stream::depth, WidthOutOfResolution(m_filterProperties.DepthResolution), HeightOutOfResolution(m_filterProperties.DepthResolution), rs::format::z16, m_filterProperties.DepthFPS);
                }
                if (m_filterProperties.Colour)
                {
                    m_dev->enable_stream(rs::stream::color, WidthOutOfResolution(m_filterProperties.ColourResolution), HeightOutOfResolution(m_filterProperties.ColourResolution), rs::format::bgr8, m_filterProperties.ColourFPS);
                }
                
                // Setting Emitter Enabled for better Depth Stream in near Range
                m_dev->set_option(rs::option::r200_emitter_enabled, 1);
            }
        }
        else if (eStage == StageGraphReady)
        {
            // All pin connections have been established in this stage so you can query your pins
            // about their media types and additional meta data.
            // Please take a look at the demo_imageproc example for further reference.
            m_Thread.Create(cKernelThread::TF_Suspended, static_cast<IKernelThreadFunc*>(this), NULL, 0);

        }

    RETURN_NOERROR;
}


tResult cRealsenseCameraFelix::Start(__exception) {
    if (m_cameraFound)
    {
        // Start the Camera
        m_dev->start();
    }
    //starting the Thread
    if (m_Thread.GetState() != cKernelThread::TS_Running)
    {
        m_Thread.Run();
    }
    return cFilter::Start(__exception_ptr);
}

tResult cRealsenseCameraFelix::Stop(__exception) {
    //suspend the thread
    if (m_Thread.GetState() == cKernelThread::TS_Running)
    {
        m_Thread.Suspend(tTrue);
    }

    if (m_cameraFound)
    {
        m_dev->stop();
    }

    return cFilter::Stop(__exception_ptr);
}

tResult cRealsenseCameraFelix::Shutdown(tInitStage eStage, __exception) {
    // In each stage clean up everything that you initiaized in the corresponding stage during Init.
    // Pins are an exception:
    // - The base class takes care of static pins that are members of this class.
    // - Dynamic pins have to be cleaned up in the ReleasePins method, please see the demo_dynamicpin
    //   example for further reference.

    switch (eStage) {
        case StageGraphReady: break;

        case StageNormal:
            m_Thread.Terminate(tTrue);
            m_Thread.Release();
            break;

        case StageFirst: break;
    }

    // call the base class implementation
    return cFilter::Shutdown(eStage, __exception_ptr);
}

// Waits for the Streams to send a Frame
tResult cRealsenseCameraFelix::ThreadFunc(adtf::cKernelThread* Thread, tVoid* data, tSize size) {
    if (m_cameraFound) {
        // Waiting for the Frame
        m_dev->wait_for_frames();
        // Transmitting Enabled Stream by calling associated Transmit function
        // RGB Stream
        if (m_dev->is_stream_enabled(rs::stream::color)) {
            TransmitRGB(m_dev->get_frame_data(rs::stream::color));
        }
        // Depth Stream
        if (m_dev->is_stream_enabled(rs::stream::depth)) {
            // Raw Depth Data
            TransmitDepthRaw(m_dev->get_frame_data(rs::stream::depth));
            // Calling Convert Function for Visualization
            std::vector<uchar> image;
            image.resize(m_BitmapFormatDepthOut.nSize);
            memcpy(image.data(), m_dev->get_frame_data(rs::stream::depth), m_BitmapFormatDepthOut.nSize);
            ConvertDepth(image.data());
        }
    }
    RETURN_NOERROR;
}

// Automatically creates Property from Camera Options
// Option Number is connected to enum of camera options provided by librealsense
void cRealsenseCameraFelix::CreateProperty(int OptionNumber) {
    // Savign the Min, Max, Step, and Default Values of the Option and putting them into the Description of the Property
    double min, max, step, def;
    cString name = OptionName(OptionNumber);
    std::stringstream desc;
    desc << m_dev->get_option_description(static_cast<rs::option>(OptionNumber));
    m_dev->get_option_range(static_cast<rs::option>(OptionNumber), min, max, step, def);
    desc << ", Min: " << min << ", Max: " << max << ", Step: " << step;
    // Setting the Property and its description
    SetPropertyFloat(name, def);
    SetPropertyStr(name + NSSUBPROP_DESCRIPTION, desc.str().c_str());

}

// Converting Raw Depth Data to Depth Visualization
tResult cRealsenseCameraFelix::ConvertDepth(void *pData) {
    cv::Mat depthImage = cv::Mat(
	    cv::Size(m_BitmapFormatDepthOut.nWidth, m_BitmapFormatDepthOut.nHeight),
	    CV_16U,
	    pData,
	    m_BitmapFormatDepthOut.nBytesPerLine
    );

    /* -> Always turns out to be 0 and 65535 */
    //cv::minMaxLoc(depthImage, &min, &max);
    std::cout << "DepthImage -> Min: " << m_filterProperties.minDepth << " | Max: " << m_filterProperties.maxDepth << "\n";

    
    
    depthImage.convertTo(depthImage, CV_16U, 65535 / (m_filterProperties.maxDepth - m_filterProperties.minDepth));
    bitwise_not(depthImage, depthImage);
    medianBlur(depthImage, depthImage, m_filterProperties.blurRadius);

    TransmitDepthVis(depthImage.data);

    RETURN_NOERROR;
}

// Transmit Function for RGB Video
tResult cRealsenseCameraFelix::TransmitRGB(const void *pData) {
    //Creating new media sample
    cObjectPtr<IMediaSample> pMediaSample;
    RETURN_IF_FAILED(AllocMediaSample((tVoid**)&pMediaSample));
    //allocing buffer memory
    RETURN_IF_FAILED(pMediaSample->AllocBuffer(m_BitmapFormatRGBOut.nSize));
    //updating media sample
    RETURN_IF_FAILED(pMediaSample->Update(_clock->GetStreamTime(), pData, m_BitmapFormatRGBOut.nSize, IMediaSample::MSF_None));
    //transmitting
    RETURN_IF_FAILED(m_outputVideoRGB.Transmit(pMediaSample));

    RETURN_NOERROR;
}

// Transmit Function for Raw Depth Data
tResult cRealsenseCameraFelix::TransmitDepthRaw(const void *pData) {
    //Creating new media sample
    cObjectPtr<IMediaSample> pMediaSample;
    RETURN_IF_FAILED(AllocMediaSample((tVoid**)&pMediaSample));
    //allocing buffer memory
    RETURN_IF_FAILED(pMediaSample->AllocBuffer(DEPTH_FRAME_SIZE));
    //updating media sample
    RETURN_IF_FAILED(pMediaSample->Update(_clock->GetStreamTime(), pData, DEPTH_FRAME_SIZE, IMediaSample::MSF_None));

    //transmitting
    RETURN_IF_FAILED(m_outputDepthRaw.Transmit(pMediaSample));

    RETURN_NOERROR;
}

// Transmit Function for Visualized Depth Video
tResult cRealsenseCameraFelix::TransmitDepthVis(const void *pData) {
    //Creating new media sample
    cObjectPtr<IMediaSample> pMediaSample;
    RETURN_IF_FAILED(AllocMediaSample((tVoid**)&pMediaSample));
    //allocing buffer memory
    RETURN_IF_FAILED(pMediaSample->AllocBuffer(m_BitmapFormatDepthOut.nSize));
    //updating media sample
    RETURN_IF_FAILED(pMediaSample->Update(_clock->GetStreamTime(), pData, m_BitmapFormatDepthOut.nSize, IMediaSample::MSF_None));
    //transmitting
    RETURN_IF_FAILED(m_outputVideoDepthVis.Transmit(pMediaSample));

    RETURN_NOERROR;
}


// Returns int Height from cString Resolution
int cRealsenseCameraFelix::HeightOutOfResolution(cString Resolution) {
    int Height = 0;
    if (Resolution == "1920x1080")
    {
        Height = 1080;
    }
    else if (Resolution == "1280x720")
    {
        Height = 720;
    }
    else if (Resolution == "640x480")
    {
        Height = 480;
    }
    else if (Resolution == "320x240")
    {
        Height = 240;
    }
    return Height;
}

// Returns int Width from cString Resolution
int cRealsenseCameraFelix::WidthOutOfResolution(cString Resolution) {

    int Width = 0;
    if (Resolution == "1920x1080")
    {
        Width = 1920;
    }
    else if (Resolution == "1280x720")
    {
        Width = 1280;
    }
    else if (Resolution == "640x480")
    {
        Width = 640;
    }
    else if (Resolution == "320x240")
    {
        Width = 320;
    }
    return Width;
}
