/*------------------------------------------------------------------------------
 ()		File: stargate.cpp
 /\		Authour: Andrew Woodward-May
/  \	Date: June 2024	License: MIT

Description:
------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "stargate/stargate.h"
//#include "devices.h"
#include "chevron_angle_table.h"
//#include "framework.h"
#include "motor/motor_controller.h"
#include "chevrons/chevron_lock.h"
#include "chevrons/lamps.h"
//#include "oled/oled_display_controller.h"
#include <Arduino.h>
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Stargate
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Stargate::InitControllers()
{
	
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Stargate::Loop()
{
	switch(Status)
	{
	case EStargateState::Idle:
		if (DHD.PollActivity())
		{
			BeginDialingSequence();
		}
		return;
	case EStargateState::DialingAddress:
		UpdateDialSequence();
		return;
	case EStargateState::Wormhole:
		Status = EStatus::Reset;
		return;
	case EStargateState::Reset:
		return;
	}
}

void Stargate::BeginDialingSequence()
{
	Status = EStargateState::DialingAddress;
	DialStatus = EDialStatus::Seek;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Stargate::UpdateDialSequence()
{
	//EDialingSymbolState::New
	//EDialingSymbolState::Seeking
	//EDialingSymbolState::ChevronLock
	switch(DialStatus)
	{
	case EDialStatus::Seek:
		Seek();
		return;
	case EDialStatus::Lock:
		LockChevron();
		DHD.Confirm(CurrentChevron);
		return;
	case EDialStatus::NextSeek:
		CurrentChevron++;
		DialStatus = EDialStatus::Seek;
		return;
	}
}

void Stargate::OnNewSymbol()
{
	static bool bOscilate = false;

	if (AddressBuffer[CurrentChevron] != NullAddress)
	{
		RingMotor.SetSpeed(10);
		RingMotor.SetTargetPosition(ChevronAngleTable[AddressBuffer[CurrentChevron]]);
		AddressBuffer[CurrentChevron] = NullAddress;

		if ((bOscilate = !bOscilate) == true)
		{
			RingMotor.SetForward();
		}
		else
		{
			RingMotor.SetBackward();
		}
	}

	Seek();
}

void Stargate::Seek()
{
	static bool bOscilate = false;

	if (AddressBuffer[CurrentChevron] != NullAddress)
	{
		RingMotor.SetSpeed(10);
		RingMotor.SetTargetPosition(ChevronAngleTable[AddressBuffer[CurrentChevron]]);
		AddressBuffer[CurrentChevron] = NullAddress;

		if ((bOscilate = !bOscilate) == true)
		{
			RingMotor.SetForward();
		}
		else
		{
			RingMotor.SetBackward();
		}
	}

	if (RingMotor.IsAtTargetPosition())
	{
		DialStatus = EDialStatus::Lock;
		ChevronTimer.Restart();
		Device::ChevronLamps.Illuminate(0);
		Device::ChevronLock.ActuateOn();
	}
}

void Stargate::LockChevron()
{
	if(ChevronTimer.ReadTime() > 1000000)
	{
		Device::ChevronLamps.Darken(0);
		Device::ChevronLamps.Illuminate(CurrentChevron);
		Device::ChevronLock.ActuateOff();

		DialStatus = EDialStatus::NextSeek;
	}
}