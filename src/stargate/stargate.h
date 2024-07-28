/*------------------------------------------------------------------------------
 ()		File: stargate.h
 /\		Authour: Andrew Woodward-May
/  \	Date: June 2024	License: MIT

Description:
------------------------------------------------------------------------------*/
#pragma once
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "dial.h"
#include "stargate_state.h"
#include "util/timer.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
using uint8 = unsigned char;
static constexpr uint8 NullAddress = 0xFF;
static constexpr uint8 NullPosition = 0xFF;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Motor One
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class Stargate
{
public:
	void InitControllers();
	void Loop();
	void Dial(int Symbol);
	void Establish();

private:
	void UpdateDialSequence();
	void Seek();
	void LockChevron();

private: 
	enum class EDialStatus {ConsumeInput, Seek, Lock, NextSeek};

	Timer ChevronTimer;

	EStatus Status {EStatus::Idle};
	EDialStatus DialStatus {EDialStatus::Seek};

	uint8 AddressBuffer[9] = {NullAddress};
	uint8 CurrentChevron = 0;
	bool bEstablish = false;

private:
	DialInputController DHD;
	MotorController RingMotor;
	ChevronLockController ChevronLock;
	ChevronLampController ChevronLamps;
};