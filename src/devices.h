/*------------------------------------------------------------------------------
 ()		File: device.h
 /\		Authour: Andrew Woodward-May
/  \	Date: June 2024	License: MIT

Description:
    Globally available hardware devices.
------------------------------------------------------------------------------*/
#pragma once
#include <Arduino.h>

//class SerialCommunication;
//class OledDisplayController;
//class MotorController;

#include "serial/serial_msg.h"
#include "oled/oled_display_controller.h"
#include "motor/motor_controller.h"

namespace Device
{
    extern SerialCommunication SerialCom;
    extern OledDisplayController Display;
    extern MotorController Motor;
}
//DisplayDevice
//MotorDevice
//ChevronLockDevice//
//ChevronLedDevice
//SoundDevice
