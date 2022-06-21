
/*************************************************
    .
    File name:      	*.*
    Author��	        	������
    ID��					00001
    Date:          		202107
    Description:    	�������ݽṹ
    Others:         	��

    History:        	��
	    1.  Date:
		    Author: 
		    ID:
		    Modification:
		    
	    2.  Date:
		    Author: 
		    ID:
		    Modification:
*************************************************/




#include "L1_LIBRARY_DataStructure.h"

#include <Libraries/Memory/L1_LIBRARY_Memory.h>

QUEUE Queue;

#define LOG_LINE_CHAR_SIZE 100

unsigned char L1_BIT_Set(unsigned char *pMapper, unsigned long long StartPageID, unsigned long long Size)
{
    unsigned char AddOneFlag = (Size % 8 == 0) ? 0 : 1;
    unsigned long ByteCount = Size / 8 + AddOneFlag;
}




/****************************************************************************
*
*  ����:   ջ���ݽṹѹջ
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
int L1_LIBRARY_StackPush(char* a, int top, char elem)
{
    a[++top]=elem;
    return top;
}





/****************************************************************************
*
*  ����:   ջ���ݽṹ��ջ
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
int L1_LIBRARY_StackPop(char * a,int top)
{
    if (top==-1) 
    {
        //printf("��ջ");
        return -1;
    }
    //printf("��ջԪ�أ�%c\n",a[top]);
    top--;
    return top;
}





/****************************************************************************
*
*  ����:   �������ݽṹ��ʼ��
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
void L1_LIBRARY_QueueInit(QUEUE *pQueue, unsigned long LineCount)
{
	// Todo: need to allocate buffer before set zero..
	L1_MEMORY_SetValue(pQueue->Buffer, 0, 40 * 100);
	pQueue->Size = LineCount;
    pQueue->Front = pQueue->Rear = 0; //��ʼ��ͷβָ��
    pQueue->LineCount = 0;
}




/****************************************************************************
*
*  ����: �������ݽṹ���
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
void L1_LIBRARY_QueueIn(QUEUE *pQueue , unsigned char *s, unsigned int Size)
{
	UINT16 i;

	if (Size > 100)
		return;

	//Todo realize pBuffer save buffer data...
	// Copy one line into buffer
	for (i = 0; i < Size; i++)
		pQueue->Buffer[pQueue->Rear][i] = s[i];

	pQueue->Buffer[pQueue->Rear][i] = '\0';
	
    pQueue->Rear = (pQueue->Rear + 1) % pQueue->Size ;    //βָ��ƫ��

	pQueue->LineCount++;

	if (pQueue->LineCount > 40)
	{
		pQueue->Front = (pQueue->Front + 1) % pQueue->Size;
	}
}




/****************************************************************************
*
*  ����:   xxxxx
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
int L1_STACK_Push(char* a,int top,char elem)
{
    a[++top]=elem;
    return top;
}





/****************************************************************************
*
*  ����:   xxxxx
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
int L1_STACK_Pop(char * a,int top)
{
    if (top==-1) 
    {
        //printf("��ջ");
        return -1;
    }
    //printf("��ջԪ�أ�%c\n",a[top]);
    top--;
    return top;
}

