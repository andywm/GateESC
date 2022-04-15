/*------------------------------------------------------------------------------
 ()		File: sequence_state.h
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:
	Generic Data Structure for feedback to motor controller about stepping 
    and rpm tick.
------------------------------------------------------------------------------*/
#pragma once

struct SequenceState
{
    bool UpdateCommutation = false;
	bool Tick = false; 
	int NextStep = -1;
	int CurrentRPM = 0;
};