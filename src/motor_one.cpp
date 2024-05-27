/*------------------------------------------------------------------------------
 ()		File: motor_one.cpp
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:
	'Application' class for Motor One.  
------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "motor_one.h"
#include "framework.h"
#include <Arduino.h>
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Motor One
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorOne::Run()
{
	Init();
	while(true)
	{
		Loop();
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorOne::Init()
{
	Control.Init();
	Control.SetSpeed(10);
	Control.SetForward();
	//Control.SetBackward();
	Framework::Debug.Init();
	Framework::Debug.SetPage(0);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int sequenceOnce[7] = {false}; 
int seq = 0;
Timer Delay;

#define DIAL_TEST 0
#define REPEATABILITY_TEST 1 

void MotorOne::Loop()
{
#if DIAL_TEST
	DialTest();
#endif

#if REPEATABILITY_TEST
	RepeatabilityTest();
#endif

	Control.Update();
	Framework::Debug.Process();
}

bool bWait = false;
bool bResetRepeatabilityTarget = true;
void MotorOne::RepeatabilityTest()
{
	if (Control.IsAtTargetPosition() && !bWait && !bResetRepeatabilityTarget)
	{
		bWait = true;
		TestTimer.Restart();
	}
	else if(bWait && TestTimer.ReadTime() > 1000000)
	{
		bWait = false;
		Control.SetTargetPosition(NO_TARGET_ANGLE);
		Control.SetSpeed(10);
		Control.SetForward();

		bResetRepeatabilityTarget = true;
	}
	else if(bResetRepeatabilityTarget)
	{
		bResetRepeatabilityTarget = false;
		Control.SetTargetPosition(180);
	}
}

void MotorOne::DialTest()
{
	//some random address...
	//23,   5,   16,   12,   32,   10,   1
	//208 : 42 : 143 : 106 : 291 : 88, : 5

	static const int sequence[] = {208, 42, 143, 106, 291, 88, 5}; 
	
	if (seq < 7)
	{
		if (!sequenceOnce[seq])
		{
			sequenceOnce[seq] = true;
			Control.SetTargetPosition(sequence[seq]);

			if(seq % 2 == 0)
			{
				Control.SetForward();
			}
			else
			{
				Control.SetBackward();
			}
		}

		if (Control.IsAtTargetPosition())
		{
			seq++;
			delay(2000);
			return;
		}
	}
}
