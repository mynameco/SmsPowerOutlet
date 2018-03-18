#include "Application.h"
#include "Channel.h"
#include "Temperature.h"
#include "Console.h"
#include "Gsm.h"
#include "GsmPower.h"
#include "GsmProtocol.h"
#include "Commander.h"
#include "Beep.h"
#include "Button.h"
#include "Storage.h"

const char name0[] = "system";
const char name1[] = "channel1";
const char name2[] = "channel2";
const char name3[] = "channel3";
const char name4[] = "channel4";
const char name5[] = "channel5";

Channel channels[] =
{
	Channel(4, name1),
	Channel(5, name2),
	Channel(6, name3),
	Channel(7, name4),
	Channel(8, name5)
};

int channelCount = sizeof(channels) / sizeof(Channel);

Button buttons[] =
{
	Button(A0, 0),
	Button(A1, 1),
	Button(A2, 2),
	Button(A3, 3),
	Button(A4, 4),
	Button(A5) // кнопка для выключения и включения всех каналов
};

int buttonCount = sizeof(buttons) / sizeof(Button);

#ifdef TEMPERATURE_MODULE
Temperature temperature;

SensorName names[] =
{
	// Датчик температуры в щитке
	SensorName(OneWireAddress({ 0x28, 0x8f, 0xa7, 0x9b, 0x06, 0x00, 0x00, 0x16 }), name0),
	// Датчики температуры в комнатах
	SensorName(OneWireAddress({ 0x28, 0x64, 0xb3, 0x9b, 0x06, 0x00, 0x00, 0xa4 }), name1),
	SensorName(OneWireAddress({ 0x28, 0x64, 0xb3, 0x9b, 0x06, 0x00, 0x00, 0xa4 }), name2),
	SensorName(OneWireAddress({ 0x28, 0x64, 0xb3, 0x9b, 0x06, 0x00, 0x00, 0xa4 }), name3),
	SensorName(OneWireAddress({ 0x28, 0x64, 0xb3, 0x9b, 0x06, 0x00, 0x00, 0xa4 }), name4),
	SensorName(OneWireAddress({ 0x28, 0x64, 0xb3, 0x9b, 0x06, 0x00, 0x00, 0xa4 }), name5),
};

int nameCount = (sizeof(names) / sizeof(SensorName));
#endif

#ifdef CONSOLE_MODULE
Console console;
#endif

Gsm gsm;

GsmPower gsmPower;

GsmProtocol gsmProtocol;

Commander commander;

void Application::Setup()
{
	gsm.Setup();

	gsmPower.Setup();

	gsmProtocol.Setup();

	for (int i = 0; i < channelCount; i++)
		channels[i].Setup();

	for (int i = 0; i < buttonCount; i++)
		buttons[i].Setup();

#ifdef CONSOLE_LOG || CONSOLE_MODULE
	Serial.begin(9600);
#endif

	Storage::RestoreChannels();
}

void Application::Loop()
{
	for (int i = 0; i < buttonCount; i++)
		buttons[i].Route();

#ifdef TEMPERATURE_MODULE
	temperature.Route();
#endif

#ifdef CONSOLE_MODULE
	console.Route();
#endif

	gsm.Route();

	gsmPower.Route();

	gsmProtocol.Route();

	commander.Route();
}
