
/*************************************************
    .
    File name:      	*.*
    Author��	        	������
    ID��					00001
    Date:          		202107
    Description:    	
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




#include <Library/UefiLib.h>
#include <Library/BaseLib.h>

#include <string.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Global/Global.h>

#include <L2_DEVICE_Screen.h>

EFI_GRAPHICS_OUTPUT_PROTOCOL       *GraphicsOutput = NULL;


/****************************************************************************
*
*  ����:   ��Ļ������ʼ�������ں�����Ļ��ͼ����ȡ��Ļ�ֱ������ں���ʹ��
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_SCREEN_Init()
{
	//��ʼ��ͼ��������
    EFI_STATUS Status = gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (VOID **) &GraphicsOutput);  
	
    //INFO_SELF(L"\r\n");    
    if (EFI_ERROR (Status)) 
    {
        INFO_SELF(L"%X\n", Status);
        return EFI_UNSUPPORTED;
    }

	//��ȡ��Ļ��ˮƽ�ֱ��ʺʹ�ֱ�ֱ���
    ScreenWidth  = GraphicsOutput->Mode->Info->HorizontalResolution;
    ScreenHeight = GraphicsOutput->Mode->Info->VerticalResolution;	
}



/****************************************************************************
*
*  ����:   ��Ļ��ͼ
*
*  pBuffer��       ���ڻ�ͼ��׼����ͼ�λ���ռ�
*  SourceX��       ����pBuffer��X���꿪ʼ����Ļ����
*  SourceY��       ����pBuffer��Y���꿪ʼ����Ļ����
*  DestinationX�� ���Ƶ���ĻĿ��X����
*  DestinationY�� ���Ƶ���ĻĿ��Y����
*  Width��         ���ƿ��
*  Height��        ���Ƹ߶�
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID EFIAPI L2_SCREEN_Draw (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *pBuffer,
									UINTN	SourceX,
									UINTN	SourceY,
									UINTN	DestinationX,
									UINTN	DestinationY,
									UINTN	Width,
									UINTN	Height)
{    
    GraphicsOutput->Blt(GraphicsOutput, 
		                (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) pBuffer,
		                EfiBltBufferToVideo,
		                SourceX, SourceY, 
		                DestinationX, DestinationY, 
		                Width, Height, 0);   
}

