/*------------------------------------------------------------------------------
 ()		File: framework.cpp
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:
	Base Framework
------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include <Arduino.h>
#include "framework.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Framework
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Framework::Timings Framework::Timing;
DebugSystem Framework::Debug(20,4);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/*
void Framework::TickTimers()
{
	Timing.this_timestamp = micros();
	if( Timing.this_timestamp - Timing.last_timestamp > 0 )
	{
		Timing.since = Timing.this_timestamp - Timing.last_timestamp;
	}
	else if ( Timing.this_timestamp - Timing.last_timestamp > INT64_MAX )
	{
		Timing.since = UINT64_MAX - Timing.last_timestamp + Timing.this_timestamp;
	}
	Timing.last_timestamp = Timing.this_timestamp;	
}
*/

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Framework::Assert(bool bCondition, const char* Msg/*=nullptr*/)
{
	if( bCondition == false )
	{
		//todo, serial write message if available.
		Halt();
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Framework::Assert0(bool bCondition, const char* Msg/*=nullptr*/)
{
	if( bCondition == true )
	{
		//todo, serial write message if available.
		Halt();
	}
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void Framework::Halt()
{
	//todo, write all registers to zero.
	abort();
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void Framework::PinMode(int Pin, EPinMode Mode)
{
	static int ModeSelect[EPinMode::EModeMax] = 
	{
		INPUT,
		OUTPUT
	};

	pinMode(Pin, ModeSelect[Mode]);
}


//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void Framework::DigitalWrite(int Pin, bool bState)
{
	digitalWrite(Pin, bState);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
int Framework::DigitalRead(int Pin)
{
	return static_cast<int>(digitalRead(Pin));
}