/*------------------------------------------------------------------------------
 ()		File: device.h
 /\		Authour: Andrew Woodward-May
/  \	Date: June 2024	License: MIT

Description:
    Globally available hardware devices.
------------------------------------------------------------------------------*/
#pragma once

class SerialCommunication;
class OledDisplayController;
class MotorController;
class ChevronLockController;
class ChevronLampController;

namespace Device
{
    extern SerialCommunication SerialCom;
    extern OledDisplayController Display;
    extern MotorController Motor;
    extern ChevronLockController ChevronLock;
    extern ChevronLampController ChevronLamps;
}