#ifndef CONSOLE_H
#define CONSOLE_H

#include "Application.h"

#ifdef CONSOLE_MODULE

#include "StringArray.h"

class Console
{
public:
	enum State
	{
		NoneState,
		ReadState,
		WaitState,
		ComputeState
	};

	void Route();

private:
	StringArray<32> command;
	State state = NoneState;
};

#endif
#endif
