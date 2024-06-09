/*
#include <Adafruit_SSD1306.h>
#include "dial.h"

static constexpr int SYMBOL_PANE_X = 100;
static constexpr int SYMBOL_PANE_W = 15;
static constexpr int SYMBOL_PANE_H = 10;

static constexpr int GATE_CENTRE_X = 40;
static constexpr int GATE_CENTRE_Y = 31;
static constexpr int GATE_MAJOR_RADIUS = 31;
static constexpr int GATE_MINOR_RADIUS = 22;
static constexpr int GATE_CHEV_RADIUS = 28;
static constexpr int GATE_DIV_RADIUS = 25;
static constexpr int TEXTSIZE_H = 4;
static constexpr int TEXTSIZE_W = 5;
static constexpr int ChevronActivationOrder[] = {1, 2, 3, 6, 7, 8, 9, 4, 5};

static float CHEVRON_ANG = 6.28f/9.0f;

void DialerProgram::AddSymbol(int Symbol)
{

}
void DialerProgram::BeginDial()
{

}

void DialerProgram::Process()
{

}


void DialerUI::SetChevron(int Chevron, bool bOn)
{
	float Angle = Chevron * CHEVRON_ANG;
	int ChevX = GATE_CENTRE_X + (GATE_CHEV_RADIUS * sin(Angle));
	int ChevY = GATE_CENTRE_Y + (GATE_CHEV_RADIUS * cos(3.14f + Angle));

	if (bOn)
	{
		Display.fillCircle(ChevX, ChevY, 2, WHITE);
	}
	else
	{
		Display.fillCircle(ChevX, ChevY, 2, BLACK);
		Display.drawCircle(ChevX, ChevY, 2, WHITE);
	}
}

void DialerUI::SetSymbolPane(int Chevron, int Symbol)
{
	int SymbolY = Chevron * (SYMBOL_PANE_H-1);
	Display.drawRect(SYMBOL_PANE_X, SymbolY, SYMBOL_PANE_W, SYMBOL_PANE_H, BLACK);
	Display.drawRect(SYMBOL_PANE_X, SymbolY, SYMBOL_PANE_W, SYMBOL_PANE_H, WHITE);	
	
	int NumChars = Symbol <10 ? 1 : 2;
	int YOffset = 1;//(SYMBOL_PANE_H - TEXTSIZE_H)/2 - 1;
	int XOffset =  (SYMBOL_PANE_W - (TEXTSIZE_W * NumChars) )/2;	
	
	Display.setTextSize(1);
	Display.setTextColor(WHITE, BLACK);
	Display.setCursor(100 + XOffset, SymbolY + YOffset);	
	
	char str[3];
	sprintf(str, "%d", Symbol);
	Display.print(str);
}

void DialerUI::SetBigSymbol(int Phase, int Symbol)
{
	Display.fillCircle(GATE_CENTRE_X, GATE_CENTRE_Y, GATE_MINOR_RADIUS-2, BLACK);

	if (Phase > 0)
	{
		int ExtraXOffset = 1;
		if (Symbol < 10)
		{
			ExtraXOffset = 2;
		}

		Display.setTextSize(Phase);
		Display.setTextColor(WHITE, BLACK);
		Display.setCursor(GATE_CENTRE_X - (Phase*TEXTSIZE_W/ExtraXOffset), GATE_CENTRE_Y -(Phase*TEXTSIZE_H));

		char str[3];
		sprintf(str, "%d", Symbol);
		Display.print(str);
		Display.drawCircle(GATE_CENTRE_X, GATE_CENTRE_Y, GATE_MINOR_RADIUS, WHITE);
	}
}
unsigned long timerZero = 0;

void DialerUI::Iniitalise() 
{
	Display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
	Display.clearDisplay();
	DrawStaticElements();
	timerZero = millis();
}

void DialerUI::DrawStaticElements()
{
	for (int Symbol=0; Symbol<7; ++Symbol)
	{
		Display.drawRect(SYMBOL_PANE_X, (SYMBOL_PANE_H-1)*Symbol, SYMBOL_PANE_W, SYMBOL_PANE_H, WHITE);
	}

	for (int Chevron=0; Chevron<9; ++Chevron)
	{
		SetChevron(Chevron, false);
	}

	Display.drawCircle(GATE_CENTRE_X, GATE_CENTRE_Y, GATE_MAJOR_RADIUS, WHITE);
	Display.drawCircle(GATE_CENTRE_X, GATE_CENTRE_Y, GATE_DIV_RADIUS, WHITE);
	Display.drawCircle(GATE_CENTRE_X, GATE_CENTRE_Y, GATE_MINOR_RADIUS, WHITE);
	Display.display();
}

void DialerUI::TickAnimation() 
{
	if (Display == nullptr)
	{
		return;
	}

	//Anim
	if (CurrentSymbol < MaxSymbol)
	{
		unsigned long TimerNew = millis();
		unsigned long Timer = TimerNew - PrevTime;
		PrevTime = TimerNew;

		if (Timer > 200 && Phase == AnimPhase::SymbSmall)
		{
			SetBigSymbol(1, Address[CurrentSymbol]);
			Stage = AnimPhase::SymbBig;
			Display.display();
		}
		else if (Timer > 400 && Phase == AnimPhase::SymbBig)
		{
			SetBigSymbol(2, Address[CurrentSymbol]);
			Phase = AnimPhase::SymFull;
			Display.display();
		}
		else if (Timer > 800 && Phase == AnimPhase::SymFull)
		{
			SetBigSymbol(3, address[CurrentSymbol]);
			Stage = AnimPhase::Pane;
			Display.display();
		}
		else if (Timer > 2200 && Phase == AnimPhase::Pane)
		{
			SetBigSymbol(0, Address[CurrentSymbol]);
			SetSymbolPane(CurrentSymbol, Address[CurrentSymbol]);
			SetChevron(ChevronActivationOrder[CurrentSymbol], true);
			Phase = AnimPhase::Reset;
			Display.display();
		}
		else if (Timer > 3000 && Phase == AnimPhase::Reset)
		{
			Phase = AnimPhase::SymbSmall;
			timerZero = timerNow;

			if (CurrentSymbol == MaxSymbol && bBigButton)
			{
				Display.fillCircle(GATE_CENTRE_X, GATE_CENTRE_Y, GATE_MINOR_RADIUS-2, WHITE);
				Display.drawCircle(GATE_CENTRE_X, GATE_CENTRE_Y, GATE_MINOR_RADIUS-10, BLACK);
				Display.drawCircle(GATE_CENTRE_X, GATE_CENTRE_Y, GATE_MINOR_RADIUS-5, BLACK);
				Display.drawCircle(GATE_CENTRE_X, GATE_CENTRE_Y, GATE_MINOR_RADIUS-15, BLACK);
				Display.drawCircle(GATE_CENTRE_X, GATE_CENTRE_Y, 2, BLACK);
				Display.display();
			}

			if (CurrentSymbol < MaxSymbol)
			{
				CurrentSymbol++;
			}
		}
	}
}
*/