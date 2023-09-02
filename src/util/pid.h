#pragma once
#include "naff_maths_utilities.h"

template<typename InputType, typename OutputType>
class PIDController
{
public:
	OutputType PID(InputType Input, float DeltaTime)
	{
		Input = Maths::Clamp(Input, InputMin, InputMax);

		const float Error = SetPoint - Input;
		if( Error < 0.0f )
		{
			return 0;
		}

		const float pTerm = Error;
		const float iTerm = Integral + (Error * DeltaTime);
		const float dTerm = (Error - PrevError) / DeltaTime;

		float Pid =  (pTerm * kProportional) + (iTerm * kIntegral) + (dTerm * kDerivative);
		Integral = iTerm;
		PrevError = Error;

		//normalise range.
		Pid = (Pid - InputMin) / (InputMax - InputMin);

		//convert to output range.
		Pid = OutputMin + (OutputMax * Pid);

		return Pid;
	}
	
	void SetTarget(InputType Target) { SetPoint = Target; }
	void SetInputRange(InputType MinVal, InputType MaxVal) { InputMin = MinVal; InputMax = MaxVal; }
	void SetOutputRange(OutputType MinVal, OutputType MaxVal) { OutputMin = MinVal; OutputMax = MaxVal; }
	void SetKp(float Kp) { kProportional = Kp; }
	void SetKi(float Ki) { kIntegral = Ki; }
	void SetKd(float Kd) { kDerivative = Kd; }

private:
	OutputType PID_Internal(InputType Input, float DeltaTime);

	float kProportional {0};
	float kIntegral {0};
	float kDerivative {0};
	float Integral {0};
	float PrevError {0};

	InputType SetPoint {0};
	InputType InputMin {0};
	InputType InputMax {0};
	OutputType OutputMin {0};
	OutputType OutputMax {0};
};