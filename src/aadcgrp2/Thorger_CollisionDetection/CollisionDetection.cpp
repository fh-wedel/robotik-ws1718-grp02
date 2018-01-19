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
#include "CollisionDetection.h"


#define SC_PROP_DEBUG_MODE "Debug Mode"


ADTF_FILTER_PLUGIN(FILTER_NAME, UNIQUE_FILTER_ID, cCollisionDetection)

cCollisionDetection::cCollisionDetection(const tChar* __info) : cStdFilter(__info),
accelXFilter(1),
accelYFilter(1),
accelZFilter(1),
m_bDebugModeEnabled(tFalse) {
    m_currentSteeringAngle = 0;
    m_currentSpeed = 0;

    SetPropertyBool(SC_PROP_DEBUG_MODE, tFalse);
    SetPropertyStr(SC_PROP_DEBUG_MODE NSSUBPROP_DESCRIPTION, "If true debug infos are plotted to console");
    SetPropertyBool(SC_PROP_DEBUG_MODE NSSUBPROP_ISCHANGEABLE, tTrue);

    SetPropertyFloat("CollisionDetection::DetectionThreshhold", 25.0f);
    SetPropertyStr("CollisionDetection::DetectionThreshhold" NSSUBPROP_DESCRIPTION, "when should a value be considered as an collision");
    SetPropertyBool("CollisionDetection::DetectionThreshhold" NSSUBPROP_ISCHANGEABLE, tTrue);

    SetPropertyInt("MedianFilter::WindowSize", 10);
    SetPropertyStr("MedianFilter::WindowSize" NSSUBPROP_DESCRIPTION, "the number of values to consider.");
    SetPropertyBool("MedianFilter::WindowSize" NSSUBPROP_ISCHANGEABLE, tTrue);

}

cCollisionDetection::~cCollisionDetection() {}

tResult cCollisionDetection::CreateInerMeasInputPin(__exception) {
    cObjectPtr<IMediaDescriptionManager> pDescManager;

    RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER, IID_ADTF_MEDIA_DESCRIPTION_MANAGER, (tVoid**)&pDescManager, __exception_ptr));

    tChar const * strDescInerMeasUnit = pDescManager->GetMediaDescription("tInerMeasUnitData");
    RETURN_IF_POINTER_NULL(strDescInerMeasUnit);
    cObjectPtr<IMediaType> pTypeInerMeasUnit = new cMediaType(0, 0, 0, "tInerMeasUnitData", strDescInerMeasUnit, IMediaDescription::MDF_DDL_DEFAULT_VERSION);

    RETURN_IF_FAILED(pTypeInerMeasUnit->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_pDescriptionInerMeasUnitData));

    RETURN_IF_FAILED(m_oInputInerMeasUnit.Create("InerMeasUnit_Struct", pTypeInerMeasUnit, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(&m_oInputInerMeasUnit));

    RETURN_NOERROR;
}

tResult cCollisionDetection::CreateFloatInputPins(__exception) {

    registerFloatInputPin("currentSpeed", &m_InputSpeed, __exception_ptr);
    registerFloatInputPin("currentSteeringAngle", &m_InputSteeringAngle, __exception_ptr);

    RETURN_NOERROR;
}

tResult cCollisionDetection::CreateOutputPins(__exception) {

    RETURN_IF_FAILED(registerBoolOutputPin("Collision Detected", &m_OutputCollisionDetected, __exception_ptr));

    RETURN_NOERROR;
}

