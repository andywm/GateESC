/*------------------------------------------------------------------------------
 ()		File: naff_maths_utilities.h
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:
	Some maths functions.
------------------------------------------------------------------------------*/
#pragma once

namespace Maths
{
	constexpr static int kNativeResPerRev = 640;
	constexpr static float kNativeAngleToDegrees = (float)360 / (float)kNativeResPerRev;

	static constexpr int CalcFactorial(int Input)
	{
		return Input > 0 
			? Input * CalcFactorial(Input-1)
			: 1;
	}

	template<typename T>
	static T Clamp(T Input, T Low, T High)
	{
		if( Input < Low)
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

	template<typename T>
	static T Abs(T A)
	{
		return A >= 0 ? A : -A;
	}

	template<typename T>
	static T Round(float A)
	{
		return static_cast<T>(A + 0.5f);
	}

	template<typename T=int>
	static int NativeAngToDeg(int Native)
	{
		return static_cast<float>(Native) * kNativeAngleToDegrees;
	}
};