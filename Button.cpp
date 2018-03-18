#include "Button.h"
#include "Storage.h"

#define IS_PRESSED (!digitalRead(inputPin))

extern Channel channels[];
extern int channelCount;

Button::Button()
{
}

Button::Button(int inputPin, int channelIndex)
{
	this->inputPin = inputPin;
	this->channelIndex = channelIndex;
}

void Button::Setup()
{
	pinMode(inputPin, INPUT);

	// подтягивающий резистор
	digitalWrite(inputPin, HIGH);
}

void Button::Route()
{
	if (state == NoneState)
	{
		state = CheckState;
	}
	else if (state == CheckState)
	{
		int buttonState = IS_PRESSED;
		if (lastState != buttonState)
		{
			lastTime = millis();
			state = DelayState;
		}
	}
	else if (state == DelayState)
	{
		if ((millis() - lastTime) > UNBOUNCE)
		{
			state = ComputeState;
		}
	}
	else if (state == ComputeState)
	{
		int buttonState = IS_PRESSED;
		if (lastState != buttonState)
		{
			lastState = buttonState;
			if (lastState == HIGH)
				DoAction();
		}
		state = CheckState;
	}
}

void Button::DoAction()
{
	if (channelIndex != -1)
	{
		channels[channelIndex].SetState(!channels[channelIndex].GetState());
	}
	else
	{
		bool any = false;
		for (int i = 0; i < channelCount; i++)
		{
			if (channels[i].GetState())
			{
				any = true;
				break;
			}
		}

		for (int i = 0; i < channelCount; i++)
			channels[i].SetState(!any);
	}

	Storage::StoreChannels();
}
