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



#include <L1_PROCESS_Multi.h>

#include <Library/UefiLib.h>

EFI_STATUS L2_COMMON_MultiProcessInit ();

VOID EFIAPI L2_TIMER_Slice(
    IN EFI_EVENT Event,
    IN VOID           *Context
    );

VOID
EFIAPI
L2_SYSTEM_Start (IN EFI_EVENT Event, IN VOID *Context);

