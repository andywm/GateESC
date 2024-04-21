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
	int DebugSensorPins[GlobalMotor::PhaseCount] = {0};

private:
	struct TachometerData
	{
		Timer MeasurementTimer;

		//For the moment, and for my sanity. I'm working in degrees, but the native resolution of the sensor is 640 increments.
		//I've just pretended for the moment that it's 360, and am taking apporximate values, but I should think of a way to work
		//with the native units.
		int Angle {0};
		int AngleInDegrees {0};
		int RPM {0};

		int RawSensorAngle {0};
		int TicksSinceLastSpeedMeasurement;
		float RpmMeasurementDt {0.0f};
	}
	Tachometer;

	int CommutatorStep {-1};

	//Stores a hash of the sensor map.
	int HashMapping[GlobalMotor::StepCount];
	int SensorsPins[GlobalMotor::PhaseCount];
	int RegisteredStates {0};

public:
	//Initialisation
	void DeclareHallPins(int Pin1, int Pin2, int Pin3);
	void DeclareQuadraturePins(int PinA, int PinB);
	int DeclareSensorState(int H1, int H2, int H3);
	void ResetSpeedMeasurement();

	// Update
	void SensePosition();
	void SenseCommutationStep();
	bool SenseSpeed();

	// State
	int GetStep() const;
	int GetAngle() const;
	int GetRPM() const;
	float GetRpmDeltaTime() const;
};