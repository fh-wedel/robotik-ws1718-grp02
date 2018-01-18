/**
Copyright (c)
Audi Autonomous Driving Cup. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3.  All advertising materials mentioning features or use of this software must display the following acknowledgement: �This product includes software developed by the Audi AG and its contributors for Audi Autonomous Driving Cup.�
4.  Neither the name of Audi nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY AUDI AG AND CONTRIBUTORS �AS IS� AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL AUDI AG OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


**********************************************************************
* $Author:: spiesra $  $Date:: 2017-05-12 10:01:55#$ $Rev:: 63111   $
**********************************************************************/


// arduinofilter.cpp : Definiert die exportierten Funktionen f�r die DLL-Anwendung.

#include "stdafx.h"
#include "cWheelSpeedController.h"

#define WSC_PROP_GAIN "Gain"
#define WSC_PROP_PID_KP "PID::Kp_value"
#define WSC_PROP_PID_KI "PID::Ki_value"
#define WSC_PROP_PID_KD "PID::Kd_value"
#define WSC_PROP_PID_SAMPLE_TIME "PID::Sample_Interval_[msec]"


#define WSC_PROP_PID_MAXOUTPUT "PID::Maximum output"
#define WSC_PROP_PID_MINOUTPUT "PID::Minimum output"
#define WSC_PROP_DEBUG_MODE "Debug Mode"
#define WSC_PROP_WAIT_TIME "Wait Time"
#define WSC_PROP_BRAKE_MARGIN "Brake Margin"


ADTF_FILTER_PLUGIN("Franz Wheel Speed Controller", OID_ADTF_WHEELSPEEDCONTROLLER, cWheelSpeedController)

cWheelSpeedController::cWheelSpeedController(const tChar* __info) : cStdFilter(__info) {

    SetPropertyFloat(WSC_PROP_GAIN, 10);
    SetPropertyBool(WSC_PROP_GAIN NSSUBPROP_ISCHANGEABLE, tTrue);
    SetPropertyStr(WSC_PROP_GAIN NSSUBPROP_DESCRIPTION, "Gain to apply after the controller");

    SetPropertyFloat(WSC_PROP_PID_KP, 0.39088);
    SetPropertyBool(WSC_PROP_PID_KP NSSUBPROP_ISCHANGEABLE, tTrue);
    SetPropertyStr(WSC_PROP_PID_KP NSSUBPROP_DESCRIPTION, "The proportional factor Kp for the PID Controller");

    SetPropertyFloat(WSC_PROP_PID_KI, 0.98867);
    SetPropertyBool(WSC_PROP_PID_KI NSSUBPROP_ISCHANGEABLE, tTrue);
    SetPropertyStr(WSC_PROP_PID_KI NSSUBPROP_DESCRIPTION, "The integral factor Ki for the PID Controller");

    SetPropertyFloat(WSC_PROP_PID_KD, 0.01398);
    SetPropertyBool(WSC_PROP_PID_KD NSSUBPROP_ISCHANGEABLE, tTrue);
    SetPropertyStr(WSC_PROP_PID_KD NSSUBPROP_DESCRIPTION, "The differential factor Kd for the PID Controller");

    SetPropertyFloat(WSC_PROP_PID_MAXOUTPUT, 20);
    SetPropertyBool(WSC_PROP_PID_MAXOUTPUT NSSUBPROP_ISCHANGEABLE, tTrue);
    SetPropertyStr(WSC_PROP_PID_MAXOUTPUT NSSUBPROP_DESCRIPTION, "The maximum allowed output for the wheel speed controller (speed in m/sec^2)");

    SetPropertyFloat(WSC_PROP_PID_MINOUTPUT, -20);
    SetPropertyBool(WSC_PROP_PID_MINOUTPUT NSSUBPROP_ISCHANGEABLE, tTrue);
    SetPropertyStr(WSC_PROP_PID_MINOUTPUT NSSUBPROP_DESCRIPTION, "The minimum allowed output for the wheel speed controller (speed in m/sec^2)");

    SetPropertyBool(WSC_PROP_DEBUG_MODE, tFalse);
    SetPropertyBool(WSC_PROP_DEBUG_MODE NSSUBPROP_ISCHANGEABLE, tTrue);
    SetPropertyStr(WSC_PROP_DEBUG_MODE NSSUBPROP_DESCRIPTION, "If true debug infos are written to output");

    SetPropertyFloat(WSC_PROP_WAIT_TIME, 5);
    SetPropertyStr(WSC_PROP_WAIT_TIME NSSUBPROP_DESCRIPTION, "Time in seconds to send 0's to the arduino before starting the controller");

    SetPropertyFloat(WSC_PROP_BRAKE_MARGIN, 0.05);
    SetPropertyBool(WSC_PROP_BRAKE_MARGIN NSSUBPROP_ISCHANGEABLE, tTrue);
    SetPropertyStr(WSC_PROP_BRAKE_MARGIN NSSUBPROP_DESCRIPTION, "Output Margin before brake lights will turn on when decellerating");

    medianIsUpToDate = false;
    windowSize = 5;

    resetController();
}

