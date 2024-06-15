/*------------------------------------------------------------------------------
 ()		File: serial_msg.h
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:
	Serial communication utility. Primarily intended for debug logging.
------------------------------------------------------------------------------*/
#pragma once
#include <stdint.h>
#include "devices.h"
//#include <Arduino.h>

class SerialCommunication
{
public:
	void Init();

	template<unsigned int BufferSize=32, typename... PackedArgs>
	void Message(const char* Msg, PackedArgs...Args)
	{
		//char Buffer[BufferSize];
		//snprintf(Buffer, BufferSize, Msg, Args...);
		//Serial.println(Buffer);
	}
};