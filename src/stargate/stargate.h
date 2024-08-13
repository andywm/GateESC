/*------------------------------------------------------------------------------
 ()		File: stargate.h
 /\		Authour: Andrew Woodward-May
/  \	Date: June 2024	License: MIT

Description:
------------------------------------------------------------------------------*/
#pragma once
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "dhd.h"
#include "util/timer.h"
#include "util/state_machine.h"
#include "motor/motor_controller.h"
#include "chevrons/lamps.h"
#include "chevrons/chevron_lock.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
using uint8 = unsigned char;

enum class StargateDialState
{
	Reset,
	WaitForSymbol,
	SeekToSymbol,
	LockChevron,
	LampOn,
	WaitForActivate,
	DialFailure,
	Wormhole,
	MAX
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Abstract Stargate class presenting a common interface to the state machine,
// thinking about if I ever do a SGA or SGU gate. Probably unncessary.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class StarGate
{
private:
	struct CurrentState
	{
		int Chevron = -1;
		int Symbol = -1;
		bool bIsFinalSymbol = false;
	} InternalState;

public:
	const struct GeneralConfiguration
	{
		bool bHasTopLock = true;
		int SymbolCount = 39;
		int ChevronCount = 9;
		int PointOfOrigin = 39;
		int LockTime = 10000;
	} Config;

	const CurrentState& State = InternalState;
	RudimentaryFSM<StargateDialState> DialFSM;

	virtual void Initialise(DialHomeDevice* InDHD) = 0;
	virtual void Update() = 0;
	virtual void Reset() = 0;
	virtual void SeekSymbolWithDirection(bool bClockwise) = 0;
	virtual bool HasFinishedSeek() = 0;
	virtual void RaiseLock() = 0;
	virtual void ReleaseLock() = 0;
	virtual void ChevronLamp() = 0;

	void UpdateState(int Symbol);
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Stargate FSM Base
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
struct StargateFSMState : public FSMState<StargateDialState> 
{
	void Init(StarGate* InStargate, DialHomeDevice* InDHD);
	StarGate* Stargate;
	DialHomeDevice* DHD;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Stargate SG1 implementation class.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class StargateSG1 final : public StarGate
{
public:
	virtual void Initialise(DialHomeDevice* InDHD) override;
	virtual void Update() override;
	virtual void Reset() override;
	virtual void SeekSymbolWithDirection(bool bClockwise) override;
	virtual bool HasFinishedSeek() override;
	virtual void RaiseLock() override;
	virtual void ReleaseLock() override;
	virtual void ChevronLamp() override;

private:
	MotorController RingMotor;
	ChevronLockController ChevronLock;
	ChevronLampController ChevronLamps;
};
