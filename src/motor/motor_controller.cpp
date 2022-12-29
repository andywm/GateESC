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
	// Create IO Bindings for ESC Switcher Pins
	Motor.DeclarePinsForPhase(0, Framework::Pinout::ESC_SOURCE_A, Framework::Pinout::ESC_SINK_A);
	Motor.DeclarePinsForPhase(1, Framework::Pinout::ESC_SOURCE_B, Framework::Pinout::ESC_SINK_B);
	Motor.DeclarePinsForPhase(2, Framework::Pinout::ESC_SOURCE_C, Framework::Pinout::ESC_SINK_C);

	// Create IO Bindings for Hall Sensor Pins
	Sensors.DeclarePinsForSensor(0, Framework::Pinout::ESC_HALL_A);
	Sensors.DeclarePinsForSensor(1, Framework::Pinout::ESC_HALL_B);
	Sensors.DeclarePinsForSensor(2, Framework::Pinout::ESC_HALL_C);

	// Create Commutation Table
	const int I =	Motor.DeclareCommutationStep(0, ESource, ESink, EFloat);
	const int II =	Motor.DeclareCommutationStep(1, ESource, EFloat, ESink);
	const int III =	Motor.DeclareCommutationStep(2, EFloat, ESource, ESink);
	const int IV =	Motor.DeclareCommutationStep(3, ESink, ESource, EFloat);
	const int V =	Motor.DeclareCommutationStep(4, ESink, EFloat, ESource);
	const int VI =	Motor.DeclareCommutationStep(5, EFloat, ESink, ESource);

	// Bind Hall Sequence to Steps.
	Sensors.Bind(I, 	1,0,1);
	Sensors.Bind(II, 	1,0,0);
	Sensors.Bind(III, 	1,1,0);
	Sensors.Bind(IV, 	0,1,0);
	Sensors.Bind(V, 	0,1,1);
	Sensors.Bind(VI, 	0,0,1);

	//Set Speed Measurement Constants
	Sensors.RPM_Calculation.MeasureOnStep = I;
	Sensors.RPM_Calculation.StepAngle = 18;

	Serial.println("Motor is Configured...");
	delay(100);

	// Default Motor Spin Direction
	Motor.SetMotorDirection(ESpinDirection::EClockwise);
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
