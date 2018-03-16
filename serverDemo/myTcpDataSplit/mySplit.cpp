#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_NETPACK_SIZE 1000
#define MAX_DATA_SIZE    256

struct NetDataHeader_t {
	int nDataType;
	int nDataSize;
};

struct NetDataBase_t {
	NetDataHeader_t dataHeader;
	double hValue;
	double lValue;
	double temperature;
};

bool HandleNetPack(NetDataHeader_t *pDataHeader);

bool TcpDataSplit(const char *szRecNetData, int nRecSize)
{
	static char szLastSaveData[MAX_NETPACK_SIZE];
	static int  nRemainSize = 0;
	static bool bFirst = true;

	if(bFirst) {
		memset(szLastSaveData, 0, sizeof(szLastSaveData));
		bFirst = false;
	}

	memcpy((char *)(szLastSaveData+nRemainSize), szRecNetData, nRecSize);
	nRemainSize = nRecSize + nRemainSize;

	NetDataHeader_t *pDataHead = (NetDataHeader_t *)szLastSaveData;

	while(nRemainSize > sizeof(NetDataHeader_t) &&
			nRemainSize >= pDataHead->nDataSize + sizeof(NetDataHeader_t)) {
		HandleNetPack(pDataHead);
		int nRecObjectSize = sizeof(NetDataHeader_t) + pDataHead->nDataSize;
		nRemainSize -= nRecObjectSize;
		pDataHead = (NetDataHeader_t *)((char *)pDataHead+nRecObjectSize);
	}

	if(szLastSaveData != (char *)pDataHead) {
		memmove(szLastSaveData, (char *)pDataHead, nRemainSize);
		memset((char *)(szLastSaveData+nRemainSize), 0, sizeof(szLastSaveData)-nRemainSize);
	}
	return true;
}

bool HandleNetPack(NetDataHeader_t *pDataHeader)
{
	if(pDataHeader->nDataType == 1) {
		NetDataBase_t *pDataBase  = (NetDataBase_t *)pDataHeader; 
		printf("hvalue is %2.10f, lValue is %2.10f, temperatue is %f\n", pDataBase->hValue, pDataBase->lValue, pDataBase->temperature);
	}
}

int main(int argc, char *argv[])
{
	NetDataBase_t dataBase1;
	dataBase1.dataHeader.nDataSize = sizeof(dataBase1) - sizeof(NetDataHeader_t);
	dataBase1.dataHeader.nDataType = 1;
	dataBase1.hValue = -1.23456789;
	dataBase1.lValue = 0.123456789;
	dataBase1.temperature = 23.5;

	NetDataBase_t dataBase2;
	dataBase2.dataHeader.nDataSize = sizeof(dataBase2) - sizeof(NetDataHeader_t);
	dataBase2.dataHeader.nDataType = 1;
	dataBase2.hValue = 1.23456789;
	dataBase2.lValue = -0.123456789;
	dataBase2.temperature = 20.5;

	char szSendData[sizeof(dataBase1)+sizeof(dataBase2)];
	memcpy(szSendData, (char *)&dataBase1, sizeof(dataBase1));
	memcpy(szSendData+sizeof(dataBase1), (char *)&dataBase2, sizeof(dataBase2));

	TcpDataSplit((char *)szSendData, 3);
	TcpDataSplit((char *)szSendData+3, 10);
	TcpDataSplit((char *)szSendData+13, sizeof(szSendData)-13);

	getchar();
	return 0;
}
