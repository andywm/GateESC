/*------------------------------------------------------------------------------
 ()		File: speed_control.cpp
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:
	Speed control functionality
------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//#include "motor/speed_control.h"
//#include "util/naff_maths_utilities.h"
//#include "framework.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// SpeedControl
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//void SpeedControl::Init()
//{
//	Time.Begin();
//	RPM = 0;
//}

//void SpeedControl::CalculatePidEffort(int MeasuredRPM)
//{
//	static float TargetRPM = 30.0f;
//	//Error Signal
//	float Error = TargetRPM - MeasuredRPM;
//
//	//Proportional
//	float Proportional = PID.ProportionalGain * Error;
//
//	//Integral
//	PID.IntegratorSum = PID.IntegratorSum + 0.5f * PID.IntegralGain * PID.SampleInterval * (Error + PID.PreviousError);
//
//	//integral anti-windup
//	const float IntegralLimitMax = Maths::Max(PID.UpperLimit - Proportional, 0.0f);
//	const float IntegralLimitMin = Maths::Min(PID.LowerLimit - Proportional, 0.0f);
//	PID.IntegratorSum = Maths::Clamp(PID.IntegratorSum, IntegralLimitMin, IntegralLimitMax);
//
//	//if(PID.UpperLimit > Proportional)
//	//{
//	//	IntegralLimitMax = PID.UpperLimit - Proportional;
//	//}
//	//if(PID.LowerLimit < Proportional)
//	//{
//	//	IntegralLimitMin = PID.LowerLimit - Proportional;
//	//}
//
//	//Derivative; band limited
//	PID.Differentiator = 2.0f * PID.DerivativeGain * (MeasuredRPM - PID.PreviousMeasurement)
//		+ (2.0f * PID.DerivativeLowPassFilter - PID.SampleInterval) * PID.Differentiator
//		/ (2.0f * PID.DerivativeLowPassFilter + PID.SampleInterval);
//	
//	PID.PID_Effort = Maths::Clamp(Proportional + PID.IntegratorSum + PID.Differentiator,
//		PID.LowerLimit, 
//		PID.UpperLimit
//		);
//
//
//	//Cache for next cycle.
//	PID.PreviousError = Error;
//	PID.PreviousMeasurement = MeasuredRPM;
//}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//int SpeedControl::MeasureRPM(const SequenceState& State)
//{
//	if(State.Step == TickPhase && State.UpdateCommutation)
//	{
//		const float AngularSpeedDegPerSecond = TickAngle / (Time.ReadTime()*1e-6);
//		const float RPMf = AngularSpeedDegPerSecond / 6.0f;
//
//		Framework::Message("EPMF %.2f, TA %d", RPMf, TickAngle);
//
//		//round
//		RPM = static_cast<int>( RPMf + 0.5f );
//
//		//reset.
//		Time.Restart();
//	}
//
//	return RPM;
//
//
//	//Ticks += static_cast<int>(State.Tick);
//	//State.Tick = false;
//
//	//average over multiple ticks
//	//if( Ticks >=5 )
//	//{
//		//w = da/dt => deg/sec
//		//conv to rpm => 60(s) / 360(deg) => 1/6 
//		//rpm => w  * (1/6) 
//	
//	//	const float AngularSpeedDegPerSecond = (TickAngle*Ticks) / (Time.ReadTime()*1e-6);
//	//	const float RPMf = AngularSpeedDegPerSecond / 6.0f;
//
//	//	Framework::Message("EPMF %.2f, Ticks %d, TA %d", RPMf, 1, TickAngle);
//
//		//round
//	//	RPM = static_cast<int>( RPMf + 0.5f );
//
//		//reset.
//	//	Time.Restart();
//		//Ticks = 0;
//		// /State.Tick = false;		
//	//}

//	return RPM;
//}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//void SpeedControl::ConfigureMeasureRPM( uint8_t Phase, uint8_t Angle )
//{
//	TickPhase = Phase;
//	TickAngle = Angle;
//}
