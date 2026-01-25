/*------------------------------------------------------------------------------
 ()		File: motor_driver.cpp
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:
	Manages motor sequencing (commutation). 
------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "motor_driver.h"
#include "framework.h"
#include "serial/serial_msg.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// MotorDriver
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorDriver::DeclarePinsForPhase(int Phase, int SourcePin, int SinkPin)
{
	//TODO fix device
	//Device::SerialCom.Message(" Phase %d; Source %d; Sink %d", Phase, SourcePin, SinkPin );

	ControlPins[(Phase*2)+PinOffset::ESink] = SinkPin;
	ControlPins[(Phase*2)+PinOffset::ESource] = SourcePin;

	Framework::PinMode(SourcePin, EOutput);
	Framework::PinMode(SinkPin, EOutput);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int MotorDriver::DeclareWinding(int SinkPhase, int SourcePhase)
{
	const int WindingIdx = RegisteredWinding++;

	WindingTable[WindingIdx].Sink = (SinkPhase*2) + PinOffset::ESink;
	WindingTable[WindingIdx].Source = (SourcePhase*2)+ PinOffset::ESource;

	return WindingIdx;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorDriver::BindClockwiseWinding(int State, int WindingIdx)
{
	WindingLookupCW[State] = WindingIdx;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorDriver::BindAntiClockwiseWinding(int State, int WindingIdx)
{
	WindingLookupACW[State] = WindingIdx;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorDriver::SetCommutatorStep(int Step)
{
	Framework::Assert(Step >= 0 && Step < GlobalMotor::StepCount);

	if( Step != CurrentStep )
	{
		CurrentStep = Step;
		//Device::SerialCom.Message( "Step %d", CurrentStep );
		if(Step > -1)
		{
			const int WindingIdx = SpinDirection == ESpinDirection::EClockwise? WindingLookupCW[Step] : WindingLookupACW[Step];
			ActiveWinding = WindingTable[WindingIdx];
		}

		CloseAllWindings();
	}
}

void MotorDriver::SetCommutatorStepBraking(int Step, bool bInitial)
{
	Framework::Assert(Step >= 0 && Step < GlobalMotor::StepCount);

	if( Step != CurrentStep || bInitial)
	{
		CurrentStep = Step;
		//Device::SerialCom.Message( "Step %d", CurrentStep );
		if(Step > -1)
		{
			const int WindingIdx = SpinDirection == ESpinDirection::EAntiClockwise? WindingLookupCW[Step] : WindingLookupACW[Step];
			ActiveWinding = WindingTable[WindingIdx];
		}

		CloseAllWindings();
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorDriver::SetMotorDirection(ESpinDirection Direction)
{
	SpinDirection = Direction;
}

ESpinDirection MotorDriver::GetMotorDirection() const
{
	return SpinDirection;
}

bool MotorDriver::IsMotorStarted() const
{
	return MotorOn;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorDriver::StartMotor()
{
	MotorOn = true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorDriver::StopMotor()
{
	CloseAllWindings();
	MotorOn = false;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorDriver::CloseAllWindings()
{
	//close all connections
	for( int Phase = 0; Phase < GlobalMotor::PhaseCount; ++Phase )
	{
		Framework::DigitalWrite(ControlPins[Phase*2 + PinOffset::ESink], Framework::Signal::NFetOpen); 
		Framework::AnalogWrite(ControlPins[Phase*2 + PinOffset::ESource], Framework::Signal::PFetOpen); 
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorDriver::SetDuty(uint8_t PWM)
{
	TargetDuty = PWM;
	
	if(CurrentStep > -1)
	{
		const int WindingIdx = SpinDirection == ESpinDirection::EClockwise? WindingLookupCW[CurrentStep] : WindingLookupACW[CurrentStep];
		ActiveWinding = WindingTable[WindingIdx];
	}

	CloseAllWindings();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
const char* DebugActiveWinding(int Winding, int Offset)
{
	int Phase = (Winding - Offset)/2;
	return Phase == 0? "A" : Phase==1 ? "B" : "C";
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorDriver::Drive()
{
	if (!MotorOn)
	{
		return;
	}

	if (ActiveWinding.Sink != -1 && ActiveWinding.Source != -1)
	{
		Duty = TargetDuty;
		//Device::SerialCom.Message( "High %s ; Low %s", DebugActiveWinding(ActiveWinding.Source, PinOffset::ESource),  DebugActiveWinding(ActiveWinding.Sink, PinOffset::ESink ));
		//Device::SerialCom.Message( "High %d ; Low %d", ControlPins[ActiveWinding.Source], ControlPins[ActiveWinding.Sink]);
		//Device::SerialCom.Message( "-----------------------------");

		Framework::DigitalWrite(ControlPins[ActiveWinding.Sink], Framework::Signal::NFetClosed);
		Framework::AnalogWrite(ControlPins[ActiveWinding.Source], Framework::Signal::PFetInterpolate(Duty));

		ActiveWinding.Source = -1;
		ActiveWinding.Sink = -1;
	}
	else if (Duty != TargetDuty)
	{
		//Update duty cycle mid state.
		//Duty = TargetDuty;
		//CloseAllWindings();

		//const int WindingIdx = SpinDirection == ESpinDirection::EClockwise? WindingLookupCW[CurrentStep] : WindingLookupACW[CurrentStep];
		//const int Source = WindingTable[WindingIdx].Source;
		//const int Sink = WindingTable[WindingIdx].Source;
		//Framework::DigitalWrite(ControlPins[Sink], Framework::Signal::NFetClosed);
		//Framework::AnalogWrite(ControlPins[Source], Framework::Signal::PFetInterpolate(Duty));
	}
}