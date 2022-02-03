
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

EFI_SHELL_PROTOCOL    *EfiShellProtocol = NULL;

//系统的指针
EFI_HANDLE SystemHandle = NULL;

//应用程序执行完成标识，需要应用程序执行完成才能执行内核的系统调用。
BOOLEAN ApplicationRunFinished;


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
VOID L2_ApplicationRun(EFI_EVENT Event,  VOID           *Context)
{
    EFI_STATUS            Status;
    ApplicationRunFinished = FALSE;
    
    EfiShellProtocol->Execute (&SystemHandle,
                             L"fs0:",
                             NULL,
                             &Status);
                             
    EfiShellProtocol->Execute (&SystemHandle,
                             L"H.efi",
                             NULL,
                             &Status);

    ApplicationRunFinished = TRUE;
    
    return EFI_SUCCESS;
}

// Use to load *.efi test
VOID L2_ApplicationRun2(UINT8 *pPath)
{

    CoreLoadImageCommon();
    CoreStartImage();
}


