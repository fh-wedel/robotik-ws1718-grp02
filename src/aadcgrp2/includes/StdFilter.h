#ifndef  _StdFilter_H_
#define  _StdFilter_H_

#include "commonIncludes.h"

//! A convenience implementation of cFilter.
/*!
 *
 *
 *
 *
 */
class cStdFilter : public adtf::cFilter {

//######################################//

public:

    /*! constructor for template class
    *    \param __info   [in] This is the name of the filter instance.
    */
    cStdFilter(const tChar* __info);

    /*! Destructor. */
    ~cStdFilter();

//######################################//

// NOTE: CONVENINIENCE HELPER Functions
protected:
// For registering pins

    tResult registerFloatInputPin(cString name, cInputPin* inputPin, __exception);
    tResult registerBoolInputPin(cString name, cInputPin* inputPin, __exception);
    tResult registerFloatOutputPin(cString name, cOutputPin* outputPin, __exception);
    tResult registerBoolOutputPin(cString name, cOutputPin* outputPin, __exception);


// For decoding input

    /*! media description for a float input pin */
    cObjectPtr<IMediaTypeDescription> m_InputFloatValueDescription;
    /*! indicates of bufferIDs were set */
    tBool m_InputFloatValueDescriptionIsInitialized;
    /*! the id for the float value of the media description */
    tBufferID m_InputFloatValueDescriptionID;
    /*! the id for the arduino time stamp of the media description */
    tBufferID m_InputFloatValueTimestampID;


    /*! media description for a boolean input pin */
    cObjectPtr<IMediaTypeDescription> m_InputBoolValueDescription;
    /*! indicates of bufferIDs were set */
    tBool m_InputBoolValueDescriptionIsInitialized;
    /*! the id for the f32value of the media description for input pin for the radius input */
    tBufferID m_InputBoolValueDescriptionID;
    /*! the id for the arduino time stamp of the media description for input pin for the timestamp */
    tBufferID m_InputBoolValueTimestampID;


    tFloat32 readFloatValue(IMediaSample* pMediaSample);
    tBool readBoolValue(IMediaSample* pMediaSample);

// For encoding output

    /*! media description for the float output pin  */
    cObjectPtr<IMediaTypeDescription> m_OutputFloatValueDescription;
    /*! indicates of bufferIDs were set */
    tBool m_OutputFloatValueDescriptionIsInitialized;
    /*! the id for the f32value of the media description for input pin for the set speed */
    tBufferID m_OutputFloatValueDescriptionID;
    /*! the id for the arduino time stamp of the media description for input pin for the set speed */
    tBufferID m_OutputFloatValueTimestampID;

    /*! media description for the bool output pin  */
    cObjectPtr<IMediaTypeDescription> m_OutputBoolValueDescription;
    /*! indicates of bufferIDs were set */
    tBool m_OutputBoolValueDescriptionIsInitialized;
    /*! the id for the boolean value of the media description */
    tBufferID m_OutputBoolValueDescriptionID;
    /*! the id for the arduino time stamp of the media description */
    tBufferID m_OutputBoolValueTimestampID;


    tResult transmitFloatValue(tFloat32 value, cOutputPin* outputPin);
    tResult transmitBoolValue(tBool value, cOutputPin* outputPin);

    cObjectPtr<IMediaSample> initMediaSample(cObjectPtr<IMediaTypeDescription> typeDescription);


};
#endif //  _StdFilter_H_
