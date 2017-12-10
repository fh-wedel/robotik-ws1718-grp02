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
* $Author:: spiesra $  $Date:: 2017-05-16 10:06:17#$ $Rev:: 63289   $
**********************************************************************/

#ifndef _AADC_REALSENSE_FILTER_H_
#define _AADC_REALSENSE_FILTER_H_

#define OID_ADTF_REALSENSE_FILTER "adtf.aadc.felix.aadc_realsense"

#include "stdafx.h"
#include "Realsensehelper.h"

#define DEPTH_FRAME_SIZE 614400
#define NUMBER_AVAILABLE_PROPERTIES 68
/*!
* @defgroup RealsenseCamera
* @{
* Output filter for Realsense Camera
*
* This filter grabs images from the Intel Realsense Camera. The camera options can be set by the filterproperties. The filter uses LibRealsense provided by Intel. The Camera provides RGB, Infrared and Dephts stream.
*
*  \image html RealsenseCamera.PNG "Plugin Realsense Camera"
*
* \b Dependencies \n
* This plugin needs the following libraries:
* \li Boost   v.1.58.0
* \li OpenCV  v.3.2.0
* \li LibRealsense	v1.11.0
*
* <b> Filter Properties</b>
* The filter has the following properties:
* <table>
* <tr><th>Property<th>Description<th>Default
* <tr><td>Enable Debug Logging<td>Logs Debug Information to the ADTF Console<td>false
* <tr><td>Depth Stream FPS<td>Framerate of the Depth Stream<td>30
* <tr><td>Colour Stream FPS<td>Framerate of the Colour Stream<td>30
* <tr><td>Depth Stream Resolution<td>Resolution for the Depth Stream<td>640x480
* <tr><td>Colour Stream Resolution<td>Resolution for the Colour Stream<td>640x480
* <tr><td>Enable Infrared 1/2 Stream<td>Enables the Infrared Streams<td>true
* </table>
*
* <b> Input Pins</b>
* <table>
* <tr><th>Pin<th>Description<th>MajorType<th>SubType
* </table>
*
* <b> Output Pins</b>
* <table>
* <tr><th>Pin<th>Description<th>MajorType<th>SubType
* <tr><td>outputRGB<td>output for RGB Stream of Camera Imagetype: PF_RGB_888<td>MEDIA_TYPE_VIDEO<td>MEDIA_SUBTYPE_VIDEO_UNCOMPRESSED
* <tr><td>outputInfraRed1<td>output for Infrared Stream of Camera Imagetype: PF_GREYSCALE_8<td>MEDIA_TYPE_VIDEO<td>MEDIA_SUBTYPE_VIDEO_UNCOMPRESSED
* <tr><td>outputInfraRed2<td>output for Infrared Stream of Camera Imagetype: PF_GREYSCALE_8<td>MEDIA_TYPE_VIDEO<td>MEDIA_SUBTYPE_VIDEO_UNCOMPRESSED
* <tr><td>outputoutputDepthVisualization<td>output for Visualization of Depth Stream of Camera Imagetype: PF_GREYSCALE_16<td>MEDIA_TYPE_VIDEO<td>MEDIA_SUBTYPE_VIDEO_UNCOMPRESSED
* <tr><td>outputDepthRaw<td>output for Raw Data of Depth Stream of Camera<td>MEDIA_TYPE_STRUCTURED_DATA<td>MEDIA_SUBTYPE_STRUCT_STRUCTURED
* </table>
*
* <b>Plugin Details</b>
* <table>
* <tr><td>Path<td>src/aadcDemo/camera/AADC_Realsense
* <tr><td>Filename<td>aadc_RealsenseCamera.plb
* <tr><td>Version<td>1.0.0
* </table>
*/

//!  Class of Realsense Camera Filter
/*!
* This is the main class of the Realsense Camera Filter
*/
class cRealsenseCamera : public adtf::cFilter, adtf::IKernelThreadFunc
{
    /*! set the filter id and version etc */
    ADTF_DECLARE_FILTER_VERSION(OID_ADTF_REALSENSE_FILTER,
        "AADC Realsense Camera Felix",
        OBJCAT_SensorDevice,
        "librealsense",
        1, 11, 0,
        "");

protected:
    /*! Output Pin for Raw Depth Data */
    cOutputPin  m_outputDepthRaw;
    /*! Output Pin for RGB Video */
    cVideoPin	m_outputVideoRGB;
    /*! Output Pin for Depth Visualization Video */
    cVideoPin	m_outputVideoDepthVis;

public:
    /*! constructor for Realsense class
        \param __info   [in] This is the name of the filter instance.
        */
    cRealsenseCamera(const tChar* __info);
    /*! the destructor for this class
    */
    virtual ~cRealsenseCamera();

    /*! enables Debug output to Console */
    static const cString PropEnableDebugName;
    /*! Default Value for enable Debug Output Property */
    static const tBool 	PropEnableDebugDefault;
    /*! Framerate for Depth Stream */
    static const cString PropFPSDepthName;
    /*! Framerate for Colour Stream */
    static const cString PropFPSColourName;
    /*! Default for Colour and Depth Stream FPS */
    static const int PropFPSDefault;
    /*! Resolution of Depth Stream */
    static const cString PropResolutionDepthName;
    /*! Resolution of Colour Stream */
    static const cString PropResolutionColourName;
    /*! Default Value for Colour and Depth Resolution */
    static const cString PropResolutionDefault;
    /*! enables Colour Stream */
    static const cString PropEnableColourName;
    /*! enables Depth Stream */
    static const cString PropEnableDepthName;
    /*! Default Value to enable Streams */
    static const tBool PropEnableStreamDefault;

