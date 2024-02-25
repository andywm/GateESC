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
#ifndef P_FETS_LOGICAL_INVERT
#define P_FETS_LOGICAL_INVERT 0
#endif

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
		constexpr static uint8_t ESC_SINK_A		= 2;
		constexpr static uint8_t ESC_SOURCE_A	= 3;
		constexpr static uint8_t ESC_SINK_B		= 4;
		constexpr static uint8_t ESC_SOURCE_B	= 5;
		constexpr static uint8_t ESC_SINK_C		= 6;
		constexpr static uint8_t ESC_SOURCE_C	= 7;
		constexpr static uint8_t POS_SENSE		= 8;
		constexpr static uint8_t ESC_HALL_1		= 9;
		constexpr static uint8_t ESC_HALL_2		= 10;
		constexpr static uint8_t ESC_HALL_3		= 11;
		constexpr static uint8_t I2C0_SDA		= 12;
		constexpr static uint8_t I2C0_SCL		= 13;
	};

	struct Signal
	{ 
		constexpr static int NFetOpen = 0; 
		constexpr static int NFetClosed = 1; 

#if (P_FETS_LOGICAL_INVERT)
		constexpr static int PFetOpen = INT16_MAX;
		constexpr static int PFetClosed = 0; 
#else
		constexpr static int PFetOpen = 0;
		constexpr static int PFetClosed = INT16_MAX; 
#endif

		static int PFetInterpolate(int Pwm)
		{
#if (P_FETS_LOGICAL_INVERT)
			return UINT8_MAX - Pwm;
#endif
			return Pwm;
		}
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
	static void DigitalWrite(int Pin, int State);

	/// Wrapper function for Arduino DigitalRead, will use FastDigitalRead
	/// if available.
	static int DigitalRead(int Pin);

	/// Wrapper function for Arduino AnalogWrite, for interface consistency.
	static void AnalogWrite(int Pin, int PwmValue);
};