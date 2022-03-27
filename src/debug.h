/*------------------------------------------------------------------------------
 ()		File: debug.h
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:
	Debugging Subsystem to write to a [2|4]x20 display
------------------------------------------------------------------------------*/
#pragma once
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include <stdint.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include "util/naff_string_utilities.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define _USING_DEBUG_DISPLAY

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// Debug Page Base Class. Override this with specific debug information.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
struct DebugDisplayPage
{
	bool Dirty = true;
	char LineData[4][21] = {{0}};

	template<typename ...PackedArgs>
	void SetLine(uint8_t Line, const char* Msg, PackedArgs... Args)
	{
		char* ThisLine = &LineData[Line][0];
		NaffStringUtility::Format(ThisLine, Msg, Args...);
	}

	virtual void Update() = 0;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// Helper to allow debug values to only trigger an update when they're chanegd.
/// requires a reference to the base DebugDisplayPage::Dirty bool
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template<typename T>
struct DebugValue
{
	DebugValue(bool&D) : Dirty(D) {};
	DebugValue(bool&D, const T& Initial) : Dirty(D), Value(Initial) {};

	T operator=(const T&NewValue)
	{
		Dirty |= (Value != NewValue);
		Value = NewValue;

		return Value;
	}

	bool& Dirty;
	T Value;
};

class DebugSystem
{
	static constexpr int DEFAULT_LCD_I2C_ADDR = 0x27;
	static constexpr int MaxPages = 10;
private:
	const int LineCount;

  	TwoWire I2CBus;
  	LiquidCrystal_I2C DisplayLCD;

	DebugDisplayPage* Pages[MaxPages] = {nullptr};
	uint8_t CurrentPage = -1;
	uint8_t NextFreePage = 0;

public:
	DebugSystem(int Width, int Height, int DeviceAddress=DEFAULT_LCD_I2C_ADDR);
	void AddPage(DebugDisplayPage& Page );
	void Init();
	void SetPage(int Page);
	void RenderCurrentPage();
};
