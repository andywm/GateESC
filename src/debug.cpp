/*------------------------------------------------------------------------------
 ()		File: debug.cpp
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:
	Debugging Subsystem to write to a [2|4]x20 display
------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "debug.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
DebugSystem::DebugSystem(int Width, int Height, int DeviceAddress/*=DEFAULT_LCD_I2C_ADDR*/)
	: LineCount(Height)
	, DisplayLCD(DeviceAddress, Width, Height, LCD_5x8DOTS)
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DebugSystem::AddPage(DebugDisplayPage& Page )
{
	Pages[NextFreePage++] = &Page;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DebugSystem::Init()
{
	DisplayLCD.begin();
	DisplayLCD.backlight();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DebugSystem::SetPage(int Page)
{
	CurrentPage = Page;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DebugSystem::RenderCurrentPage()
{
	DebugDisplayPage* Page = nullptr;

	if(CurrentPage < UINT8_MAX 
		&& (Page = Pages[CurrentPage]) != nullptr 
		&& Page->Dirty == true 
		)
	{
		Page->Update();
		
		DisplayLCD.clear();
		for( int LineID = 0; LineID < LineCount; ++LineID )
		{
			DisplayLCD.setCursor(0,LineID);
			DisplayLCD.printstr(&Page->LineData[LineID][0]);	
		}
		Page->Dirty = false;
	}
}