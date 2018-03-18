#ifndef COMMANDER_H
#define COMMANDER_H

#include "Application.h"
#include "StringArray.h"
#include "Temperature.h"

class Commander
{
public:
	enum State
	{
		NoneState,
		ExecuteState,
		TemperatureState,
		WaitTemperatureState,
		BalanceState,
		WaitBalanceState,
		GsmPowerState,
		WaitGsmPowerState,
		GsmProtocolState,
		WaitGsmProtocolState,
		CompleteState
	};
	
	void Route();

	bool SetCommand(const char* command, bool console = false);
	bool IsComplete();
	bool IsOk();
	bool Release();
	const StringArray<160>& GetResponse();

private:
	void ChangeChannel(int channel, bool state);
	bool ComputeKeyState(const StringArray<8>& key, bool state);
#ifdef TEMPERATURE_MODULE
	void WriteSensorName(const SensorValue& sensor);
#endif
	void WriteStateChannel(int index);

private:
	long lastTime;
	bool ok = false;
	bool console;
	StringArray<32> command;
	StringArray<160> response;
	State state = NoneState;
};

#endif
