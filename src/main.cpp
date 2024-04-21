#include <Arduino.h>
#include <math.h>
#include "motor_one.h"

void setup()
{
	init();
	Serial.begin(9600);
	MotorOne program;
	program.Run();
}

void loop()
{

}