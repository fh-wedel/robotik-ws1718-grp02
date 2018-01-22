#ifndef  _StdFilter_H_
#define  _StdFilter_H_

#include "commonIncludes.h"

/**
 * A convenience implementation of cFilter.
 * It supplies the user with easier registration of pins as well
 * as value readings and transmissions of float and boolean type pins.
 *
 * @param __info the name of the filter instance
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
protected:
// For registering pins

    /**
     * Registers a float input pin using a given name.
     * @param  name        the name to display in the configuration editor.
     * @param  inputPin    the object to store a reference in.
     * @param  __exception exception
     * @return             standard atdf error code
     */
    tResult registerFloatInputPin(cString name, cInputPin* inputPin, __exception);
    /**
     * Registers a boolean input pin using a given name.
     * @param  name        the name to display in the configuration editor.
     * @param  inputPin    the object to store a reference in.
     * @param  __exception exception
     * @return             standard atdf error code
     */
    tResult registerBoolInputPin(cString name, cInputPin* inputPin, __exception);

    /**
     * Registers a float output pin using a given name.
     * @param  name        the name to display in the configuration editor.
     * @param  inputPin    the object to store a reference in.
     * @param  __exception exception
     * @return             standard atdf error code
     */
    tResult registerFloatOutputPin(cString name, cOutputPin* outputPin, __exception);

    /**
     * Registers a boolean output pin using a given name.
     * @param  name        the name to display in the configuration editor.
     * @param  inputPin    the object to store a reference in.
     * @param  __exception exception
     * @return             standard atdf error code
     */
    tResult registerBoolOutputPin(cString name, cOutputPin* outputPin, __exception);

    //TODO: why __exception? shouldnt it be ucom::IException** __exception_ptr = NULL

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

    /**
     * Transmits a float value via a given output pin.
     *
     * @param  value     value to transmit.
     * @param  outputPin pin to use.
     * @return           standard atdf error code
     */
    tResult transmitFloatValue(tFloat32 value, cOutputPin* outputPin);

    /**
     * Transmits a boolean value via a given output pin.
     *
     * @param  value     value to transmit
     * @param  outputPin pin to use
     * @return           standard atdf error code
     */
    tResult transmitBoolValue(tBool value, cOutputPin* outputPin);

    /**
     * Prepares a media sample ready for transmission.
     *
     * @param  typeDescription the type description
     * @return                 prepared media sample
     */
    cObjectPtr<IMediaSample> initMediaSample(cObjectPtr<IMediaTypeDescription> typeDescription);

};
#endif //  _StdFilter_H_
