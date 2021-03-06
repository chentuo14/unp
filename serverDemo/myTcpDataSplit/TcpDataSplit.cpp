// TcpDataSplit.cpp

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NETPACK_SIZE 10000
#define MAX_DATA_SIZE    4086

/* 数据包头类型 */
struct NetDataHeader_t {
	int nDataType;
	int nDataSize;
};

/* 数据报类型 */
struct NetDataBase_t {
	NetDataHeader_t dataHeader;
	char szData[MAX_DATA_SIZE];
};

struct NetDataPeople_t {
	NetDataHeader_t dataHeader;
	int  nAge;
	char szName[10];
};

struct NetDataSchool_t {
	NetDataHeader_t dataHeader;
	char szSchoolName[20];
	char szSchoolAddress[30];
};

/* 处理整理好的对象 */
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

	/* 本次接收到的数据拼接到上次数据 */
	memcpy((char *)(szLastSaveData+nRemainSize), szRecNetData, nRecSize);
	nRemainSize = nRecSize + nRemainSize;

	/* 强制转换成NetDataPack指针 */
	NetDataHeader_t *pDataHead = (NetDataHeader_t *)szLastSaveData;

	/* 核心算法 */
	while(nRemainSize > sizeof(NetDataHeader_t) && /* 合并的大小大于头结构 */
			nRemainSize >= pDataHead->nDataSize + sizeof(NetDataHeader_t)) {  /* 合并大小大于头结构的大小值加上头结构 */ 
		HandleNetPack(pDataHead);
		//本次收到的对象大小
		int nRecObjectSize = sizeof(NetDataHeader_t)+pDataHead->nDataSize;
		nRemainSize -= nRecObjectSize;
		//移动下一个对象
		pDataHead = (NetDataHeader_t *)((char *)pDataHead+nRecObjectSize);
	}

	/* 余下数据未能组成一个对象，先保存起来 */
	if(szLastSaveData != (char *)pDataHead) {
		memmove(szLastSaveData, (char *)pDataHead, nRemainSize);
		memset((char *)(szLastSaveData+nRemainSize), 0, sizeof(szLastSaveData)-nRemainSize);
	}

	return true;
}

/* 处理整理好的对象 */
bool HandleNetPack(NetDataHeader_t *pDataHeader)
{
	//处理数据包
	if(pDataHeader->nDataType == 1) {
		NetDataPeople_t *pPeople = (NetDataPeople_t *)pDataHeader;
		printf("People, Age:%d, Name:%s\n", pPeople->nAge, pPeople->szName);
	} else if(pDataHeader->nDataType == 2) {
		NetDataSchool_t *pSchool = (NetDataSchool_t *)pDataHeader;
		printf("School, Name:%s, Address:%s\n", pSchool->szSchoolName, pSchool->szSchoolAddress);
	}

	return true;
}

int main(int argc, char *argv[])
{
	/* 本子里以两个对象作为接收到的数据 */
	NetDataPeople_t people;
	people.dataHeader.nDataSize = sizeof(people) - sizeof(NetDataHeader_t);
	people.dataHeader.nDataType = 1;
	people.nAge = 20;
	sprintf(people.szName, "Jim");

	NetDataSchool_t school;
	school.dataHeader.nDataSize = sizeof(school) - sizeof(NetDataHeader_t);
	school.dataHeader.nDataType = 2;
	sprintf(school.szSchoolName, "清华大学");
	sprintf(school.szSchoolAddress, "北京市北京路");

	/* 将两个对象数据合并到一个地址里面以便重现粘包 */
	char szSendData[sizeof(people)+sizeof(school)];
	memcpy(szSendData, (char *)&people, sizeof(people));
	memcpy(szSendData+sizeof(people), (char *)&school, sizeof(school));

	TcpDataSplit((char *)szSendData, 3);
	TcpDataSplit((char *)szSendData+3, 10);
	TcpDataSplit((char *)szSendData+13, sizeof(szSendData)-13);

	getchar();
	return 0;
}

