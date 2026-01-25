#include <Arduino.h>
#include "serial/serial_msg.h"
#include "motor/motor_controller.h"
#include "dpm.h"
#include "stargate/dhd.h"
#include "stargate/stargate.h"
#include "Wire.h"

StargateSG1 Stargate;
DialHomeDevice DHD;
Keypad ESCBoardKeypad;
TwoWire I2CBus(Framework::Pinout::I2C0_SDA, Framework::Pinout::I2C0_SCL);

void setup()
{
	//Init HW
	init();
	ESCBoardKeypad.Init(Framework::Pinout::KEYPAD_LATCH, Framework::Pinout::KEYPAD_CLOCK,Framework::Pinout::KEYPAD_DATA);
	Stargate.Initialise(&DHD);

	//Init Debug UI
	PageManager.InitialiseDisplaySubsystem(128, 64, I2CBus);
	Stargate.RegisterDebugHooks();
}

void loop()
{
	//Device::DHD.Update();
	Stargate.Update();

	ESCBoardKeypad.UpdatePadState();
	PageManager.SelectPage();

	if(ScreenPage* Page = PageManager.Current())
	{
		PageManager.Update(ESCBoardKeypad);
		Page->Update(ESCBoardKeypad);
		Page->Draw();
	}
}