#ifndef CHANNEL_H
#define CHANNEL_H

#include "Application.h"

class Channel
{
private:
	Channel();

public:
	Channel(int outputPin, const char* name = 0);

	void Setup();

	void SetState(bool state);
	int GetState();
	const char* GetName() const;

private:
	void UpdateState();

private:
	int outputPin;
	const char* name;
	bool channelState;
};

#endif
