#pragma once
#include "naff_maths_utilities.h"
#include "framework.h"

template<typename InputType, typename OutputType>
class PIDController
{
public:
bool bAntiWindup = true;

	OutputType PID(InputType Input, float DeltaTime)
	{
		//PidDebugLog& Log = Framework::PigLog;
		//Log.SpeedInput = Input;
		//Log.TimeInput = DeltaTime;

		const float Error = SetPoint - Input;
		//Log.Error = Error;

		if (bInitial)
		{
			bInitial = false;
			PrevError = Error;
		}

		const float pTerm = Error;
		//Log.pTerm = pTerm;

		const float iTerm = Integral + (Error * DeltaTime);
		//Log.iTerm = iTerm;
		
		const float dTerm = (Error - PrevError) / DeltaTime;
		//Log.eDiff = Error - PrevError;
		//Log.dTerm = dTerm;

		float Pid =  (pTerm * kProportional) + (iTerm * kIntegral) + (dTerm * kDerivative);
		//Log.RawPid = Pid;
		if(bAntiWindup == false)
		{
			Integral = iTerm;
		}
		PrevError = Error;

		//if (DebugSampleTimer <= 0.0f)
		//{
			//Framework::Message("Input = %.2f", Input);
			//Framework::Message("Error = %.2f", Error);
			//Framework::Message("Value = %.2f", Pid);
		//	DebugSampleTimer = 5.0f;
		//}
		//DebugSampleTimer -= DeltaTime;

		//normalise range.
		//Pid = Maths::Clamp(Pid, InputMin, InputMax);
		//Pid = (Pid - InputMin) / (InputMax - InputMin);
		//Log.InterPid = Pid;

		//convert to output range.
		//Pid = OutputMin + (OutputMax * Pid);
		Pid = Maths::Clamp(Pid, (float)OutputMin, (float)OutputMax);
		//Log.OutputPid = Pid;

		//if(SetPoint == 0)
		//Framework::PidDebugging();

		return Pid;
	}
	
	void SetTarget(InputType Target) { SetPoint = Target; }
	void SetInputRange(InputType MinVal, InputType MaxVal) { InputMin = MinVal; InputMax = MaxVal; }
	void SetOutputRange(OutputType MinVal, OutputType MaxVal) { OutputMin = MinVal; OutputMax = MaxVal; }
	void SetKp(float Kp) { kProportional = Kp; }
	void SetKi(float Ki) { kIntegral = Ki; }
	void SetKd(float Kd) { kDerivative = Kd; }
	void Reset() { bAntiWindup = true; Integral = 0; bInitial = true; }

private:
	OutputType PID_Internal(InputType Input, float DeltaTime);

	float kProportional {0};
	float kIntegral {0};
	float kDerivative {0};
	float Integral {0};
	float PrevError {0};
	bool bInitial{true};

	InputType SetPoint {0};
	InputType InputMin {0};
	InputType InputMax {0};
	OutputType OutputMin {0};
	OutputType OutputMax {0};
};