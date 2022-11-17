/*------------------------------------------------------------------------------
 ()		File: timer.h
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:
	Timer utility class
------------------------------------------------------------------------------*/
#pragma once
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

using TimeMicroSeconds = unsigned long;

class Timer
{
public:
	void Begin();
	void Restart();
	TimeMicroSeconds ReadTime();

private:
	TimeMicroSeconds StartTime{0};
	//unsigned long this_timestamp = 0;
	//unsigned long since = 0;
};	