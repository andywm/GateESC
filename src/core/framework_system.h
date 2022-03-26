/*------------------------------------------------------------------------------
 ()		File: framework_system.h
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:
	Base Framework
------------------------------------------------------------------------------*/
#pragma once
#include <stdint.h>

class FameworkSystem
{
    virtual void Tick() = 0;
};