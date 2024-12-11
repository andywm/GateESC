
#include "dhd.h"

struct SGCDialScreen : public IDialingDevice //: public RenderPage
{
	virtual void Initialise() override;
	virtual void Enabled(bool bEnable) override;
	virtual void Reset() override;
	virtual uint8 PollAndEat() override;
	virtual void Confirm(uint8 SymbId, uint8 Symb) override;
	virtual bool IsActivated() override;
};