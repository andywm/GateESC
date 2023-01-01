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
#include "motor/motor_driver.h"
#include "motor/motor_sensors.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

class MotorController
{
private:
	MotorSensors Sensors;
	MotorDriver Motor;

public:
	void Init();
	void Update();
	void Stop();
	void SetForward();
	void SetBackward();
	void SetSpeed() {};
	void SetTargetPosition() {};
};