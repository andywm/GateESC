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
#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "util/naff_string_utilities.h"
#include "util/timer.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define _USING_DEBUG_DISPLAY

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// Debug Page Base Class. Override this with specific debug information.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
using TextBuffer = char[4][21];
struct DebugPage
{
	TextBuffer Buffer;

	DebugPage()
	{
		memset(&Buffer[0][0], 0, 20);
		memset(&Buffer[1][0], 0, 20);
		memset(&Buffer[2][0], 0, 20);
		memset(&Buffer[3][0], 0, 20);
	}
	virtual ~DebugPage() = default;
	virtual bool Update() = 0;

protected:
	template<typename ...PackedArgs>
	void SetLine(uint8_t Line, const char* Msg, PackedArgs... Args)
	{
		char* ThisLine = &Buffer[Line][0];
		NaffStringUtility::Format(ThisLine, Msg, Args...);
	}

	bool Dirty {true};
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
	static constexpr int MaxPages = 10;
private:
	const int LineCount;

  	TwoWire I2CBus;
	Adafruit_SSD1306 Display;

	DebugPage* Pages[MaxPages] = {nullptr};
	TextBuffer ScreenBuffer;
	uint8_t CurrentPage = -1;
	uint8_t NextFreePage = 0;
	Timer LimitTimer;

	struct Metadata_LCD
	{
		enum class EState
		{
			Idle,
			RateLimited,
			WritingBuffer, 
			ClearBuffer, 
		} State {EState::ClearBuffer};
		uint32_t UpdateMask[4] = {0,0,0,0};
		uint8_t Page = -1;
		uint8_t CurrentLine {0};
		uint8_t CurrentChar {0};


	}LCDMetadata;

public:
	DebugSystem();

	TextBuffer* UpdateBackBuffer();

	void AddPage(DebugPage& Page);
	void Init();
	void SetPage(int Page);
	void Process();

private:
	bool IsRateLimited();
	void SetRatedLimited();
	bool WriteToScreenBuffer();
	bool PrimeScreenBuffer();
};
