#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Flight.h"
#include "General.h"
#include "fileHelper.h"
#include "myMacros.h"


int		isFlightInRoute(const Flight* pFlight, const char* codeSource, const char* codeDest)
{
	if ((strcmp(pFlight->originCode, codeSource) == 0) &&
		(strcmp(pFlight->destCode, codeDest) == 0))
		return 1;

	return 0;
}

int		countFlightsInRoute(Flight** arr, int size, const char* codeSource,
	const char* codeDest)
{
	int count = 0;
	for (int i = 0; i < size; i++)
	{
		if (isFlightInRoute(arr[i], codeSource, codeDest))
			count++;
	}
	return count;
}

void	printFlight(const Flight* pFlight)
{
	printf("Flight From %s To %s\t", pFlight->originCode, pFlight->destCode);
	printDate(&pFlight->date);
	printf("Hour: %d\n", pFlight->hour);
}

void	printFlightV(const void* val)
{
	const Flight* pFlight = *(const Flight**)val;
	printFlight(pFlight);
}

void	initFlight(Flight* pFlight, const AirportManager* pManager)
{
	Airport* pPortOr = setAiportToFlight(pManager, "Enter code of origin airport:");
	strcpy(pFlight->originCode, pPortOr->code);
	int same;
	Airport* pPortDes;
	do {
		pPortDes = setAiportToFlight(pManager, "Enter code of destination airport:");
		same = isSameAirport(pPortOr, pPortDes);
		if (same)
			printf("Same origin and destination airport\n");
	} while (same);
	strcpy(pFlight->destCode, pPortDes->code);
	getCorrectDate(&pFlight->date);
	pFlight->hour = getFlightHour();
}

int getFlightHour()
{
	int h;
	do {
		printf("Enter flight hour [0-23]:\t");
		scanf("%d", &h);
	} while (h < 0 || h>23);
	return h;
}

Airport* setAiportToFlight(const AirportManager* pManager, const char* msg)
{
	char code[MAX_STR_LEN];
	Airport* port;
	do
	{
		printf("%s\t", msg);
		myGets(code, MAX_STR_LEN, stdin);
		port = findAirportByCode(pManager, code);
		if (port == NULL)
			printf("No airport in this country - try again\n");
	} while (port == NULL);

	return port;
}

int saveFlightToFile(const Flight* pF, FILE* fp)
{

	if (fwrite(pF, sizeof(Flight), 1, fp) != 1)
	{
		printf("Error write flight\n");
		return 0;
	}

	return 1;
}

int		writeFlightBytes(FILE* fp, Flight* pFlight)
{
	if (fwrite(pFlight->originCode, sizeof(char), CODE_LENGTH, fp) != CODE_LENGTH)
		MSG_CLOSE_RETURN_0(fp, Error writing sorce code);
	if (fwrite(pFlight->destCode, sizeof(char), CODE_LENGTH, fp) != CODE_LENGTH)
		MSG_CLOSE_RETURN_0(fp, Error writing destination code);
	int numbers = pFlight->date.year << 14, tmp;
	tmp = pFlight->date.month << 10;
	numbers += tmp;
	tmp = pFlight->date.day << 5;
	numbers += tmp;
	numbers += pFlight->hour;
	if (fwrite(&numbers, sizeof(int), 1, fp) != 1)
		MSG_CLOSE_RETURN_0(fp, Error writing date & hour);
	return 1;
}


int loadFlightFromFile(Flight* pF, const AirportManager* pManager, FILE* fp)
{

	if (fread(pF, sizeof(Flight), 1, fp) != 1)
	{
		printf("Error read flight\n");
		return 0;
	}

	return 1;
}

int		readFlightBytes(FILE* fp, Flight* pFlight, AirportManager* pManager)
{
	char src[CODE_LENGTH + 1] = { '\0' };
	char dst[CODE_LENGTH + 1] = { '\0' };
	if (fread(src, sizeof(char), CODE_LENGTH, fp) != CODE_LENGTH)
		MSG_CLOSE_RETURN_0(fp, Error reading sorce);
	if (fread(dst, sizeof(char), CODE_LENGTH, fp) != CODE_LENGTH)
		MSG_CLOSE_RETURN_0(fp, Error reading destination);
	if (!checkUniqeCode(src, pManager))
		strcpy(pFlight->originCode, src);
	if (!checkUniqeCode(dst, pManager))
		strcpy(pFlight->destCode, dst);
	int numbers;
	if (fread(&numbers, sizeof(int), 1, fp) != 1)
		MSG_CLOSE_RETURN_0(fp, Error reading date & hour);
	pFlight->hour = (numbers & (createMask(4, 0)));
	pFlight->date.day = ((numbers >> 5) & (createMask(4, 0)));
	pFlight->date.month = ((numbers >> 10) & (createMask(3, 0)));
	pFlight->date.year = (numbers >> 14);
	return 1;
}

int	compareByCodeOrig(const void* flight1, const void* flight2)
{
	const Flight* pFlight1 = *(const Flight**)flight1;
	const Flight* pFlight2 = *(const Flight**)flight2;
	return strcmp(pFlight1->originCode, pFlight2->originCode);
}

int	compareByCodeDest(const void* flight1, const void* flight2)
{
	const Flight* pFlight1 = *(const Flight**)flight1;
	const Flight* pFlight2 = *(const Flight**)flight2;
	return strcmp(pFlight1->destCode, pFlight2->destCode);
}

int	compareByHour(const void* flight1, const void* flight2)
{
	const Flight* pFlight1 = *(const Flight**)flight1;
	const Flight* pFlight2 = *(const Flight**)flight2;
	return (pFlight1->hour - pFlight2->hour);
}

int		compareByDate(const void* flight1, const void* flight2)
{
	const Flight* pFlight1 = *(const Flight**)flight1;
	const Flight* pFlight2 = *(const Flight**)flight2;

	Date d1 = pFlight1->date;
	Date d2 = pFlight2->date;

	if (d1.year > d2.year)
		return 1;
	if (d1.year < d2.year)
		return -1;

	if (d1.month > d2.month)
		return 1;
	if (d1.month < d2.month)
		return -1;

	if (d1.day > d2.day)
		return 1;
	if (d1.day < d2.day)
		return -1;

	return 0;
}



void	freeFlight(void* val)
{
	Flight* pFlight = *(Flight**)val;
	free(pFlight);
}
