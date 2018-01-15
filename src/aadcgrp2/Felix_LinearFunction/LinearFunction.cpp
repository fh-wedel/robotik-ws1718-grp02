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
#include "LinearFunction.h"


#define SC_PROP_DEBUG_MODE "Debug Mode"

#define IfDebug(x) std::cout << x << endl;


ADTF_FILTER_PLUGIN(FILTER_NAME, UNIQUE_FILTER_ID, cLinearFunction)

cLinearFunction::cLinearFunction(const tChar* __info) : cStdFilter(__info), m_bDebugModeEnabled(tFalse) {
    SetPropertyBool(SC_PROP_DEBUG_MODE, tFalse);
    SetPropertyStr(SC_PROP_DEBUG_MODE NSSUBPROP_DESCRIPTION, "If true debug infos are plotted to console");

    SetPropertyFloat("Gain::Gain", 1);
    SetPropertyStr("Gain::Gain" NSSUBPROP_DESCRIPTION, "gain");
    SetPropertyBool("Gain::Gain" NSSUBPROP_ISCHANGEABLE, tTrue);

    SetPropertyBool("Gain::OverwritePin", false);
    SetPropertyStr("Gain::OverwritePin" NSSUBPROP_DESCRIPTION, "Ignore gain input pin.");
    SetPropertyBool("Gain::OverwritePin" NSSUBPROP_ISCHANGEABLE, tTrue);

    SetPropertyFloat("Offset::Offset", 0);
    SetPropertyStr("Offset::Offset" NSSUBPROP_DESCRIPTION, "offset");
    SetPropertyBool("Offset::Offset" NSSUBPROP_ISCHANGEABLE, tTrue);

    SetPropertyBool("Offset::OverwritePin", false);
    SetPropertyStr("Offset::OverwritePin" NSSUBPROP_DESCRIPTION, "Ignore offset input pin.");
    SetPropertyBool("Offset::OverwritePin" NSSUBPROP_ISCHANGEABLE, tTrue);

    m_mostRecentValue = 0;
}

cLinearFunction::~cLinearFunction() {}


tResult cLinearFunction::CreateInputPins(__exception) {

    RETURN_IF_FAILED(registerFloatInputPin("input",     &m_InputValue,  __exception_ptr));
    RETURN_IF_FAILED(registerFloatInputPin("gain",      &m_InputGain,   __exception_ptr));
    RETURN_IF_FAILED(registerFloatInputPin("offset",    &m_InputOffset, __exception_ptr));

    RETURN_NOERROR;
}

tResult cLinearFunction::CreateOutputPins(__exception) {

    RETURN_IF_FAILED(registerFloatOutputPin("output", &m_OutputValue, __exception_ptr));

    RETURN_NOERROR;
}

tResult cLinearFunction::Init(tInitStage eStage, __exception) {
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr))

    if (eStage == StageFirst) {
        RETURN_IF_FAILED(CreateInputPins(__exception_ptr));
        RETURN_IF_FAILED(CreateOutputPins(__exception_ptr));
    } else if (eStage == StageNormal) {
        m_bDebugModeEnabled = GetPropertyBool(SC_PROP_DEBUG_MODE);
    } else if(eStage == StageGraphReady) {}


    RETURN_NOERROR;
}


// FELIX' MODIFICATIONS BELOW

tResult cLinearFunction::PropertyChanged(const tChar* strName) {
    RETURN_IF_FAILED(cFilter::PropertyChanged(strName));
    //associate the properties to the member
    if (cString::IsEqual(strName, "Gain::Gain")) {
        m_filterProperties.gain = GetPropertyFloat("Gain::Gain");

    } else if (cString::IsEqual(strName, "Gain::OverwritePin")) {
        m_filterProperties.overwriteGain = GetPropertyBool("Gain::OverwritePin");

    } else if (cString::IsEqual(strName, "Offset::Offset")) {
        m_filterProperties.offset = GetPropertyFloat("Offset::Offset");

    } else if (cString::IsEqual(strName, "Offset::OverwritePin")) {
        m_filterProperties.overwriteOffset = GetPropertyBool("Offset::OverwritePin");
    }



	RETURN_NOERROR;
}

tResult cLinearFunction::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived && pMediaSample != NULL) {
        RETURN_IF_POINTER_NULL(pMediaSample);

        if (pSource == &m_InputValue) {
            m_mostRecentValue = readFloatValue(pMediaSample);
        } else if (pSource == &m_InputGain) {
            m_filterProperties.gain = readFloatValue(pMediaSample);
        } else if (pSource == &m_InputOffset) {
            m_filterProperties.offset = readFloatValue(pMediaSample);
        }

        return OnValueChanged();

    }
    RETURN_NOERROR;
}

tResult cLinearFunction::OnValueChanged() {
    // Calculate output like:
    // y = gain * x + offset
    tFloat32 output = m_filterProperties.gain * m_mostRecentValue + m_filterProperties.offset;

    RETURN_IF_FAILED(transmitFloatValue(output, &m_OutputValue));

    RETURN_NOERROR;
}
