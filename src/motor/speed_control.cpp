/*------------------------------------------------------------------------------
 ()		File: speed_control.cpp
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:
	Speed control functionality
------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "motor/speed_control.h"
#include "framework.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// SpeedControl
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SpeedControl::Init()
{
	Time.Begin();
	RPM = 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int SpeedControl::MeasureRPM(const SequenceState& State)
{
	if(State.Step == TickPhase && State.UpdateCommutation)
	{
		const float AngularSpeedDegPerSecond = TickAngle / (Time.ReadTime()*1e-6);
		const float RPMf = AngularSpeedDegPerSecond / 6.0f;

		Framework::Message("EPMF %.2f, TA %d", RPMf, TickAngle);

		//round
		RPM = static_cast<int>( RPMf + 0.5f );

		//reset.
		Time.Restart();
	}

	return RPM;


	//Ticks += static_cast<int>(State.Tick);
	//State.Tick = false;

	//average over multiple ticks
	//if( Ticks >=5 )
	//{
		//w = da/dt => deg/sec
		//conv to rpm => 60(s) / 360(deg) => 1/6 
		//rpm => w  * (1/6) 
	
	//	const float AngularSpeedDegPerSecond = (TickAngle*Ticks) / (Time.ReadTime()*1e-6);
	//	const float RPMf = AngularSpeedDegPerSecond / 6.0f;

	//	Framework::Message("EPMF %.2f, Ticks %d, TA %d", RPMf, 1, TickAngle);

		//round
	//	RPM = static_cast<int>( RPMf + 0.5f );

		//reset.
	//	Time.Restart();
		//Ticks = 0;
		// /State.Tick = false;		
	//}

	//return RPM;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SpeedControl::ConfigureMeasureRPM( uint8_t Phase, uint8_t Angle )
{
	TickPhase = Phase;
	TickAngle = Angle;
}