cWheelSpeedController::~cWheelSpeedController() {}


tResult cWheelSpeedController::PropertyChanged(const tChar* strPropertyName) {
    RETURN_IF_FAILED(cFilter::PropertyChanged(strPropertyName));

    if (strPropertyName != NULL) {
        RETURN_IF_POINTER_NULL(strPropertyName);

        if (cString::IsEqual(strPropertyName, WSC_PROP_GAIN)) {
            m_f64Gain = GetPropertyFloat(WSC_PROP_GAIN);

        } else if (cString::IsEqual(strPropertyName, WSC_PROP_PID_KP)) {
            m_f64PIDKp = GetPropertyFloat(WSC_PROP_PID_KP);

        } else if (cString::IsEqual(strPropertyName, WSC_PROP_PID_KD)) {
            m_f64PIDKd = GetPropertyFloat(WSC_PROP_PID_KD);

        } else if (cString::IsEqual(strPropertyName, WSC_PROP_PID_KI)) {
            m_f64PIDKi = GetPropertyFloat(WSC_PROP_PID_KI);

        } else if (cString::IsEqual(strPropertyName, WSC_PROP_DEBUG_MODE)) {
            m_bShowDebug = static_cast<tBool>(GetPropertyBool(WSC_PROP_DEBUG_MODE));

        } else if (cString::IsEqual(strPropertyName, WSC_PROP_PID_MINOUTPUT)) {
            m_f64PIDMinimumOutput = GetPropertyFloat(WSC_PROP_PID_MINOUTPUT);

        } else if (cString::IsEqual(strPropertyName, WSC_PROP_PID_MAXOUTPUT)) {
            m_f64PIDMaximumOutput = GetPropertyFloat(WSC_PROP_PID_MAXOUTPUT);

        } else if (cString::IsEqual(strPropertyName, WSC_PROP_WAIT_TIME)) {
            m_waitTime = GetPropertyFloat(WSC_PROP_WAIT_TIME);

        } else if (cString::IsEqual(strPropertyName, WSC_PROP_BRAKE_MARGIN)) {
            m_f64BrakeMargin = GetPropertyFloat(WSC_PROP_BRAKE_MARGIN);

        }

    }

    RETURN_NOERROR;
}


tResult cWheelSpeedController::CreateInputPins(__exception) {

    RETURN_IF_FAILED(registerFloatInputPin("set_WheelSpeed", &m_oInputSetWheelSpeed, __exception_ptr));
    RETURN_IF_FAILED(registerFloatInputPin("measured_wheelSpeed", &m_oInputMeasWheelSpeed, __exception_ptr));
    RETURN_IF_FAILED(registerBoolInputPin("emergencystop", &m_oInputEmergencyStop, __exception_ptr));

    RETURN_NOERROR;
}

tResult cWheelSpeedController::CreateOutputPins(__exception) {

    RETURN_IF_FAILED(registerFloatOutputPin("actuator_output", &m_oOutputActuator, __exception_ptr));
    RETURN_IF_FAILED(registerBoolOutputPin("brake_lights", &m_oOutputBrakeLights, __exception_ptr));

    RETURN_NOERROR;
}

