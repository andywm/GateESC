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
#include <Arduino.h>
#include "debug.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//#define DISABLE_MOTOR_EXEC

/// Pin Modes
enum EPinMode
{
	EInput,
	EOutput,
	EInputPullDown,
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
#if defined(IS_NANO_BUILD)
		constexpr static uint8_t ESC_SOURCE_A	= 11;
		constexpr static uint8_t ESC_SOURCE_B	= 10;
		constexpr static uint8_t ESC_SOURCE_C	= 9;
		constexpr static uint8_t ESC_SINK_A		= 8;
		constexpr static uint8_t ESC_SINK_B		= 7;
		constexpr static uint8_t ESC_SINK_C		= 6;
		constexpr static uint8_t ESC_HALL_1		= 4;
		constexpr static uint8_t ESC_HALL_2		= 3;
		constexpr static uint8_t ESC_HALL_3		= 2;
#elif defined(IS_BLUEPILL_BUILD)
		constexpr static uint8_t ESC_SINK_A		= PA8;
		constexpr static uint8_t ESC_SOURCE_A	= PA9;
		constexpr static uint8_t ESC_SINK_B		= PB12;
		constexpr static uint8_t ESC_SOURCE_B	= PB13;
		constexpr static uint8_t ESC_SINK_C		= PB14;
		constexpr static uint8_t ESC_SOURCE_C	= PB15;
		constexpr static uint8_t ESC_HALL_A		= PA15;
		constexpr static uint8_t ESC_HALL_B		= PB3;
		constexpr static uint8_t ESC_HALL_C		= PB4;
		constexpr static uint8_t dir_output		= PA15;
		constexpr static uint8_t dir_input		= PB3;
#endif
	};

private:
	//static struct Timings
	//{
	//	unsigned long last_timestamp = 0;
	//	unsigned long this_timestamp = 0;
	//	unsigned long since = 0;
	//} Timing;


public:
	static DebugSystem Debug;

	template<typename... PackedArgs>
	static void Message(const char* Msg, PackedArgs...Args)
	{
		char Buffer[32];
		snprintf(Buffer,32,Msg, Args...);
		Serial.println(Buffer);
	}

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

	/// Wrapper function for Arduino AnalogWrite, for interface consistency.
	static void AnalogWrite(int Pin, int8_t PwmValue);
};