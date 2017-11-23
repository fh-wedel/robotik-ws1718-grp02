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

ADTF_FILTER_PLUGIN("RadiusToAngle", OID_ADTF_STEERINGCONTROLLER, Controller)

Controller::Controller(const tChar* __info) : cFilter(__info), m_bDebugModeEnabled(tFalse) {
    SetPropertyBool(SC_PROP_DEBUG_MODE, tFalse);
    SetPropertyStr(SC_PROP_DEBUG_MODE NSSUBPROP_DESCRIPTION, "If true debug infos are plotted to console");
}

Controller::~Controller() {}


tResult Controller::CreateInputPins(__exception) {
    // create description manager
    cObjectPtr<IMediaDescriptionManager> pDescManager;
    RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER,IID_ADTF_MEDIA_DESCRIPTION_MANAGER,(tVoid**)&pDescManager,__exception_ptr));

    // get media tayp
    tChar const * strDescSignalValue = pDescManager->GetMediaDescription("tSignalValue");
    RETURN_IF_POINTER_NULL(strDescSignalValue);
    cObjectPtr<IMediaType> pTypeSignalValue = new cMediaType(0, 0, 0, "tSignalValue", strDescSignalValue,IMediaDescription::MDF_DDL_DEFAULT_VERSION);

    // set member media description
    RETURN_IF_FAILED(pTypeSignalValue->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_RadiusDescription));

    // create pins
    RETURN_IF_FAILED(m_InputRadius.Create("radius", pTypeSignalValue, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(&m_InputRadius));

    RETURN_NOERROR;
}

tResult Controller::CreateOutputPins(__exception) {
    // create description manager
    cObjectPtr<IMediaDescriptionManager> pDescManager;
    RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER,IID_ADTF_MEDIA_DESCRIPTION_MANAGER,(tVoid**)&pDescManager,__exception_ptr));

    // get media type
    tChar const * strDescSignalValue = pDescManager->GetMediaDescription("tSignalValue");
    RETURN_IF_POINTER_NULL(strDescSignalValue);
    cObjectPtr<IMediaType> pTypeSignalValue = new cMediaType(0, 0, 0, "tSignalValue", strDescSignalValue, IMediaDescription::MDF_DDL_DEFAULT_VERSION); //TODO: Soll angeblich ein "deprecated constructor" sein !!

    // set member media description
    RETURN_IF_FAILED(pTypeSignalValue->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_pDescriptionServoAngle));

    // create pin
    RETURN_IF_FAILED(m_OutputAngle.Create("angle", pTypeSignalValue, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(&m_OutputAngle));

    RETURN_NOERROR;
}

tResult Controller::Init(tInitStage eStage, __exception) {
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr))

    if (eStage == StageFirst)
    {
        RETURN_IF_FAILED(CreateInputPins(__exception_ptr));
        RETURN_IF_FAILED(CreateOutputPins(__exception_ptr));
    }
    else if (eStage == StageNormal)
    {
        m_bDebugModeEnabled = GetPropertyBool(SC_PROP_DEBUG_MODE);
    }
    else if(eStage == StageGraphReady)
    {
        // set the flags which indicate if the
        // media descriptions strings were set to NO
        m_AngleDescriptionIsInitialized = false;
        m_RadiusDescriptionIsInitialized = false;
    }


    RETURN_NOERROR;
}


// FELIX MODIFICATIONS BELOW

tResult Controller::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived && pMediaSample != NULL) {
        RETURN_IF_POINTER_NULL(pMediaSample);

        if (pSource == &m_InputRadius) {
            LOG_INFO("Received Radius | Reading...");

            tFloat32 radius = readRadius(pMediaSample);
            LOG_INFO(cString::Format("Read Radius: %lf | Converting...", radius));

            tFloat32 angle = convertToAngle(radius);
            LOG_INFO(cString::Format("Converted to Angle: %lf | Transmitting...", angle));

            return transmitAngle(angle);

        }
    }
    RETURN_NOERROR;
}


tFloat32 Controller::readRadius(IMediaSample* pMediaSample) {
    tFloat32 radius = 0;
    tUInt32 timestamp = 0;

    {
        // focus for sample read lock
        // read data from the media sample with the coder of the descriptor
        __adtf_sample_read_lock_mediadescription(m_RadiusDescription, pMediaSample, pCoder);

        if(!m_RadiusDescriptionIsInitialized) {

            pCoder->GetID("f32Value", m_RadiusDescriptionID);
            pCoder->GetID("ui32ArduinoTimestamp", m_RadiusTimestampID);
            m_RadiusDescriptionIsInitialized = true;

        }

        // get values from media sample
        pCoder->Get(m_RadiusDescriptionID, (tVoid*)&radius);
        pCoder->Get(m_RadiusTimestampID, (tVoid*)&timestamp);
    }

    return radius;
}

tFloat32 Controller::convertToAngle(tFloat32 radius) {

    return 0;
}

tResult Controller::transmitAngle(tFloat32 angle) {

    cObjectPtr<IMediaSample> pMediaSample = initMediaSample(m_AngleDescription);
    {
        // focus for sample write lock
        // read data from the media sample with the coder of the descriptor
        __adtf_sample_write_lock_mediadescription(m_AngleDescription, pMediaSample, pCoder);

        if(!m_AngleDescriptionIsInitialized)
        {
            //TODO: Am I allowed to rename the dictionary entry? (f32Value)
            pCoder->GetID("f32Value", m_AngleDescriptionID);
            pCoder->GetID("ui32ArduinoTimestamp", m_AngleTimestampID);
            m_AngleDescriptionIsInitialized = true;
        }

        //write values to media sample
        pCoder->Set(m_AngleDescriptionID, (tVoid*)&angle);
    }

    //transmit media sample over output pin
    RETURN_IF_FAILED(pMediaSample->SetTime(_clock->GetStreamTime()));
    RETURN_IF_FAILED(m_OutputAngle.Transmit(pMediaSample));

    RETURN_NOERROR;
}

cObjectPtr<IMediaSample> Controller::initMediaSample(cObjectPtr<IMediaTypeDescription> typeDescription) {

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
