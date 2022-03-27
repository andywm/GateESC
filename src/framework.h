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
#include "debug.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#define DISABLE_MOTOR_EXEC

/// Pin Modes
enum EPinMode
{
	EInput,
	EOutput,
	//pullup, pulldowns?
	EModeMax,
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Framework
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class Framework
{
public:
	struct Pinout
	{
		constexpr static uint8_t ESC_SOURCE_A	= 2;
		constexpr static uint8_t ESC_SINK_A		= 3;
		constexpr static uint8_t ESC_SOURCE_B	= 4;
		constexpr static uint8_t ESC_SINK_B		= 5;
		constexpr static uint8_t ESC_SOURCE_C	= 6;
		constexpr static uint8_t ESC_SINK_C		= 7;
		constexpr static uint8_t ESC_HALL_A		= 8;
		constexpr static uint8_t ESC_HALL_B		= 9;
		constexpr static uint8_t ESC_HALL_C		= 10;
		constexpr static uint8_t dir_output		= 11;
		constexpr static uint8_t dir_input		= 12;
	};

private:
	static struct Timings
	{
		unsigned long last_timestamp = 0;
		unsigned long this_timestamp = 0;
		unsigned long since = 0;
	} Timing;


public:
	static DebugSystem Debug;

	// Halting Assert - asserts if the condition is false
	static void Assert(bool bCondition, const char* Msg=nullptr);

	// Halting Assert - asserts if the condition is true
	static void Assert0(bool bCondition, const char* Msg=nullptr);

	/// Put the microcontroller into an infinite loop, clear all interrupts.
	/// use for safety if an unrecoverrable condition is detected.
	static void Halt();

	/// Wrapper function for Arduino DigitalWrite, will use FastDigitalWrite
	/// if available.
	static void PinMode(int Pin, EPinMode Mode);

	/// Wrapper function for Arduino DigitalWrite, will use FastDigitalWrite
	/// if available.
	static void DigitalWrite(int Pin, bool bState);

	/// Wrapper function for Arduino DigitalRead, will use FastDigitalRead
	/// if available.
	static int DigitalRead(int Pin);
};