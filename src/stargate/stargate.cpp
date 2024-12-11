/*------------------------------------------------------------------------------
 ()		File: stargate.cpp
 /\		Authour: Andrew Woodward-May
/  \	Date: June 2024	License: MIT

Description:
------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "stargate/stargate.h"
#include "chevron_angle_table.h"
#include "motor/motor_controller.h"
#include "chevrons/chevron_lock.h"
#include "chevrons/lamps.h"
#include <Arduino.h>
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/* Dialing State Machine
									OnReset
		[Reset] <-------------------------------------------\
		  |	 ^												|
		  |	 | OnTimeout									|
  OnReady |	 |						  /-------> [FailDial] -|
		  |  | 	    /--------> [Commit] ------> [Wormhole] -|
		  |  |     /ActivatorPressed
		  v	 |	  /						  LockSymbol		  Light
	 	[WaitInput] -------> [SeekSymbol] ----------> [Lock] --------> [Lamp]
		  ^			SymbPress			\----------------------------->[Lamp]
		  |											Light				 |
		  \-------------------------------------------------------------/
							WaitForNextInput
*/

//------------------------------------------------------------------------------
// Reset
//------------------------------------------------------------------------------
struct SGS_ResetAndReady final : public StargateFSMState
{
	void OnEnter() override 
	{
		Stargate->Reset();
		FSM->Transition(StargateDialState::WaitForInput);
	}
};

//------------------------------------------------------------------------------
// WaitSym
//------------------------------------------------------------------------------
struct SGS_WaitingForInput final : public StargateFSMState
{
	void OnProcess() override
	{
		const uint8 Symb = DialingInterface::Get().PollAndEat();
		if (Symb != UINT8_MAX)
		{
			Stargate->CurrentChevron = Symb;
			FSM->Transition(StargateDialState::SeekToSymbol);

			return;
		}

		if (DialingInterface::Get().IsActivatorPressed())
		{
			//check address.
			//FSM->Transition(StargateDialState::Wormhole);
			FSM->Transition(StargateDialState::DialFailure);
		}
	}
};

//------------------------------------------------------------------------------
// SeekSymbol
//------------------------------------------------------------------------------
struct SGS_SeekingSymbol final : public StargateFSMState
{
	void OnEnter() override
	{
		static bool bOscilate = false;
		Stargate->SeekSymbolWithDirection((bOscilate = !bOscilate));
	}

	void OnProcess() override
	{
		if (Stargate->HasFinishedSeek())
		{
			FSM->Transition(StargateDialState::LockChevron);
		}
	}
};

//------------------------------------------------------------------------------
// Lock
//------------------------------------------------------------------------------
struct SGS_ChevronLock final : public StargateFSMState
{
	void OnEnter() override
	{
		TimerS.Restart();
		Stargate->RaiseLock();
	}

	void OnProcess() override
	{
		if (TimerS.ReadTime() > Stargate->Config.LockTime)
		{
			FSM->Transition(StargateDialState::LampOn);
		}
	}

	void OnLeave() override
	{
		Stargate->ReleaseLock();
	}

	Timer TimerS;
};

//------------------------------------------------------------------------------
// Lamp
//------------------------------------------------------------------------------
struct SGS_SymbolLamp final : public StargateFSMState
{
	void OnEnter() override
	{
		DialingInterface::Get().Encode(Stargate->CurrentChevron, Stargate->CurrentSymbol);

		Stargate->ChevronLamp();

		FSM->Transition(StargateDialState::WaitForInput);
	}
};

//------------------------------------------------------------------------------
// FailDial
//------------------------------------------------------------------------------
struct SGS_DialFailure final : public StargateFSMState
{
	void OnEnter() override
	{
		FSM->Transition(StargateDialState::Reset);
	}
};

//------------------------------------------------------------------------------
// Wormhole
//------------------------------------------------------------------------------
struct SGS_Wormhole final : public StargateFSMState
{
	void OnEnter() override
	{
		FSM->Transition(StargateDialState::Reset);
	}
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// StargateFSMState
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void StargateFSMState::Init(StarGate* InStargate)
{
	Stargate = InStargate;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// StarGate
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Stargate SG1 Gate
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void StargateSG1::Initialise(DialHomeDevice* InDHD)
{
	RingMotor.Init();
	ChevronLock.Init();
	ChevronLamps.Init();

	DialFSM.Add<SGS_ResetAndReady>(StargateDialState::Reset, this);
	DialFSM.Add<SGS_WaitingForInput>(StargateDialState::WaitForInput, this);
	DialFSM.Add<SGS_SeekingSymbol>(StargateDialState::SeekToSymbol, this);
	DialFSM.Add<SGS_ChevronLock>(StargateDialState::LockChevron, this);
	DialFSM.Add<SGS_SymbolLamp>(StargateDialState::LampOn, this);
	DialFSM.Add<SGS_DialFailure>(StargateDialState::DialFailure, this);
	DialFSM.Add<SGS_Wormhole>(StargateDialState::Wormhole, this);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void StargateSG1::Update()
{
	DialFSM.Process();
	RingMotor.Update();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void StargateSG1::Reset()
{
	RingMotor.Stop();
	ChevronLamps.Reset();
	ChevronLock.Reset();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void StargateSG1::SeekSymbolWithDirection(bool bClockwise)
{
	RingMotor.SetSpeed(10);
	RingMotor.SetTargetPosition(CurrentSymbol);

	if (bClockwise)
	{
		RingMotor.SetForward();
	}
	else
	{
		RingMotor.SetBackward();
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool StargateSG1::HasFinishedSeek()
{
	return RingMotor.IsAtTargetPosition();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void StargateSG1::RaiseLock()
{
	ChevronLock.ActuateOn();
	ChevronLamps.Illuminate(9);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void StargateSG1::ReleaseLock()
{
	ChevronLock.ActuateOff();
	ChevronLamps.Darken(9);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void StargateSG1::ChevronLamp()
{
	ChevronLamps.Illuminate(CurrentChevron);
}
