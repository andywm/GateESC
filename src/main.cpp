#include <Arduino.h>
#include <math.h>
#include "motor_one.h"
//#include "debug.h"

void setup()
{
	//don't actually do anything for now.

	init();
	Serial.begin(9600);
	MotorOne program;
	program.Run();
}

void loop()
{

}