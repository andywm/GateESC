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
void MotorSensors::Sense()
{
	ReadState();
	CalculateRPM();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorSensors::CalculateRPM()
{
	if(CommutatorStep == RPM_Calculation.MeasureOnStep && bChanged)
	{
		const float AngularSpeedDegPerSecond = RPM_Calculation.StepAngle / (RPM_Calculation.Time.ReadTime()*1e-6);
		const float RPMf = AngularSpeedDegPerSecond / 6.0f;

		//Framework::Message("EPMF %.2f, TA %d", RPMf, RPM_Calculation.StepAngle);

		//round
		RPM = static_cast<int>( RPMf + 0.5f );

		//reset.
		RPM_Calculation.Time.Restart();
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

