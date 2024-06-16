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
class DialController;

namespace Device
{
    extern SerialCommunication SerialCom;
    extern OledDisplayController Display;
    extern MotorController Motor;
    extern ChevronLockController ChevronLock;
    extern ChevronLampController ChevronLamps;
    extern DialController DHD;
}

/*
template<typename T, typename Rtn, typename ...TArgs>
struct Interconnect
{
    using Function Rtn T::(*fn)(TArgs&&...args));

    template<typename T, typename Rtn, typename ...TArgs>
    static Rtn Function((TArgs&&...args))
    {
        (*Ptr).ClassPtr(std::forward(...args);)
    }

    static Function FnPtr = nullptr;
    static T* ClassPtr = nullptr;
    T* Ptr;
}

Interconnect<Stargate, void, int>::Fn;

*/