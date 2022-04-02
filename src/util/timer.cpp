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

void Timer::Begin()
{
	this_timestamp = micros();
	if( this_timestamp - last_timestamp > 0 )
	{
		since = this_timestamp - last_timestamp;
	}
	else if ( this_timestamp - last_timestamp > INT64_MAX )
	{
		since = UINT64_MAX - last_timestamp + this_timestamp;
	}
	last_timestamp = this_timestamp;	
}

void Timer::Restart()
{

}

int Timer::ReadTime()
{

}
