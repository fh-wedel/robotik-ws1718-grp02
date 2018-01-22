#include "StdFilter.h"


// Construction / Deconstruction

cStdFilter::cStdFilter(const tChar* __info) : cFilter(__info) {
    m_InputFloatValueDescriptionIsInitialized = false;
    m_InputBoolValueDescriptionIsInitialized = false;

    m_OutputFloatValueDescriptionIsInitialized = false;
    m_OutputBoolValueDescriptionIsInitialized = false;
}
cStdFilter::~cStdFilter() {}



// Pin Registration

tResult cStdFilter::registerFloatInputPin(cString name, cInputPin* inputPin, __exception) {
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

    RETURN_NOERROR;
}
tResult cStdFilter::registerBoolInputPin(cString name, cInputPin* inputPin, __exception) {

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

    RETURN_NOERROR;
}

tResult cStdFilter::registerFloatOutputPin(cString name, cOutputPin* outputPin, __exception) {
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

    RETURN_NOERROR;
}
tResult cStdFilter::registerBoolOutputPin(cString name, cOutputPin* outputPin, __exception) {

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

    RETURN_NOERROR;
}



// Input Value Processing

tFloat32 cStdFilter::readFloatValue(IMediaSample* pMediaSample) {
    tFloat32 value = 0;
    tUInt32 timestamp = 0;

    {
        // focus for sample read lock
        // read data from the media sample with the coder of the descriptor
        __adtf_sample_read_lock_mediadescription(m_InputFloatValueDescription, pMediaSample, pCoder);

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
tBool cStdFilter::readBoolValue(IMediaSample* pMediaSample) {
    tBool value = false;
    tUInt32 timestamp = 0;

    {
        // focus for sample read lock
        // read data from the media sample with the coder of the descriptor
        __adtf_sample_read_lock_mediadescription(m_InputBoolValueDescription, pMediaSample, pCoder);

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

cObjectPtr<IMediaSample> cStdFilter::initMediaSample(cObjectPtr<IMediaTypeDescription> typeDescription) {

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

tResult cStdFilter::transmitFloatValue(tFloat32 value, cOutputPin* outputPin) {

    if (!(outputPin->IsConnected())) {
        RETURN_NOERROR;
    }

    cObjectPtr<IMediaSample> pMediaSample = initMediaSample(m_OutputFloatValueDescription);
    {
        // focus for sample write lock
        // read data from the media sample with the coder of the descriptor
        __adtf_sample_write_lock_mediadescription(m_OutputFloatValueDescription, pMediaSample, pCoder);

        if(!m_OutputFloatValueDescriptionIsInitialized) {
            pCoder->GetID("f32Value", m_OutputFloatValueDescriptionID);
            pCoder->GetID("ui32ArduinoTimestamp", m_OutputFloatValueTimestampID);
            m_OutputFloatValueDescriptionIsInitialized = tTrue;
        }

        //write values to media sample
        pCoder->Set(m_OutputFloatValueDescriptionID, (tVoid*)&value);
    }

    //transmit media sample over output pin
    RETURN_IF_FAILED(pMediaSample->SetTime(_clock->GetStreamTime()));
    RETURN_IF_FAILED(outputPin->Transmit(pMediaSample));

    RETURN_NOERROR;
}
tResult cStdFilter::transmitBoolValue(tBool value, cOutputPin* outputPin) {

    if (!(outputPin->IsConnected())) {
        RETURN_NOERROR;
    }

    cObjectPtr<IMediaSample> pMediaSample = initMediaSample(m_OutputBoolValueDescription);
    {
        // focus for sample write lock
        // read data from the media sample with the coder of the descriptor
        __adtf_sample_write_lock_mediadescription(m_OutputBoolValueDescription, pMediaSample, pCoder);

        if(!m_OutputBoolValueDescriptionIsInitialized) {
            pCoder->GetID("bValue", m_OutputBoolValueDescriptionID);
            pCoder->GetID("ui32ArduinoTimestamp", m_OutputBoolValueTimestampID);
            m_OutputBoolValueDescriptionIsInitialized = tTrue;
        }

        //write values to media sample
        pCoder->Set(m_OutputBoolValueDescriptionID, (tVoid*)&value);
    }

    //transmit media sample over output pin
    RETURN_IF_FAILED(pMediaSample->SetTime(_clock->GetStreamTime()));
    RETURN_IF_FAILED(outputPin->Transmit(pMediaSample));

    RETURN_NOERROR;
}
