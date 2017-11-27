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
#include "FloatValueGenerator.h"


#define SC_PROP_DEBUG_MODE "Debug Mode"

ADTF_FILTER_PLUGIN("FloatValueGenerator", OID_ADTF_STEERINGCONTROLLER, cFloatValueGenerator)

cFloatValueGenerator::cFloatValueGenerator(const tChar* __info) : cFilter(__info), m_bDebugModeEnabled(tFalse) {
    SetPropertyBool(SC_PROP_DEBUG_MODE, tFalse);
    SetPropertyStr(SC_PROP_DEBUG_MODE NSSUBPROP_DESCRIPTION, "If true debug infos are plotted to console");


    /*! the distance between axles (Radstand). */
    SetPropertyFloat("outputValue", 0.0);
    SetPropertyStr("outputValue" NSSUBPROP_DESCRIPTION, "Value to output");
    SetPropertyBool("outputValue" NSSUBPROP_ISCHANGEABLE, tTrue);
}

cFloatValueGenerator::~cFloatValueGenerator() {}

tResult cFloatValueGenerator::CreateInputPins(__exception)
{
	// create description manager
    cObjectPtr<IMediaDescriptionManager> pDescManager;
    RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER, IID_ADTF_MEDIA_DESCRIPTION_MANAGER, (tVoid**)&pDescManager, __exception_ptr));

	tChar const * strDescBoolSignalValue = pDescManager->GetMediaDescription("tBoolSignalValue");
	cObjectPtr<IMediaType> pTypeBoolSignalValue3 = new cMediaType(0, 0, 0, "tBoolSignalValue", strDescBoolSignalValue, IMediaDescription::MDF_DDL_DEFAULT_VERSION);

    RETURN_IF_FAILED(m_oInputWatchdog.Create("WatchdogAlive", pTypeBoolSignalValue3, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(&m_oInputWatchdog));

    RETURN_NOERROR;
}

tResult cFloatValueGenerator::CreateOutputPins(__exception) {
    // create description manager
    cObjectPtr<IMediaDescriptionManager> pDescManager;
    RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER,IID_ADTF_MEDIA_DESCRIPTION_MANAGER,(tVoid**)&pDescManager,__exception_ptr));

    // get media type
    tChar const * strDescSignalValue = pDescManager->GetMediaDescription("tSignalValue");
    RETURN_IF_POINTER_NULL(strDescSignalValue);
    cObjectPtr<IMediaType> pTypeSignalValue = new cMediaType(0, 0, 0, "tSignalValue", strDescSignalValue, IMediaDescription::MDF_DDL_DEFAULT_VERSION); //TODO: Soll angeblich ein "deprecated constructor" sein !!

    // set member media description
    RETURN_IF_FAILED(pTypeSignalValue->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_FloatDescription));

    // create pin
    RETURN_IF_FAILED(m_outputPin.Create("floatValue", pTypeSignalValue, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(&m_outputPin));

    RETURN_NOERROR; 
}

tResult cFloatValueGenerator::Init(tInitStage eStage, __exception) {
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr));

    if (eStage == StageFirst)
    {
        RETURN_IF_FAILED(CreateOutputPins(__exception_ptr));
        RETURN_IF_FAILED(CreateInputPins(__exception_ptr));
    }
    else if (eStage == StageNormal)
    {
        m_bDebugModeEnabled = GetPropertyBool(SC_PROP_DEBUG_MODE);
    }
    else if(eStage == StageGraphReady)
    {

    }


    RETURN_NOERROR;
}


// FELIX' MODIFICATIONS BELOW

tResult cFloatValueGenerator::PropertyChanged(const tChar* strName) {
    RETURN_IF_FAILED(cFilter::PropertyChanged(strName));
    //associate the properties to the member
    
    if (cString::IsEqual(strName, "outputValue")) {
        m_filterProperties.outputValue = GetPropertyFloat("outputValue");
	}
    
	RETURN_NOERROR;
}

tResult cFloatValueGenerator::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived && pMediaSample != NULL) {
        RETURN_IF_POINTER_NULL(pMediaSample);
		
		transmitValue(m_filterProperties.outputValue);
        
    }
    RETURN_NOERROR;
}


tResult cFloatValueGenerator::transmitValue(tFloat32 angle) {

    cObjectPtr<IMediaSample> pMediaSample = initMediaSample(m_FloatDescription);
    
    static bool hasID = false;
    /*! the id for the f32value of the media description for input pin for the set speed */
    static tBufferID m_FloatDescriptionID;
    /*! the id for the arduino time stamp of the media description for input pin for the set speed */
    static tBufferID m_FloatTimestampID;
    
    {
        // focus for sample write lock
        // read data from the media sample with the coder of the descriptor
        __adtf_sample_write_lock_mediadescription(m_FloatDescription, pMediaSample, pCoder);

        if(!hasID)
        {
            pCoder->GetID("f32Value", m_FloatDescriptionID);
            pCoder->GetID("ui32ArduinoTimestamp", m_FloatTimestampID);
            hasID = true;
        }

        //write values to media sample
        pCoder->Set(m_FloatDescriptionID, (tVoid*)&angle);
    }

    //transmit media sample over output pin
    RETURN_IF_FAILED(pMediaSample->SetTime(_clock->GetStreamTime()));
    RETURN_IF_FAILED(m_outputPin.Transmit(pMediaSample));

    RETURN_NOERROR;
}

cObjectPtr<IMediaSample> cFloatValueGenerator::initMediaSample(cObjectPtr<IMediaTypeDescription> typeDescription) {

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
