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
#include "util/pid.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

class MotorController
{
private:
	MotorSensors Sensors;
	MotorDriver Motor;
	PIDController<float, int> SpeedPID;
	
	uint8_t TargetRPM{0};

public:
	void Init();
	void Update();
	void Stop();
	void SetForward();
	void SetBackward();
	void SetSpeed(uint8_t RPM);
	void SetTargetPosition() {};
};