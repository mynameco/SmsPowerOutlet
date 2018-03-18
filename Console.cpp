#include "Console.h"
#include "Commander.h"

#ifdef CONSOLE_MODULE

extern Commander commander;

void Console::Route()
{
	if (state == NoneState)
	{
		if (Serial)
		{
			PRINTLN(F("Ready"));
			state = ReadState;
		}
	}
	else if (state == ReadState)
	{
		if (Serial.available() > 0)
		{
			command = "";

			while (Serial.available() > 0)
				command += (char)Serial.read();
			command.Trim();

			PRINT(F("command : '"));
			PRINT(command.GetString());
			PRINTLN(F("'"));

			state = WaitState;
		}
	}
	else if (state == WaitState)
	{
		if (commander.SetCommand(command.GetString(), true))
		{
			state = ComputeState;
		}
	}
	else if (state == ComputeState)
	{
		if (commander.IsComplete())
		{
			const StringArray<160>& response = commander.GetResponse();
			commander.Release();

			if (commander.IsOk())
			{
				PRINTLN(F("Command execute Ok : "));
				PRINTLN(response.GetString());
			}
			else
			{
				PRINTLN(F("Command execute Error"));
			}

			state = ReadState;
		}
	}
}

#endif
