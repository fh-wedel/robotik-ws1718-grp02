#ifndef  _ADTFHelper_H_
#define  _ADTFHelper_H_

#include "commonIncludes.h"

// For decoding input

    /*! media description for the input pin */
    cObjectPtr<IMediaTypeDescription> m_InputBoolValueDescription;
    cObjectPtr<IMediaTypeDescription> m_InputFloatValueDescription;

// For encoding output

    /*! media description for the output pin  */
    cObjectPtr<IMediaTypeDescription> m_OutputBoolValueDescription;
    cObjectPtr<IMediaTypeDescription> m_OutputFloatValueDescription;

#endif //  _ADTFHelper_H_
