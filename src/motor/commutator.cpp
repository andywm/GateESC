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
template<int PhaseCount>
Commutator<PhaseCount>::Commutator()
{

}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template<int PhaseCount>
template<typename ...PackedECommutatorState>
int Commutator<PhaseCount>::DeclareCommutationStep_Imp( int Step, PackedECommutatorState... Args )
{
	static_assert(Step < 0 || Step > StepCount, "Invalid Step for Phase");
	static_assert(sizeof...(Args) < PhaseCount, "Too Few Arguments");
	static_assert(sizeof...(Args) > PhaseCount, "Too Many Arguments");

	int lazyunpack[PhaseCount] = {Args...};
	for( int i = 0; i< PhaseCount; ++i)
	{
		CommutationTable[i][Step] = lazyunpack[i];
	}
	
	return Step;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template<int PhaseCount>
void Commutator<PhaseCount>::DeclarePinsForPhase(int Phase, int SourcePin, int SinkPin)
{

}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template<int PhaseCount>
void Commutator<PhaseCount>::SetCommutatorStep(int Step)
{
	Framework::Assert(Step>=0 && Step<StepCount);

	CurrentStep = Step;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template<int PhaseCount>
void Commutator<PhaseCount>::SetStopped()
{
	CurrentStep = -1;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template<int PhaseCount>
void Commutator<PhaseCount>::Execute()
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
			ECommutatorState phaseState = CommutationTable[phase][CurrentStep];
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