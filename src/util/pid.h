class PIDController
{
public:
	void SetTarget(float Target);
	float PID(float Input, float DeltaTime);
	//processadjust
	
	void SetControlRange(float RangeMin, float RangeMax);
	void SetKp(float Kp);
	void SetKi(float Ki);
	void SetKd(float Kd);

private:
	
	float kProportional {0};
	float kIntegral {0};
	float kDerivative {0};

	float SetPoint {0};
	float ControlMin {0};
	float ControlMax {0};

	float Integral {0};
	float PrevError {0};
};