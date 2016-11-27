#include "Util.h"

int hourFormat12(uint8_t t)
{
	if (t == 0)
		return 12;
	else if (t > 12)
		return t - 12;
	else
		return t;
}

uint8_t isPM(uint8_t t)
{
	return (t >= 12);
}

uint8_t isAM(uint8_t t)
{
	return !isPM(t);
}