/*------------------------------------------------------------------------------
 ()		File: motor_sensors.h
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:

------------------------------------------------------------------------------*/
#pragma once
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "util/naff_maths_utilities.h"
#include "motor/global_motor.h"
#include "framework.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// HallSequence
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class MotorSensors
{
public:
	struct RPM_Data
	{
		int MeasureOnStep{0};
		int StepAngle{0};
		Timer Time;

	}RPM_Calculation;
private:
	int RegisteredStates {0};

	//Stores a hash of the sensor map.
	int HashMapping[GlobalMotor::StepCount];
	int SensorsPins[GlobalMotor::PhaseCount];

	static int PositionTick;

	int LastHash = -1;
	int CountHash = 0;

	int RPM {0};
	int CommutatorStep {-1};
	bool bChanged {false};

private:
	void ReadState();
	static void PositionInterrupt();
	void CalculateRPM();

public:
	//Initialisation
	void DeclareHallPins(int Pin1, int Pin2, int Pin3);
	int DeclareSensorState(int H1, int H2, int H3);
	void DeclarePositionPins(int Pin);

	// Update
	void Sense();

	// State
	bool GetChanged() const;
	int GetStep() const;
	int GetRPM() const;
	int GetAngle() const;
};