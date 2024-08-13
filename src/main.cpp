#include <Arduino.h>
#include "serial/serial_msg.h"
#include "motor/motor_controller.h"
#include "oled/oled_display_controller.h"
#include "stargate/dhd.h"
#include "stargate/stargate.h"

StargateSG1 Stargate;
DialHomeDevice DHD;

void setup()
{
	init();
	
	Stargate.Initialise(&DHD);
	//Device::SerialCom.Init();
	//Device::Display.Init();
	//Device::Display.SetPage(0);

}

void loop()
{
	//Device::DHD.Update();
	Stargate.Update();

	//Gate.Loop(); 
	//Device::Motor.Update();
}