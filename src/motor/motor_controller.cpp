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
struct MotorControlPage : public DebugPage
{
	virtual bool Update() override
	{
		if(Dirty)
		{
			Dirty = false;

			//______L__|12345678901234567890|
			SetLine(0, "Step #              ", step.Value);
			SetLine(1, "Measured RPM ###    ", rpm.Value);
			SetLine(2, "                    ");
			SetLine(3, "                    ");
			
			return true;
		}
		return false;
	}
	DebugValue<int> rpm = {Dirty};
	DebugValue<int> step = {Dirty};
} ControllerDebug;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorController::Init()
{
	enum Phases { A,B,C };
	Serial.println("===Config===");

	// Create IO Bindings for ESC Switcher Pins
	Motor.DeclarePinsForPhase(A, Framework::Pinout::ESC_SOURCE_A, Framework::Pinout::ESC_SINK_A);
	Motor.DeclarePinsForPhase(B, Framework::Pinout::ESC_SOURCE_B, Framework::Pinout::ESC_SINK_B);
	Motor.DeclarePinsForPhase(C, Framework::Pinout::ESC_SOURCE_C, Framework::Pinout::ESC_SINK_C);

	// Create IO Bindings for Hall Sensor Pins
	Sensors.DeclareHallPins(Framework::Pinout::ESC_HALL_1, Framework::Pinout::ESC_HALL_2, Framework::Pinout::ESC_HALL_3);

	// Declare Valid Hall States
	const int State_101 = Sensors.DeclareSensorState(1,0,1);
	const int State_100 = Sensors.DeclareSensorState(1,0,0);
	const int State_110 = Sensors.DeclareSensorState(1,1,0);
	const int State_010 = Sensors.DeclareSensorState(0,1,0);
	const int State_011 = Sensors.DeclareSensorState(0,1,1);
	const int State_001 = Sensors.DeclareSensorState(0,0,1);

	//Set Speed Measurement Constants
	Sensors.RPM_Calculation.MeasureOnStep = State_100;
	Sensors.RPM_Calculation.StepAngle = 18;

	// Create Windings; Sink to Source
	const int Winding_AB = Motor.DeclareWinding(A, B);
	const int Winding_AC = Motor.DeclareWinding(A, C);
	const int Winding_BA = Motor.DeclareWinding(B, A);
	const int Winding_BC = Motor.DeclareWinding(B, C);
	const int Winding_CA = Motor.DeclareWinding(C, A);
	const int Winding_CB = Motor.DeclareWinding(C, B);

	//Clockwise State|Response
	Motor.BindClockwiseWinding(State_101, Winding_BA);
	Motor.BindClockwiseWinding(State_100, Winding_CA);
	Motor.BindClockwiseWinding(State_110, Winding_CB);
	Motor.BindClockwiseWinding(State_010, Winding_AB);
	Motor.BindClockwiseWinding(State_011, Winding_AC);
	Motor.BindClockwiseWinding(State_001, Winding_BC);

	//Anti-Clockwise State|Response
	Motor.BindAntiClockwiseWinding(State_101, Winding_AB);
	Motor.BindAntiClockwiseWinding(State_100, Winding_CB);
	Motor.BindAntiClockwiseWinding(State_110, Winding_AC);
	Motor.BindAntiClockwiseWinding(State_010, Winding_BC);
	Motor.BindAntiClockwiseWinding(State_011, Winding_CA);
	Motor.BindAntiClockwiseWinding(State_001, Winding_BA);

	Serial.println("Motor Ready...");
	delay(100);

	Motor.Ready();
	Sensors.RPM_Calculation.Time.Begin();

	// Add Debug Page
	Framework::Debug.AddPage(ControllerDebug);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorController::Update()
{
	Sensors.Sense(); 

	//Debug Stuff
	ControllerDebug.rpm = Sensors.GetRPM();
	ControllerDebug.step = Sensors.GetStep();	

	//Speed PID Loop
	//... Modulate PWM?

	//Motor Control
	Motor.SetCommutatorStep(Sensors.GetStep());
	Motor.Drive();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorController::Stop()
{
	//Motor.SetStopped();
	//Motor.Execute();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorController::SetForward()
{
	Motor.SetMotorDirection(ESpinDirection::EClockwise);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorController::SetBackward()
{
	Motor.SetMotorDirection(ESpinDirection::EAntiClockwise);
}
