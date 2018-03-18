#ifndef APPLICATION_H
#define APPLICATION_H

#include "Arduino.h"

#define SENSOR_MAX 6
#define MAX_IN_LINE 48
#define ONE_WIRE_PORT 10
#define UNBOUNCE 30

#define TEMPERATURE_MODULE

//#define CONSOLE_MODULE
//#define CONSOLE_LOG
#ifdef CONSOLE_LOG
#define PRINT(text) if (Serial)  { Serial.print(text); }
#define PRINTLN(text) if (Serial)  { Serial.println(text); }
#else
#define PRINT(text)
#define PRINTLN(text)
#endif

#define TIMEOUT_COUNT 4
#define TIMEOUT_TIME 5000
#define GSM_COMMAND_DELAY 200

#define BEEP_MODULE
#define BEEP_PIN 11
#define BEEP_DELAY1 50
#define BEEP_DELAY2 80

#define GSM_POWER_CHECK 3
#define GSM_POWER 2
#define GSM_POWER_RELAY 12

#define STORAGE_CHANNELS 0

class Application
{
public:
	void Setup();
	void Loop();
};

#endif
