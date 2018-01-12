#ifndef  _ADTFHelper_H_
#define  _ADTFHelper_H_

#include "commonIncludes.h"


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

    /*! media description for the input pin */
    cObjectPtr<IMediaTypeDescription> m_InputBoolValueDescription;
    cObjectPtr<IMediaTypeDescription> m_InputFloatValueDescription;

    tFloat32 readFloatValue(IMediaSample* pMediaSample);
    tBool readBoolValue(IMediaSample* pMediaSample);

// For encoding output

    /*! media description for the output pin  */
    cObjectPtr<IMediaTypeDescription> m_OutputBoolValueDescription;
    cObjectPtr<IMediaTypeDescription> m_OutputFloatValueDescription;

    tResult transmitFloatValue(tFloat32 value, cOutputPin* outputPin);
    tResult transmitBoolValue(tBool value, cOutputPin* outputPin);

    cObjectPtr<IMediaSample> initMediaSample(cObjectPtr<IMediaTypeDescription> typeDescription);


};
#endif //  _ADTFHelper_H_
