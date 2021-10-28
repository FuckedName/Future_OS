#pragma once


#define SYSTEM_LOG_DATA_WIDTH 200
#define SYSTEM_LOG_DATA_LINE 40

typedef struct
{
	UINT8  Buffer[SYSTEM_LOG_DATA_LINE][SYSTEM_LOG_DATA_WIDTH];
	UINT16 Front;
	UINT16 Rear;
	UINT16 LineCount;
}QUEUE;

void L1_LIBRARY_QueueInit(QUEUE *pQueue);

int L1_LIBRARY_StackPush(char* a, int top, char elem);

void L1_LIBRARY_QueueIn(QUEUE *pQueue , unsigned char *s, unsigned int Size);

int L1_LIBRARY_StackPop(char * a,int top);

unsigned char L1_BIT_Set(unsigned char *pMapper, unsigned long long StartPageID, unsigned long long Size);

