int hourFormat12(uint8_t t)
{
	if (t == 0)
		return 12;
	else if (t  > 12)
		return t - 12;
	else
		return t;
}

uint8_t isAM(uint8_t t)
{
	return !isPM(t);
}

uint8_t isPM(uint8_t t)
{
	return (t >= 12);
}

void padDigit(int digit, String& str)
{
	if (digit < 10)
	{
		str += "0";
		str += digit;
		return;
	}

	str += digit;
}
