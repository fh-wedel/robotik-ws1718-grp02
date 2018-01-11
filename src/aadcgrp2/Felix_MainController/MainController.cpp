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
#include "MainController.h"


#define SC_PROP_DEBUG_MODE "Debug Mode"

#define IfDebug(x) std::cout << x << endl;


ADTF_FILTER_PLUGIN(FILTER_NAME, UNIQUE_FILTER_ID, cMainController)

cMainController::cMainController(const tChar* __info) : cFilter(__info), m_bDebugModeEnabled(tFalse) {
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

cMainController::~cMainController() {}


// PIN REGISTRATION HELPER

tResult cMainController::registerFloatInputPin(cString name, cInputPin* inputPin, __exception) {
    // create description manager
    cObjectPtr<IMediaDescriptionManager> pDescManager;
    RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER,IID_ADTF_MEDIA_DESCRIPTION_MANAGER,(tVoid**)&pDescManager,__exception_ptr));

    // get media tayp
    tChar const * strDescSignalValue = pDescManager->GetMediaDescription("tSignalValue");
    RETURN_IF_POINTER_NULL(strDescSignalValue);
    cObjectPtr<IMediaType> pTypeSignalValue = new cMediaType(0, 0, 0, "tSignalValue", strDescSignalValue,IMediaDescription::MDF_DDL_DEFAULT_VERSION);


    // Input Offset
    // set member media description
    RETURN_IF_FAILED(pTypeSignalValue->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_InputFloatDescription));

    // create pin
    RETURN_IF_FAILED(inputPin->Create(name, pTypeSignalValue, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(inputPin));
}
tResult cMainController::registerBooleanInputPin(cString name, cInputPin* inputPin, __exception) {

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
    RETURN_IF_FAILED(pTypeSignalValue->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_InputBooleanValueDescription));

    // create pin
    RETURN_IF_FAILED(inputPin->Create(name, pTypeSignalValue, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(inputPin));
}

tResult cMainController::registerFloatOutputPin(cString name, cOutputPin* outputPin, __exception) {
    // create description manager
    cObjectPtr<IMediaDescriptionManager> pDescManager;
    RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER,IID_ADTF_MEDIA_DESCRIPTION_MANAGER,(tVoid**)&pDescManager,__exception_ptr));

    // get media tayp
    tChar const * strDescSignalValue = pDescManager->GetMediaDescription("tSignalValue");
    RETURN_IF_POINTER_NULL(strDescSignalValue);
    cObjectPtr<IMediaType> pTypeSignalValue = new cMediaType(0, 0, 0, "tSignalValue", strDescSignalValue,IMediaDescription::MDF_DDL_DEFAULT_VERSION);


    // Input Offset
    // set member media description
    RETURN_IF_FAILED(pTypeSignalValue->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_InputFloatDescription));

    // create pin
    RETURN_IF_FAILED(outputPin->Create(name, pTypeSignalValue, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(outputPin));
}
tResult cMainController::registerBooleanOutputPin(cString name, cOutputPin* outputPin, __exception) {

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
    RETURN_IF_FAILED(pTypeSignalValue->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_InputBooleanValueDescription));

    // create pin
    RETURN_IF_FAILED(outputPin->Create(name, pTypeSignalValue, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(outputPin));
}


// PIN CREATION

tResult cMainController::CreateFloatInputPins(__exception) {
    /* inputs for movement */
    RETURN_IF_FAILED(registerFloatInputPin("steeringAngle", &m_InputSteeringAngle, __exception_ptr));
    RETURN_IF_FAILED(registerFloatInputPin("speed",         &m_InputSpeed, __exception_ptr));

    RETURN_NOERROR;
}
tResult cMainController::CreateBooleanInputPins(__exception) {

    /* inputs for collision detection */
    RETURN_IF_FAILED(registerBooleanInputPin("obstacleDetected",    &m_InputObstacleDetected, __exception_ptr));
    RETURN_IF_FAILED(registerBooleanInputPin("collisionDetected",   &m_InputCollisionDetected, __exception_ptr));

    /* inputs for having a sense of surroundings */
    RETURN_IF_FAILED(registerBooleanInputPin("CrossingHasLeft",     &m_InputCrossingHasLeft, __exception_ptr));
    RETURN_IF_FAILED(registerBooleanInputPin("CrossingHasRight",    &m_InputCrossingHasRight, __exception_ptr));
    RETURN_IF_FAILED(registerBooleanInputPin("CrossingHasStraight", &m_InputCrossingHasStraight, __exception_ptr));

    RETURN_NOERROR;
}