    /*! the struct with all the properties*/
    struct filterProperties
    {
        /*! stores if debug output should be printed */
        tBool enableDebugOutput;
        /*! stores Framerate for Depth Stream */
        int DepthFPS;
        /*! stores Framrate for Colour Stream */
        int ColourFPS;
        /*! stores Resolution for Depth Stream */
        cString DepthResolution;
        /*! stores Resolution for Colour Stream* */
        cString ColourResolution;
        /*! stores if Colour Stream is Enabled */
        tBool Colour;
        /*! stores if Depth Stream is Enabled */
        tBool Depth;

        int maxDepth;
        int minDepth;

        int blurRadius;
    }
    /*! the filter properties*/
    m_filterProperties;

    tResult PropertyChanged(const tChar* strName);


    /*!  context for librealsense */
    rs::context* m_ctx;
    /*! member variable to save the Camera in */
    rs::device* m_dev;

    /*! if a camera was found */
    tBool m_cameraFound;

protected:
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

    /*!
    *   Implements the default cFilter state machine call. It will be
    *   called automatically by changing the filters state and needs
    *   to be overwritten by the special filter.
    *   Please see page_filter_life_cycle for further information on when the state of a filter changes.
    *
    *   \param [in,out] __exception_ptr   An Exception pointer where exceptions will be put when failed.
    *                                   If not using the cException smart pointer, the interface has to
    *                                   be released by calling Unref().
    *   \param  [in] eStage The Init function will be called when the filter state changes as follows:\n   *
    *   \result Returns a standard result code.
    *
    */
    tResult Shutdown(tInitStage eStage, ucom::IException** __exception_ptr = NULL);

    /*! Implements the default cFilter state machine calls. It will be
    *    called automatically by changing the filters state IFilter::State_Ready -> IFilter::State_Running
    *    and can be overwritten by the special filter.
    *    \param __exception_ptr  [inout] An Exception pointer where exceptions will be put when failed.
    *        If not using the cException smart pointer, the interface has to
    *        be released by calling Unref().
    *    \return Standard Result Code.
    *    \note This method will be also called during the shutdown of a configuration if the Filter is connected to the Message Bus.
    *    (see:  section_message_bus)! This has to be done, to disconnect the Message Bus and to avoid Race Conditions.
    *
    */
    tResult Start(ucom::IException** __exception_ptr = NULL);

    /*!  Implements the default cFilter state machine calls. It will be
    *   called automatically by changing the filters state IFilter::State_Running -> IFilter::State_Ready
    *   and can be overwritten by the special filter.
    *   \param __exception_ptr  [inout] An Exception pointer where exceptions will be put when failed.
    *   If not using the cException smart pointer, the interface has to
    *   be released by calling Unref().
    *   \return Standard Result Code.
    *   \note This method will be also called during the shutdown of a configuration if the Filter is connected to the Message Bus.
    *   (see: section_message_bus)! This has to be done, to disconnect the Message Bus and to avoid Race Conditions.
    */
    tResult Stop(ucom::IException** __exception_ptr = NULL);

    /*! Transmits the RBG Data over the outputRBG Video Pin
    *	\param pData pointer to the Data to be transmitted
    *	\return Standard Result Code
    */
    tResult TransmitRGB(const void* pData);

    /*! Transmit Raw Depth Data over outputDepthRaw pin
    *	\param pData pointer to the Data to be transmitted
    *	\return Standard Result Code
    */
    tResult TransmitDepthRaw(const void* pData);

    /*! Transmit Visualization of the Depth Data over outputDepthVisualization Video pin
    *	\param pData pointer to the Data to be transmitted
    *	\return Standard Result Code
    */
    tResult TransmitDepthVis(const void* pData);

    /*! Converts Raw Depth Data to Depth Visualization
    *	\param pData pointer to the Data to be transmitted
    *	\return Standard Result Code
    */
    tResult ConvertDepth(void* pData);

    /*! Creates Filter Property automatically from Camera Properties
    *	\param OptionNumber int to enum of Camera Properties (see Librealsense Documentation for the enum)
    *	\return void
    */
    void CreateProperty(int OptionNumber);

    /*! Returns Height of the Stream from Resolution
    *	\param Resolution as cString
    *	\return integer of Height
    */
    int HeightOutOfResolution(cString Resolution);

    /*! Returns Width of the Stream from Resolution
    *	\param Resolution as cString
    *	\return integer of Width
    */
    int WidthOutOfResolution(cString Resolution);

    /*! Function to be called in the Thread
    *	\param Thread pointer to the Thread belonging to the Function
    *	\param data pointer to the Data needed by the Thread
    *	\param size size of the Data needed by the Thread
    *	\return Standard Result Code
    */
    tResult ThreadFunc(adtf::cKernelThread* Thread, tVoid* data, tSize size);


	void initStaticPins();
	void initDynamicPins();


private:
    /*! Thread to wait for Camera Picture */
    cKernelThread m_Thread;
    /*! Bitmap Format for RBG Output */
    tBitmapFormat m_BitmapFormatRGBOut;
    /*! Bitmap Format for Depth Visualization Output */
    tBitmapFormat m_BitmapFormatDepthOut;


};


/*!
*@}
*/


//*************************************************************************************************
#endif // _AADC_REALSENSE_FILTER_H_
