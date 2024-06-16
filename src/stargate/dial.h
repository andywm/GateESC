#pragma once

class DialController
{
  using DialFn = void(*)(int);
  using EstablishFn = void(*)();

  void Connect_Dialer(DialFn Dial);
  void Connect_Establish(EstablishFn Est);
  void Update();

private:
  DialFn DialGate = nullptr;
  EstablishFn Activate = nullptr;
  bool bExecuteHardcodedDialSeq = true;
};