#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

typedef struct NetDataHeader_t *NHeader;
struct NetDataHeader_t {
	int nDataType;
	int nDataSize;
};

typedef struct NetDataBase_t *NBase;
struct NetDataBase_t {
	NetDataHeader_t dataHeader;
	double hValue;
	double lValue;
	double temperature;
};
	
int main(int argc, char *argv[])
{
	NBase myData = (NBase)malloc(sizeof(struct NetDataBase_t));
	NBase reData = (NBase)malloc(sizeof(struct NetDataBase_t));
	char *buff = (char *)malloc(sizeof(struct NetDataBase_t));
	myData->dataHeader.nDataType = 1;
	myData->dataHeader.nDataSize = sizeof(struct NetDataBase_t) - sizeof(struct NetDataHeader_t);
	myData->hValue = 0.123456789;
	myData->lValue = 0.987654321;
	myData->temperature = 23.1;

	memcpy(buff, myData, sizeof(struct NetDataBase_t)); 
	memcpy(reData, buff, sizeof(struct NetDataBase_t));


	return 0;
}
