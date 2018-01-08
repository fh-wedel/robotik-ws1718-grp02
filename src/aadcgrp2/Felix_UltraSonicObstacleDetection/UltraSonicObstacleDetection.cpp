/**********************************************************************
Copyright (c)
Audi Autonomous Driving Cup. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3.  All advertising materials mentioning features or use of this software must display the following acknowledgement: �This product includes software developed by the Audi AG and its contributors for Audi Autonomous Driving Cup.�
4.  Neither the name of Audi nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY AUDI AG AND CONTRIBUTORS �AS IS� AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL AUDI AG OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


**********************************************************************
* $Author:: spie#$  $Date:: 2017-05-12 09:39:39#$ $Rev:: 63110   $
**********************************************************************/


#include "stdafx.h"
#include "UltraSonicObstacleDetection.h"


#define SC_PROP_DEBUG_MODE "Debug Mode"


ADTF_FILTER_PLUGIN(FILTER_NAME, UNIQUE_FILTER_ID, cUltraSonicObstacleDetection)

cUltraSonicObstacleDetection::cUltraSonicObstacleDetection(const tChar* __info) : cFilter(__info),
frontRightFilter(1),
frontCenterRightFilter(1),
frontCenterFilter(1),
frontCenterLeftFilter(1),
frontLeftFilter(1),
rearRightFilter(1),
rearCenterFilter(1),
rearLeftFilter(1),
m_bDebugModeEnabled(tFalse) {
    m_currentSteeringAngle = 0;

    SetPropertyBool(SC_PROP_DEBUG_MODE, tFalse);
    SetPropertyStr(SC_PROP_DEBUG_MODE NSSUBPROP_DESCRIPTION, "If true debug infos are plotted to console");
    SetPropertyBool(SC_PROP_DEBUG_MODE NSSUBPROP_ISCHANGEABLE, tTrue);

    SetPropertyFloat("ObstacleDetection::FrontThreshhold", 25.0f);
    SetPropertyStr("ObstacleDetection::FrontThreshhold" NSSUBPROP_DESCRIPTION, "when should a value be considered as an obstacle");
    SetPropertyBool("ObstacleDetection::FrontThreshhold" NSSUBPROP_ISCHANGEABLE, tTrue);

    SetPropertyFloat("ObstacleDetection::RearThreshhold", 25.0f);
    SetPropertyStr("ObstacleDetection::RearThreshhold" NSSUBPROP_DESCRIPTION, "when should a value be considered as an obstacle");
    SetPropertyBool("ObstacleDetection::RearThreshhold" NSSUBPROP_ISCHANGEABLE, tTrue);

    SetPropertyInt("MedianFilter::WindowSize", 10);
    SetPropertyStr("MedianFilter::WindowSize" NSSUBPROP_DESCRIPTION, "the number of values to consider.");
    SetPropertyBool("MedianFilter::WindowSize" NSSUBPROP_ISCHANGEABLE, tTrue);

}

cUltraSonicObstacleDetection::~cUltraSonicObstacleDetection() {}

tResult cUltraSonicObstacleDetection::CreateUSSInputPin(__exception) {
    cObjectPtr<IMediaDescriptionManager> pDescManager;

    RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER, IID_ADTF_MEDIA_DESCRIPTION_MANAGER, (tVoid**)&pDescManager, __exception_ptr));

    tChar const * strUltrasonicStruct = pDescManager->GetMediaDescription("tUltrasonicStruct");
    RETURN_IF_POINTER_NULL(strUltrasonicStruct);
    cObjectPtr<IMediaType> pTypeUltrasonicStruct = new cMediaType(0, 0, 0, "tUltrasonicStruct", strUltrasonicStruct, IMediaDescription::MDF_DDL_DEFAULT_VERSION);

    RETURN_IF_FAILED(pTypeUltrasonicStruct->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_pDescriptionUsData));

    RETURN_IF_FAILED(m_oInputUssStruct.Create("UltrasonicStruct", pTypeUltrasonicStruct, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(&m_oInputUssStruct));

    RETURN_NOERROR;
}

