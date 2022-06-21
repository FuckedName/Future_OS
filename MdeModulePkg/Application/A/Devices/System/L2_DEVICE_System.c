
/*************************************************
    .
    File name:      	*.*
    Author：	        	任启红
    ID：					00001
    Date:          		202107
    Description:    	
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


#include <Library/UefiBootServicesTableLib.h>

#include <Library/UefiRuntimeLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include "L2_DEVICE_System.h"
#include <Global/Global.h>


/****************************************************************************
*
*  描述:   系统关机。
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L2_System_Shutdown()
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_System_Shutdown \n", __LINE__);

    gBS->SetTimer( TimerOne, TimerCancel, 0 );
    gBS->CloseEvent( TimerOne );  
    gRT->ResetSystem (EfiResetShutdown, EFI_SUCCESS, 0, NULL); 
    
    return EFI_SUCCESS;
}



