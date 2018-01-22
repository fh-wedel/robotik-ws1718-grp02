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

cMainController::cMainController(const tChar* __info) : cStdFilter(__info), m_bDebugModeEnabled(tFalse) {
    SetPropertyBool(SC_PROP_DEBUG_MODE, tFalse);
    SetPropertyStr(SC_PROP_DEBUG_MODE NSSUBPROP_DESCRIPTION, "If true debug infos are plotted to console");
    SetPropertyBool(SC_PROP_DEBUG_MODE NSSUBPROP_ISCHANGEABLE, tTrue);

    SetPropertyBool("Properties::ResetCollisionDetected", false);
    SetPropertyStr("Properties::ResetCollisionDetected" NSSUBPROP_DESCRIPTION, "Reset the collision detected flag and continue operation.");
    SetPropertyBool("Properties::ResetCollisionDetected" NSSUBPROP_ISCHANGEABLE, tTrue);
}

cMainController::~cMainController() {}

// PIN CREATION

tResult cMainController::CreateFloatInputPins(__exception) {
    /* inputs for movement */
    RETURN_IF_FAILED(registerFloatInputPin("targetSteeringAngle", &m_InputSteeringAngle, __exception_ptr));
    RETURN_IF_FAILED(registerFloatInputPin("targetSpeed",         &m_InputSpeed, __exception_ptr));

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

    RETURN_IF_FAILED(registerBoolOutputPin("EmergencyStop",   &m_OutputEmergencyStop, __exception_ptr));

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
    if (cString::IsEqual(strName, "Properties::ResetCollisionDetected")
    //    && GetPropertyBool("Properties::ResetCollisionDetected")
    ) {
        m_collisionDetected = false;
    }

	RETURN_NOERROR;
}

tResult cMainController::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived && pMediaSample != NULL) {
        RETURN_IF_POINTER_NULL(pMediaSample);

        if (pSource == &m_InputSteeringAngle) {
            m_targetSteeringAngle = readFloatValue(pMediaSample);
        } else if (pSource == &m_InputSpeed) {
            m_targetSpeed = readFloatValue(pMediaSample);
        } else if (pSource == &m_InputObstacleDetected) {
            m_obstacleDetected = readBoolValue(pMediaSample);
        } else if (pSource == &m_InputCollisionDetected) {
            m_collisionDetected |= readBoolValue(pMediaSample);
        } else if (pSource == &m_InputCrossingHasLeft) {
            m_crossingHasLeft = readBoolValue(pMediaSample);
        } else if (pSource == &m_InputCrossingHasRight) {
            m_crossingHasRight = readBoolValue(pMediaSample);
        } else if (pSource == &m_InputCrossingHasStraight) {
            m_crossingHasStraight = readBoolValue(pMediaSample);
        }

        return OnValueChanged();

    }
    RETURN_NOERROR;
}

tResult cMainController::OnValueChanged() {

    bool headLightsOn = false;
    bool brakeLightsOn = false;
    bool emergencyStop = false;

    bool blinkerLeftOn = false;
    bool blinkerRightOn = false;

    bool turnRight = false;
    bool turnLeft = false;
    bool keepStraight = false;

    float steeringAngle = 0.0f;
    float speed = 0.0f;

    if (m_collisionDetected || m_obstacleDetected) {

        /* Stop and keep wheels in same direction. */
        speed = 0.0f;
        steeringAngle = m_previousWrittenSteeringAngle;

        // Be a Christmas Tree :-)
        headLightsOn = true;
        brakeLightsOn = true;
        emergencyStop = true;


    } else {

        steeringAngle = m_targetSteeringAngle;
        speed = m_targetSpeed;

        headLightsOn = true;

        // TODO: BrakeLights could be on as well...
        brakeLightsOn =  false;

        /* No collision */
        emergencyStop = false;

        /*NOTE: Where do we want to go? */
        turnLeft = m_crossingHasLeft;
        turnRight = m_crossingHasRight && !turnLeft;
        keepStraight = m_crossingHasStraight && !turnLeft && !turnRight;

        blinkerLeftOn = false;
        blinkerRightOn = false;
    }

    RETURN_IF_FAILED(transmitFloatValue(steeringAngle, &m_OutputSteeringAngle));
    m_previousWrittenSteeringAngle = steeringAngle;

    RETURN_IF_FAILED(transmitFloatValue(speed, &m_OutputSpeed));

    RETURN_IF_FAILED(transmitBoolValue(headLightsOn, &m_OutputHeadLights));
    RETURN_IF_FAILED(transmitBoolValue(brakeLightsOn, &m_OutputBrakeLights));

    RETURN_IF_FAILED(transmitBoolValue(emergencyStop, &m_OutputHazardLights));

    /* indicate turn direction */
    RETURN_IF_FAILED(transmitBoolValue(blinkerLeftOn, &m_OutputBlinkerLeft));
    RETURN_IF_FAILED(transmitBoolValue(blinkerRightOn, &m_OutputBlinkerRight));

    /* outputs for behaviour */
    RETURN_IF_FAILED(transmitBoolValue(turnLeft, &m_OutputCrossingTurnLeft));
    RETURN_IF_FAILED(transmitBoolValue(turnRight, &m_OutputCrossingTurnRight));
    RETURN_IF_FAILED(transmitBoolValue(keepStraight, &m_OutputCrossingGoStraight));

    RETURN_IF_FAILED(transmitBoolValue(emergencyStop, &m_OutputEmergencyStop));

    RETURN_NOERROR;
}
