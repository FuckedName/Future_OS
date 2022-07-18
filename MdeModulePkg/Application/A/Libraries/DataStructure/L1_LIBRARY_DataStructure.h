
/*************************************************
    .
    File name:          *.*
    Author£∫                »Œ∆Ù∫Ï
    ID£∫                    00001
    Date:                  202107
    Description:        
    Others:             Œﬁ

    History:            Œﬁ
        1.  Date:
            Author: 
            ID:
            Modification:
            
        2.  Date:
            Author: 
            ID:
            Modification:
*************************************************/




#pragma once

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