tResult cUltraSonicObstacleDetection::CreateAngleInputPin(__exception) {
    // create description manager
    cObjectPtr<IMediaDescriptionManager> pDescManager;
    RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER,IID_ADTF_MEDIA_DESCRIPTION_MANAGER,(tVoid**)&pDescManager,__exception_ptr));

    // get media tayp
    tChar const * strDescSignalValue = pDescManager->GetMediaDescription("tSignalValue");
    RETURN_IF_POINTER_NULL(strDescSignalValue);
    cObjectPtr<IMediaType> pTypeSignalValue = new cMediaType(0, 0, 0, "tSignalValue", strDescSignalValue,IMediaDescription::MDF_DDL_DEFAULT_VERSION);


    // Input Value
    // set member media description
    RETURN_IF_FAILED(pTypeSignalValue->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_pDescriptionSteeringAngle));

    // create pin
    RETURN_IF_FAILED(m_InputSteeringAngle.Create("currentSteeringAngle", pTypeSignalValue, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(&m_InputSteeringAngle));

    RETURN_NOERROR;
}

tResult cUltraSonicObstacleDetection::CreateOutputPins(__exception) {
    //get the media description manager for this filter
    cObjectPtr<IMediaDescriptionManager> pDescManager;
    RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER,IID_ADTF_MEDIA_DESCRIPTION_MANAGER,(tVoid**)&pDescManager,__exception_ptr));
    //get description
    tChar const * strDescSignalValue = pDescManager->GetMediaDescription("tBoolSignalValue");
    // checks if exists
    RETURN_IF_POINTER_NULL(strDescSignalValue);

    //get mediatype
    cObjectPtr<IMediaType> pTypeSignalValue = new cMediaType(0, 0, 0, "tBoolSignalValue", strDescSignalValue, IMediaDescription::MDF_DDL_DEFAULT_VERSION);

    //get mediatype description
    RETURN_IF_FAILED(pTypeSignalValue->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_OutputValueDescription));

    // create front pin
    RETURN_IF_FAILED(m_OutputObstacleInFront.Create("obstacleInFront", pTypeSignalValue, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(&m_OutputObstacleInFront));

    // create rear pin
    RETURN_IF_FAILED(m_OutputObstacleBehind.Create("obstacleBehind", pTypeSignalValue, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(&m_OutputObstacleBehind));


    // create driving direction pin
    RETURN_IF_FAILED(m_OutputObstacleDrivingDirection.Create("obstacleInDrivingDirection", pTypeSignalValue, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(&m_OutputObstacleDrivingDirection));


    RETURN_NOERROR;
}

tResult cUltraSonicObstacleDetection::Init(tInitStage eStage, __exception) {
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr));

    switch (eStage) {
    case StageFirst:
        RETURN_IF_FAILED(CreateUSSInputPin(__exception_ptr));
        RETURN_IF_FAILED(CreateAngleInputPin(__exception_ptr));
        RETURN_IF_FAILED(CreateOutputPins(__exception_ptr));
        break;

    case StageNormal:
        m_bDebugModeEnabled = GetPropertyBool(SC_PROP_DEBUG_MODE);
        break;

    case StageGraphReady:
        break;
    }

    RETURN_NOERROR;
}


// FELIX' MODIFICATIONS BELOW

tResult cUltraSonicObstacleDetection::PropertyChanged(const tChar* strName) {
    RETURN_IF_FAILED(cFilter::PropertyChanged(strName));
    //associate the properties to the member

  	if (cString::IsEqual(strName, SC_PROP_DEBUG_MODE)) {
  		m_bDebugModeEnabled = GetPropertyBool(SC_PROP_DEBUG_MODE);

    } else if (cString::IsEqual(strName, "ObstacleDetection::FrontThreshhold")) {
        m_filterProperties.frontDetectionThreshhold = GetPropertyFloat("ObstacleDetection::FrontThreshhold");

    } else if (cString::IsEqual(strName, "ObstacleDetection::RearThreshhold")) {
        m_filterProperties.rearDetectionThreshhold = GetPropertyFloat("ObstacleDetection::RearThreshhold");

    } else if (cString::IsEqual(strName, "MedianFilter::WindowSize")) {

        tInt windowSize = GetPropertyInt("MedianFilter::WindowSize");

        frontLeftFilter.windowSize = windowSize;
        frontCenterLeftFilter.windowSize = windowSize;
        frontCenterFilter.windowSize = windowSize;
        frontCenterRightFilter.windowSize = windowSize;
        frontRightFilter.windowSize = windowSize;

        rearLeftFilter.windowSize = windowSize;
        rearCenterFilter.windowSize = windowSize;
        rearRightFilter.windowSize = windowSize;
    }

	RETURN_NOERROR;
}

