#include "Channel.h"

// Эти реле инверсные
#define RELAY_ON LOW
#define RELAY_OFF HIGH

Channel::Channel()
{
}

Channel::Channel(int outputPin, const char* name)
{
	this->outputPin = outputPin;
	this->name = name;
}

void Channel::Setup()
{
	pinMode(outputPin, OUTPUT);

	UpdateState();
}

void Channel::SetState(bool state)
{
	if (channelState != state)
	{
		channelState = state;

		UpdateState();
	}
}

int Channel::GetState()
{
	return channelState;
}

void Channel::UpdateState()
{
	digitalWrite(outputPin, channelState ? RELAY_ON : RELAY_OFF);
}

const char* Channel::GetName() const
{
	return name;
}
