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
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// SpeedControl
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SpeedControl::UpdateInputParamaters(SequenceState& State)
{
	if( State.Tick )
	{
		//w = da/dt
		Time.ReadTime();
		//6 degrees somehow? 120 (3 sensors, 180 deg apart) / 20 (40 magnets, 20 of readable polarity)


		//float timeInSeconds = static_cast<float>(SpeedControl.time_since_last_change) * static_cast<float>(1e-6);
		//float traversedAngle = static_cast<float>(tickDelta) * stepConstant;
		//float radiansPerSecond = traversedAngle/timeInSeconds; //w = da/dt
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SpeedControl::SetPoleCount( uint8_t Count )
{
	PoleCount = Count;


}