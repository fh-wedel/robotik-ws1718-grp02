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
#include "MedianFilter.h"


#define SC_PROP_DEBUG_MODE "Debug Mode"

#define IfDebug(x) std::cout << x << endl;


ADTF_FILTER_PLUGIN(FILTER_NAME, UNIQUE_FILTER_ID, cMedianFilter)

cMedianFilter::cMedianFilter(const tChar* __info) : cFilter(__info), m_bDebugModeEnabled(tFalse) {
    SetPropertyBool(SC_PROP_DEBUG_MODE, tFalse);
    SetPropertyStr(SC_PROP_DEBUG_MODE NSSUBPROP_DESCRIPTION, "If true debug infos are plotted to console");

    SetPropertyInt("Filter::WindowSize", 40);
    SetPropertyStr("Filter::WindowSize" NSSUBPROP_DESCRIPTION, "Number of values to keep in storage.");
    SetPropertyBool("Filter::WindowSize" NSSUBPROP_ISCHANGEABLE, tTrue);
}

cMedianFilter::~cMedianFilter() {}


tResult cMedianFilter::CreateInputPins(__exception) {
    // create description manager
    cObjectPtr<IMediaDescriptionManager> pDescManager;
    RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER,IID_ADTF_MEDIA_DESCRIPTION_MANAGER,(tVoid**)&pDescManager,__exception_ptr));

    // get media tayp
    tChar const * strDescSignalValue = pDescManager->GetMediaDescription("tSignalValue");
    RETURN_IF_POINTER_NULL(strDescSignalValue);
    cObjectPtr<IMediaType> pTypeSignalValue = new cMediaType(0, 0, 0, "tSignalValue", strDescSignalValue,IMediaDescription::MDF_DDL_DEFAULT_VERSION);


    // Input Value
    // set member media description
    RETURN_IF_FAILED(pTypeSignalValue->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_InputValueDescription));

    // create pin
    RETURN_IF_FAILED(m_InputValue.Create("input", pTypeSignalValue, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(&m_InputValue));

    RETURN_NOERROR;
}

tResult cMedianFilter::CreateOutputPins(__exception) {
    // create description manager
    cObjectPtr<IMediaDescriptionManager> pDescManager;
    RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER,IID_ADTF_MEDIA_DESCRIPTION_MANAGER,(tVoid**)&pDescManager,__exception_ptr));

    // get media type
    tChar const * strDescSignalValue = pDescManager->GetMediaDescription("tSignalValue");
    RETURN_IF_POINTER_NULL(strDescSignalValue);
    cObjectPtr<IMediaType> pTypeSignalValue = new cMediaType(0, 0, 0, "tSignalValue", strDescSignalValue, IMediaDescription::MDF_DDL_DEFAULT_VERSION); //TODO: Soll angeblich ein "deprecated constructor" sein !!

    // set member media description
    RETURN_IF_FAILED(pTypeSignalValue->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_OutputValueDescription));

    // create pin
    RETURN_IF_FAILED(m_OutputValue.Create("output", pTypeSignalValue, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(&m_OutputValue));


    RETURN_NOERROR;
}

tResult cMedianFilter::Init(tInitStage eStage, __exception) {
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

tResult cMedianFilter::PropertyChanged(const tChar* strName) {
    RETURN_IF_FAILED(cFilter::PropertyChanged(strName));
    //associate the properties to the member
    if (cString::IsEqual(strName, "Filter::WindowSize")) {
        m_filterProperties.windowSize = GetPropertyFloat("Filter::WindowSize");
    }

	RETURN_NOERROR;
}

tResult cMedianFilter::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived && pMediaSample != NULL) {
        RETURN_IF_POINTER_NULL(pMediaSample);

        if (pSource == &m_InputValue) {
            tFloat32 newValue = readInputValue(pMediaSample);
            return OnValueChanged(newValue);
        }
    }

    RETURN_NOERROR;
}

tFloat32 cMedianFilter::calculateMedian(std::vector<tFloat32> values) {

    // sort values ascending
    std:sort(values.begin(), values.end());

    // 0 / 2 = 0    -> 0
    // 1 / 2 = 0.5  -> 0
    // 2 / 2 = 1    -> 1
    // 3 / 2 = 1.5  -> 1
    // 4 / 2 = 2    -> 2
    // 5 / 2 = 2.5  -> 2
    int medianIndex = values.size() / 2;

    return values[medianIndex];
}

tResult cMedianFilter::OnValueChanged(tFloat32 newValue) {

    // erase oldest values if window size has been reached
    int amountOfOverflownElements = m_filterProperties.windowSize - m_storedValues.size()
    if (amountOfOverflownElements > 0) {
        m_storedValues.erase(
            m_storedValues.begin(),
            m_storedValues.begin() + amountOfOverflownElements
        );
    }

    // append new value
    m_storedValues.push_back(newValue);

    // apply median filter and transmit
    tFloat32 median = calculateMedian(m_storedValues);
    transmitValue(median);

    RETURN_NOERROR;
}

// Input Value PROCESSING

tFloat32 cMedianFilter::readInputValue(IMediaSample* pMediaSample) {
    tFloat32 value = 0;
    tUInt32 timestamp = 0;

    {
        // focus for sample read lock
        // read data from the media sample with the coder of the descriptor
        __adtf_sample_read_lock_mediadescription(m_InputValueDescription, pMediaSample, pCoder);

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

tResult cMedianFilter::transmitValue(tFloat32 value) {

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
            pCoder->GetID("f32Value", m_OutputValueDescriptionID);
            pCoder->GetID("ui32ArduinoTimestamp", m_OutputValueTimestampID);
            m_OutputValueDescriptionIsInitialized = tTrue;
        }

        //write values to media sample
        pCoder->Set(m_OutputValueDescriptionID, (tVoid*)&value);
    }

    //transmit media sample over output pin
    RETURN_IF_FAILED(pMediaSample->SetTime(_clock->GetStreamTime()));
    RETURN_IF_FAILED(m_OutputValue.Transmit(pMediaSample));

    RETURN_NOERROR;
}

cObjectPtr<IMediaSample> cMedianFilter::initMediaSample(cObjectPtr<IMediaTypeDescription> typeDescription) {

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
