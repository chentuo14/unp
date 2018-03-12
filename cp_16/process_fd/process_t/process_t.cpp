#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

typedef struct {
	int index;
	int chanel[2];
}process_t;

int main(int argc, char *argv[])
{
	process_t *pprocess;
	int i;

	pprocess = (process_t *)malloc(sizeof(process_t)*3);
	
	pprocess->index = 0;
	(pprocess+1)->index = 1;
	(pprocess+2)->index = 2;

	for(i=0;i<3;i++)
		printf("pprocess%d index : %d\n", i, (pprocess+i)->index);
	return 0;
}
