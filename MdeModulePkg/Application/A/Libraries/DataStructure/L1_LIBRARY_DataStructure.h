#pragma once
/*************************************************
    .
    File name:      	A.c
    Author：	        任启红
    ID：		00001
    Version: Date:  	202107
    Description:    	整个模块的主入口函数
    Others:         	无
    History:        	无
	    1.  Date:
		    Author: 
		    ID:
		    Modification:
		    
	    2.  Date:
		    Author: 
		    ID:
		    Modification:
*************************************************/




typedef struct
{
	unsigned char Buffer[40][100];
	unsigned long Front;
	unsigned long Rear;
	unsigned long Size;
	unsigned long LineCount;
}QUEUE;


void L1_LIBRARY_QueueInit(QUEUE *pQueue, unsigned long BufferSize);

int L1_LIBRARY_StackPush(char* a, int top, char elem);

void L1_LIBRARY_QueueIn(QUEUE *pQueue , unsigned char *s, unsigned int Size);

int L1_LIBRARY_StackPop(char * a,int top);

unsigned char L1_BIT_Set(unsigned char *pMapper, unsigned long long StartPageID, unsigned long long Size);

