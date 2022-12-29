/*------------------------------------------------------------------------------
 ()		File: motor_driver.cpp
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:
	Manages motor sequencing (commutation). 
------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "framework.h"
#include "motor_driver.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Commutator
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
MotorDriver::MotorDriver(int Phases)
	: PhaseCount(Phases)
	, StepCount(Maths::CalcFactorial(Phases))
{
	//I suppose leaking one-off allocations doesn't really matter on an MCU.
	CommutationTable = new ECommutatorState[PhaseCount*StepCount];
	ControlPins = new int[PhaseCount*2];
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorDriver::DeclarePinsForPhase(int Phase, int SourcePin, int SinkPin)
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
void MotorDriver::SetCommutatorStep(int Step)
{
	Framework::Assert(Step>=0 && Step<StepCount);

	if( Step != CurrentStep )
	{
		CurrentStep = Step;
		UpdateCommutator();
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorDriver::SetMotorDirection(ESpinDirection Direction)
{
	SpinDirection = Direction;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorDriver::SetStopped()
{
	CurrentStep = -1;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
const char* DebugCommutatorState(ECommutatorState State)
{
	static const char* commutatorStateToDebugString[] = {"0", "-", "+"};
	const int index = State + 1;
	return commutatorStateToDebugString[index];
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorDriver::UpdateCommutator()
{
	//close all connections
	for( int Phase = 0; Phase < PhaseCount; ++Phase )
	{
		Framework::DigitalWrite(ControlPins[Phase*2 + PinOffset::ESink], false); 
		Framework::DigitalWrite(ControlPins[Phase*2 + PinOffset::ESource], false); 
	}

	int PinIndex = 0;

	// convert current commutator step state to io pins
	if( CurrentStep >= 0)
	{
		for( int phase = 0; phase < PhaseCount; ++phase )
		{
			ECommutatorState StateForPhase = CommutationTable[CurrentStep*PhaseCount + phase];
			if( StateForPhase > ECommutatorState::EFloat )
			{
				//anti-clockwise just flips source/sink for a given phase,
				//so we can just bit twiddle it. Sequencing also has to be inverted
				//but that is handled external to this class anyway.
				const int pinToEnable = (SpinDirection == ESpinDirection::EClockwise) 
					? StateForPhase
					: 0x1 >> StateForPhase;				

				// Cache high pins, will be modulated by pwm in Drive()
				ActivePins[PinIndex++] = phase*2 + pinToEnable; 
			}
		}
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorDriver::Drive()
{
	if( ActivePins[0] != -1 && ActivePins[1] != -1 )
	{
		Framework::DigitalWrite(ControlPins[ActivePins[0]], true); 
		Framework::DigitalWrite(ControlPins[ActivePins[1]], true);

		// just emulate old behaviour for now, and set once per update.
		ActivePins[0] = -1;
		ActivePins[1] = -1;
	}
}