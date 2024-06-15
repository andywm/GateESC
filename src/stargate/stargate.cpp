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

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Stargate::Loop()
{
	switch(Status)
	{
	case EStatus::Idle:
		return;
	case EStatus::DialingAddress:
		UpdateDialSequence();
		return;
	case EStatus::Wormhole:
		Status = EStatus::Reset;
		return;
	case EStatus::Reset:
		return;
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Stargate::UpdateDialSequence()
{
	switch(DialStatus)
	{
	case EDialStatus::Seek:
		Seek();
		return;
	case EDialStatus::Lock:
		LockChevron();
		return;
	case EDialStatus::NextSeek:
		CurrentChevron++;
		DialStatus = EDialStatus::Seek;
		return;
	}
}

void Stargate::Seek()
{
	static bool bOscilate = false;

	if (AddressBuffer[CurrentChevron] != NullAddress)
	{
		Device::Motor.SetSpeed(10);
		Device::Motor.SetTargetPosition(ChevronAngleTable[AddressBuffer[CurrentChevron]]);
		AddressBuffer[CurrentChevron] = NullAddress;

		if ((bOscilate = !bOscilate) == true)
		{
			Device::Motor.SetForward();
		}
		else
		{
			Device::Motor.SetBackward();
		}
	}

	if (Device::Motor.IsAtTargetPosition())
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