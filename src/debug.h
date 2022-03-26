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
#include "core/framework_system.h"
#include "util/naff_string_utilities.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

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

struct DemoPage : public DebugDisplayPage
{
	virtual void Update() override
	{
		//______L__|12345678901234567890|
		SetLine(0, "Measured RPM ###    ", rpm.Value);
		SetLine(1, "Measured RPM ###    ", rpm.Value);
		SetLine(2, "Measured RPM ###    ", rpm.Value);
		SetLine(3, "Measured RPM ###    ", rpm.Value);
	}
	DebugValue<int> rpm = {Dirty};
};

//class Debug final : public FameworkSystem
//{
//	virtual void Tick() override
//	{
//		DebugDisplayPage page;
//		DebugDisplayPage::FormatLine(1, )
//		page.Line1
//
//	}
//};