tResult cCollisionDetection::Init(tInitStage eStage, __exception) {
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr));

    switch (eStage) {
    case StageFirst:
        RETURN_IF_FAILED(CreateInerMeasInputPin(__exception_ptr));
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

tResult cCollisionDetection::PropertyChanged(const tChar* strName) {
    RETURN_IF_FAILED(cFilter::PropertyChanged(strName));
    //associate the properties to the member

  	if (cString::IsEqual(strName, SC_PROP_DEBUG_MODE)) {
  		m_bDebugModeEnabled = GetPropertyBool(SC_PROP_DEBUG_MODE);

    } else if (cString::IsEqual(strName, "CollisionDetection::DetectionThreshhold")) {
        m_filterProperties.detectionThreshhold = GetPropertyFloat("CollisionDetection::DetectionThreshhold");

    } else if (cString::IsEqual(strName, "MedianFilter::WindowSize")) {

        tInt windowSize = GetPropertyInt("MedianFilter::WindowSize");

        accelXFilter.windowSize = windowSize;
        accelYFilter.windowSize = windowSize;
        accelZFilter.windowSize = windowSize;
    }

	RETURN_NOERROR;
}

tResult cCollisionDetection::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived && pMediaSample != NULL) {
        RETURN_IF_POINTER_NULL(pMediaSample);

        if (pSource == &m_oInputInerMeasUnit) {
            RETURN_IF_FAILED(DecodeInerMeasSample(pMediaSample));
        } else if (pSource == &m_InputSteeringAngle) {
            m_currentSteeringAngle = readFloatValue(pMediaSample);
        } else if (pSource == &m_InputSpeed) {
            m_currentSpeed = readFloatValue(pMediaSample);
        }
    }
    RETURN_NOERROR;

}

tResult cCollisionDetection::DecodeInerMeasSample(IMediaSample* pMediaSample) {
    tUInt32 ui32ArduinoTimestamp = 0;

    tFloat32 accelX = 0;
    tFloat32 accelY = 0;
    tFloat32 accelZ = 0;

    static tUInt32 lastArduinoTimestamp = 0;

    static bool hasID = false;
    static tBufferID szIDInerMeasUnitArduinoTimestamp;
    static tBufferID szIDInerMeasUnitF32A_x;
    static tBufferID szIDInerMeasUnitF32A_y;
    static tBufferID szIDInerMeasUnitF32A_z;

    {
        __adtf_sample_read_lock_mediadescription(m_pDescriptionInerMeasUnitData, pMediaSample, pCoderInput);

        if (!hasID)
        {
            pCoderInput->GetID("ui32ArduinoTimestamp", szIDInerMeasUnitArduinoTimestamp);

            pCoderInput->GetID("f32A_x", szIDInerMeasUnitF32A_x);
            pCoderInput->GetID("f32A_y", szIDInerMeasUnitF32A_y);
            pCoderInput->GetID("f32A_z", szIDInerMeasUnitF32A_z);

            hasID = true;
        }

        pCoderInput->Get(szIDInerMeasUnitF32A_x, (tVoid*)&accelX);
        pCoderInput->Get(szIDInerMeasUnitF32A_y, (tVoid*)&accelY);
        pCoderInput->Get(szIDInerMeasUnitF32A_z, (tVoid*)&accelZ);

        pCoderInput->Get(szIDInerMeasUnitArduinoTimestamp, (tVoid*)&ui32ArduinoTimestamp);

    }

    // seconds in floaties
    tFloat32 deltaT = static_cast<tFloat32> (ui32ArduinoTimestamp - lastArduinoTimestamp) / 1000000.f;

    OnValueChanged(deltaT, accelX, accelY, accelZ);

    RETURN_NOERROR;
}

tResult cCollisionDetection::OnValueChanged(
    tFloat32 deltaT,
    tFloat32 accelX, tFloat32 accelY, tFloat32 accelZ
) {

    if (m_bDebugModeEnabled) {
        printf("\n\t\t<%4.2f | %4.2f | %4.2f>\n",
            accelX, accelY, accelZ
        );
    }

    // Add raw values to median filter
    accelXFilter.pushValue(accelX);
    accelYFilter.pushValue(accelY);
    accelZFilter.pushValue(accelZ);

    tFloat32 a_x = accelXFilter.calculateMedian(); 
    tFloat32 a_y = accelYFilter.calculateMedian();
    tFloat32 a_z = accelYFilter.calculateMedian();
    
    //Berechnung der Beschleunigung über Pythagoras in drei Dimensionen
    tFloat32 a = sqrt(a_x*a_x + a_y*a_y + a_z*a_z);

    bool collisionDetected = true;
    
    //TODO: werte ermitteln
    if (a > 0.1) {
    	collisionDetected = false;
    }

    if (m_bDebugModeEnabled) {
	printf("\n\t\t<%4.2f | %4.2f | %4.2f>\n", a_x, a_y, a_z);
	printf("Collision Detected: %s.\n",(collisionDetected) ? "yes" : " no");
    }

    // Transmit Results
    transmitBoolValue(collisionDetected, &m_OutputCollisionDetected);

    RETURN_NOERROR;
}
