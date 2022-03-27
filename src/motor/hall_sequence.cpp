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
HallSequence::HallSequence( int Phases )
	: PhaseCount(Phases)
	, Steps(Maths::CalcFactorial(PhaseCount))
{
	SensorsPins = new int[PhaseCount];
	HashMapping = new int[Steps];
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void HallSequence::DeclarePinsForSensor(int SensorID, int Pin)
{
	SensorsPins[SensorID] = Pin;
	Framework::PinMode(Pin, EInput);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int HallSequence::ReadState(int& NextStep)
{
	LastState = CurrentState;

	// Calc Hash
	int Hash = 0;
	for( int SensorID = 0; SensorID < PhaseCount; ++SensorID )
	{
		const int PinState = Framework::DigitalRead(SensorsPins[SensorID]);
		Hash |= (PinState << SensorID);
	}

	//Framework::Message("Debug = %d", Hash );
	// Find Step for Hash
	for( int Step = 0; Step < Steps; ++Step )
	{
		if( HashMapping[Step] == Hash )
		{
			CurrentState = Step;
			NextStep = Step;
			return CurrentState != LastState;
		}
	}

	// Should Never Hit... But just do nothing if it does.
	NextStep = -1;
	return false;
}
