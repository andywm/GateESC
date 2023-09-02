#include "pid.h"

void PIDController::SetTarget(float Target)
{
	SetPoint = Target;
}

void PIDController::SetControlRange(float RangeMin, float RangeMax)
{
	ControlMin = RangeMin;
	ControlMax = RangeMax;
}

void PIDController::SetKp(float Kp)
{
	kProportional = Kp;
}

void PIDController::SetKi(float Ki)
{
	kIntegral = Ki;
}

void PIDController::SetKd(float Kd)
{
	kDerivative = Kd;
}

float PIDController::PID(float Input, float DeltaTime)
{
	const float Error = SetPoint - Input;
	const float pTerm = Error;
	const float iTerm = Integral + (Error * DeltaTime);
	const float dTerm = (Error - PrevError) / DeltaTime;

	const float Pid =  (pTerm * kProportional) + (iTerm * kIntegral) + (dTerm * kDerivative);
	Integral = iTerm;
	PrevError = Error;

	return Pid;
}
