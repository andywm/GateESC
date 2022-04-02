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
void HallSequence::ReadState(SequenceState& State)
{
	LastState = CurrentState;

	// Calc Hash
	int Hash = 0;
	for( int SensorID = 0; SensorID < PhaseCount; ++SensorID )
	{
		const int PinState = Framework::DigitalRead(SensorsPins[SensorID]);
		Hash |= (PinState << SensorID);
	}

	State.Tick = (Hash & (Hash-1)) == 1; // Tick when exactly 1 Sensor is active.
	State.UpdateCommutation = false;
	State.NextStep = -1;

	//Framework::Message("Debug = %d", Hash );
	// Find Step for Hash
	for( int Step = 0; Step < Steps; ++Step )
	{
		if( HashMapping[Step] == Hash )
		{
			CurrentState = Step;
			State.NextStep = Step;
			State.UpdateCommutation = (CurrentState != LastState);
		}
	}
}
