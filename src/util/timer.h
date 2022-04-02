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
class Timer
{
public:
	void Begin();
	void Restart();
	int ReadTime();

private:
	unsigned long last_timestamp = 0;
	unsigned long this_timestamp = 0;
	unsigned long since = 0;
};	