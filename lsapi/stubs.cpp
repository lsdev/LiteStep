#include "lsapi.h"

BOOL WINAPI LSLog(int nLevel, LPCSTR pszModule, LPCSTR pszMessage)
{
	return TRUE;
}

BOOL WINAPIV LSLogPrintf(int nLevel, LPCSTR pszModule, LPCSTR pszFormat, ...)
{
	return TRUE;
}

int GetRCCoordinate(LPCSTR pszKeyName, int nDefault, int nMaxVal)
{
	CHAR strVal[MAX_LINE_LENGTH];

	if (!GetRCString(pszKeyName, strVal, NULL, MAX_LINE_LENGTH))
		return nDefault;

	return ParseCoordinate(strVal, nDefault, nMaxVal);
}

int ParseCoordinate(LPCSTR szString, int nDefault, int nMaxVal)
{
	BOOL negative = false;
	BOOL center = false;
	BOOL percentual = false;
	int length = 0;

	int i = 0;
	int value = 0;

	if (!szString[0])
		return nDefault;

	if (szString[0] == '-')
	{
		negative = true;
		szString++;
	}
	else if (szString[0] == '+')
	{
		szString++;
	}

	length = strlen(szString);

	while (i < length)
	{
		if (szString[i] >= '0' && szString[i] <= '9')
			value = (value * 10) + (szString[i] - '0');
		else
			break;

		i++;
	}

	if (i == 0)
		return nDefault;

	while (i < length)
	{
		if (szString[i] == 'c' || szString[i] == 'C' && !center)
			center = true;

		else if (szString[i] == '%' && !percentual)
			percentual = true;

		else
			//		{
			//			LSLogPrintf(LOG_WARNING, "SettingsManager", "incorrect coordinate (%s), using \"%s%d%s%s\"", strVal, negative?"-":"",value, percentual?"%":"", center?"c":"");
			break;
		//		}

		i++;
	}

	if (percentual)
		value = nMaxVal * value / 100;

	if (center)
	{
		if (negative)
			value = (nMaxVal / 2) - value;
		else
			value = (nMaxVal / 2) + value;
	}
	/*	else if (percentual) // percentual positioning ie 30% from left side, -30% to count from right
		{
			if (negative)
				value = nMaxVal*(1-value/100);
			else
				value = nMaxVal*value/100;
		}*/
	else if (negative)
	{
		value = nMaxVal - value;
	}

	return value;
}
