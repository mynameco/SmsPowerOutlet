#include "Temperature.h"

#ifdef TEMPERATURE_MODULE

OneWire oneWire(ONE_WIRE_PORT);

Temperature::Temperature()
{
}

bool Temperature::Request()
{
	if (state != NoneState)
		return false;

	values.count = 0;
	state = NextState;

	return true;
}

void Temperature::Route()
{
	if (state == NoneState)
	{
	}
	else if (state == NextState)
	{
		if (values.count >= SENSOR_MAX)
		{
			state = CompleteState;
			return;
		}
		if (!Next())
		{
			state = CompleteState;
			return;
		}

		lastTime = millis();
		state = WaitingState;
	}
	else if (state == WaitingState)
	{
		if ((millis() - lastTime) > 1000)
		{
			state = ComputeState;
		}
	}
	else if (state == ComputeState)
	{
		ComputeCurrent();
		values.count++;
		state = NextState;
	}
	else if (state == CompleteState)
	{
	}
}

bool Temperature::IsComplete()
{
	return state == CompleteState;
}

SensorValues Temperature::GetValuesAndReset()
{
	state = NoneState;
	return values;
}

bool Temperature::Next()
{
	byte addr[8];

	if (!oneWire.search(addr))
	{
		oneWire.reset_search();
		//delay(250);
		return false;
	}

	// CRC is not valid!
	if (OneWire::crc8(addr, 7) != addr[7])
	{
#ifdef CONSOLE_LOG
		PRINTLN(F("CRC is not valid!"));
#endif
		return false;
	}

	// the first ROM byte indicates which chip
	// 0x28 DS18B20
	// 0x22 DS1822
	if (addr[0] != 0x28 && addr[0] != 0x22)
	{
#ifdef CONSOLE_LOG
		PRINTLN(F("Sensor not support"));
#endif
		return false;
	}

	oneWire.reset();
	oneWire.select(addr);
	oneWire.write(0x44, 1); // start conversion, with parasite power on at the end

	storedAddress = addr;

	return true;
}

void Temperature::ComputeCurrent()
{
	byte present = oneWire.reset();
	oneWire.select(storedAddress.GetData());
	oneWire.write(0xBE); // Read Scratchpad

	byte data[12];
	for (int i = 0; i < 9; i++) // we need 9 bytes
		data[i] = oneWire.read();

	// Convert the data to actual temperature
	// because the result is a 16 bit signed integer, it should
	// be stored to an "int16_t" type, which is always 16 bits
	// even when compiled on a 32 bit processor.
	int16_t raw = (data[1] << 8) | data[0];
	byte cfg = (data[4] & 0x60);
	// at lower res, the low bits are undefined, so let's zero them
	if (cfg == 0x00)
		raw = raw & ~7;  // 9 bit resolution, 93.75 ms
	else if (cfg == 0x20)
		raw = raw & ~3; // 10 bit res, 187.5 ms
	else if (cfg == 0x40)
		raw = raw & ~1; // 11 bit res, 375 ms

	float celsius = (float)raw / 16.0;

	values.sensors[values.count].address = storedAddress;
	values.sensors[values.count].value = (int8_t)celsius;
}

#endif
