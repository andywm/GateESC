/*------------------------------------------------------------------------------
 ()		File: motor_controller.cpp
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:
	Interface for the motor.
------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "motor/motor_controller.h"
#include "framework.h"
#include "debug.h"
#include <Arduino.h>
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Debug Info
//------------------------------------------------------------------------------
struct MotorControlPage : public DebugDisplayPage
{
	virtual void Update() override
	{
		//______L__|12345678901234567890|
		SetLine(0, "Step #              ", step.Value);
		SetLine(1, "Measured RPM ###    ", rpm.Value);
		SetLine(2, "                    ");
		SetLine(3, "                    ");
	}
	DebugValue<int> rpm = {Dirty};
	DebugValue<int> step = {Dirty};
} ControllerDebug;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorController::Init()
{
	// Create IO Bindings for ESC Switcher Pins
	Motor.DeclarePinsForPhase(0, Framework::Pinout::ESC_SOURCE_A, Framework::Pinout::ESC_SINK_A);
	Motor.DeclarePinsForPhase(1, Framework::Pinout::ESC_SOURCE_B, Framework::Pinout::ESC_SINK_B);
	Motor.DeclarePinsForPhase(2, Framework::Pinout::ESC_SOURCE_C, Framework::Pinout::ESC_SINK_C);

	Serial.println("Motor Pins... Done");
	delay(100);

	// Create IO Bindings for Hall Sensor Pins
	Sequence.DeclarePinsForSensor(0, Framework::Pinout::ESC_HALL_A);
	Sequence.DeclarePinsForSensor(1, Framework::Pinout::ESC_HALL_B);
	Sequence.DeclarePinsForSensor(2, Framework::Pinout::ESC_HALL_C);

	Serial.println("Hall Pins... Done");
	delay(100);

	// Create Commutation Table
	const int I =	Motor.DeclareCommutationStep(0, ESource, ESink, EFloat);
	const int II =	Motor.DeclareCommutationStep(1, ESource, EFloat, ESink);
	const int III =	Motor.DeclareCommutationStep(2, EFloat, ESource, ESink);
	const int IV =	Motor.DeclareCommutationStep(3, ESink, ESource, EFloat);
	const int V =	Motor.DeclareCommutationStep(4, ESink, EFloat, ESource);
	const int VI =	Motor.DeclareCommutationStep(5, EFloat, ESink, ESource);

	Serial.println("Commutator... Done");
	delay(100);

	// Bind Hall Sequence to Steps.
	Sequence.Bind(I, 	1,0,1);
	Sequence.Bind(II, 	1,0,0);
	Sequence.Bind(III, 	1,1,0);
	Sequence.Bind(IV, 	0,1,0);
	Sequence.Bind(V, 	0,1,1);
	Sequence.Bind(VI, 	0,0,1);

	Serial.println("Hall Steps... Done");
	delay(100);

	// Default Motor Spin Direction
	Motor.SetMotorDirection(ESpinDirection::EClockwise);

	// Add Debug Page
	Framework::Debug.AddPage(ControllerDebug);

	Serial.println("Add Debug Page... Done");
	delay(100);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorController::Update()
{
	int NextStep = -1;

	if( Sequence.ReadState(NextStep) )
	{
		ControllerDebug.step = NextStep;
		ControllerDebug.rpm = 0;

		Motor.SetCommutatorStep(NextStep);
		Motor.Execute();
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorController::Stop()
{
	Motor.SetStopped();
	Motor.Execute();
}