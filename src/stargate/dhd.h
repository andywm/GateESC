#pragma once
using uint8 = unsigned char;
#define U8MAX 0xFF

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