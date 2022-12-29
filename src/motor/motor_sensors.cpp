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
// HallSequence
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
MotorSensors::MotorSensors( int Phases )
	: PhaseCount(Phases)
	, Steps(Maths::CalcFactorial(PhaseCount))
{
	SensorsPins = new int[PhaseCount];
	HashMapping = new int[Steps];
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorSensors::DeclarePinsForSensor(int SensorID, int Pin)
{
	SensorsPins[SensorID] = Pin;
	Framework::PinMode(Pin, EInput);
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

		Framework::Message("EPMF %.2f, TA %d", RPMf, RPM_Calculation.StepAngle);

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
	for( int SensorID = 0; SensorID < PhaseCount; ++SensorID )
	{
		const int PinState = Framework::DigitalRead(SensorsPins[SensorID]);
		Hash |= (PinState << SensorID);
	}

	//Framework::Message("Debug Hash= %d", Hash );
	// Find Step for Hash
	for( int Step = 0; Step < Steps; ++Step )
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

