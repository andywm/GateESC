#pragma once
#include "stargate_state.h"

class DialInputController
{
  using DialFn = void(*)(int);
  using EstablishFn = void(*)();

public:
  void Connect_Dialer(DialFn Dial);
  void Connect_Establish(EstablishFn Est);
  void Update();

private:
	uint8 AddressBuffer[9] = {NullAddress};
  DialFn DialGate = nullptr;
  EstablishFn Activate = nullptr;
  bool bExecuteHardcodedDialSeq = true;
};