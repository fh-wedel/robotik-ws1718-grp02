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

cUltraSonicObstacleDetection::cUltraSonicObstacleDetection(const tChar* __info) : cStdFilter(__info),
frontRightFilter(1),
frontCenterRightFilter(1),
frontCenterFilter(1),
frontCenterLeftFilter(1),
frontLeftFilter(1),
rearRightFilter(1),
rearCenterFilter(1),
rearLeftFilter(1),
sideRightFilter(1),
sideLeftFilter(1),
m_bDebugModeEnabled(tFalse) {
    m_currentSteeringAngle = 0;
    m_currentSpeed = 0;

    SetPropertyBool(SC_PROP_DEBUG_MODE, tFalse);
    SetPropertyStr(SC_PROP_DEBUG_MODE NSSUBPROP_DESCRIPTION, "If true debug infos are plotted to console");
    SetPropertyBool(SC_PROP_DEBUG_MODE NSSUBPROP_ISCHANGEABLE, tTrue);

    SetPropertyFloat("ObstacleDetection::FrontThreshhold", 25.0f);
    SetPropertyStr("ObstacleDetection::FrontThreshhold" NSSUBPROP_DESCRIPTION, "when should a value be considered as an obstacle");
    SetPropertyBool("ObstacleDetection::FrontThreshhold" NSSUBPROP_ISCHANGEABLE, tTrue);

    SetPropertyFloat("ObstacleDetection::RearThreshhold", 25.0f);
    SetPropertyStr("ObstacleDetection::RearThreshhold" NSSUBPROP_DESCRIPTION, "when should a value be considered as an obstacle");
    SetPropertyBool("ObstacleDetection::RearThreshhold" NSSUBPROP_ISCHANGEABLE, tTrue);

    SetPropertyFloat("ObstacleDetection::DynamicSteeringAndSpeedThreshhold", 30.0f);
    SetPropertyStr("ObstacleDetection::DynamicSteeringAndSpeedThreshhold" NSSUBPROP_DESCRIPTION, "when a value should be considered as an obstacle");
    SetPropertyBool("ObstacleDetection::DynamicSteeringAndSpeedThreshhold" NSSUBPROP_ISCHANGEABLE, tTrue);

    SetPropertyFloat("ObstacleDetection::TerminalThreshhold", 7.0f);
    SetPropertyStr("ObstacleDetection::TerminalThreshhold" NSSUBPROP_DESCRIPTION, "when a value should be considered as an obstacle");
    SetPropertyBool("ObstacleDetection::TerminalThreshhold" NSSUBPROP_ISCHANGEABLE, tTrue);

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

tResult cUltraSonicObstacleDetection::CreateFloatInputPins(__exception) {

    registerFloatInputPin("currentSpeed", &m_InputSpeed, __exception_ptr);
    registerFloatInputPin("currentSteeringAngle", &m_InputSteeringAngle, __exception_ptr);

    RETURN_NOERROR;
}

tResult cUltraSonicObstacleDetection::CreateOutputPins(__exception) {

    RETURN_IF_FAILED(registerBoolOutputPin("obstacleInFront", &m_OutputObstacleInFront, __exception_ptr));
    RETURN_IF_FAILED(registerBoolOutputPin("obstacleBehind", &m_OutputObstacleBehind, __exception_ptr));
    RETURN_IF_FAILED(registerBoolOutputPin("obstacleInDrivingDirection", &m_OutputObstacleDrivingDirection, __exception_ptr));

    RETURN_NOERROR;
}

tResult cUltraSonicObstacleDetection::Init(tInitStage eStage, __exception) {
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr));

    switch (eStage) {
    case StageFirst:
        RETURN_IF_FAILED(CreateUSSInputPin(__exception_ptr));
        RETURN_IF_FAILED(CreateFloatInputPins(__exception_ptr));
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

    } else if (cString::IsEqual(strName, "ObstacleDetection::DynamicSteeringAndSpeedThreshhold")) {
        m_filterProperties.dynamicSteeringAndSpeedThreshhold = GetPropertyFloat("ObstacleDetection::DynamicSteeringAndSpeedThreshhold");

    } else if (cString::IsEqual(strName, "ObstacleDetection::TerminalThreshhold")) {
        m_filterProperties.terminalThreshhold = GetPropertyFloat("ObstacleDetection::TerminalThreshhold");

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

        sideLeftFilter.windowSize = windowSize;
        sideRightFilter.windowSize = windowSize;
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
            m_currentSteeringAngle = readFloatValue(pMediaSample);
        } else if (pSource == &m_InputSpeed) {
            m_currentSpeed = readFloatValue(pMediaSample);
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
        printf("\t\t<%4.2f | %4.2f>\n",
            pSampleData->tSideLeft.f32Value,
            pSampleData->tSideRight.f32Value
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

    sideLeftFilter.pushValue(pSampleData->tSideLeft.f32Value);
    sideRightFilter.pushValue(pSampleData->tSideRight.f32Value);

    rearLeftFilter.pushValue(pSampleData->tRearLeft.f32Value);
    rearCenterFilter.pushValue(pSampleData->tRearCenter.f32Value);
    rearRightFilter.pushValue(pSampleData->tRearRight.f32Value);

    // Check for Obstacles

    float frontLeftMedian           = frontLeftFilter.calculateMedian();
    float frontCenterLeftMedian     = frontCenterLeftFilter.calculateMedian();
    float frontCenterMedian         = frontCenterFilter.calculateMedian();
    float frontCenterRightMedian    = frontCenterRightFilter.calculateMedian();
    float frontRightMedian          = frontRightFilter.calculateMedian();

    float rearLeftMedian            = rearLeftFilter.calculateMedian();
    float rearCenterMedian          = rearCenterFilter.calculateMedian();
    float rearRightMedian           = rearRightFilter.calculateMedian();

    float sideLeftMedian            = sideLeftFilter.calculateMedian();
    float sideRightMedian           = sideRightFilter.calculateMedian();

    bool frontLeftSensorIsValid         = frontLeftMedian           > 0;
    bool frontCenterLeftSensorIsValid   = frontCenterLeftMedian     > 0;
    bool frontCenterSensorIsValid       = frontCenterMedian         > 0;
    bool frontCenterRightSensorIsValid  = frontCenterRightMedian    > 0;
    bool frontRightSensorIsValid        = frontRightMedian          > 0;

    bool rearLeftSensorIsValid          = rearLeftMedian    > 0;
    bool rearCenterSensorIsValid        = rearCenterMedian  > 0;
    bool rearRightSensorIsValid         = rearRightMedian   > 0;

    bool sideLeftSensorIsValid          = sideLeftMedian    > 0;
    bool sideRightSensorIsValid         = sideRightMedian   > 0;

    bool obstacleInFront =
            (frontLeftSensorIsValid         && frontLeftMedian         < m_filterProperties.frontDetectionThreshhold)
        ||  (frontCenterLeftSensorIsValid   && frontCenterLeftMedian   < m_filterProperties.frontDetectionThreshhold)
        ||  (frontCenterSensorIsValid       && frontCenterMedian       < m_filterProperties.frontDetectionThreshhold)
        ||  (frontCenterRightSensorIsValid  && frontCenterRightMedian  < m_filterProperties.frontDetectionThreshhold)
        ||  (frontRightSensorIsValid        && frontRightMedian        < m_filterProperties.frontDetectionThreshhold);


    bool obstacleBehind  =
            (rearLeftSensorIsValid   && rearLeftMedian    < m_filterProperties.rearDetectionThreshhold)
        ||  (rearCenterSensorIsValid && rearCenterMedian  < m_filterProperties.rearDetectionThreshhold)
        ||  (rearRightSensorIsValid  && rearRightMedian   < m_filterProperties.rearDetectionThreshhold);

    bool obstacleInDrivingDirection  =  (
            (sideLeftSensorIsValid
            && (sideLeftMedian          < getThreshholdForMountingAngle(-90.0f))
            )
        ||  (frontLeftSensorIsValid
            && (frontLeftMedian         < getThreshholdForMountingAngle(-56.0f))
            )
        ||  (frontCenterLeftSensorIsValid
            && (frontCenterLeftMedian   < getThreshholdForMountingAngle(-28.0f))
            )
        ||  (frontCenterSensorIsValid
            && (frontCenterMedian       < getThreshholdForMountingAngle(0.0f))
            )
        ||  (frontCenterRightSensorIsValid
            && (frontCenterRightMedian  < getThreshholdForMountingAngle(28.0f))
            )
        ||  (frontRightSensorIsValid
            && (frontRightMedian        < getThreshholdForMountingAngle(56.0f))
            )
        ||  (sideRightSensorIsValid
            && (sideRightMedian         < getThreshholdForMountingAngle(90.0f))
            )
    );

    if (m_bDebugModeEnabled) {
        printf("\n\t\t<%4.2f | %4.2f | %4.2f | %4.2f | %4.2f>\n",
            frontLeftMedian,
            frontCenterLeftMedian,
            frontCenterMedian,
            frontCenterRightMedian,
            frontRightMedian
        );
        printf("\t\t<%4.2f | %4.2f>\n",
            sideLeftMedian,
            sideRightMedian
        );
        printf("\t\t<%4.2f | %4.2f | %4.2f>\n\n",
            rearLeftMedian,
            rearCenterMedian,
            rearRightMedian
        );
    }


    if (m_bDebugModeEnabled) {
        printf("OBSTACLES: %d %d %d\n", obstacleInFront, obstacleBehind, obstacleInDrivingDirection);
    }

    // Transmit Results

    transmitBoolValue(obstacleInFront, &m_OutputObstacleInFront);
    transmitBoolValue(obstacleBehind, &m_OutputObstacleBehind);
    transmitBoolValue(obstacleInDrivingDirection, &m_OutputObstacleDrivingDirection);

    //TODO: Transmit TRUE if too many sensors are faulty.

    RETURN_NOERROR;
}

tFloat32 cUltraSonicObstacleDetection::getAmplificationForMountingAngle(tFloat32 mountingAngle) {

    // calc parabola amplification for this sensors direction
    tFloat32 amplification = 1 - powf((mountingAngle - m_currentSteeringAngle) / 180 * M_PI, 2.0f);

    // NOTE: React more strictly if velocity is high.
    #define DEFAULT_SPEED (0.25)
    #define fbound(low, x, up) (fmin(up, fmax(x, low)))
    float speedFactor = fbound(1.0f, m_currentSpeed / DEFAULT_SPEED, 2.0f);
    amplification *= speedFactor;

    // cap it so if way too close it still reacts.
    amplification = fmax(m_filterProperties.terminalThreshhold, amplification);

    if (m_bDebugModeEnabled) {
        printf("\nSteeringAngle: %4.2f \t\t| MountingAngle: %4.2f \t\t| Amplification: %4.2f\n", m_currentSteeringAngle, mountingAngle, amplification);
    }

    return amplification;
}

tFloat32 cUltraSonicObstacleDetection::getThreshholdForMountingAngle(tFloat32 mountingAngle) {
    return m_filterProperties.dynamicSteeringAndSpeedThreshhold * getAmplificationForMountingAngle(mountingAngle);
}
