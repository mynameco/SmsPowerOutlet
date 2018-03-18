#include "Commander.h"
#include "Channel.h"
#include "GsmPower.h"
#include "GsmProtocol.h"
#include "Console.h"
#include "Storage.h"

extern Channel channels[];
extern int channelCount;

#ifdef TEMPERATURE_MODULE
extern Temperature temperature;
extern SensorName names[];
extern int nameCount;
#endif

extern GsmPower gsmPower;
extern GsmProtocol gsmProtocol;

void Commander::ChangeChannel(int channel, bool on)
{
	channels[channel].SetState(on);
	WriteStateChannel(channel);
}

bool Commander::ComputeKeyState(const StringArray<8>& key, bool state)
{
	if (key == "all")
	{
		for (int i = 0; i < channelCount; i++)
			ChangeChannel(i, state);

		Storage::StoreChannels();
	}
	else if ((key.GetSize() == 1) && (key[0] >= '1') && (key[0] <= '5'))
	{
		ChangeChannel((key[0] - '1'), state);

		Storage::StoreChannels();
	}
	else
	{
		for (int index = 0; index < channelCount; index++)
		{
			if (channels[index].GetName() != 0 && key == channels[index].GetName())
			{
				ChangeChannel(index, state);
				return true;
			}
		}

		return false;
	}
	return true;
}

void Commander::Route()
{
	if (state == NoneState)
	{
	}
	else if (state == ExecuteState)
	{
#ifdef CONSOLE_MODULE
		if (command.Equal("gsm_power", true) && console)
		{
			state = GsmPowerState;
		}
		else if (command.StartsWith("AT") && console)
		{
			state = GsmProtocolState;
		}
		else
#endif
			if (command.EndsWith("on", true) || command.EndsWith("off", true))
			{
				bool on = command.EndsWith("on", true);

				StringArray<8> key;
				key.CopyFrom(command, 0, command.GetSize() - (on ? 2 : 3));
				key.Trim();
				key.ToLowerCase();

				ok = ComputeKeyState(key, on);
				state = CompleteState;
			}
			else if (command.Equal("state", true))
			{
				for (int i = 0; i < channelCount; i++)
					WriteStateChannel(i);

				ok = true;
				state = CompleteState;
			}
#ifdef TEMPERATURE_MODULE
			else if (command.Equal("temp", true))
			{
				state = TemperatureState;
			}
#endif
			else if (command.Equal("balance", true))
			{
				state = BalanceState;
			}
			else if (command == "?")
			{
				response = "";
				for (int index = 0; index < channelCount; index++)
				{
					response += (index + 1);
					if (channels[index].GetName() != 0)
					{
						response += "/";
						response += channels[index].GetName();
					}
					response += "   on/off\r\n";
				}
				response += "all   on/off\r\n";
				response += "state\r\n";
#ifdef TEMPERATURE_MODULE
				response += "temp\r\n";
#endif
				response += "balance\r\n";

#ifdef CONSOLE_MODULE
				if (console)
				{
					response += "gsm_power\r\n";
					response += "AT...\r\n";
					response += "AT+CMGR=<index> : read sms\r\n";
					response += "AT+CMGS=\"<phone>\"|<text> : send sms\r\n";
				}
#endif

				ok = true;
				state = CompleteState;
			}
			else
			{
#ifdef CONSOLE_LOG
				PRINTLN(F("Command not found"));
#endif

				ok = false;
				state = CompleteState;
			}
	}
#ifdef TEMPERATURE_MODULE
	else if (state == TemperatureState)
	{
		if (!temperature.Request())
		{
			PRINTLN(F("Error command state"));
			return;
		}

		state = WaitTemperatureState;
	}
	else if (state == WaitTemperatureState)
	{
		if (temperature.IsComplete())
		{
			SensorValues values = temperature.GetValuesAndReset();

			for (int index = 0; index < values.count; index++)
			{
				response += values.sensors[index].value;
				response += "` : ";
				WriteSensorName(values.sensors[index]);
				response += "\r\n";
			}

			ok = true;
			state = CompleteState;
		}
	}
#endif
	else if (state == BalanceState)
	{
		if (!gsmProtocol.SetCommand("ATD#100#", "", true))
		{
#ifdef CONSOLE_LOG
			PRINTLN(F("Error command state"));
#endif
			return;
		}

		state = WaitBalanceState;
	}
	else if (state == WaitBalanceState)
	{
		if (gsmProtocol.IsComplete())
		{
			const StringArray<256> gsmResponse = gsmProtocol.GetResponse();
			gsmProtocol.Release();

			if (gsmProtocol.IsOk())
			{
				int index1 = gsmResponse.IndexOf("+CUSD:");
				if (index1 != -1)
				{
					int index2 = gsmResponse.IndexOf("\"", index1);
					if (index2 != -1)
					{
						int index3 = gsmResponse.IndexOf("\"", index2 + 1);
						if (index3 != -1)
						{
							response.CopyFrom(gsmResponse, index2 + 1, index3);

							ok = true;
							state = CompleteState;
							return;
						}
					}
				}
			}

			ok = false;
			state = CompleteState;
		}
	}
	else if (state == GsmPowerState)
	{
		if (!gsmPower.Restart())
		{
#ifdef CONSOLE_LOG
			PRINTLN(F("Error command state"));
#endif
			return;
		}

		state = WaitGsmPowerState;
	}
	else if (state == WaitGsmPowerState)
	{
		if (gsmPower.IsComplete())
		{
			gsmPower.Release();

			response = "Gsm restarted";

			ok = true;
			state = CompleteState;
		}
	}
	else if (state == GsmProtocolState)
	{
		if (!gsmProtocol.IsReady())
		{
#ifdef CONSOLE_LOG
			PRINTLN(F("Error command state"));
#endif
			return;
		}

		int index = command.IndexOf("|");
		if (index != -1)
		{
			StringArray<32> data;
			data.CopyFrom(command, index + 1);
			command.Substring(0, index);

#ifdef CONSOLE_LOG
			PRINT(F("Command : "));
			PRINTLN(command.GetString());
			PRINT(F("Data : "));
			PRINTLN(data.GetString());
#endif

			gsmProtocol.SetCommand(command.GetString(), data.GetString());
		}
		else
		{
			gsmProtocol.SetCommand(command.GetString());
		}

		state = WaitGsmProtocolState;
	}
	else if (state == WaitGsmProtocolState)
	{
		if (gsmProtocol.IsComplete())
		{
			response = gsmProtocol.GetResponse();

			gsmProtocol.Release();

			ok = gsmProtocol.IsOk();
			state = CompleteState;
		}
	}
	else if (state == CompleteState)
	{
	}
}

