/*------------------------------------------------------------------------------
 ()		File: speed_control.h
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:
	Speed control functionality
------------------------------------------------------------------------------*/
//#pragma once
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//#include "motor/sequence_state.h"
//#include "util/timer.h"
//#include <stdint.h>
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//struct PID_Data
//{
//	// Controller Gains
//	float ProportionalGain = 1.0f;
//	float IntegralGain = 1.0f;
//	float DerivativeGain = 1.0f;
//
//	// Low-pass filter for derivative
//	float DerivativeLowPassFilter = 0.0f;
//
//	float SampleInterval = 0.0f;
//
//
//	float UpperLimit = 0.0f;
//	float LowerLimit = 0.0f;
//
//	//Integrator
//	float IntegratorSum;
//	float PreviousError;
//	float Differentiator;
//	float PreviousMeasurement;
//
//	float PID_Effort;
//};

//struct MotorPWM
//{
//	float Duty;
//	float PulseWidth;
//
//	float Percent;
//};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// SpeedControl
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//class SpeedControl
//{
//public:
//	void Init();
//	int MeasureRPM(const SequenceState& State);
//	void ConfigureMeasureRPM( uint8_t Phase, uint8_t Angle );
//	void CalculatePidEffort(int MeasuredRPM);
//
//	PID_Data PID;
//	Timer Time;
//	uint8_t TickAngle{0};
//	int TickPhase{0};
//	int RPM{0};
//};