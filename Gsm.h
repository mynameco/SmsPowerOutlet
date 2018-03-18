#ifndef GSM_H
#define GSM_H

#include "Application.h"
#include "GsmProtocol.h"

class Gsm
{
public:
	enum State
	{
		NoneState,
		RestartGsmState,
		WaitRestartGsmState,

		AtState,
		WaitAtState,

		ReadSmsCommandState,
		WaitSmsCommandState,

		DeleteSmsState,
		WaitDeleteSmsState,

		ExecuteCommandState,
		ComputeCommandState,

		SendSmsResponseState,
		WaitSendSmsResponseState,

		StopState,
	};

	void Setup();
	void Route();

private:
	State state = NoneState;
	long lastTime;
	int smsIndexValue = 1;
	StringArray<16> smsPhone;
	StringArray<16> smsCommand;
	StringArray<160> commandResponse;
	bool hasSms;
};

#endif
