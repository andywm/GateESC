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
#include "motor/speed_control.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

class MotorController
{
private:
	HallSequence Sequence = HallSequence(3);
	Commutator Motor = Commutator(3);
	SpeedControl SpeedControl;

public:
	void Init();
	void Update();
	void Stop();
};