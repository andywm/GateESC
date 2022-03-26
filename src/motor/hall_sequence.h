/*------------------------------------------------------------------------------
 ()		File: hall_sequence.h
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:

------------------------------------------------------------------------------*/
#pragma once
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "util/naff_maths_utilities.h"
#include "framework.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// HallSequence
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class HallSequence
{
private:
	const int PhaseCount;
	const int Steps; //= Maths::CalcFactorial(PhaseCount);

	/// Step I, ..., Step !N
	/// layout i.e..  {1={1,..,.n}, ..., !N={1,...,n}}
	/// each segment represents the hall state for a step.
	//int Mapping[PhaseCount][Steps];

	//Stores a hash of the sensor map.
	int* HashMapping;//[Steps];
	int* SensorsPins;//[PhaseCount];
	int LastState = -1;
	int CurrentState = -1;

private:
	template<typename ...PackedState>
	void Bind_Arg(int Step, int SubIndex, int Param, PackedState...Args);

	template<typename ...PackedState>
	void Bind_Arg(int Step, int SubIndex);

public:
	HallSequence( int Phases );

	template<typename ...PackedState>
	void Bind(int Step, PackedState...Args)
	{	
		Framework::Assert( (int)sizeof...(Args) > Steps, "Too Many Steps" );
		Framework::Assert( sizeof...(Args) == 0, "Too Few Steps" );

		Bind_Arg(Step, 0, Args...);
	}

	void DeclarePinsForSensor(int SensorID, int Pin);
	int ReadState(int& NextStep);
};