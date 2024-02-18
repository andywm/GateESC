/*------------------------------------------------------------------------------
 ()		File: motor_sensors.cpp
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:

------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "motor/motor_sensors.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// MotorSensors
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#define USE_ANGLE_TICK 0
#define USE_RPM_AVG 1

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

int MotorSensors::PositionTick = 0;
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorSensors::DeclarePositionPins(int Pin)
{
#if USE_ANGLE_TICK
	Framework::Message("Position Sensor; pin %d", Pin );
	Framework::PinMode(Pin, EInput);

	attachInterrupt(digitalPinToInterrupt(Pin), PositionInterrupt, RISING);
#endif
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorSensors::Sense()
{
	ReadState();
	UpdateTachometerHalls();

	//Note: Currently the position sensor is higher resolution than the hall sense
	//circuit, but the read isn't as reliable yet. For now use the HSC, but in future
	//ideally this would entirely be the duty of the position sensor. A future version
	//of the ESC may even switch to back emf, eliminating the halls entirely.
#if USE_ANGLE_TICK
	if( PositionTick >= 360 )
	{
		PositionTick = 0;
	}
#endif

	if(bChanged)
	{
		//Framework::Message("RPM = %d, Angle = ", RPM);
		//Framework::Message("RPM = %d, Angle = %d", RPM, PositionTick);
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int avg_two_most_similar(int a, int b, int c)
{
    int diffAB = a > b? a - b : b - a;
    int diffAC = a > c? a - c : c - a;
    int diffBC = b > c? b - c : c - b;
    
    if( diffAB < diffAC && diffAB < diffBC)
    {
        return (a + b) / 2;
    }
    else if( diffAC < diffAB && diffAC < diffBC)
    {
        return (a + c) / 2;
    }
    return (b + c) /2;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorSensors::UpdateTachometerHalls()
{
	if(CommutatorStep == Tachometer.Config.MeasureOnStep && bChanged)
	{
		Tachometer.TimeInterval = Tachometer.MeasurementTimer.ReadTime() * 1e-6;

		const float AngularSpeedDegPerSecond = Tachometer.Config.StepAngle / Tachometer.TimeInterval;
		const float RPMf = AngularSpeedDegPerSecond / 6.0f;

		//Framework::Message("EPMF %.2f, TA %d", RPMf, RPM_Calculation.StepAngle);

		//round
#if USE_RPM_AVG
		rpm_old = rpm_mid;
		rpm_mid = rpm_new;
		rpm_new = static_cast<int>( RPMf + 0.5f );

		RPM = avg_two_most_similar(rpm_old, rpm_mid, rpm_new);
#else
		RPM = static_cast<int>( RPMf + 0.5f );
#endif

		//reset.
		Tachometer.MeasurementTimer.Restart();
	}
}

void MotorSensors::UpdateTachometerPos()
{
	int Pos = GetAngle();

	if(Pos != Tachometer.OldPosition)
	{
		int Ticks = Maths::Abs(Pos - Tachometer.OldPosition);
		if(Pos >= 360)
		{
			Pos -= 360;
		}
		Tachometer.OldPosition = Pos;
		
		Tachometer.TimeInterval = Tachometer.MeasurementTimer.ReadTime() * 1e-6;

		//This only works as the mk1 optical encoder has 360 segments, Calc should be (EncoderResolution*Ticks)
		const float AngularSpeedDegPerSecond = Ticks / Tachometer.TimeInterval;
		const float RPMf = AngularSpeedDegPerSecond / 6.0f;

		//Framework::Message("EPMF %.2f, TA %d", RPMf, RPM_Calculation.StepAngle);

		//round
#if USE_RPM_AVG
		rpm_old = rpm_mid;
		rpm_mid = rpm_new;
		rpm_new = static_cast<int>( RPMf + 0.5f );

		RPM = avg_two_most_similar(rpm_old, rpm_mid, rpm_new);
#else
		RPM = static_cast<int>( RPMf + 0.5f );
#endif

		//reset.
		Tachometer.MeasurementTimer.Restart();
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorSensors::ReadState()
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
			bChanged = (Step != CommutatorStep);
			CommutatorStep = Step;

			break;
		}
	}
}

void MotorSensors::PositionInterrupt()
{
	static unsigned long last_interrupt_time = 0;
	unsigned long interrupt_time = micros();
	unsigned long diff = interrupt_time - last_interrupt_time;
	last_interrupt_time = interrupt_time;

	if (diff > 100)
	{
		PositionTick++;
	}
}

bool MotorSensors::GetChanged() const
{
	return bChanged;
}

int MotorSensors::GetStep() const
{
	return CommutatorStep;
}

int MotorSensors::GetRPM() const
{
	return RPM;
}

float MotorSensors::GetTimeInterval() const
{
	return Tachometer.TimeInterval;
}

int MotorSensors::GetAngle() const
{
	return PositionTick;
}
