/*------------------------------------------------------------------------------
 ()		File: debug.h
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:
	Debugging Subsystem to write to a 4x20 display
------------------------------------------------------------------------------*/
#pragma once
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "util/naff_string_utilities.h"
#include <stdint.h>
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
	char LineData[4][20];

	template<typename ...PackedArgs>
	void SetLine(uint8_t Line, const char* Msg, PackedArgs... Args)
	{
		//char test = LineData[0][0];
		NaffStringUtility::Format(&LineData[20][Line], Msg, Args...);
	}

	virtual void Update() = 0;
	void Render();
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

	T& operator=(T&NewValue)
	{
		Dirty |= Value != NewValue;
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
	DebugDisplayPage* Pages[MaxPages] = {nullptr};
	int CurrentPage = 0;
	int NextFreePage = 0;

public:
	void AddPage(DebugDisplayPage& Page );
	void Init();
	void SetPage(int Page);
	void RenderCurrentPage();
};
