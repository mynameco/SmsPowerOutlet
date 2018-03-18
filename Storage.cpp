#include "Storage.h"
#include "EEPROM.h"
#include "Channel.h"

extern Channel channels[];
extern int channelCount;

void Storage::StoreChannels()
{
	if (channelCount > 8)
	{
#ifdef CONSOLE_LOG
		PRINTLN(F("Channels count error"));
#endif
		return;
	}

	byte data = 0;
	for (int i = 0; i < channelCount; i++)
	{
		if (channels[i].GetState())
			data |= (1 << i);
	}

	EEPROM.write(STORAGE_CHANNELS, data);
}

void Storage::RestoreChannels()
{
	if (channelCount > 8)
	{
#ifdef CONSOLE_LOG
		PRINTLN(F("Channels count error"));
#endif
		return;
	}

	byte data = EEPROM.read(STORAGE_CHANNELS);

	for (int i = 0; i < channelCount; i++)
		channels[i].SetState((data & (1 << i)) != 0);
}

