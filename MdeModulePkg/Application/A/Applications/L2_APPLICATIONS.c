
/*************************************************
    .
    File name:      	*.*
    Author：	        	任启红
    ID：					00001
    Date:          		202201
    Description:    	用于给应用提供接口
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

#include <Library/UefiRuntimeLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/LoadedImage.h>
#include <Protocol/Shell.h>

#include "L2_APPLICATIONS.h"

#include <Global/Global.h>


/****************************************************************************
*
*  描述:   操作系统运行应用程序
*
*  ApplicationCallID：     应用调用接口编号
*  pParameters：           参数列表
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID L2_ApplicationRun(EFI_HANDLE        ImageHandle)
{
    // ASSERT_EFI_ERROR(-1);
      EFI_SHELL_PROTOCOL    *EfiShellProtocol;
      EFI_STATUS            Status;
      //初始化协议
      Status = gBS->LocateProtocol (&gEfiShellProtocolGuid,
                                    NULL,
                                    (VOID **)&EfiShellProtocol);
    
      //Print(L"%d Status: %d\r\n", __LINE__, Status);
      L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_ApplicationRun Status: %d\n", __LINE__, Status);
    
      if (EFI_ERROR (Status)) 
      {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_ApplicationRun Status: %d\n", __LINE__, Status);
        return EFI_SUCCESS; 
      }
    
      EfiShellProtocol->Execute (&ImageHandle,
                                 L"fs0:",
                                 NULL,
                                 &Status);
                                 
      L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_ApplicationRun Status: %d\n", __LINE__, Status);
      
      EfiShellProtocol->Execute (&ImageHandle,
                                 L"H.efi",
                                 NULL,
                                 &Status);
      L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_ApplicationRun Status: %d\n", __LINE__, Status);
    //5. Run Buffer2 after apply cpu time slice;
}

// Use to load *.efi test
VOID L2_ApplicationRun2(UINT8 *pPath)
{

    CoreLoadImageCommon();
    CoreStartImage();
}


