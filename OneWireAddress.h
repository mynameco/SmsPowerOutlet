#ifndef ONE_WIRE_ADDRESS_H
#define ONE_WIRE_ADDRESS_H

#include "Arduino.h"

#define ADDRESS_COUNT 8

class OneWireAddress
{
public:
	OneWireAddress()
	{
		memset(data, 0, ADDRESS_COUNT);
	}

	OneWireAddress(const byte (&value)[ADDRESS_COUNT])
	{
		memcpy(data, value, ADDRESS_COUNT);
	}

	OneWireAddress(const OneWireAddress& value)
	{
		memcpy(data, value.data, ADDRESS_COUNT);
	}

	void operator=(const byte (&value)[ADDRESS_COUNT])
	{
		memcpy(data, value, ADDRESS_COUNT);
	}

	void operator=(const OneWireAddress& value)
	{
		memcpy(data, value.data, ADDRESS_COUNT);
	}

	bool operator==(const OneWireAddress& value) const
	{
		return memcmp(data, value.data, ADDRESS_COUNT) == 0;
	}

	const byte* GetData() const
	{
		return data;
	}

private:
	byte data[ADDRESS_COUNT];
};

#endif
