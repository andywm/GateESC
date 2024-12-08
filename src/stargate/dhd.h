#pragma once
using uint8 = unsigned char;
#define U8MAX 0xFF

struct IDialingDevice
{
	uint8 AddressBuffer[9] = {U8MAX};

	void Initialise();
	void Enabled();
	void Update();
	void Reset();
	uint8 PollAndEat();
	void Confirm(uint8 SymbId, uint8 Symb);
	bool IsActivated();
}

class DialingInterface
{
	static constexpr int32 NAX_DIALERS = 2;

	void Update() 
	{
		for (int32 Idx = 0; Idx < DialerCount; ++Idx)
		{
			DialingDevices[Idx]->Update();
		}
	}

	uint8 PollAndEat() {return 0;}
	bool IsActivatorPressed() {return false;}

	void AddDevice(IDialingDevice& Device)
	{
		if (DialerCount < MAX_DIALERS)
		{
			Device.Initialise();
			DialingDevices[DialerCount++] = &Device;	
		}
	}

	int32 DialerCount = 0;
	IDialingDevice* DialingDevices[NAX_DIALERS] = {nullptr};
};

class DialHomeDevice
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