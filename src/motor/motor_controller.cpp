/*------------------------------------------------------------------------------
 ()		File: motor_controller.cpp
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:
	Interface for the motor.
------------------------------------------------------------------------------*/
#include "framework.h"
#include "motor/motor_controller.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorController::Init()
{
	// Create IO Bindings for ESC Switcher Pins
	Motor.DeclarePinsForPhase(0, Framework::Pinout.ESC_SOURCE_A, Framework::Pinout.ESC_SINK_A);
	Motor.DeclarePinsForPhase(1, Framework::Pinout.ESC_SOURCE_B, Framework::Pinout.ESC_SINK_B);
	Motor.DeclarePinsForPhase(2, Framework::Pinout.ESC_SOURCE_C, Framework::Pinout.ESC_SINK_C);

	// Create IO Bindings for Hall Sensor Pins
	Motor.DeclarePinsForSensor(0, Framework::Pinout.ESC_HALL_A);
	Motor.DeclarePinsForSensor(1, Framework::Pinout.ESC_HALL_B);
	Motor.DeclarePinsForSensor(2, Framework::Pinout.ESC_HALL_C);

	// Create Commutation Table
	const int I =	Motor.DeclareCommutationStep(0, ESource, ESink, EFloat);
	const int II =	Motor.DeclareCommutationStep(1, ESource, EFloat, ESink);
	const int III =	Motor.DeclareCommutationStep(2, EFloat, ESource, ESink);
	const int IV =	Motor.DeclareCommutationStep(3, ESink, ESource, EFloat);
	const int V =	Motor.DeclareCommutationStep(4, ESink, EFloat, ESource);
	const int VI =	Motor.DeclareCommutationStep(5, EFloat, ESink, ESource);

	// Bind Hall Sequence to Steps.
	Sequence.Bind(I, 	0,0,0);
	Sequence.Bind(II, 	0,0,0);
	Sequence.Bind(III, 	0,0,0);
	Sequence.Bind(IV, 	0,0,0);
	Sequence.Bind(V, 	0,0,0);
	Sequence.Bind(VI, 	0,0,0);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorController::Update()
{
	int ReadBuffer[3];
	Sequence.ReadState(ReadBuffer);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorController::Stop()
{

}