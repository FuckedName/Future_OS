
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
*  描述:   屏幕参数初始化，用于后续屏幕绘图，获取屏幕分辨率用于后续使用
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID L2_SCREEN_Init()
{
	//初始化图形输出句柄
    EFI_STATUS Status = gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (VOID **) &GraphicsOutput);  
	
    //INFO_SELF(L"\r\n");    
    if (EFI_ERROR (Status)) 
    {
        INFO_SELF(L"%X\n", Status);
        return EFI_UNSUPPORTED;
    }

	//获取屏幕的水平分辨率和垂直分辨率
    ScreenWidth  = GraphicsOutput->Mode->Info->HorizontalResolution;
    ScreenHeight = GraphicsOutput->Mode->Info->VerticalResolution;	
}



/****************************************************************************
*
*  描述:   屏幕绘图
*
*  pBuffer：       用于绘图已准备的图形缓存空间
*  SourceX：       基于pBuffer的X坐标开始往屏幕绘制
*  SourceY：       基于pBuffer的Y坐标开始往屏幕绘制
*  DestinationX： 绘制的屏幕目标X坐标
*  DestinationY： 绘制的屏幕目标Y坐标
*  Width：         绘制宽度
*  Height：        绘制高度
*
*  返回值： 成功：XXXX，失败：XXXXX
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

