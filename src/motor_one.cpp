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
	Framework::Debug.Init();
	Framework::Debug.SetPage(0);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MotorOne::Loop()
{
	Control.Update();
	Framework::Debug.RenderCurrentPage();
}