tResult cUltraSonicObstacleDetection::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived && pMediaSample != NULL) {
        RETURN_IF_POINTER_NULL(pMediaSample);

        if (pSource == &m_oInputUssStruct && pMediaSample->GetSize() == sizeof(tUltrasonicStruct)) {
            tUltrasonicStruct* pSampleData = NULL;
            if (IS_OK(pMediaSample->Lock((const tVoid**)&pSampleData))) {

                OnValueChanged(pSampleData);

                pMediaSample->Unlock(pSampleData);
            }
        } else if (pSource == &m_InputSteeringAngle) {
            m_currentSteeringAngle = readInputValue(pMediaSample);
        }
    }
    RETURN_NOERROR;
}

tResult cUltraSonicObstacleDetection::OnValueChanged(tUltrasonicStruct* pSampleData) {

    if (m_bDebugModeEnabled) {
        printf("\n\t\t<%4.2f | %4.2f | %4.2f | %4.2f | %4.2f>\n",
            pSampleData->tFrontLeft.f32Value,
            pSampleData->tFrontCenterLeft.f32Value,
            pSampleData->tFrontCenter.f32Value,
            pSampleData->tFrontCenterRight.f32Value,
            pSampleData->tFrontRight.f32Value
        );
        printf("\t\t<%4.2f | %4.2f | %4.2f>\n\n",
            pSampleData->tRearLeft.f32Value,
            pSampleData->tRearCenter.f32Value,
            pSampleData->tRearRight.f32Value
        );
    }

    // Add raw values to median filter

    frontLeftFilter.pushValue(pSampleData->tFrontLeft.f32Value);
    frontCenterLeftFilter.pushValue(pSampleData->tFrontCenterLeft.f32Value);
    frontCenterFilter.pushValue(pSampleData->tFrontCenter.f32Value);
    frontCenterRightFilter.pushValue(pSampleData->tFrontCenterRight.f32Value);
    frontRightFilter.pushValue(pSampleData->tFrontRight.f32Value);

    rearLeftFilter.pushValue(pSampleData->tRearLeft.f32Value);
    rearCenterFilter.pushValue(pSampleData->tRearCenter.f32Value);
    rearRightFilter.pushValue(pSampleData->tRearRight.f32Value);

    // Check for Obstacles

    bool obstacleInFront =
            frontLeftFilter.calculateMedian()           < m_filterProperties.frontDetectionThreshhold
        ||  frontCenterLeftFilter.calculateMedian()     < m_filterProperties.frontDetectionThreshhold
        ||  frontCenterFilter.calculateMedian()         < m_filterProperties.frontDetectionThreshhold
        ||  frontCenterRightFilter.calculateMedian()    < m_filterProperties.frontDetectionThreshhold
        ||  frontRightFilter.calculateMedian()          < m_filterProperties.frontDetectionThreshhold;


    bool obstacleBehind  =
            rearLeftFilter.calculateMedian()    < m_filterProperties.rearDetectionThreshhold
        ||  rearCenterFilter.calculateMedian()  < m_filterProperties.rearDetectionThreshhold
        ||  rearRightFilter.calculateMedian()   < m_filterProperties.rearDetectionThreshhold;


    bool obstacleInDrivingDirection  =
            getAmplificationForMountingAngle(-45.0f)    * frontLeftFilter.calculateMedian()          < m_filterProperties.frontDetectionThreshhold
        ||  getAmplificationForMountingAngle(-22.5f)    * frontCenterLeftFilter.calculateMedian()    < m_filterProperties.frontDetectionThreshhold
        ||  getAmplificationForMountingAngle(0.0f)      * frontCenterFilter.calculateMedian()        < m_filterProperties.frontDetectionThreshhold
        ||  getAmplificationForMountingAngle(22.5f)     * frontCenterRightFilter.calculateMedian()   < m_filterProperties.frontDetectionThreshhold
        ||  getAmplificationForMountingAngle(45.0f)     * frontRightFilter.calculateMedian()         < m_filterProperties.frontDetectionThreshhold;



    if (m_bDebugModeEnabled) {
        printf("\n\t\t<%4.2f | %4.2f | %4.2f | %4.2f | %4.2f>\n",
            frontLeftFilter.calculateMedian(),
            frontCenterLeftFilter.calculateMedian(),
            frontCenterFilter.calculateMedian(),
            frontCenterRightFilter.calculateMedian(),
            frontRightFilter.calculateMedian()
        );
        printf("\t\t<%4.2f | %4.2f | %4.2f>\n\n",
            rearLeftFilter.calculateMedian(),
            rearCenterFilter.calculateMedian(),
            rearRightFilter.calculateMedian()
        );
    }


    if (m_bDebugModeEnabled) {
        printf("OBSTACLES: %d %d %d\n", obstacleInFront, obstacleBehind, obstacleInDrivingDirection);
    }

    // Transmit Results

    transmitValue(obstacleInFront, &m_OutputObstacleInFront);
    transmitValue(obstacleBehind, &m_OutputObstacleBehind);
    transmitValue(obstacleInDrivingDirection, &m_OutputObstacleDrivingDirection);

    RETURN_NOERROR;
}

