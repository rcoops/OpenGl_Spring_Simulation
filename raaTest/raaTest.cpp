// raaTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <conio.h>

#include <raaLinkedList/raaLinkedList.h>
#include <raaPajParser/raaPajParser.h>

const static unsigned int csg_uiData = 1;

typedef struct _raaData
{
	char m_acName[32];
} raaData;

raaData* initData(raaData *pData, char *acName)
{
	if (pData) sprintf_s(pData->m_acName, "%s", acName);
	return pData;
}

void printer(raaLinkedListElement *pElement)
{
	if(pElement->m_uiType==csg_uiData)
	{
		printf("%d->%s\n", (unsigned int)pElement, ((raaData*)pElement->m_pData)->m_acName);
	}
}

void parseSectionFunction(const char *acRaw, const char* acSection, const char* acDescription, const char *acType, const char* acData)
{
	printf("Section %s::%s -> %s:%s\n", acSection, acDescription, acType, acData);
}

void parseNetworkFunction(const char *acRaw, const char* acId, const char* acName, const char *acY, const char* acZ)
{
	printf("Network %s::%s -> (%s, %s)\n", acId, acName, acY, acZ);
}

void parseArcFunction(const char *acRaw, const char* acId0, const char* acId1, const char *acStrength)
{
	printf("Arc %s->%s (%s)\n", acId0, acId1, acStrength);
}

void parsePartitionFunction(const char *acRaw, const char* acValue)
{
	printf("Partition %s\n", acValue);
}

void parseVectorFunction(const char *acRaw, const char* acValue)
{
	printf("Vector %s\n", acValue);
}


int main()
{
	parse("..//data//world_trade.paj", parseSectionFunction, parseNetworkFunction, parseArcFunction, parsePartitionFunction, parseVectorFunction);

	return _getch();
}

