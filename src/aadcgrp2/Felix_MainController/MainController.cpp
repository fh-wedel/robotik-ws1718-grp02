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

// PIN CREATION

tResult cMainController::CreateFloatInputPins(__exception) {
    /* inputs for movement */
    RETURN_IF_FAILED(registerFloatInputPin("steeringAngle", &m_InputSteeringAngle, __exception_ptr));
    RETURN_IF_FAILED(registerFloatInputPin("speed",         &m_InputSpeed, __exception_ptr));

    RETURN_NOERROR;
}
tResult cMainController::CreateBoolInputPins(__exception) {

    /* inputs for collision detection */
    RETURN_IF_FAILED(registerBoolInputPin("obstacleDetected",    &m_InputObstacleDetected, __exception_ptr));
    RETURN_IF_FAILED(registerBoolInputPin("collisionDetected",   &m_InputCollisionDetected, __exception_ptr));

    /* inputs for having a sense of surroundings */
    RETURN_IF_FAILED(registerBoolInputPin("CrossingHasLeft",     &m_InputCrossingHasLeft, __exception_ptr));
    RETURN_IF_FAILED(registerBoolInputPin("CrossingHasRight",    &m_InputCrossingHasRight, __exception_ptr));
    RETURN_IF_FAILED(registerBoolInputPin("CrossingHasStraight", &m_InputCrossingHasStraight, __exception_ptr));

    RETURN_NOERROR;
}

tResult cMainController::CreateFloatOutputPins(__exception) {

    /* outputs for movement*/
    RETURN_IF_FAILED(registerFloatOutputPin("speed",            &m_OutputSpeed, __exception_ptr));
    RETURN_IF_FAILED(registerFloatOutputPin("steeringAngle",    &m_OutputSteeringAngle, __exception_ptr));

    RETURN_NOERROR;
}
tResult cMainController::CreateBoolOutputPins(__exception) {

    /* outputs for behaviour */
    RETURN_IF_FAILED(registerBoolOutputPin("CrossingTurnLeft",   &m_OutputCrossingTurnLeft, __exception_ptr));
    RETURN_IF_FAILED(registerBoolOutputPin("CrossingTurnRight",  &m_OutputCrossingTurnRight, __exception_ptr));
    RETURN_IF_FAILED(registerBoolOutputPin("CrossingGoStraight", &m_OutputCrossingGoStraight, __exception_ptr));

    /* auxiliary outputs */
    RETURN_IF_FAILED(registerBoolOutputPin("HeadLights",     &m_OutputHeadLights, __exception_ptr));
    RETURN_IF_FAILED(registerBoolOutputPin("BrakeLights",    &m_OutputBrakeLights, __exception_ptr));
    RETURN_IF_FAILED(registerBoolOutputPin("BlinkerLeft",    &m_OutputBlinkerLeft, __exception_ptr));
    RETURN_IF_FAILED(registerBoolOutputPin("BlinkerRight",   &m_OutputBlinkerRight, __exception_ptr));
    RETURN_IF_FAILED(registerBoolOutputPin("HazardLights",   &m_OutputHazardLights, __exception_ptr));

    RETURN_NOERROR;

}

tResult cMainController::Init(tInitStage eStage, __exception) {
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr))

    if (eStage == StageFirst) {
        RETURN_IF_FAILED(CreateFloatInputPins(__exception_ptr));
        RETURN_IF_FAILED(CreateBoolInputPins(__exception_ptr));
        RETURN_IF_FAILED(CreateFloatOutputPins(__exception_ptr));
        RETURN_IF_FAILED(CreateBoolOutputPins(__exception_ptr));
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
