
/*************************************************
    .
    File name:      	*.*
    Author£∫	        	»Œ∆Ù∫Ï
    ID£∫					00001
    Date:          		202107
    Description:    	
    Others:         	Œﬁ

    History:        	Œﬁ
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


void L1_MEMORY_SetValue(UINT8 *pBuffer, UINT8 Value, UINT32 Length);

void *L1_MEMORY_Copy(UINT8 *dest, const UINT8 *src, UINT32 count);

VOID L1_MEMORY_Memset(void *s, UINT8 c, UINT32 n);

VOID L1_MEMORY_MemsetWidth(int *s, int c, UINT32 n);

