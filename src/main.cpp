#include <Arduino.h>
#include <math.h>
#include "motor_one.h"
//#include "debug.h"

#define USERMAIN

int main(int argc, char**argv)
{
	init();
	Serial.begin(9600);
	//delay(100);
	MotorOne program;
	program.Run();
}