tResult cMainController::CreateFloatOutputPins(__exception) {

    /* outputs for movement*/
    RETURN_IF_FAILED(registerFloatOutputPin("speed",            &m_OutputSpeed, __exception_ptr));
    RETURN_IF_FAILED(registerFloatOutputPin("steeringAngle",    &m_OutputSteeringAngle, __exception_ptr));

    RETURN_NOERROR;
}
tResult cMainController::CreateBooleanOutputPins(__exception) {

    /* outputs for behaviour */
    RETURN_IF_FAILED(registerBooleanOutputPin("CrossingTurnLeft",   &m_OutputCrossingTurnLeft, __exception_ptr));
    RETURN_IF_FAILED(registerBooleanOutputPin("CrossingTurnRight",  &m_OutputCrossingTurnRight, __exception_ptr));
    RETURN_IF_FAILED(registerBooleanOutputPin("CrossingGoStraight", &m_OutputCrossingGoStraight, __exception_ptr));

    /* auxiliary outputs */
    RETURN_IF_FAILED(registerBooleanOutputPin("HeadLights",     &m_OutputHeadLights, __exception_ptr));
    RETURN_IF_FAILED(registerBooleanOutputPin("BrakeLights",    &m_OutputBrakeLights, __exception_ptr));
    RETURN_IF_FAILED(registerBooleanOutputPin("BlinkerLeft",    &m_OutputBlinkerLeft, __exception_ptr));
    RETURN_IF_FAILED(registerBooleanOutputPin("BlinkerRight",   &m_OutputBlinkerRight, __exception_ptr));
    RETURN_IF_FAILED(registerBooleanOutputPin("HazardLights",   &m_OutputHazardLights, __exception_ptr));

    RETURN_NOERROR;

}



tResult cMainController::Init(tInitStage eStage, __exception) {
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr))

    if (eStage == StageFirst) {
        RETURN_IF_FAILED(CreateFloatInputPins(__exception_ptr));
        RETURN_IF_FAILED(CreateBooleanInputPins(__exception_ptr));
        RETURN_IF_FAILED(CreateFloatOutputPins(__exception_ptr));
        RETURN_IF_FAILED(CreateBooleanOutputPins(__exception_ptr));
    } else if (eStage == StageNormal) {
        m_bDebugModeEnabled = GetPropertyBool(SC_PROP_DEBUG_MODE);
    } else if(eStage == StageGraphReady) {}


    RETURN_NOERROR;
}


// FELIX' MODIFICATIONS BELOW

tResult cMainController::PropertyChanged(const tChar* strName) {
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

tResult cMainController::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived && pMediaSample != NULL) {
        RETURN_IF_POINTER_NULL(pMediaSample);

        // TODO: Store read values.

        if (pSource == &m_InputSteeringAngle) {
            readInputFloatValue(pMediaSample);
        } else if (pSource == &m_InputSpeed) {
            readInputFloatValue(pMediaSample);
        } else if (pSource == &m_InputObstacleDetected) {
            readInputBoolValue(pMediaSample);
        } else if (pSource == &m_InputCollisionDetected) {
            readInputBoolValue(pMediaSample);
        } else if (pSource == &m_InputCrossingHasLeft) {
            readInputBoolValue(pMediaSample);
        } else if (pSource == &m_InputCrossingHasRight) {
            readInputBoolValue(pMediaSample);
        } else if (pSource == &m_InputCrossingHasStraight) {
            readInputBoolValue(pMediaSample);
        }

        return OnValueChanged();

    }
    RETURN_NOERROR;
}

tResult cMainController::OnValueChanged() {

    /* outputs for movement*/
    transmitFloatValue(0.0f, m_OutputSpeed);
    transmitFloatValue(0.0f, m_OutputSteeringAngle);

    /* outputs for behaviour */
    transmitBoolValue(true, &m_OutputCrossingTurnLeft);
    transmitBoolValue(true, &m_OutputCrossingTurnRight);
    transmitBoolValue(true, &m_OutputCrossingGoStraight);

    /* auxiliary outputs */
    transmitBoolValue(true, &m_OutputHeadLights);
    transmitBoolValue(false, &m_OutputHeadLights);
    transmitBoolValue(false, &m_OutputBlinkerLeft);
    transmitBoolValue(false, &m_OutputBlinkerRight);
    transmitBoolValue(false, &m_OutputHazardLights);

    RETURN_NOERROR;
}

// Input Value processing

tFloat32 cMainController::readInputFloatValue(IMediaSample* pMediaSample) {
    tFloat32 value = 0;
    tUInt32 timestamp = 0;

    {
        // focus for sample read lock
        // read data from the media sample with the coder of the descriptor
        __adtf_sample_read_lock_mediadescription(m_InputFloatValueDescription, pMediaSample, pCoder);

        /*! indicates of bufferIDs were set */
        static tBool m_InputFloatValueDescriptionIsInitialized = false;
        /*! the id for the f32value of the media description for input pin for the radius input */
        static tBufferID m_InputFloatValueDescriptionID;
        /*! the id for the arduino time stamp of the media description for input pin for the timestamp */
        static tBufferID m_InputFloatValueTimestampID;

        if(!m_InputFloatValueDescriptionIsInitialized) {

            pCoder->GetID("f32Value", m_InputFloatValueDescriptionID);
            pCoder->GetID("ui32ArduinoTimestamp", m_InputValueTimestampID);
            m_InputFloatValueDescriptionIsInitialized = true;

        }

        // get values from media sample
        pCoder->Get(m_InputFloatValueDescriptionID, (tVoid*)&value);
        pCoder->Get(m_InputFloatValueTimestampID, (tVoid*)&timestamp);
    }

    return value;
}

