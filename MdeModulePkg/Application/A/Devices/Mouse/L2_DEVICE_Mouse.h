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



#include <L1_DEVICE_Mouse.h>
//#include <L1_DEVICE_Screen.h>


// for mouse move & click
VOID
EFIAPI
L2_MOUSE_Event (IN EFI_EVENT Event, IN VOID *Context);


EFI_STATUS L2_DEVICE_MouseInit();

