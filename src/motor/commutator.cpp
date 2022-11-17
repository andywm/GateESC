/*------------------------------------------------------------------------------
 ()		File: commutator.cpp
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:
	Manages motor sequencing (commutation). 
------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "framework.h"
#include "commutator.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Commutator
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Commutator::Commutator(int Phases)
	: PhaseCount(Phases)
	, StepCount(Maths::CalcFactorial(Phases))
{
	//I suppose leaking one-off allocations doesn't really matter on an MCU.
	CommutationTable = new ECommutatorState[PhaseCount*StepCount];
	ControlPins = new int[PhaseCount*2];
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Commutator::DeclarePinsForPhase(int Phase, int SourcePin, int SinkPin)
{
	Framework::Message("Phase=%d, Source=%d, Sink=%d", Phase, SourcePin, SinkPin );

	ControlPins[(Phase*2)+PinOffset::ESink] = SinkPin;
	ControlPins[(Phase*2)+PinOffset::ESource] = SourcePin;

	Framework::Message("Source=%d, Sink=%d",  ControlPins[(Phase*2)+PinOffset::ESource], ControlPins[(Phase*2)+PinOffset::ESink] );


	Framework::PinMode(SourcePin, EOutput);
	Framework::PinMode(SinkPin, EOutput);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Commutator::SetCommutatorStep(int Step)
{
	Framework::Assert(Step>=0 && Step<StepCount);

	CurrentStep = Step;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Commutator::SetMotorDirection(ESpinDirection Direction)
{
	SpinDirection = Direction;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Commutator::SetStopped()
{
	CurrentStep = -1;
}

const char* DebugCommutatorState(ECommutatorState State)
{
	static const char* commutatorStateToDebugString[] = {"0", "-", "+"};
	const int index = State + 1;
	return commutatorStateToDebugString[index];
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Commutator::Execute()
{
	//close all connections
	for( int phase = 0; phase < PhaseCount; ++phase )
	{
		Framework::DigitalWrite(ControlPins[phase*2 + PinOffset::ESink], false); 
		Framework::DigitalWrite(ControlPins[phase*2 + PinOffset::ESource], false); 
	}

#if defined( DISABLE_MOTOR_EXEC )
	//bail
	return;
#endif
	//ECommutatorState debug[PhaseCount] = {ECommutatorState::EFloat,ECommutatorState::EFloat,ECommutatorState::EFloat};

	// write current commutator step state to io.
	if( CurrentStep >= 0)
	{
		for( int phase = 0; phase < PhaseCount; ++phase )
		{
			ECommutatorState phaseState = CommutationTable[CurrentStep*PhaseCount + phase];
			if( phaseState > ECommutatorState::EFloat )
			{
				//anti-clockwise just flips source/sink for a given phase,
				//so we can just bit twiddle it. Sequencing also has to be inverted
				//but that is handled external to this class anyway.
				const int pinToEnable = (SpinDirection == ESpinDirection::EClockwise) 
					? phaseState
					: 0x1 >> phaseState;				

				//debug[phase] = phaseState;
				Framework::DigitalWrite(ControlPins[phase*2 + pinToEnable], true); 
			}
		}

		//Framework::Message("%s|%s|%s", DebugCommutatorState(debug[0]),  DebugCommutatorState(debug[1]),  DebugCommutatorState(debug[2]));
	}
}