#include <Arduino.h>
#include "stargate/stargate.h"
#include "serial/serial_msg.h"
#include "motor/motor_controller.h"
#include "oled/oled_display_controller.h"

Stargate Gate;

void setup()
{
	init();
	Device::SerialCom.Init();
	Device::Display.Init();
	//Device::Display.SetPage(0);

	Device::DHD.Connect_Dialer([](int Symb){Gate.Dial(Symb);});
	Device::DHD.Connect_Establish([](){Gate.Establish();});
}

void loop()
{
	Device::DHD.Update();

	Gate.Loop(); 
	Device::Motor.Update();
}