#ifndef BUTTON_H
#define BUTTON_H

#include "Application.h"
#include "Channel.h"

class Button
{
private:
	Button();

	enum State
	{
		NoneState,
		CheckState,
		DelayState,
		ComputeState
	};

public:
	Button(int inputPin, int channelIndex = -1);

	void Setup();
	void Route();

private:
	void DoAction();

private:
	int inputPin;
	long lastTime = 0;
	int lastState = LOW;
	State state = NoneState;
	int channelIndex;
};

#endif
