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
#include "RadiusToAngle.h"


#define SC_PROP_DEBUG_MODE "Debug Mode"

#define IfDebug(x) std::cout << x << endl;


ADTF_FILTER_PLUGIN(FILTER_NAME, UNIQUE_FILTER_ID, cRadiusToAngleConverter)

cRadiusToAngleConverter::cRadiusToAngleConverter(const tChar* __info) : cStdFilter(__info), m_bDebugModeEnabled(tFalse) {
    SetPropertyBool(SC_PROP_DEBUG_MODE, tFalse);
    SetPropertyStr(SC_PROP_DEBUG_MODE NSSUBPROP_DESCRIPTION, "If true debug infos are plotted to console");
    SetPropertyBool(SC_PROP_DEBUG_MODE NSSUBPROP_ISCHANGEABLE, tTrue);

    /*! the distance between axles (Radstand). */
    SetPropertyFloat("Algorithm::WheelBase", 360.0);
    SetPropertyStr("Algorithm::WheelBase" NSSUBPROP_DESCRIPTION, "the distance between axles (Radstand).");
    SetPropertyBool("Algorithm::WheelBase" NSSUBPROP_ISCHANGEABLE, tTrue);

    /*! the distance between the middle of left and right tires (Spurweite). */
    SetPropertyFloat("Algorithm::Tread", 270.0);
    SetPropertyStr("Algorithm::Tread" NSSUBPROP_DESCRIPTION, "the distance between the middle of left and right tires (Spurweite).");
    SetPropertyBool("Algorithm::Tread" NSSUBPROP_ISCHANGEABLE, tTrue);

}

cRadiusToAngleConverter::~cRadiusToAngleConverter() {}

tResult cRadiusToAngleConverter::Init(tInitStage eStage, __exception) {
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr))

    if (eStage == StageFirst) {
        RETURN_IF_FAILED(registerFloatInputPin("radius", &m_InputRadius, __exception_ptr));
        RETURN_IF_FAILED(registerFloatOutputPin("angle", &m_OutputAngle, __exception_ptr));
    } else if (eStage == StageNormal) {
        m_bDebugModeEnabled = GetPropertyBool(SC_PROP_DEBUG_MODE);
    } else if(eStage == StageGraphReady) {}


    RETURN_NOERROR;
}


// FELIX' MODIFICATIONS BELOW

tResult cRadiusToAngleConverter::PropertyChanged(const tChar* strName) {
    RETURN_IF_FAILED(cFilter::PropertyChanged(strName));
    //associate the properties to the member
    if (cString::IsEqual(strName, "Algorithm::WheelBase"))
        m_filterProperties.wheelbase = GetPropertyFloat("Algorithm::WheelBase");
    else if (cString::IsEqual(strName, "Algorithm::Tread"))
        m_filterProperties.tread = GetPropertyFloat("Algorithm::Tread");
    else if (cString::IsEqual(strName, SC_PROP_DEBUG_MODE))
        m_bDebugModeEnabled = GetPropertyBool(SC_PROP_DEBUG_MODE);

	RETURN_NOERROR;
}

tResult cRadiusToAngleConverter::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived && pMediaSample != NULL) {
        RETURN_IF_POINTER_NULL(pMediaSample);

        if (pSource == &m_InputRadius) {
            IfDebug("Received Radius | Reading...")

            tFloat32 radius = readFloatValue(pMediaSample);
            IfDebug(cString::Format("Read Radius: %lf | Converting...", radius))

            tFloat32 angle = convertRadiusToAngle(radius);
            IfDebug(cString::Format("Converted to Angle: %lf | Transmitting...", angle))

            return transmitFloatValue(angle, &m_OutputAngle);
        }
    }
    RETURN_NOERROR;
}


tFloat32 cRadiusToAngleConverter::convertRadiusToAngle(tFloat32 radius) {

    /* angle = inverse tangens (wheelbase / radius) */
    if (fabs(radius) <= 0.1f) {
        return 0;
    }

    return (tFloat32) atan(m_filterProperties.wheelbase / radius) * 180 / 3.14159265;
}
