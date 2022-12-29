/*------------------------------------------------------------------------------
 ()		File: timer.cpp
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:
	Timer utility class
------------------------------------------------------------------------------*/
#include "util/timer.h"
#include <Arduino.h>
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

// If the difference is over half of uintmax64, then we've rolled over.
constexpr TimeMicroSeconds UnreasonableDelta = static_cast<TimeMicroSeconds>(INT64_MAX);

void Timer::Begin()
{
	Restart();
}

void Timer::Restart()
{
	StartTime = micros();
}

TimeMicroSeconds Timer::ReadTime()
{
	TimeMicroSeconds CurrentTime = micros();
	TimeMicroSeconds Since = 0ul;

	if( CurrentTime - StartTime < UnreasonableDelta )
	{
		Since = CurrentTime - StartTime;
	}
	else
	{
		Since = UINT64_MAX - StartTime + CurrentTime;
	}

	return Since;
}
