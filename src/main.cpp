#include <Arduino.h>
#include "device.h"

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