#ifdef TEMPERATURE_MODULE

void Commander::WriteSensorName(const SensorValue& sensor)
{
	for (int index = 0; index < nameCount; index++)
	{
		if (names[index].address == sensor.address)
		{
			response += names[index].name;
			return;
		}
	}

	const byte* data = sensor.address.GetData();
	for (int index = 0; index < ADDRESS_COUNT; index++)
	{
		byte value = data[index];
		response.AddHex(value);
		if (index < (ADDRESS_COUNT - 1))
			response += '.';
	}
}

#endif

bool Commander::SetCommand(const char* command, bool console)
{
	if (state != NoneState)
		return false;

	this->command = command;
	this->console = console;

	response = "";

	state = ExecuteState;

#ifdef CONSOLE_LOG
	PRINT(F("Set command : "));
	PRINTLN(this->command.GetString());
#endif

	return true;
}

bool Commander::IsComplete()
{
	return state == CompleteState;
}

bool Commander::IsOk()
{
	return ok;
}

bool Commander::Release()
{
	if (!IsComplete())
		return false;

	state = NoneState;
	return true;
}

const StringArray<160>& Commander::GetResponse()
{
	return response;
}

void Commander::WriteStateChannel(int index)
{
	response += index + 1;
	if (channels[index].GetName() != 0)
	{
		response += "(";
		response += channels[index].GetName();
		response += ")";
	}
	response += "   ";
	response += channels[index].GetState() ? "on" : "off";
	response += "\r\n";
}

