#include "GsmPower.h"
#include "Arduino.h"
#include "Beep.h"

// Эти реле прямые
#define RELAY_ON HIGH
#define RELAY_OFF LOW

void GsmPower::Setup()
{
	pinMode(GSM_POWER, OUTPUT);
	pinMode(GSM_POWER_CHECK, INPUT);
	pinMode(GSM_POWER_RELAY, OUTPUT);

	digitalWrite(GSM_POWER_RELAY, RELAY_OFF);
}

void GsmPower::Route()
{
	if (state == NoneState)
	{
	}
	else if (state == OffState)
	{
		Beep(2);

		if (digitalRead(GSM_POWER_CHECK) == HIGH)
		{
			digitalWrite(GSM_POWER, HIGH);

			lastTime = millis();
			state = WaitOffState;
		}
		else
		{
			state = OnState;
		}
	}
	else if (state == WaitOffState)
	{
		if ((millis() - lastTime) > 3000)
		{
			digitalWrite(GSM_POWER, LOW);

			lastTime = millis();
			state = CompleteOffState;
		}
	}
	else if (state == CompleteOffState)
	{
		if ((millis() - lastTime) > 3000)
		{
			state = OnState;

#ifdef CONSOLE_LOG
			PRINTLN();
			PRINTLN((digitalRead(POWER_CHECK) == LOW ? F("POWER OFF") : F("POWER ON")));
#endif

			// Не выключился
			if (digitalRead(GSM_POWER_CHECK) == HIGH)
			{
				Beep(3);
				state = HardResetState;
			}
		}
	}
	else if (state == OnState)
	{
		digitalWrite(GSM_POWER, HIGH);

		lastTime = millis();
		state = WaitOnState;
	}
	else if (state == WaitOnState)
	{
		if ((millis() - lastTime) > 3000)
		{
			digitalWrite(GSM_POWER, LOW);

			lastTime = millis();
			state = CompleteOnState;
		}
	}
	else if (state == CompleteOnState)
	{
		if ((millis() - lastTime) > 3000)
		{
			state = CompleteState;

#ifdef CONSOLE_LOG
			PRINTLN();
			PRINTLN((digitalRead(POWER_CHECK) == LOW ? F("POWER OFF") : F("POWER ON")));
#endif

			// Не включился
			if (digitalRead(GSM_POWER_CHECK) == LOW)
			{
				Beep(4);
				state = HardResetState;
			}
		}
	}
	else if (state == HardResetState)
	{
#ifdef CONSOLE_LOG
		PRINTLN();
		PRINTLN(F("HARD RESET ON"));
#endif

		digitalWrite(GSM_POWER_RELAY, RELAY_ON);

		lastTime = millis();
		state = WaitHardResetState;
	}
	else if (state == WaitHardResetState)
	{
		if ((millis() - lastTime) > 3000)
		{
#ifdef CONSOLE_LOG
			PRINTLN();
			PRINTLN(F("HARD RESET OFF"));
#endif

			digitalWrite(GSM_POWER_RELAY, RELAY_OFF);

			lastTime = millis();
			state = CooldownHardResetState;
		}
	}
	else if (state == CooldownHardResetState)
	{
		if ((millis() - lastTime) > 3000)
		{
			state = OffState;
		}
	}
	else if (state == CompleteState)
	{
	}
}

bool GsmPower::Restart()
{
	if (state != NoneState)
		return false;

	state = OffState;

	return true;
}

bool GsmPower::Release()
{
	if (state != CompleteState)
		return false;

	state = NoneState;

	return true;
}

bool GsmPower::IsComplete()
{
	return state == CompleteState;
}
