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
//#include "oled/oled_display_controller.h"
#include <Arduino.h>
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/* Dialing State Machine
																													OnReset
		[Reset] <-----------------------------------------------------------------------------------------------------------\
		  |	 ^																												 |
		  |	 | OnTimeout					 AtSymb(NoLock)									Failed							 |
  OnReady |	 | OnCancel				  .-------------------------|					 /------------------> [FailDial] --------|
		  v  | 				 		 /						    v					/										/
	 	[WaitSym] -------> [SeekSymbol] -------> [Lock] -----> [Lamp] ---------> [WaitActivate] ---------> [Wormhole] -----/
		  ^		  DialSymb				AtSymb			Light	|	 OnFinalSymb				Success
		  |														|
		  \-----------------------------------------------------/
							OnNextSymb
*/

//------------------------------------------------------------------------------
// Reset
//------------------------------------------------------------------------------
struct SGS_ResetAndReady final : public StargateFSMState
{
	void OnEnter() override 
	{
		Stargate->Reset();
		FSM->Transition(StargateDialState::WaitForSymbol);
	}
};

//------------------------------------------------------------------------------
// WaitSym
//------------------------------------------------------------------------------
struct SGS_WaitingForSymbol final : public StargateFSMState
{
	void OnProcess() override
	{
		const uint8 Symb = DHD->PollAndEat();
		if (Symb != UINT8_MAX)
		{
			Stargate->UpdateState(Symb);
			FSM->Transition(StargateDialState::SeekToSymbol);
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
		Stargate->ChevronLamp();

		FSM->Transition(Stargate->State.bIsFinalSymbol 
			? StargateDialState::WaitForSymbol 
			: StargateDialState::WaitForActivate
			);
	}
};

//------------------------------------------------------------------------------
// WaitActivate
//------------------------------------------------------------------------------
struct SGS_WaitingForActivate final : public StargateFSMState
{
	void OnProcess() override
	{
		//DHD->CheckActivator()
		FSM->Transition(StargateDialState::DialFailure);
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
void StargateFSMState::Init(StarGate* InStargate, DialHomeDevice* InDHD)
{
	Stargate = InStargate;
	DHD = InDHD;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// StarGate
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void StarGate::UpdateState(int Symbol)
{
	InternalState.Chevron++;
	InternalState.Symbol = Symbol;
	InternalState.bIsFinalSymbol = Symbol == Config.PointOfOrigin;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Stargate SG1 Gate
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void StargateSG1::Initialise(DialHomeDevice* InDHD)
{
	DialFSM.Add<SGS_ResetAndReady>(StargateDialState::Reset, this, InDHD);
	DialFSM.Add<SGS_WaitingForSymbol>(StargateDialState::WaitForSymbol, this, InDHD);
	DialFSM.Add<SGS_SeekingSymbol>(StargateDialState::SeekToSymbol, this, InDHD);
	DialFSM.Add<SGS_ChevronLock>(StargateDialState::LockChevron, this, InDHD);
	DialFSM.Add<SGS_SymbolLamp>(StargateDialState::LampOn, this, InDHD);
	DialFSM.Add<SGS_WaitingForActivate>(StargateDialState::WaitForActivate, this, InDHD);
	DialFSM.Add<SGS_DialFailure>(StargateDialState::DialFailure, this, InDHD);
	DialFSM.Add<SGS_Wormhole>(StargateDialState::Wormhole, this, InDHD);
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
	RingMotor.SetTargetPosition(State.Symbol);

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
	ChevronLamps.Illuminate(State.Chevron);
}
