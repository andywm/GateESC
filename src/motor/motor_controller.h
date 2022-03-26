/*------------------------------------------------------------------------------
 ()		File: motor_controller.h
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:
	Interface for the motor.
------------------------------------------------------------------------------*/
#pragma once
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "motor/commutator.h"
#include "motor/hall_sequence.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

class MotorController
{
private:
	HallSequence<3> Sequence;
	Commutator<3> Motor;

public:
	void Init();
	void Update();
	void Stop();
};