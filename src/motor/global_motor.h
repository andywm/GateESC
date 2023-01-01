/*------------------------------------------------------------------------------
 ()		File: motor_controller.cpp
 /\		Authour: Andrew Woodward-May
/  \	Date: January 2023	License: MIT

Description:
	Global constants for motor config.
------------------------------------------------------------------------------*/
#pragma once

struct GlobalMotor
{
    static constexpr int PhaseCount = 3;
    static constexpr int StepCount = 6;
};