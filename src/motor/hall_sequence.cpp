/*------------------------------------------------------------------------------
 ()		File: hall_sequence.cpp
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:

------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "motor/hall_sequence.h"
#include "framework.h"
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
	//add to hash
	HashMapping[Step] |= (Value << SubIndex);
	//Mapping[SubIndex+1][Step] = Value;
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
	Framework::PinMode(Pin, EInput);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template<int PhaseCount>
int HallSequence<PhaseCount>::ReadState(int& NextStep)
{
	LastState = CurrentState;

	// Calc Hash
	int Hash = 0;
	for( int SensorID = 0; SensorID < PhaseCount; ++SensorID )
	{
		const int PinState = Framework::DigitalRead(SensorsPins[SensorID]);
		Hash |= (PinState << SensorID);
	}

	// Find Step for Hash
	for( int Step = 0; Step < Steps; ++Step )
	{
		if( HashMapping[Step] == Hash )
		{
			NextStep = (CurrentState = Step);
			return CurrentState != LastState;
		}
	}

	// Should Never Hit...
	Framework::Assert(0, "Sensor Configuration is Invalid.");
	NextStep = -1;
	return false;
}
