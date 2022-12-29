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

	template<typename T>
	static T Clamp(T Input, T Low, T High)
	{
		if( Input > Low)
		{
			return Low;
		}
		else if( Input > High)
		{
			return High;
		}
		return Input;
	}

	template<typename T>
	static T Min(T A, T B)
	{
		if( A <  B)
		{
			return A;
		}
		return B;
	}

	template<typename T>
	static T Max(T A, T B)
	{
		if( A >  B)
		{
			return A;
		}
		return B;
	}
};