tResult cWheelSpeedController::GetInterface(const tChar* idInterface, tVoid** ppvObject) {
    return cFilter::GetInterface(idInterface, ppvObject);
}

tResult cWheelSpeedController::Init(tInitStage eStage, __exception) {
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr))

    switch (eStage) {
    case StageFirst:
        RETURN_IF_FAILED(CreateInputPins(__exception_ptr));
        RETURN_IF_FAILED(CreateOutputPins(__exception_ptr));
        break;

    case StageNormal:
        if (m_bShowDebug) {
            printf("Speed Controller Stage Normal\n");
        }

        break;

    case StageGraphReady:
        // set the flags which indicate if the media descriptions strings were set
        m_bInputMeasWheelSpeedGetID = tFalse;
        m_bInputSetWheelSpeedGetID = tFalse;
        m_bInputEmergStopGetID = tFalse;
        m_bInputActuatorGetID = tFalse;

        m_bEmergencyStop = tFalse;
        m_bBrakeLights = tTrue;
        m_f64LastOutput = 0.0f;

        if (m_bShowDebug) {
            printf("Speed Controller StageGraphReady\n");
        }
        break;
    }

    RETURN_NOERROR;
}

tResult cWheelSpeedController::Start(__exception) {

    resetController();

    // Timestamps are in us
    m_startupTime = GetTime() + (tTimeStamp)(1000000 * m_waitTime);

    if (m_bShowDebug) {
        printf("Speed Controller Start\n");
    }

    return cFilter::Start(__exception_ptr);
}

tResult cWheelSpeedController::Stop(__exception) { return cFilter::Stop(__exception_ptr); }

tResult cWheelSpeedController::Shutdown(tInitStage eStage, __exception) {
    if (eStage == StageNormal)
    {
        m_oActive.clear();

        m_oLock.Release();
    }
    return cFilter::Shutdown(eStage,__exception_ptr);
}

void cWheelSpeedController::resetController() {
    if (m_bShowDebug) {
        if (m_bEmergencyStop) {
            std::cout << "Emergergency Stop! \n";
        }
        std::cout << "Controller Reset" << '\n';
    }
    m_f64LastOutput = 0;
    m_f64LastMeasuredError = 0;
    m_f64SetPoint = 0;
    m_lastSampleTime = GetTime();
    m_f64LastSpeedValue = 0;
    m_f64accumulatedVariable = 0;
    m_bBrakeLights = tTrue;
}

tResult cWheelSpeedController::OnPinEvent(    IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {

    __synchronized_obj(m_critSecOnPinEvent);
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived && pMediaSample != NULL) {
        RETURN_IF_POINTER_NULL(pMediaSample);

        if (pSource == &m_oInputMeasWheelSpeed) {

          // Receive new measured Speed

            m_f64MeasuredVariable = static_cast<tFloat64>(readFloatValue(pMediaSample));

            //calculation
            // if the desired output speed is 0 and we have stoped, immediately stop the motor
            // if the system just started, wait for the controller to start up
            // if a emergency stop is requested, reset Controller to stop immediately
            if ((m_f64SetPoint == 0 && m_f64MeasuredVariable == 0) || m_startupTime > GetTime() || m_bEmergencyStop) {

                resetController();

            } else {
                tFloat64 currentOutput = getControllerValue(m_f64MeasuredVariable);

                m_bBrakeLights = currentOutput + m_f64BrakeMargin < m_f64LastOutput;

                if (m_bShowDebug) {
                    std::cout << "Current Output: " << currentOutput << '\n';
                    std::cout << "Last Output: " << m_f64LastOutput << '\n';
                    std::cout << "Brake Lights" << m_bBrakeLights << '\n';
                }

                m_f64LastOutput = currentOutput;

            }

            // change the sign of the output to drive in the right direction
            tFloat32 outputValue = static_cast<tFloat32>(m_f64LastOutput) * -1;

            RETURN_IF_FAILED(transmitFloatValue(outputValue, &m_oOutputActuator));
            RETURN_IF_FAILED(transmitBoolValue(m_bBrakeLights, &m_oOutputBrakeLights));

        } else if (pSource == &m_oInputSetWheelSpeed) {

            // Receive new target Speed

            // read Value
            m_f64SetPoint = static_cast<tFloat64>(readFloatValue(pMediaSample));

        } else if (pSource == &m_oInputEmergencyStop) {

            // Receive emergency stop

            // read Value
            m_bEmergencyStop = readBoolValue(pMediaSample);

        }
    }

    RETURN_NOERROR;
}

