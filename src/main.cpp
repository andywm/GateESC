#include <Arduino.h>
#include <math.h>
#include "motor_one.h"
//#include "debug.h"

#if !defined(IS_NANO_EVERY_BUILD)

int main(int argc, char**argv)
{
	init();
	Serial.begin(9600);
	MotorOne program;
	program.Run();
}
#else
void setup()
{
	Serial.begin(9600);
	MotorOne program;
	program.Run();
}

void loop(){}
#endif