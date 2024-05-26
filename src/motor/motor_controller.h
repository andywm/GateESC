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

constexpr int NO_TARGET_ANGLE = -1;

class MotorController
{
private:
	MotorSensors Sensors;
	MotorDriver Motor;
	PIDController<float, int> SpeedPID;
	PIDController<float, int> DistancePID;
	
	uint8_t TargetRPM{0};
	int TargetAngle {NO_TARGET_ANGLE};
	bool AtTarget = false;
	bool bDoEffortCalculation = false;

	enum class EControlMode {Stopped, Moving, Braking};
	EControlMode ControlMode {EControlMode::Stopped};

public:
	void Init();
	void Update();
	void Stop();
	void SetForward();
	void SetBackward();
	void SetSpeed(uint8_t RPM);
	void SetTargetPosition(int Angle);
	bool IsAtTargetPosition();

	void UpdateDebug();

private:
	void Moving();
	void Braking();
};