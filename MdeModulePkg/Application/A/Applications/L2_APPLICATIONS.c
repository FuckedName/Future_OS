
/*************************************************
    .
    File name:      	*.*
    Author��	        	������
    ID��					00001
    Date:          		202201
    Description:    	���ڸ�Ӧ���ṩ�ӿ�
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

#include <Library/UefiRuntimeLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/LoadedImage.h>
#include <Protocol/Shell.h>

#include "L2_APPLICATIONS.h"

#include <Global/Global.h>

EFI_SHELL_PROTOCOL    *EfiShellProtocol = NULL;

//ϵͳ��ָ��
EFI_HANDLE SystemHandle = NULL;

//Ӧ�ó���ִ����ɱ�ʶ����ҪӦ�ó���ִ����ɲ���ִ���ں˵�ϵͳ���á�
BOOLEAN ApplicationRunFinished;

/****************************************************************************
*
*  ����:   ����ϵͳ����Ӧ�ó���
*
*  ApplicationCallID��     Ӧ�õ��ýӿڱ��
*  pParameters��           �����б�
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_ApplicationInit()
{    
    EFI_STATUS            Status;
    //��ʼ��Э��
    Status = gBS->LocateProtocol (&gEfiShellProtocolGuid,
                                NULL,
                                (VOID **)&EfiShellProtocol);
	    
    if (EFI_ERROR (Status)) 
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_ApplicationRun Status: %d\n", __LINE__, Status);
        return EFI_SUCCESS; 
    }
}


/****************************************************************************
*
*  ����:   ����ϵͳ����Ӧ�ó���
*
*  ApplicationCallID��     Ӧ�õ��ýӿڱ��
*  pParameters��           �����б�
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_ApplicationRun(EFI_EVENT Event,  CHAR16       *FileName)
{
    EFI_STATUS            Status;
    ApplicationRunFinished = FALSE;
    
    EfiShellProtocol->Execute (&SystemHandle,
                             L"fs0:",
                             NULL,
                             &Status);
                             
    EfiShellProtocol->Execute (&SystemHandle,
                             FileName,
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


