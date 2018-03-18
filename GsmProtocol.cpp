#include "GsmProtocol.h"

void GsmProtocol::Setup()
{
	Serial1.begin(9600);
}

void GsmProtocol::Route()
{
	if (state == NoneState)
	{
	}
	else if (state == SendState)
	{
#ifdef CONSOLE_LOG
		PRINT(F("Send command : '"));
		PRINT(command.GetString());
		PRINTLN(F("'"));
#endif

		if (Serial1.available() > 0)
		{
#ifdef CONSOLE_LOG
			PRINT(F("["));
#endif
			while (Serial1.available() > 0)
			{
				char sym = (char)Serial1.read();
#ifdef CONSOLE_LOG
				PRINT(sym);
#endif
			}
#ifdef CONSOLE_LOG
			PRINTLN(F("]"));
#endif
		}

		buffer = "";
		countInLine = 0;
		lastTime = millis();

		Serial1.println(command.GetString());

		state = WaitState;
	}
	else if (state == WaitState)
	{
		while (Serial1.available() > 0)
		{
			char sym = (char)Serial1.read();
			if (sym == '\r' || sym == '\n')
				countInLine = 0;

			// Не всю строку считываем
			if (countInLine < MAX_IN_LINE || balance)
			{
				buffer += sym;
				countInLine++;
			}
		}

		if (balance)
		{
			if ((millis() - lastTime) > (TIMEOUT_TIME * TIMEOUT_TIME))
			{
#ifdef CONSOLE_LOG
				PRINTLN(F("Timeout"));
#endif

				ok = false;
				timeout = true;
				lastTime = millis();
				state = DelayState;
				return;
			}

			int index = buffer.IndexOf("+CUSD:");
			if (index != -1)
			{
				int index2 = buffer.IndexOf("\n", index);
				if (index2 != -1)
				{
#ifdef CONSOLE_LOG
					PRINT(F("Response Ok : '"));
					PRINT(buffer.GetString());
					PRINTLN(F("'"));
#endif

					ok = true;
					lastTime = millis();
					state = DelayState;
				}
			}
		}
		else
		{
			if ((millis() - lastTime) > TIMEOUT_TIME)
			{
#ifdef CONSOLE_LOG
				PRINT(F("Timeout : "));
				PRINTLN(countTimeout);
#endif

				countTimeout++;
				if (countTimeout < TIMEOUT_COUNT)
				{
					state = SendState;
					return;
				}

				ok = false;
				timeout = true;
				lastTime = millis();
				state = DelayState;
			}

			if (buffer.EndsWith("OK\r\n"))
			{
#ifdef CONSOLE_LOG
				PRINT(F("Response Ok : '"));
				PRINT(buffer.GetString());
				PRINTLN(F("'"));
#endif

				ok = true;
				lastTime = millis();
				state = DelayState;
			}
			else if (data.GetSize() != 0 && buffer.EndsWith(">"))
			{
#ifdef CONSOLE_LOG
				PRINT(F("Response Data Ok : '"));
				PRINT(buffer.GetString());
				PRINTLN(F("'"));
#endif

				state = DataState;
			}
			else if (buffer.EndsWith("ERROR\r\n"))
			{
#ifdef CONSOLE_LOG
				PRINT(F("Response Error : '"));
				PRINT(buffer.GetString());
				PRINTLN(F("'"));
#endif

				ok = false;
				timeout = false;
				lastTime = millis();
				state = DelayState;
			}
		}
	}
	else if (state == DataState)
	{
#ifdef CONSOLE_LOG
		PRINT(F("Send data : '"));
		PRINT(data.GetString());
		PRINTLN(F("'"));
#endif

		if (Serial1.available() > 0)
		{
#ifdef CONSOLE_LOG
			PRINT(F("["));
#endif
			while (Serial1.available() > 0)
			{
				char sym = (char)Serial1.read();
#ifdef CONSOLE_LOG
				PRINT(sym);
#endif
			}
#ifdef CONSOLE_LOG
			PRINTLN(F("]"));
#endif
		}

		buffer = "";
		countInLine = 0;
		lastTime = millis();

		Serial1.print(data.GetString());
		Serial1.print((char)26);

		state = WaitState;
	}
	else if (state == DelayState)
	{
		if ((millis() - lastTime) > GSM_COMMAND_DELAY)
		{
			state = CompleteState;
		}
	}
	else if (state == CompleteState)
	{
	}
}

bool GsmProtocol::SetCommand(const char* command, const char* data, bool balance)
{
	if (state != NoneState)
		return false;

	this->command = command;
	this->data = data;
	this->balance = balance;

	state = SendState;
	countTimeout = 0;

	return true;
}

bool GsmProtocol::IsReady()
{
	return state == NoneState;
}

bool GsmProtocol::IsComplete()
{
	return state == CompleteState;
}

bool GsmProtocol::IsOk()
{
	return ok;
}

bool GsmProtocol::IsTimeout()
{
	return timeout;
}

bool GsmProtocol::Release()
{
	if (!IsComplete())
		return false;

	state = NoneState;
	return true;
}

const StringArray<256>&  GsmProtocol::GetResponse()
{
	return buffer;
}