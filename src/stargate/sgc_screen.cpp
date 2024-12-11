#include "dhd.h"
#include "sgc_screen.h"


#include "oled/oled_display_controller.h"
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

void SGCDialScreen::Initialise()
{

}

void SGCDialScreen::Enabled(bool bEnable)
{

}

void SGCDialScreen::Reset()
{

}

uint8 SGCDialScreen::PollAndEat()
{

}
void SGCDialScreen::Confirm(uint8 SymbId, uint8 Symb)
{

}

bool SGCDialScreen::IsActivated()
{

}

/*
struct SGCDialScreen : public RenderPage
{
	enum class AnimPhase {SequenceInit, SymbSmall, SymbBig, SymFull, Pane, Reset, Lock};
	int Address[9] = {0};

	unsigned long PrevTime = 0;
	int CurrentSymbol = 0;
	int MaxSymbol = 0;
	bool bBigButton = false;
	unsigned long timerZero = 0;

	AnimPhase Phase = AnimPhase::SymbSmall;

	void SetChevron(Adafruit_SSD1306& Display, int Chevron, bool bOn)
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

	void SetSymbolPane(Adafruit_SSD1306& Display, int Chevron, int Symbol)
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

	void SetBigSymbol(Adafruit_SSD1306& Display, int Phase, int Symbol)
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

	void DrawStaticElements(Adafruit_SSD1306& Display)
	{
		for (int Symbol=0; Symbol<7; ++Symbol)
		{
			Display.drawRect(SYMBOL_PANE_X, (SYMBOL_PANE_H-1)*Symbol, SYMBOL_PANE_W, SYMBOL_PANE_H, WHITE);
		}

		for (int Chevron=0; Chevron<9; ++Chevron)
		{
			SetChevron(Display, Chevron, false);
		}

		Display.drawCircle(GATE_CENTRE_X, GATE_CENTRE_Y, GATE_MAJOR_RADIUS, WHITE);
		Display.drawCircle(GATE_CENTRE_X, GATE_CENTRE_Y, GATE_DIV_RADIUS, WHITE);
		Display.drawCircle(GATE_CENTRE_X, GATE_CENTRE_Y, GATE_MINOR_RADIUS, WHITE);
		Display.display();
	}

	void Draw(Adafruit_SSD1306& Display) 
	{
		//Animate Display
		if (CurrentSymbol < MaxSymbol)
		{
			if (CurrentSymbol == 0 && Phase == AnimPhase::SequenceInit)
			{
				timerZero = millis();
				Phase = AnimPhase::SymbSmall;
			}

			unsigned long TimerNew = millis();
			unsigned long Timer = TimerNew - PrevTime;
			PrevTime = TimerNew;

			if (Timer > 200 && Phase == AnimPhase::SymbSmall)
			{
				SetBigSymbol(Display, 1, Address[CurrentSymbol]);
				Phase = AnimPhase::SymbBig;
				Display.display();
			}
			else if (Timer > 400 && Phase == AnimPhase::SymbBig)
			{
				SetBigSymbol(Display, 2, Address[CurrentSymbol]);
				Phase = AnimPhase::SymFull;
				Display.display();
			}
			else if (Timer > 800 && Phase == AnimPhase::SymFull)
			{
				SetBigSymbol(Display, 3, Address[CurrentSymbol]);
				Phase = AnimPhase::Pane;
				Display.display();
			}
			else if (Timer > 2200 && Phase == AnimPhase::Pane)
			{
				SetBigSymbol(Display, 0, Address[CurrentSymbol]);
				SetSymbolPane(Display, CurrentSymbol, Address[CurrentSymbol]);
				SetChevron(Display, ChevronActivationOrder[CurrentSymbol], true);
				Phase = AnimPhase::Reset;
				Display.display();
			}
			else if (Timer > 3000 && Phase == AnimPhase::Reset)
			{
				Phase = AnimPhase::SymbSmall;
				if (CurrentSymbol == MaxSymbol)
				{
					Phase = AnimPhase::Lock;
				}

				CurrentSymbol++;
			}
			else if (bBigButton && Phase == AnimPhase::Lock)
			{
				Display.fillCircle(GATE_CENTRE_X, GATE_CENTRE_Y, GATE_MINOR_RADIUS-2, WHITE);
				Display.drawCircle(GATE_CENTRE_X, GATE_CENTRE_Y, GATE_MINOR_RADIUS-10, BLACK);
				Display.drawCircle(GATE_CENTRE_X, GATE_CENTRE_Y, GATE_MINOR_RADIUS-5, BLACK);
				Display.drawCircle(GATE_CENTRE_X, GATE_CENTRE_Y, GATE_MINOR_RADIUS-15, BLACK);
				Display.drawCircle(GATE_CENTRE_X, GATE_CENTRE_Y, 2, BLACK);
				Display.display();

				Phase = AnimPhase::SequenceInit;
			}
		}
	}
};
*/

/*
void DialInputController::Update()
{
	//Mock Dialing sequence. Eventually this class will handle DHD inputs.
	if (bExecuteHardcodedDialSeq && DialGate && Activate)
	{
		bExecuteHardcodedDialSeq = false;

		DialGate(27);
		DialGate(6);
		DialGate(36);
		DialGate(12);
		DialGate(9);
		DialGate(2);
		DialGate(39);
		Activate();
	}
}*/