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
//Framework::Timings Framework::Timing;
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
		Serial.println(Msg);
		delay(1000);
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
		Serial.println(Msg);
		delay(1000);
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
		OUTPUT,
		INPUT, //INPUT_PULLDOWN
	};

	pinMode(Pin, ModeSelect[Mode]);
}


//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void Framework::DigitalWrite(int Pin, bool bState)
{
	//Framework::Message("Pin=%d - Value=%d", Pin, (int)bState);
	digitalWrite(Pin, bState);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
int Framework::DigitalRead(int Pin)
{
	return static_cast<int>(digitalRead(Pin));
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void Framework::AnalogWrite(int Pin, int8_t PwmValue)
{
	//Framework::Message("Pin=%d - Value=%d", Pin, (int)bState);
	analogWrite(Pin, PwmValue);
}