tFloat32 cUltraSonicObstacleDetection::getAmplificationForMountingAngle(tFloat32 mountingAngle) {

    tFloat32 amplification = 2.0f - expf(-3.0f * powf((mountingAngle - m_currentSteeringAngle) / 180 * M_PI, 2.0f));

    if (m_bDebugModeEnabled) {
        printf("\nSteeringAngle: %4.2f \t\t| MountingAngle: %4.2f \t\t| Amplification: %4.2f\n", m_currentSteeringAngle, mountingAngle, amplification);
    }

    return amplification;
}

// Input Value PROCESSING

tFloat32 cUltraSonicObstacleDetection::readInputValue(IMediaSample* pMediaSample) {
    tFloat32 value = 0;
    tUInt32 timestamp = 0;

    {
        // focus for sample read lock
        // read data from the media sample with the coder of the descriptor
        __adtf_sample_read_lock_mediadescription(m_pDescriptionSteeringAngle, pMediaSample, pCoder);

        /*! indicates of bufferIDs were set */
        static tBool m_InputValueDescriptionIsInitialized = false;
        /*! the id for the f32value of the media description for input pin for the radius input */
        static tBufferID m_InputValueDescriptionID;
        /*! the id for the arduino time stamp of the media description for input pin for the timestamp */
        static tBufferID m_InputValueTimestampID;

        if(!m_InputValueDescriptionIsInitialized) {

            pCoder->GetID("f32Value", m_InputValueDescriptionID);
            pCoder->GetID("ui32ArduinoTimestamp", m_InputValueTimestampID);
            m_InputValueDescriptionIsInitialized = true;

        }

        // get values from media sample
        pCoder->Get(m_InputValueDescriptionID, (tVoid*)&value);
        pCoder->Get(m_InputValueTimestampID, (tVoid*)&timestamp);
    }

    return value;
}

// Write Output Values

tResult cUltraSonicObstacleDetection::transmitValue(tBool value, cOutputPin* outputPin) {

    cObjectPtr<IMediaSample> pMediaSample = initMediaSample(m_OutputValueDescription);
    {
        // focus for sample write lock
        // read data from the media sample with the coder of the descriptor
        __adtf_sample_write_lock_mediadescription(m_OutputValueDescription, pMediaSample, pCoder);

        /*! indicates of bufferIDs were set */
        static tBool m_OutputValueDescriptionIsInitialized = false;
        /*! the id for the f32value of the media description for input pin for the set speed */
        static tBufferID m_OutputValueDescriptionID;
        /*! the id for the arduino time stamp of the media description for input pin for the set speed */
        static tBufferID m_OutputValueTimestampID;

        if(!m_OutputValueDescriptionIsInitialized) {
            pCoder->GetID("bValue", m_OutputValueDescriptionID);
            pCoder->GetID("ui32ArduinoTimestamp", m_OutputValueTimestampID);
            m_OutputValueDescriptionIsInitialized = tTrue;
        }

        //write values to media sample
        pCoder->Set(m_OutputValueDescriptionID, (tVoid*)&value);
    }

    //transmit media sample over output pin
    RETURN_IF_FAILED(pMediaSample->SetTime(_clock->GetStreamTime()));
    RETURN_IF_FAILED(outputPin->Transmit(pMediaSample));

    RETURN_NOERROR;
}

cObjectPtr<IMediaSample> cUltraSonicObstacleDetection::initMediaSample(cObjectPtr<IMediaTypeDescription> typeDescription) {

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
