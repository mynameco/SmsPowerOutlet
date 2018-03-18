#include "Gsm.h"
#include "GsmProtocol.h"
#include "Commander.h"
#include "GsmPower.h"
#include "Beep.h"

extern Commander commander;
extern GsmPower gsmPower;
extern GsmProtocol gsmProtocol;

void Gsm::Setup()
{
	Serial1.begin(9600);
}

void Gsm::Route()
{
	if (state == NoneState)
	{
		state = RestartGsmState;
	}
	else if (state == RestartGsmState)
	{
		if (!gsmPower.Restart())
			return;

#ifdef CONSOLE_LOG
		PRINTLN(F("Restart gsm"));
#endif

		state = WaitRestartGsmState;
	}
	else if (state == WaitRestartGsmState)
	{
		if (gsmPower.IsComplete())
		{
			gsmPower.Release();

			state = AtState;
		}
	}
	else if (state == AtState)
	{
#ifdef CONSOLE_LOG
		PRINTLN();
		PRINTLN(F("AT"));
#endif

		if (!gsmProtocol.SetCommand("AT"))
		{
#ifdef CONSOLE_LOG
			PRINTLN(F("Error gsmProtocol state"));
#endif
			return;
		}

		state = WaitAtState;
	}
	else if (state == WaitAtState)
	{
		if (gsmProtocol.IsComplete())
		{
			gsmProtocol.Release();

			if (gsmProtocol.IsOk())
				state = ReadSmsCommandState;
			else
				state = RestartGsmState;
		}
	}
	else if (state == ReadSmsCommandState)
	{
		hasSms = false;

#ifdef CONSOLE_LOG
		PRINTLN();
		PRINT(F("Read sms : "));
		PRINTLN(smsIndexValue);
#endif

		StringArray<32> command;
		command += "AT+CMGR=";
		command += smsIndexValue;
		if (!gsmProtocol.SetCommand(command.GetString()))
		{
#ifdef CONSOLE_LOG
			PRINTLN(F("Error gsmProtocol state"));
#endif
			return;
		}

		state = WaitSmsCommandState;
	}
	else if (state == WaitSmsCommandState)
	{
		if (gsmProtocol.IsComplete())
		{
			const StringArray<256>& response = gsmProtocol.GetResponse();
			gsmProtocol.Release();

			if (gsmProtocol.IsOk())
			{
				int index = response.IndexOf("+CMGR:");
				if (index != -1)
				{
#ifdef CONSOLE_LOG
					PRINTLN(F("Sms found"));
#endif

					int index1 = response.IndexOf(",", index);
					if (index1 != -1)
					{
						int index2 = response.IndexOf(",", index1 + 1);
						if (index2 != -1)
						{
							smsPhone.CopyFrom(response, index1 + 1, index2);

#ifdef CONSOLE_LOG
							PRINT(F("Phone : "));
							PRINTLN(smsPhone.GetString());
#endif

							int index3 = response.IndexOf("\r\n", index2);
							if (index3 != -1)
							{
								int index4 = response.IndexOf("\r\n", index3 + 1);
								if (index4 != -1)
								{
									smsCommand.CopyFrom(response, index3 + 2, index4);

#ifdef CONSOLE_LOG
									PRINT(F("Text : "));
									PRINTLN(smsCommand.GetString());
#endif

									hasSms = true;
								}
							}
						}
					}

					state = DeleteSmsState;
				}
				else
				{
#ifdef CONSOLE_LOG
					PRINTLN(F("Sms not found in : "));
					PRINTLN(response.GetString());
#endif
					smsIndexValue++;
					state = ReadSmsCommandState;
				}
			}
			else
			{
				if (gsmProtocol.IsTimeout())
				{
					state = AtState;
				}
				else
				{
					// Индекс может быть заведомо больше, ошибка ожидаема
					state = ReadSmsCommandState;
					smsIndexValue = 1;
				}
			}
		}
	}
	else if (state == DeleteSmsState)
	{
#ifdef CONSOLE_LOG
		PRINTLN();
		PRINT(F("Delete sms : "));
		PRINTLN(smsIndexValue);
#endif

		StringArray<32> command;
		command += "AT+CMGD=";
		command += smsIndexValue;
		if (!gsmProtocol.SetCommand(command.GetString()))
		{
#ifdef CONSOLE_LOG
			PRINTLN(F("Error gsmProtocol state"));
#endif
			return;
		}

		state = WaitDeleteSmsState;
	}
	else if (state == WaitDeleteSmsState)
	{
		if (gsmProtocol.IsComplete())
		{
			gsmProtocol.Release();

			if (gsmProtocol.IsOk())
			{
				state = ExecuteCommandState;
				smsIndexValue++;
			}
			else
			{
				state = AtState;
			}
		}
	}
	else if (state == ExecuteCommandState)
	{
		if (!commander.SetCommand(smsCommand.GetString()))
			return;

		state = ComputeCommandState;
	}
	else if (state == ComputeCommandState)
	{
		if (commander.IsComplete())
		{
			commandResponse = commander.GetResponse();
			commander.Release();

			if (commander.IsOk())
			{
				commandResponse.Trim();

				Beep(1);

#ifdef CONSOLE_LOG
				PRINT(F("Command response : "));
				PRINTLN(commandResponse.GetString());
#endif

				state = SendSmsResponseState;
			}
			else
			{
				state = ReadSmsCommandState;
			}
		}
	}
	else if (state == SendSmsResponseState)
	{
#ifdef CONSOLE_LOG
		PRINTLN();
		PRINTLN(F("Send sms"));
#endif

		StringArray<32> command;
		command += "AT+CMGS=";
		command += smsPhone;

		if (!gsmProtocol.SetCommand(command.GetString(), commandResponse.GetString()))
		{
#ifdef CONSOLE_LOG
			PRINTLN(F("Error gsmProtocol state"));
#endif
			return;
		}

		state = WaitSendSmsResponseState;
	}
	else if (state == WaitSendSmsResponseState)
	{
		if (gsmProtocol.IsComplete())
		{
			gsmProtocol.Release();

			state = ReadSmsCommandState;
		}
	}
	else if (state == StopState)
	{
	}
}
