#include <Arduino.h>
#include "stargate/stargate.h"
#include "serial/serial_msg.h"
#include "motor/motor_controller.h"

Stargate Gate;

void setup()
{
	init();
	Device::SerialCom.Init();
	Device::Display.Init();
	//Device::Display.SetPage(0);

	Device::DHD.Connect_Dialer([](int Sym){Device::Stargate.Dial(Symb)});
	Device::DHD.Connect_Establish([](){Device::Stargate.Establish()});
}

void loop()
{
	Gate.Loop(); 
	Device::Motor.Update();
	Device::DHD.Update();
}