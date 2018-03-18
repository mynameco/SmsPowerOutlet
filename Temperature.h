#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include "Application.h"

#ifdef TEMPERATURE_MODULE

#include "OneWire.h"
#include "OneWireAddress.h"

class SensorName
{
public:
	SensorName(OneWireAddress address, const char* name)
	{
		this->address = address;
		this->name = name;
	}

public:
	OneWireAddress address;
	const char* name;
};

class SensorValue
{
public:
	OneWireAddress address;
	int8_t value = 0;
};

class SensorValues
{
public:
	SensorValue sensors[SENSOR_MAX];
	byte count = 0;
};

class Temperature
{
private:
	enum State
	{
		NoneState,
		NextState,
		WaitingState,
		ComputeState,
		CompleteState
	};

public:
	Temperature();

	void Route();

	bool Request();
	bool IsComplete();
	SensorValues GetValuesAndReset();

private:
	bool Next();
	void ComputeCurrent();

private:
	SensorValues values;
	long lastTime;
	OneWireAddress storedAddress;
	State state = NoneState;
};

#endif
#endif
