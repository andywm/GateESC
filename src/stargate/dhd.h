#pragma once
using uint8 = unsigned char;
#define U8MAX 0xFF

#include <stdint.h>

struct IDialingDevice
{
	uint8 AddressBuffer[9] = {U8MAX};

	virtual void Initialise() {};
	virtual void Enabled(bool bEnable) {};
	virtual void Update() {};
	virtual void Reset() {};
	virtual uint8 PollAndEat() {return 0;};
	virtual void Confirm(uint8 SymbId, uint8 Symb) {};
	virtual bool IsActivated() {return false;};
};

class DialingInterface
{
	static constexpr int MAX_DIALERS = 2;
	//int Count=0;
	//uint8 ActiveSymbol = -1;

public:
	static DialingInterface& Get()
	{
		static DialingInterface Interface;
		return Interface;
	}

	void Update() 
	{
		for (int Idx = 0; Idx < DialerCount; ++Idx)
		{
			DialingDevices[Idx]->Update();
		}
	}

	uint8 PollAndEat() 
	{
		for (int Idx = 0; Idx < DialerCount; ++Idx)
		{
			uint8 Symb = DialingDevices[Idx]->PollAndEat();
			if (Symb != UINT8_MAX)
			{
				//ActiveSymbol = Symb;
				return Symb;
			}
		}
		return UINT8_MAX;
	}

	void Encode(uint8 SymbId, uint8 Symb)
	{

	}

	bool IsActivatorPressed()
	{
		return false;
	}

	void AddDevice(IDialingDevice& Device)
	{
		if (DialerCount < MAX_DIALERS)
		{
			Device.Initialise();
			DialingDevices[DialerCount++] = &Device;	
		}
	}

	int DialerCount = 0;
	IDialingDevice* DialingDevices[MAX_DIALERS] = {nullptr};
};

class DialHomeDevice : public IDialingDevice
{
public:
	void Initialise() {}
	void Update() {}
	uint8 PollAndEat() {return 0;}
	bool IsActivatorPressed() {return false;}

private:
	uint8 AddressBuffer[9] = {U8MAX};
	uint8 CurrentAddressIndex = 0;
	bool bActivatorPressed = false;

	bool bExecuteHardcodedDialSeq = true;
};