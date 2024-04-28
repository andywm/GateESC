/*------------------------------------------------------------------------------
 ()		File: motor_sensors.cpp
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:

------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "motor/motor_sensors.h"
#include "quadrature.pio.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// MotorSensors
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
const PIO QuadraturePIO = pio0;
const uint QuadratureStateMachine = 0;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorSensors::DeclareHallPins(int Pin1, int Pin2, int Pin3)
{
	Framework::Message(" Hall 1; pin %d", Pin1 );
	Framework::PinMode(Pin1, EInput);
	SensorsPins[0] = Pin1;

	Framework::Message(" Hall 2; pin %d", Pin2 );
	Framework::PinMode(Pin2, EInput);
	SensorsPins[1] = Pin2;

	Framework::Message(" Hall 3; pin %d", Pin3 );
	Framework::PinMode(Pin3, EInput);
	SensorsPins[2] = Pin3;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int MotorSensors::DeclareSensorState(int H1, int H2, int H3)
{
	const int State = RegisteredStates++;

	HashMapping[State] = H1 << 0 | H2 << 1 | H3 << 2;
	return State;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorSensors::DeclareQuadraturePins(int PinA, int PinB)
{
	//Pins A and B must be consecutive, with A being the numerically earlier pin.
	Framework::PinMode(PinA, EInput);
	Framework::PinMode(PinB, EInput);

	pio_add_program(QuadraturePIO, &quadrature_encoder_program);
  	quadrature_encoder_program_init(QuadraturePIO, QuadratureStateMachine, PinA, 0);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorSensors::SensePosition()
{
  	const int NewSensorReading = quadrature_encoder_get_count(QuadraturePIO, QuadratureStateMachine);
  	const int Delta = NewSensorReading - Tachometer.RawSensorAngle;
	
	Tachometer.Angle += Delta;

	if (Tachometer.Angle >= Maths::kNativeResPerRev)
	{
		Tachometer.Angle -= Maths::kNativeResPerRev;
  	}
	else if (Tachometer.Angle < 0)
	{
		Tachometer.Angle += Maths::kNativeResPerRev;
	}

  	Tachometer.RawSensorAngle = NewSensorReading;
	Tachometer.TicksSinceLastSpeedMeasurement += (Delta > 0)? Delta : -Delta;
	Tachometer.AngleInDegrees = Maths::NativeAngToDeg(Tachometer.Angle);
}

bool MotorSensors::SenseSpeed()
{
	//Sample at a fixed time step. This would be more accurate on a timer.
	if (Tachometer.MeasurementTimer.ReadTime() < 25000)
	{
		return false;
	}

	Tachometer.RpmMeasurementDt = Tachometer.MeasurementTimer.ReadTime() * 1e-6;
	const float AngleDegrees = Maths::NativeAngToDeg(Tachometer.TicksSinceLastSpeedMeasurement);
	const float AngularSpeedDegPerSecond = AngleDegrees / Tachometer.RpmMeasurementDt;
	Tachometer.RPM = Maths::Round<int>(AngularSpeedDegPerSecond / 6.0f);

	Tachometer.MeasurementTimer.Restart();
	Tachometer.TicksSinceLastSpeedMeasurement = 0;
	return true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorSensors::ResetSpeedMeasurement()
{
	Tachometer.MeasurementTimer.Restart();
	Tachometer.TicksSinceLastSpeedMeasurement = 0;
	Tachometer.RpmMeasurementDt = 0;
	Tachometer.RPM = 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorSensors::SenseCommutationStep()
{
	// Calc Hash
	int Hash = 0;
	for( int SensorID = 0; SensorID < GlobalMotor::PhaseCount; ++SensorID )
	{
		const int PinState = Framework::DigitalRead(SensorsPins[SensorID]);
		DebugSensorPins[SensorID] = PinState;
		Hash |= (PinState << SensorID);
	}

	//Framework::Message("Debug Hash= %d", Hash );
	// Find Step for Hash
	for( int Step = 0; Step < GlobalMotor::StepCount; ++Step )
	{
		if( HashMapping[Step] == Hash )
		{
			CommutatorStep = Step;
			break;
		}
	}
}

int MotorSensors::GetStep() const
{
	return CommutatorStep;
}

int MotorSensors::GetAngle() const
{
	return Tachometer.AngleInDegrees;
}

int MotorSensors::GetRPM() const
{
	return Tachometer.RPM;
}

float MotorSensors::GetRpmDeltaTime() const
{
	return 0.025f; //Tachometer.RpmMeasurementDt;
}