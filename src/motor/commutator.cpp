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
// Commutator <N Phase>
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//template<int PhaseCount>
//Commutator<PhaseCount>::Commutator()
//{;

//}

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
	ControlPins[Phase*2+0] = SourcePin;
	ControlPins[Phase*2+1] = SinkPin;

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

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Commutator::Execute()
{
	//close all connections
	for( int phase = 0; phase < PhaseCount; ++phase )
	{
		Framework::DigitalWrite(ControlPins[phase*2 + 0], false); 
		Framework::DigitalWrite(ControlPins[phase*2 + 1], false); 
	}

	// write current commutator step state to io.
	if( CurrentStep >= 0)
	{
		for( int phase = 0; phase < PhaseCount; ++phase )
		{
			ECommutatorState phaseState = CommutationTable[CurrentStep*StepCount + phase];
			if( phaseState > ECommutatorState::EFloat )
			{
				//anti-clockwise just flips source/sink for a given phase,
				//so we can just bit twiddle it. Sequencing also has to be inverted
				//but that is handled external to this class anyway.
				const int pinToEnable = (SpinDirection == ESpinDirection::EClockwise)
					? phaseState
					: 0x1 >> phaseState
					;
				Framework::DigitalWrite(ControlPins[phase*2 + pinToEnable], true); 
			}
		}
	}
}