#include "L1_LIBRARY_DataStructure.h"

#include <Libraries/Memory/L1_LIBRARY_Memory.h>

unsigned char L1_BIT_Set(unsigned char *pMapper, unsigned long long StartPageID, unsigned long long Size)
{
    unsigned char AddOneFlag = (Size % 8 == 0) ? 0 : 1;
    unsigned long ByteCount = Size / 8 + AddOneFlag;
}

int L1_LIBRARY_StackPush(char* a, int top, char elem)
{
    a[++top]=elem;
    return top;
}


int L1_LIBRARY_StackPop(char * a,int top)
{
    if (top==-1) 
    {
        //printf("空栈");
        return -1;
    }
    //printf("弹栈元素：%c\n",a[top]);
    top--;
    return top;
}


void L1_LIBRARY_QueueInit(QUEUE *pQueue)
{
	L1_MEMORY_SetValue(pQueue->Buffer, SYSTEM_LOG_DATA_LINE * SYSTEM_LOG_DATA_WIDTH, 0);
		
    pQueue->Front = pQueue->Rear = 0; //初始化头尾指针
    pQueue->LineCount = 0;
}

void L1_LIBRARY_QueueIn(QUEUE *pQueue , unsigned char *s, unsigned int Size)
{
	UINT16 i;

	if (Size > 100)
		return;
	
	// Copy one line into buffer
	for (i = 0; i < Size; i++)
		pQueue->Buffer[pQueue->Rear][i] = s[i];

	pQueue->Buffer[pQueue->Rear][i] = '\0';
	
    pQueue->Rear = (pQueue->Rear + 1) % SYSTEM_LOG_DATA_LINE ;    //尾指针偏移

	pQueue->LineCount++;

	if (pQueue->LineCount > 40)
	{
		pQueue->Front = (pQueue->Front + 1) % SYSTEM_LOG_DATA_LINE;
	}
}

int L1_STACK_Push(char* a,int top,char elem)
{
    a[++top]=elem;
    return top;
}


int L1_STACK_Pop(char * a,int top)
{
    if (top==-1) 
    {
        //printf("空栈");
        return -1;
    }
    //printf("弹栈元素：%c\n",a[top]);
    top--;
    return top;
}

