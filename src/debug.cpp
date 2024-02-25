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
#include <Arduino.h>
#include "framework.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#define USE_SERIAL_DEBUG 0

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
DebugSystem::DebugSystem()
	: LineCount(4)
	, I2CBus(Framework::Pinout::I2C0_SDA, Framework::Pinout::I2C0_SCL)
	, Display(128, 32, &I2CBus, -1)
{
	
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DebugSystem::AddPage(DebugPage& Page )
{
	Pages[NextFreePage++] = &Page;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DebugSystem::Init()
{
	Display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
	Display.clearDisplay();
	Display.display();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DebugSystem::SetPage(int Page)
{
	CurrentPage = Page;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DebugSystem::Process()
{
	//If updates are rate limited, bail.
	switch(LCDMetadata.State)
	{
	case Metadata_LCD::EState::RateLimited:
		{
			if( IsRateLimited() )
			{
				return;
			}

#if USE_SERIAL_DEBUG
			Framework::Message("Debug - Was Rated Limited...");
#endif //USE_SERIAL_DEBUG
			LCDMetadata.State = Metadata_LCD::EState::Idle;
			//explit fallthrough to Idle.
		}
	case Metadata_LCD::EState::Idle:
		{
			if(!PrimeScreenBuffer())
			{
				return;
			}
#if USE_SERIAL_DEBUG
			Framework::Message("Debug - Was Idle");
#endif //USE_SERIAL_DEBUG
			LCDMetadata.State = Metadata_LCD::EState::WritingBuffer;
			//explit fallthrough to WritingBuffer
		}

	case Metadata_LCD::EState::WritingBuffer:
		{
			if(WriteToScreenBuffer())
			{
				return;
			}

			//done writing, lockout screen for 250ms.
			SetRatedLimited();
			break;
		}
	case Metadata_LCD::EState::ClearBuffer:
		{
#if USE_SERIAL_DEBUG
			Framework::Message("Debug - Clear...");
#endif //USE_SERIAL_DEBUG

			//clear is expensive, ~3ms, so swtich to a rate limit.
			Display.clearDisplay();
  			Display.setTextColor(WHITE);
  			Display.setTextSize(1);
 			Display.setCursor(0, 0);

			//done writing, lockout screen for 250ms.
			SetRatedLimited();
			break;
		}
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool DebugSystem::IsRateLimited()
{
	//return LimitTimer.ReadTime() < 333333;
	return LimitTimer.ReadTime() < 200000;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DebugSystem::SetRatedLimited()
{
#if USE_SERIAL_DEBUG
	Framework::Message("Debug - Lockout");
#endif //USE_SERIAL_DEBUG

	LCDMetadata.State = Metadata_LCD::EState::RateLimited;
	LimitTimer.Restart();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool DebugSystem::PrimeScreenBuffer()
{
	if( CurrentPage != UINT8_MAX && !Pages[CurrentPage]->Update() )
	{
		return false;
	}
	else if( CurrentPage == UINT8_MAX && LCDMetadata.Page != UINT8_MAX )
	{
		//Clear
		LCDMetadata.Page = UINT8_MAX;
		LCDMetadata.State = Metadata_LCD::EState::ClearBuffer;
		return false;
	}

	LCDMetadata.Page = CurrentPage;
	TextBuffer& Buffer = Pages[CurrentPage]->Buffer;

	//Create difference mask
	for(uint8_t LineIdx = 0; LineIdx<4; ++LineIdx)
	{
		for( uint8_t CharIdx = 0; CharIdx<21; ++CharIdx )
		{
			const char NewChar = Buffer[LineIdx][CharIdx];
			const char OldChar = ScreenBuffer[LineIdx][CharIdx];
			const uint32_t DiffBit = static_cast<uint32_t>(NewChar != OldChar) << CharIdx;
			LCDMetadata.UpdateMask[LineIdx] |= DiffBit;
		}
	}

	//Reset writeout progress.
	LCDMetadata.CurrentLine = 0;
	LCDMetadata.CurrentChar = 0;

	//Prime screen buffer.
	memcpy(&ScreenBuffer, &Buffer, 21*4);

#if USE_SERIAL_DEBUG
	Framework::Message("1: %s", &ScreenBuffer[0][0]);
	Framework::Message("2: %s", &ScreenBuffer[1][0]);
	Framework::Message("3: %s", &ScreenBuffer[2][0]);
	Framework::Message("4: %s", &ScreenBuffer[3][0]);
#endif //USE_SERIAL_DEBUG

	return true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool DebugSystem::WriteToScreenBuffer()
{
	uint8_t& LineIdx = LCDMetadata.CurrentLine;
	uint8_t& CharIdx = LCDMetadata.CurrentChar;

	auto NextLine = [&]()
	{
		++LineIdx;
		CharIdx = 0;
	};

	bool WroteChar = false;

	for(; LineIdx<4; NextLine())
	{
		uint32_t& Update = LCDMetadata.UpdateMask[LineIdx];
		if(Update == 0)
		{
			continue;
		}

		for( ; CharIdx<21; ++CharIdx )
		{			
			uint32_t CharMask = 0x1<<CharIdx;
			if(Update & CharMask)
			{
				Update &= ~CharMask;
				
#if USE_SERIAL_DEBUG
				Framework::Message("Writing Pos L=%d, C=%d: %c", LineIdx, CharIdx, ScreenBuffer[LineIdx][CharIdx]);
#endif //USE_SERIAL_DEBUG

 				Display.setCursor(CharIdx * 6, LineIdx * 8);
				Display.setTextColor(WHITE, BLACK);
				Display.print(' ');
				//Single char, takes about 1.5ms...
 				Display.setCursor(CharIdx * 6, LineIdx * 8);
				Display.print(ScreenBuffer[LineIdx][CharIdx]);
				Display.display();

				WroteChar = true;
				break;
			}
		}

		if(WroteChar)
		{
			return true;
		}
	}

	return false;
}
