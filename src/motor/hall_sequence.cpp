/*------------------------------------------------------------------------------
 ()		File: hall_sequence.cpp
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:

------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "motor/hall_sequence.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// HallSequence
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template<int PhaseCount>
template<typename ...PackedState>
void HallSequence<PhaseCount>::Bind_Imp(int Step, PackedState...Args)
{
	Framework::Assert( sizeof...(Args) > Steps, "Too Many Steps" );
	Framework::Assert( sizeof...(Args) < 0, "Too Few Steps" );

	Bind_Arg(Step, 0, Args...);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template<int PhaseCount>
template<typename ...PackedState>
void HallSequence<PhaseCount>::Bind_Arg(int Step, int SubIndex, int Value, PackedState...Args)
{
	Mapping[SubIndex][Step] = Value;
	Bind_Arg(Step, ++SubIndex, Args...);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template<int PhaseCount>
template<typename ...PackedState>
void HallSequence<PhaseCount>::Bind_Arg(int Step, int SubIndex)
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template<int PhaseCount>
void HallSequence<PhaseCount>::DeclarePinsForSensor(int SensorID, int Pin)
{
	SensorsPins[SensorID] = Pin;
	pinMode(Pin, INPUT);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template<int PhaseCount>
void HallSequence<PhaseCount>::ReadState(int (&Value)[PhaseCount])
{
	// Value =
}
