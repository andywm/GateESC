#pragma once

class DialController
{
  using DialFn = void(*)(int);
  using EstablishFn = void(*)();

  void Connect(DialFn Dial);
  void ConnectEstablish(EstablishFn Est);
  void Update();

private:
  DialFn Gate = nullptr;
  EstablishFn Activate = nullptr;
  bool bExecuteHardcodedDialSeq = true;
};