/**********************************************************************
Copyright (c)
Audi Autonomous Driving Cup. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3.  All advertising materials mentioning features or use of this software must display the following acknowledgement: �This product includes software developed by the Audi AG and its contributors for Audi Autonomous Driving Cup.�
4.  Neither the name of Audi nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY AUDI AG AND CONTRIBUTORS �AS IS� AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL AUDI AG OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

**********************************************************************/

#ifndef _MedianFilter_H_
#define _MedianFilter_H_

#include "stdafx.h"
#include "median.h"
#include "../includes/StdFilter.h"

#define UNIQUE_FILTER_ID "adtf.aadc.felix.MedianFilter"
#define FILTER_NAME "Felix MedianFilter"

/*! this is the main class for the steering controller filter */
class cMedianFilter : public cStdFilter {


    /*! This macro does all the plugin setup stuff
    * Warning: This macro opens a "protected" scope see UCOM_IMPLEMENT_OBJECT_INFO(...) in object.h
    */
    ADTF_DECLARE_FILTER_VERSION(UNIQUE_FILTER_ID, FILTER_NAME, OBJCAT_DataFilter, FILTER_NAME, 1, 0, 0, "");

    /* the input value which is fed into the linear function. */
    cInputPin m_InputValue;

    /* the value */
    cOutputPin m_OutputValue;

public:

    /*! constructor for template class
    *    \param __info   [in] This is the name of the filter instance.
    */
    cMedianFilter(const tChar* __info);

    /*! Destructor. */
    virtual ~cMedianFilter();

protected: // overwrites cFilter
    /*! Implements the default cFilter state machine call. It will be
    *	    called automatically by changing the filters state and needs
    *	    to be overwritten by the special filter.
    *    Please see page_filter_life_cycle for further information on when the state of a filter changes.
    *
    *    \param [in,out] __exception_ptr   An Exception pointer where exceptions will be put when failed.
    *        If not using the cException smart pointer, the interface has to
    *        be released by calling Unref().
    *    \param  [in] eStage The Init function will be called when the filter state changes as follows:\n
    *    \return Standard Result Code.
    */
    tResult Init(tInitStage eStage, ucom::IException** __exception_ptr);


    /*! This Function will be called by all pins the filter is registered to.
    *   \param [in] pSource Pointer to the sending pin's IPin interface.
    *   \param [in] nEventCode Event code. For allowed values see IPinEventSink::tPinEventCode
    *   \param [in] nParam1 Optional integer parameter.
    *   \param [in] nParam2 Optional integer parameter.
    *   \param [in] pMediaSample Address of an IMediaSample interface pointers.
    *   \return   Returns a standard result code.
    *   \warning This function will not implement a thread-safe synchronization between the calls from different sources.
    *   You need to synchronize this call by your own. Have a look to adtf_util::__synchronized , adtf_util::__synchronized_obj .
    */
    tResult OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample);


// PROPERTIES which can be set in the configuration editor

    /**
     * Gets called when a filter property is changed via the GUI.
     *
     * @param  strName the property identifier
     * @return         standard adtf error code
     */
    tResult PropertyChanged(const tChar* strName);

    /*! the struct with all the properties*/
    struct filterProperties {

        tInt windowSize;

    } m_filterProperties;


    /*! median filter instance */
    cMedian medianFilter;

private:

    /*! if the debug mode is enabled */
    tBool m_bDebugModeEnabled;

    /**
     * Gets called when a new input value has been received.
     * Used to update the outputs.
     *
     * @return standard adtf error code
     */
    tResult OnValueChanged(tFloat32 value);

};
/*! @} */ // end of group
#endif // _MedianFilter_H_
