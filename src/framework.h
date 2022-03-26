/*------------------------------------------------------------------------------
 ()		File: framework.h
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:
	Base Framework
------------------------------------------------------------------------------*/
#pragma once
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include <stdint.h>
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Framework
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class Framework
{
public:
	static const struct PinData
	{
		uint8_t ESC_SOURCE_A	= 2;
		uint8_t ESC_SINK_A		= 3;
		uint8_t ESC_SOURCE_B	= 4;
		uint8_t ESC_SINK_B		= 5;
		uint8_t ESC_SOURCE_C	= 6;
		uint8_t ESC_SINK_C		= 7;
		uint8_t ESC_HALL_A		= 8;
		uint8_t ESC_HALL_B		= 9;
		uint8_t ESC_HALL_C		= 10;
		uint8_t dir_output		= 11;
		uint8_t dir_input		= 12;
	} Pinout;

private:
	static struct Timings
	{
		unsigned long last_timestamp = 0;
		unsigned long this_timestamp = 0;
		unsigned long since = 0;
	} Timing;

private:
	void Init();
	void TickTimers();
	void Tick();

public:
	void Run();

	// Halting Assert - asserts if the condition is false
	static void Assert(bool bCondition, const char* Msg=nullptr);

	// Halting Assert - asserts if the condition is true
	static void Assert0(bool bCondition, const char* Msg=nullptr);

	/// Put the microcontroller into an infinite loop, clear all interrupts.
	/// use for safety if an unrecoverrable condition is detected.
	static void Halt();

	/// Wrapper function for Arduino DigitalWrite, will use FastDigitalWrite
	/// if available.
	static void DigitalWrite(int Pin, bool bState);
};