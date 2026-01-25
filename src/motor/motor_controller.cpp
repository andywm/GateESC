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
#include "dpm.h"
#include "framework.h"
#include <Arduino.h>
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Debug Info
//------------------------------------------------------------------------------
struct HighLevelStatusPage : public InfoTextPage
{
	PageInt rpm;
	PageInt angle;
	PageInt pwm1;
	PageInt pwm2;
	PageInt pwm3;
	PageInt pwm4;
	PageInt sym;

	HighLevelStatusPage()
	{
		//______L__|12345678901234567890|
		SetLine<0>("RPM ###      ANG ###", rpm, angle);
		SetLine<1>("PWM ### ### ### ### ", pwm1, pwm2, pwm3, pwm4);
		SetLine<2>("                    ");
		SetLine<3>("Dial SYM ##         ", sym);
	}

} ControllerDebug;

//------------------------------------------------------------------------------
// Debug Info - fundamental states, useful for validating/debugging new hardware.
//------------------------------------------------------------------------------
struct LowLevelStatusPage : public InfoTextPage
{
	PageInt step;
	PageInt h1;
	PageInt h2;
	PageInt h3;

	LowLevelStatusPage()
	{
		//______L__|12345678901234567890|
		SetLine<0>("Step #              ", step);
		SetLine<1>("A# B# C#            ", h1, h2, h3);
	}
} DebugFundamentals;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorController::Init()
{
	enum Phases { A,B,C };
	Serial.println("===Config===");

	// Create IO Bindings for ESC Switcher Pins.
	Motor.DeclarePinsForPhase(A, Framework::Pinout::ESC_SOURCE_A, Framework::Pinout::ESC_SINK_A);
	Motor.DeclarePinsForPhase(B, Framework::Pinout::ESC_SOURCE_B, Framework::Pinout::ESC_SINK_B);
	Motor.DeclarePinsForPhase(C, Framework::Pinout::ESC_SOURCE_C, Framework::Pinout::ESC_SINK_C);

	//Create IO Binding, a PIO handler for Position Sensor.
	Sensors.DeclareQuadraturePins(Framework::Pinout::QUADRATURE_A, Framework::Pinout::QUADRATURE_B);

	// Create IO Bindings for Hall Sensor Pins.
	Sensors.DeclareHallPins(Framework::Pinout::ESC_HALL_1, Framework::Pinout::ESC_HALL_2, Framework::Pinout::ESC_HALL_3);

	// Declare Valid Hall States.
	const int State_101 = Sensors.DeclareSensorState(1,0,1);
	const int State_100 = Sensors.DeclareSensorState(1,0,0);
	const int State_110 = Sensors.DeclareSensorState(1,1,0);
	const int State_010 = Sensors.DeclareSensorState(0,1,0);
	const int State_011 = Sensors.DeclareSensorState(0,1,1);
	const int State_001 = Sensors.DeclareSensorState(0,0,1);

	// Create Windings; Sink to Source.
	const int Winding_AB = Motor.DeclareWinding(A, B);
	const int Winding_AC = Motor.DeclareWinding(A, C);
	const int Winding_BA = Motor.DeclareWinding(B, A);
	const int Winding_BC = Motor.DeclareWinding(B, C);
	const int Winding_CA = Motor.DeclareWinding(C, A);
	const int Winding_CB = Motor.DeclareWinding(C, B);

	//Clockwise; Hall State & Winding Response.
	Motor.BindClockwiseWinding(State_101, Winding_BA);
	Motor.BindClockwiseWinding(State_100, Winding_CA);
	Motor.BindClockwiseWinding(State_110, Winding_CB);
	Motor.BindClockwiseWinding(State_010, Winding_AB);
	Motor.BindClockwiseWinding(State_011, Winding_AC);
	Motor.BindClockwiseWinding(State_001, Winding_BC);

	//Anti-Clockwise; Hall State & Winding Response.
	Motor.BindAntiClockwiseWinding(State_101, Winding_AB);
	Motor.BindAntiClockwiseWinding(State_100, Winding_AC);
	Motor.BindAntiClockwiseWinding(State_110, Winding_BC);
	Motor.BindAntiClockwiseWinding(State_010, Winding_BA);
	Motor.BindAntiClockwiseWinding(State_011, Winding_CA);
	Motor.BindAntiClockwiseWinding(State_001, Winding_CB);

	//Configure speed control PID.
	SpeedPID.SetKp(2.0f);//1.5
	SpeedPID.SetKi(0.5f);//1.2
	SpeedPID.SetKd(0.1f);//0
	SpeedPID.SetInputRange(0.0f, 420.0f);
	SpeedPID.SetOutputRange(0, UINT8_MAX);

	//Configure target PID.
	DistancePID.SetKp(0.5f);
	DistancePID.SetKi(0);
	DistancePID.SetKd(1);
	DistancePID.SetInputRange(-20, 0);
	DistancePID.SetOutputRange(0, 1);

	Serial.println("Motor Ready...");
	delay(100);
 
	Sensors.ResetSpeedMeasurement();

	// Add Debug Page
	//TODO Fix device
	//Device::Display.AddPage(ControllerDebug);
	ControllerDebug.pwm1 = 0;
	ControllerDebug.pwm2 = 0;
	ControllerDebug.pwm3 = 0;
	ControllerDebug.pwm4 = 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
struct DebugUpdater
{
	MotorController& Controller;
	~DebugUpdater()
	{
		Controller.UpdateDebug();
	}
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorController::Update()
{
	DebugUpdater UpdateDebugDisplay {*this};

	Sensors.SenseCommutationStep();
	Sensors.SensePosition();
	bDoEffortCalculation = Sensors.SenseSpeed();

	if (ControlMode == EControlMode::Stopped)
	{
		return;
	}

	if (ControlMode == EControlMode::Moving)
	{
		Moving();
	}
}

void MotorController::Moving()
{
	if (Sensors.GetAngle() == TargetAngle)
	{
		AtTarget = true;
		ControlMode = EControlMode::Stopped;
		Stop();
		return;
	}

	Motor.SetCommutatorStep(Sensors.GetStep());

	//Speed Control.
	if (bDoEffortCalculation)
	{
		if(Sensors.GetRPM() > 0)
		{
			SpeedPID.bAntiWindup = false;
		}

		int Distance = DistanceToTarget();
		if (Distance < 40)
		{
			DistancePID.bAntiWindup = false;
			const int Dist = DistancePID.PID(Distance, Sensors.GetRpmDeltaTime());
			SpeedPID.SetTarget(-Dist);
		}
		const int PWM = SpeedPID.PID(Sensors.GetRPMf(), Sensors.GetRpmDeltaTime());
		
		//Device::SerialCom.Message("Dist=%d", Dist);

		Motor.SetDuty(PWM);

		ControllerDebug.pwm4 = ControllerDebug.pwm3;
		ControllerDebug.pwm3 = ControllerDebug.pwm2;
		ControllerDebug.pwm2 = ControllerDebug.pwm1;
		ControllerDebug.pwm1 = PWM;
	}

	Motor.Drive();
}

int MotorController::DistanceToTarget()
{
	int Ang = 360;

	if (TargetAngle == NO_TARGET_ANGLE)
	{
		return Ang;
	}

	if (Motor.GetMotorDirection() == ESpinDirection::EClockwise)
	{
		Ang = Sensors.GetAngle() - TargetAngle;
	}
	else if (Motor.GetMotorDirection() == ESpinDirection::EAntiClockwise)
	{
		Ang = TargetAngle - Sensors.GetAngle();
	}

	if(Ang < 0)
	{
		return 360 + Ang;
	}
	return Ang;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorController::UpdateDebug()
{
	//Debug Stuff
	DebugFundamentals.step = Sensors.GetStep();
	DebugFundamentals.h1 = Sensors.DebugSensorPins[0];
	DebugFundamentals.h2 = Sensors.DebugSensorPins[1];
	DebugFundamentals.h3 = Sensors.DebugSensorPins[2];

	ControllerDebug.rpm = Sensors.GetRPM(); 
	ControllerDebug.angle = Sensors.GetAngle();
	ControllerDebug.sym = 1 + ((TargetAngle - (TargetAngle<200? 2 : 5 )) / 9);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorController::RegisterDebugHooks()
{
	PageManager.RegisterRoot(DebugFundamentals);
	PageManager.RegisterRoot(ControllerDebug);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorController::Stop()
{
	//Serial.println("Stop");
	Motor.StopMotor();
	ControlMode = EControlMode::Stopped;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorController::SetForward()
{
	Motor.SetMotorDirection(ESpinDirection::EClockwise);
	Motor.StartMotor();
	Sensors.ResetSpeedMeasurement();
	SpeedPID.Reset();
	DistancePID.Reset();
	ControlMode = EControlMode::Moving;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorController::SetBackward()
{
	Motor.SetMotorDirection(ESpinDirection::EAntiClockwise);
	Motor.StartMotor();
	SpeedPID.Reset();
	DistancePID.Reset();
	Sensors.ResetSpeedMeasurement();
	ControlMode = EControlMode::Moving;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorController::SetSpeed(uint8_t RPM)
{
	//RPMSetPoint = RPM;
	TargetRPM = RPM;
	SpeedPID.SetTarget(static_cast<float>(RPM));
	DistancePID.SetOutputRange(-TargetRPM, -1);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorController::SetTargetPosition(int Angle)
{
	AtTarget = false;
	TargetAngle = Angle;

	DistancePID.SetTarget(0);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool MotorController::IsAtTargetPosition()
{
	return AtTarget;
}