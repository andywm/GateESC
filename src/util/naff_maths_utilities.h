/*------------------------------------------------------------------------------
 ()		File: naff_maths_utilities.h
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:
	Some maths functions.
------------------------------------------------------------------------------*/
#pragma once

struct Maths
{
	static constexpr int CalcFactorial(int Input)
	{
		return Input > 0 
			? Input * CalcFactorial(Input-1)
			: 1;
	}
};