tFloat64 cWheelSpeedController::getControllerValue(tFloat64 i_f64MeasuredValue) {

    // calculate time delta since last sample
    tTimeStamp deltaT = GetTime() - m_lastSampleTime;
    m_lastSampleTime = GetTime();

    if (m_bShowDebug) {
      std::cout << "deltaT: " <<  deltaT << '\n';
      std::cout << "current Speed: " << i_f64MeasuredValue << '\n';
      std::cout << "Set Value: " << m_f64SetPoint << " Measured Value: " << i_f64MeasuredValue << '\n';
    }

    tFloat64 f64Result = 0;

    //error:
    tFloat64 f64Error = (m_f64SetPoint - i_f64MeasuredValue);

    //algorithm:
    //esum = esum + e
    //y = Kp * e + Ki * Ta * esum + Kd * (e - ealt) / deltaT
    //ealt = e

    // accumulated error (in m/s):
    m_f64accumulatedVariable += f64Error * deltaT / 1e6;

    tFloat64 y_p = m_f64PIDKp * f64Error;
    tFloat64 y_i = m_f64PIDKi * m_f64accumulatedVariable;
    tFloat64 y_d = m_f64PIDKd * (f64Error - m_f64LastMeasuredError) / deltaT;

    f64Result =  y_p + y_i + y_d;

    m_f64LastMeasuredError = f64Error;

    if (m_bShowDebug) {
        std::cout << "Error: " << f64Error << " Accumulated Error " << m_f64accumulatedVariable << '\n';
        std::cout << "Output Value before limit " << f64Result  << '\n';
    }

    // Apply gain
    f64Result *= m_f64Gain;

    // checking for minimum and maximum limits
    //f64Result = min(m_f64PIDMaximumOutput, max(m_f64PIDMinimumOutput, f64Result));
    if(f64Result > m_f64PIDMaximumOutput) f64Result = m_f64PIDMaximumOutput;
    if(f64Result < m_f64PIDMinimumOutput) f64Result = m_f64PIDMinimumOutput;

    if (m_bShowDebug) {
        std::cout << "Output Value after limit and Output gain applied" << f64Result  << '\n';
    }

    return f64Result;
}

/*! Yields the current time in microseconds */
tTimeStamp cWheelSpeedController::GetTime() {
    return (_clock != NULL) ? _clock->GetTime () : cSystem::GetTime();
}

void cWheelSpeedController::pushValue(tBool newValue) {
    // erase oldest values if window size has been reached
    int amountOfOverflownElements = storedValues.size() - windowSize;
    if (amountOfOverflownElements > 0) {
        storedValues.erase(
            storedValues.begin(),
            storedValues.begin() + amountOfOverflownElements
        );
    }

    // append new value
    storedValues.push_back(newValue);
    medianIsUpToDate = false;
}

bool cWheelSpeedController::calculateMedian() {

    if (!medianIsUpToDate) {
        std::vector<bool> values = storedValues;

        // sort values ascending
        std::sort(values.begin(), values.end());

        // 0 / 2 = 0    -> 0
        // 1 / 2 = 0.5  -> 0
        // 2 / 2 = 1    -> 1
        // 3 / 2 = 1.5  -> 1
        // 4 / 2 = 2    -> 2
        // 5 / 2 = 2.5  -> 2
        int medianIndex = values.size() / 2;

        calculatedMedian = values[medianIndex];
    }

    return calculatedMedian;
}
