#include "ADTF_Helper.h"



// Pin Registration

static tResult registerFloatInputPin(cString name, cInputPin* inputPin, __exception) {
    // create description manager
    cObjectPtr<IMediaDescriptionManager> pDescManager;
    RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER,IID_ADTF_MEDIA_DESCRIPTION_MANAGER,(tVoid**)&pDescManager,__exception_ptr));

    // get media tayp
    tChar const * strDescSignalValue = pDescManager->GetMediaDescription("tSignalValue");
    RETURN_IF_POINTER_NULL(strDescSignalValue);
    cObjectPtr<IMediaType> pTypeSignalValue = new cMediaType(0, 0, 0, "tSignalValue", strDescSignalValue,IMediaDescription::MDF_DDL_DEFAULT_VERSION);


    // Input Offset
    // set member media description
    RETURN_IF_FAILED(pTypeSignalValue->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_InputFloatValueDescription));

    // create pin
    RETURN_IF_FAILED(inputPin->Create(name, pTypeSignalValue, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(inputPin));
}
static tResult registerBoolInputPin(cString name, cInputPin* inputPin, __exception) {

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
    RETURN_IF_FAILED(pTypeSignalValue->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_InputBoolValueDescription));

    // create pin
    RETURN_IF_FAILED(inputPin->Create(name, pTypeSignalValue, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(inputPin));
}

static tResult registerFloatOutputPin(cString name, cOutputPin* outputPin, __exception) {
    // create description manager
    cObjectPtr<IMediaDescriptionManager> pDescManager;
    RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER,IID_ADTF_MEDIA_DESCRIPTION_MANAGER,(tVoid**)&pDescManager,__exception_ptr));

    // get media tayp
    tChar const * strDescSignalValue = pDescManager->GetMediaDescription("tSignalValue");
    RETURN_IF_POINTER_NULL(strDescSignalValue);
    cObjectPtr<IMediaType> pTypeSignalValue = new cMediaType(0, 0, 0, "tSignalValue", strDescSignalValue,IMediaDescription::MDF_DDL_DEFAULT_VERSION);


    // Input Offset
    // set member media description
    RETURN_IF_FAILED(pTypeSignalValue->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_OutputFloatValueDescription));

    // create pin
    RETURN_IF_FAILED(outputPin->Create(name, pTypeSignalValue, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(outputPin));
}
static tResult registerBoolOutputPin(cString name, cOutputPin* outputPin, __exception) {

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
    RETURN_IF_FAILED(pTypeSignalValue->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_OutputBoolValueDescription));

    // create pin
    RETURN_IF_FAILED(outputPin->Create(name, pTypeSignalValue, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(outputPin));
}



// Input Value Processing

static tFloat32 readFloatValue(IMediaSample* pMediaSample) {
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
            pCoder->GetID("ui32ArduinoTimestamp", m_InputFloatValueTimestampID);
            m_InputFloatValueDescriptionIsInitialized = true;

        }

        // get values from media sample
        pCoder->Get(m_InputFloatValueDescriptionID, (tVoid*)&value);
        pCoder->Get(m_InputFloatValueTimestampID, (tVoid*)&timestamp);
    }

    return value;
}
static tBool readBoolValue(IMediaSample* pMediaSample) {
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
            pCoder->GetID("ui32ArduinoTimestamp", m_InputBoolValueTimestampID);
            m_InputBoolValueDescriptionIsInitialized = true;

        }

        // get values from media sample
        pCoder->Get(m_InputBoolValueDescriptionID, (tVoid*)&value);
        pCoder->Get(m_InputBoolValueTimestampID, (tVoid*)&timestamp);
    }

    return value;
}



// Output Value Transmission

static cObjectPtr<IMediaSample> initMediaSample(cObjectPtr<IMediaTypeDescription> typeDescription) {

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

static tResult transmitFloatValue(tFloat32 value, cOutputPin* outputPin) {

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
        static tBufferID m_OutputFloatValueTimestampID;

        if(!m_OutputFloatValueDescriptionIsInitialized) {
            pCoder->GetID("f32Value", m_OutputFloatValueDescriptionID);
            pCoder->GetID("ui32ArduinoTimestamp", m_OutputFloatValueTimestampID);
            m_OutputFloatValueDescriptionIsInitialized = tTrue;
        }

        //write values to media sample
        pCoder->Set(m_OutputFloatValueDescriptionID, (tVoid*)&value);
        // TODO: write timestamp
    }

    //transmit media sample over output pin
    RETURN_IF_FAILED(pMediaSample->SetTime(_clock->GetStreamTime()));
    RETURN_IF_FAILED(outputPin->Transmit(pMediaSample));

    RETURN_NOERROR;
}
static tResult transmitBoolValue(tBool value, cOutputPin* outputPin) {

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
        static tBufferID m_OutputBoolValueTimestampID;

        if(!m_OutputBoolValueDescriptionIsInitialized) {
            pCoder->GetID("f32Value", m_OutputBoolValueDescriptionID);
            pCoder->GetID("ui32ArduinoTimestamp", m_OutputBoolValueTimestampID);
            m_OutputBoolValueDescriptionIsInitialized = tTrue;
        }

        //write values to media sample
        pCoder->Set(m_OutputBoolValueDescriptionID, (tVoid*)&value);
        // TODO: write timestamp
    }

    //transmit media sample over output pin
    RETURN_IF_FAILED(pMediaSample->SetTime(_clock->GetStreamTime()));
    RETURN_IF_FAILED(outputPin->Transmit(pMediaSample));

    RETURN_NOERROR;
}
