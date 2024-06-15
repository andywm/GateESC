#include "serial_msg.h"
#include <Arduino.h>

void SerialCommunication::Init()
{
	Serial.begin(9600);
}