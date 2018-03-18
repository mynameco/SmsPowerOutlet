#ifndef GSM_POWER_H
#define GSM_POWER_H

#include "Application.h"

class GsmPower
{
public:
	enum State
	{
		NoneState,
		OffState,
		WaitOffState,
		CompleteOffState,
		OnState,
		WaitOnState,
		CompleteOnState,
		HardResetState,
		WaitHardResetState,
		CooldownHardResetState,
		CompleteState
	};

	void Setup();
	void Route();

	bool Restart();
	bool IsComplete();
	bool Release();

private:
	State state = NoneState;
	long lastTime;
};

#endif
