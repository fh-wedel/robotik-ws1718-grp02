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

#define IfDebug(x) std::cout << x << endl;


ADTF_FILTER_PLUGIN(FILTER_NAME, UNIQUE_FILTER_ID, cAngleToServoConverter)

cAngleToServoConverter::cAngleToServoConverter(const tChar* __info) : cFilter(__info), m_bDebugModeEnabled(tFalse) {
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


tResult cAngleToServoConverter::CreateInputPins(__exception) {
    // create description manager
    cObjectPtr<IMediaDescriptionManager> pDescManager;
    RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER,IID_ADTF_MEDIA_DESCRIPTION_MANAGER,(tVoid**)&pDescManager,__exception_ptr));

    // get media tayp
    tChar const * strDescSignalValue = pDescManager->GetMediaDescription("tSignalValue");
    RETURN_IF_POINTER_NULL(strDescSignalValue);
    cObjectPtr<IMediaType> pTypeSignalValue = new cMediaType(0, 0, 0, "tSignalValue", strDescSignalValue,IMediaDescription::MDF_DDL_DEFAULT_VERSION);

    // set member media description
    RETURN_IF_FAILED(pTypeSignalValue->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_AngleDescription));

    // create pins
    RETURN_IF_FAILED(m_InputAngle.Create("angle", pTypeSignalValue, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(&m_InputAngle));

    RETURN_NOERROR;
}

tResult cAngleToServoConverter::CreateOutputPins(__exception) {
    // create description manager
    cObjectPtr<IMediaDescriptionManager> pDescManager;
    RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER,IID_ADTF_MEDIA_DESCRIPTION_MANAGER,(tVoid**)&pDescManager,__exception_ptr));

    // get media type
    tChar const * strDescSignalValue = pDescManager->GetMediaDescription("tSignalValue");
    RETURN_IF_POINTER_NULL(strDescSignalValue);
    cObjectPtr<IMediaType> pTypeSignalValue = new cMediaType(0, 0, 0, "tSignalValue", strDescSignalValue, IMediaDescription::MDF_DDL_DEFAULT_VERSION); //TODO: Soll angeblich ein "deprecated constructor" sein !!

    // set member media description
    RETURN_IF_FAILED(pTypeSignalValue->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_ServoDescription));

    // create pin
    RETURN_IF_FAILED(m_OutputServoValue.Create("servoValue", pTypeSignalValue, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(&m_OutputServoValue));

    RETURN_NOERROR;
}

tResult cAngleToServoConverter::Init(tInitStage eStage, __exception) {
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

            tFloat32 angle = readAngle(pMediaSample);
            IfDebug(cString::Format("Read Angle: %lf | Converting...", angle))

            tFloat32 servoValue = convertAngleToServoValue(angle);
            IfDebug(cString::Format("Converted to servoValue: %lf | Transmitting...", servoValue))

            return transmitValue(servoValue);

        }
    }
    RETURN_NOERROR;
}


// RADIUS PROCESSING

tFloat32 cAngleToServoConverter::readAngle(IMediaSample* pMediaSample) {
    tFloat32 angle = 0;
    tUInt32 timestamp = 0;

    {
        // focus for sample read lock
        // read data from the media sample with the coder of the descriptor
        __adtf_sample_read_lock_mediadescription(m_AngleDescription, pMediaSample, pCoder);

        /*! indicates of bufferIDs were set */
        static tBool m_AngleDescriptionIsInitialized = false;
        /*! the id for the f32value of the media description for input pin for the radius input */
        static tBufferID m_AngleDescriptionID;
        /*! the id for the arduino time stamp of the media description for input pin for the timestamp */
        static tBufferID m_AngleTimestampID;

        if(!m_AngleDescriptionIsInitialized) {

            pCoder->GetID("f32Value", m_AngleDescriptionID);
            pCoder->GetID("ui32ArduinoTimestamp", m_AngleTimestampID);
            m_AngleDescriptionIsInitialized = true;

        }

        // get values from media sample
        pCoder->Get(m_AngleDescriptionID, (tVoid*)&angle);
        pCoder->Get(m_AngleTimestampID, (tVoid*)&timestamp);
    }

    return angle;
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

tResult cAngleToServoConverter::transmitValue(tFloat32 servoValue) {

    cObjectPtr<IMediaSample> pMediaSample = initMediaSample(m_ServoDescription);
    {
        // focus for sample write lock
        // read data from the media sample with the coder of the descriptor
        __adtf_sample_write_lock_mediadescription(m_ServoDescription, pMediaSample, pCoder);

        /*! indicates of bufferIDs were set */
        static tBool m_ServoDescriptionIsInitialized = false;
        /*! the id for the f32value of the media description for input pin for the set speed */
        static tBufferID m_ServoDescriptionID;
        /*! the id for the arduino time stamp of the media description for input pin for the set speed */
        static tBufferID m_ServoTimestampID;


        if(!m_ServoDescriptionIsInitialized) {
            pCoder->GetID("f32Value", m_ServoDescriptionID);
            pCoder->GetID("ui32ArduinoTimestamp", m_ServoTimestampID);
            m_ServoDescriptionIsInitialized = tTrue;
        }

        //write values to media sample
        pCoder->Set(m_ServoDescriptionID, (tVoid*)&servoValue);
    }

    //transmit media sample over output pin
    RETURN_IF_FAILED(pMediaSample->SetTime(_clock->GetStreamTime()));
    RETURN_IF_FAILED(m_OutputServoValue.Transmit(pMediaSample));

    RETURN_NOERROR;
}

cObjectPtr<IMediaSample> cAngleToServoConverter::initMediaSample(cObjectPtr<IMediaTypeDescription> typeDescription) {

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
