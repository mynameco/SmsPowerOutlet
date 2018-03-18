#ifndef GSM_PROTOCOL_H
#define GSM_PROTOCOL_H

#include "Application.h"
#include "StringArray.h"

class GsmProtocol
{
public:
	enum State
	{
		NoneState,
		SendState,
		WaitState,
		DataState,
		DelayState,
		CompleteState
	};

	void Setup();
	void Route();

	bool SetCommand(const char* command, const char* data = "", bool balance = false);
	bool IsReady();
	bool IsComplete();
	bool IsOk();
	bool IsTimeout();
	bool Release();
	const StringArray<256>& GetResponse();

private:
	long lastTime;
	bool ok;
	bool timeout;
	StringArray<256> buffer;
	StringArray<32> command;
	StringArray<160> data;
	State state = NoneState;
	int countInLine;
	int countTimeout;
	bool balance;
};

#endif