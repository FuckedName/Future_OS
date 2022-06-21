
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

#include <L2_PARTITION.h>

typedef enum 
{
    INIT_ACCESS_STATE = 0,    //
    ROOT_PATH_STATE,
    FILE_ITEM_ACCESS_STATE,
    FOLDER_ITEM_ACCESS_STATE,
    PARENT_PATH_ACCESS_STATE,
    MAX_ACCESS_STATE
}PARTITION_ITEM_ACCESS_STATE;

typedef enum 
{
    ROOT_PATH_ACCESS_EVENT = 0,
    FOLDER_ACCESS_EVENT,
    FILE_ACCESS_EVENT,
    PARENT_ACCESS_EVENT,
    CLOSE_ACCESS_EVENT,
    MAX_ACCESS_EVENT
}PARTITION_ITEM_ACCESS_EVENT;


typedef struct
{
    PARTITION_ITEM_ACCESS_STATE     CurrentState;
    PARTITION_ITEM_ACCESS_EVENT     event;
    PARTITION_ITEM_ACCESS_STATE     NextState;
    EFI_STATUS                      (*pFunc)(); 
}PARTITION_ITEM_ACCESS_STATE_TRANSFORM;



EFI_STATUS L3_APPLICATION_ReadFile(UINT8 *FileName, UINT8 NameLength, UINT8 *pBuffer);




