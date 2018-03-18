#include "Beep.h"

void Beep(int count)
{
#ifdef BEEP_MODULE
	for (int index = 0; index < count; index++)
	{
		analogWrite(BEEP_PIN, 3);
		delay(BEEP_DELAY1);
		analogWrite(BEEP_PIN, 0);

		delay(BEEP_DELAY2);
	}
#endif
}
