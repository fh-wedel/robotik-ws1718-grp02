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
#include "AngleToServo.h"


#define SC_PROP_DEBUG_MODE "Debug Mode"

#define IfDebug(x) /*std::cout << x << endl*/;


ADTF_FILTER_PLUGIN(FILTER_NAME, UNIQUE_FILTER_ID, cAngleToServoConverter)

cAngleToServoConverter::cAngleToServoConverter(const tChar* __info) : cStdFilter(__info), m_bDebugModeEnabled(tFalse) {
    SetPropertyBool(SC_PROP_DEBUG_MODE, tFalse);
    SetPropertyStr(SC_PROP_DEBUG_MODE NSSUBPROP_DESCRIPTION, "If true debug infos are plotted to console");

    /*! maximum deflection of front tires (this value corresponds to an inner diameter of 80cm) */
    SetPropertyFloat("Algorithm::MaxLeftAngle", 34.2);
    SetPropertyStr("Algorithm::MaxLeftAngle" NSSUBPROP_DESCRIPTION, "maximum deflection of front left tire");
    SetPropertyBool("Algorithm::MaxLeftAngle" NSSUBPROP_ISCHANGEABLE, tTrue);

    /*! maximum deflection of front right tires (this value corresponds to an inner diameter of 80cm between tires) */
    SetPropertyFloat("Algorithm::MaxRightAngle", 34.2);
    SetPropertyStr("Algorithm::MaxRightAngle" NSSUBPROP_DESCRIPTION, "maximum deflection of front right tire");
    SetPropertyBool("Algorithm::MaxRightAngle" NSSUBPROP_ISCHANGEABLE, tTrue);

}

cAngleToServoConverter::~cAngleToServoConverter() {}


tResult cAngleToServoConverter::Init(tInitStage eStage, __exception) {
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr))

    if (eStage == StageFirst) {
        RETURN_IF_FAILED(registerFloatInputPin("angle", &m_InputAngle, __exception_ptr));
        RETURN_IF_FAILED(registerFloatOutputPin("servoValue", &m_OutputServoValue, __exception_ptr));
    } else if (eStage == StageNormal) {
        m_bDebugModeEnabled = GetPropertyBool(SC_PROP_DEBUG_MODE);
    } else if(eStage == StageGraphReady) {}

    RETURN_NOERROR;
}


// FELIX' MODIFICATIONS BELOW

tResult cAngleToServoConverter::PropertyChanged(const tChar* strName) {
    RETURN_IF_FAILED(cFilter::PropertyChanged(strName));
    //associate the properties to the member
    if (cString::IsEqual(strName, "Algorithm::MaxLeftAngle"))
        m_filterProperties.maxLeftAngle = GetPropertyFloat("Algorithm::MaxLeftAngle");
    else if (cString::IsEqual(strName, "Algorithm::MaxRightAngle"))
        m_filterProperties.maxRightAngle = GetPropertyFloat("Algorithm::MaxRightAngle");

	RETURN_NOERROR;
}

tResult cAngleToServoConverter::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived && pMediaSample != NULL) {
        RETURN_IF_POINTER_NULL(pMediaSample);

        if (pSource == &m_InputAngle) {
            IfDebug("Received Angle | Reading...")

            tFloat32 angle = readFloatValue(pMediaSample);
            IfDebug(cString::Format("Read Angle: %lf | Converting...", angle))

            tFloat32 servoValue = convertAngleToServoValue(angle);
            IfDebug(cString::Format("Converted to servoValue: %lf | Transmitting...", servoValue))

            return transmitFloatValue(servoValue, &m_OutputServoValue);
        }
    }
    RETURN_NOERROR;
}


tFloat32 cAngleToServoConverter::convertAngleToServoValue(tFloat32 angle) {

    tFloat32 percentage = 0;

    if (fabs(angle) <= 0.1f) {
        return 0;
    }

    if (angle < 0) {
        // go left

        angle = fabs(angle);
        if (angle > m_filterProperties.maxRightAngle) {
            percentage = -1;
        } else {
            percentage = -(angle / m_filterProperties.maxRightAngle);
        }

    } else {
        // go right

        if (angle > m_filterProperties.maxLeftAngle) {
            percentage = 1;
        } else {
            percentage = angle / m_filterProperties.maxLeftAngle;
        }

    }

    return percentage * 100.0f;
}
