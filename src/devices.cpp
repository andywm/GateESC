/*------------------------------------------------------------------------------
 ()		File: device.cpp
 /\		Authour: Andrew Woodward-May
/  \	Date: June 2024	License: MIT

Description:
    Globally available hardware devices.
------------------------------------------------------------------------------*/
#include "devices.h"

#include <Arduino.h>
#include "serial/serial_msg.h"
#include "oled/oled_display_controller.h"
#include "motor/motor_controller.h"
#include "chevrons/chevron_lock.h"
#include "chevrons/lamps.h"
#include "stargate/dial.h"

namespace Device
{
    SerialCommunication SerialCom;
    OledDisplayController Display;
    //MotorController Motor;
    //ChevronLockController ChevronLock;
    //ChevronLampController ChevronLamps;
    //DialController DHD;
}