tBool cMainController::readInputBoolValue(IMediaSample* pMediaSample) {
    tBool value = false;
    tUInt32 timestamp = 0;

    {
        // focus for sample read lock
        // read data from the media sample with the coder of the descriptor
        __adtf_sample_read_lock_mediadescription(m_InputBoolValueDescription, pMediaSample, pCoder);

        /*! indicates of bufferIDs were set */
        static tBool m_InputBoolValueDescriptionIsInitialized = false;
        /*! the id for the f32value of the media description for input pin for the radius input */
        static tBufferID m_InputBoolValueDescriptionID;
        /*! the id for the arduino time stamp of the media description for input pin for the timestamp */
        static tBufferID m_InputBoolValueTimestampID;

        if(!m_InputBoolValueDescriptionIsInitialized) {

            pCoder->GetID("bValue", m_InputBoolValueDescriptionID);
            pCoder->GetID("ui32ArduinoTimestamp", m_InputValueTimestampID);
            m_InputBoolValueDescriptionIsInitialized = true;

        }

        // get values from media sample
        pCoder->Get(m_InputBoolValueDescriptionID, (tVoid*)&value);
        pCoder->Get(m_InputBoolValueTimestampID, (tVoid*)&timestamp);
    }

    return value;
}

// Output Value Transmission

tResult cMainController::transmitFloatValue(tFloat32 value, cOutputPin* outputPin) {

    cObjectPtr<IMediaSample> pMediaSample = initMediaSample(m_OutputFloatValueDescription);
    {
        // focus for sample write lock
        // read data from the media sample with the coder of the descriptor
        __adtf_sample_write_lock_mediadescription(m_OutputFloatValueDescription, pMediaSample, pCoder);

        /*! indicates of bufferIDs were set */
        static tBool m_OutputFloatValueDescriptionIsInitialized = false;
        /*! the id for the f32value of the media description for input pin for the set speed */
        static tBufferID m_OutputFloatValueDescriptionID;
        /*! the id for the arduino time stamp of the media description for input pin for the set speed */
        static tBufferID m_OutputValueTimestampID;

        if(!m_OutputFloatValueDescriptionIsInitialized) {
            pCoder->GetID("f32Value", m_OutputFloatValueDescriptionID);
            pCoder->GetID("ui32ArduinoTimestamp", m_OutputValueTimestampID);
            m_OutputFloatValueDescriptionIsInitialized = tTrue;
        }

        //write values to media sample
        pCoder->Set(m_OutputFloatValueDescriptionID, (tVoid*)&value);
    }

    //transmit media sample over output pin
    RETURN_IF_FAILED(pMediaSample->SetTime(_clock->GetStreamTime()));
    RETURN_IF_FAILED(m_OutputValue.Transmit(pMediaSample));

    RETURN_NOERROR;
}

tResult cMainController::transmitBoolValue(tBool value, cOutputPin* outputPin) {

    cObjectPtr<IMediaSample> pMediaSample = initMediaSample(m_OutputBoolValueDescription);
    {
        // focus for sample write lock
        // read data from the media sample with the coder of the descriptor
        __adtf_sample_write_lock_mediadescription(m_OutputBoolValueDescription, pMediaSample, pCoder);

        /*! indicates of bufferIDs were set */
        static tBool m_OutputBoolValueDescriptionIsInitialized = false;
        /*! the id for the f32value of the media description for input pin for the set speed */
        static tBufferID m_OutputBoolValueDescriptionID;
        /*! the id for the arduino time stamp of the media description for input pin for the set speed */
        static tBufferID m_OutputValueTimestampID;

        if(!m_OutputBoolValueDescriptionIsInitialized) {
            pCoder->GetID("f32Value", m_OutputBoolValueDescriptionID);
            pCoder->GetID("ui32ArduinoTimestamp", m_OutputValueTimestampID);
            m_OutputBoolValueDescriptionIsInitialized = tTrue;
        }

        //write values to media sample
        pCoder->Set(m_OutputBoolValueDescriptionID, (tVoid*)&value);
    }

    //transmit media sample over output pin
    RETURN_IF_FAILED(pMediaSample->SetTime(_clock->GetStreamTime()));
    RETURN_IF_FAILED(m_OutputValue.Transmit(pMediaSample));

    RETURN_NOERROR;
}

cObjectPtr<IMediaSample> cMainController::initMediaSample(cObjectPtr<IMediaTypeDescription> typeDescription) {

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
