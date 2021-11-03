
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




#pragma once


void L1_MEMORY_SetValue(UINT8 *pBuffer, UINT32 Length, UINT8 Value);

void *L1_MEMORY_Copy(UINT8 *dest, const UINT8 *src, UINT8 count);

VOID L1_MEMORY_Memset(void *s, UINT8 c, UINT32 n);


