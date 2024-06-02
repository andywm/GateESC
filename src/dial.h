class DialerProgram
{
  int Address[9] = {0};

  void AddSymbol(int Symbol);
  void BeginDial();
  void Process();
};

struct DialerUI
{
    void Iniitalise();
    void DrawStaticElements();
    void SetChevron(int Chevron, bool bOn);
    void SetSymbolPane(int Chevron, int Symbol);
    void SetBigSymbol(int Phase, int Symbol);
};