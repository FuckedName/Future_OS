
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




#pragma once

#include <Protocol/GraphicsOutput.h>
#include <Library/BaseLib.h>

#include <Global/Global.h>

#include <L1_GRAPHICS.h>


// Master File Table
typedef enum
{
	MFT_ITEM_DOLLAR_MFT = 0,
	MFT_ITEM_DOLLAR_MFT_MIRR,
	MFT_ITEM_DOLLAR_LOG_FILE,
	MFT_ITEM_DOLLAR_VOLUME,
	MFT_ITEM_DOLLAR_ATTR_DEF,
	MFT_ITEM_DOLLAR_ROOT,
	MFT_ITEM_DOLLAR_BIT_MAP,
	MFT_ITEM_DOLLAR_BOOT,
	MFT_ITEM_DOLLAR_BAD_CLUS,
	MFT_ITEM_DOLLAR_QUOTA,
	MFT_ITEM_DOLLAR_SECURE,
	MFT_ITEM_DOLLAR_UP_CASE,
	MFT_ITEM_DOLLAR_EXTEND_METADATA_DIRECTORY,
	MFT_ITEM_DOLLAR_EXTEND_REPARSE,
	MFT_ITEM_DOLLAR_EXTEND_USNJML,
	MFT_ITEM_DOLLAR_EXTEND_QUOTA,
	MFT_ITEM_DOLLAR_EXTEND_OBJECT_ID,
	MFT_ITEM_DOLLAR_MAX
}MFT_ITEM;

typedef enum 
{
    START_MENU_INIT_CLICKED_EVENT = 0,
    START_MENU_CLICKED_EVENT,
    MY_COMPUTER_CLICKED_EVENT,
    SETTING_CLICKED_EVENT,
    MEMORY_INFORMATION_CLICKED_EVENT,
    MY_COMPUTER_PARTITION_ITEM_CLICKED_EVENT,
    MY_COMPUTER_FOLDER_ITEM_CLICKED_EVENT,
    MY_COMPUTER_CLOSE_CLICKED_EVENT,
    SYSTEM_LOG_CLOSE_CLICKED_EVENT,
    MEMORY_INFORMATION_CLOSE_CLICKED_EVENT,
    SYSTEM_SETTING_CLOSE_CLICKED_EVENT,
    SYSTEM_LOG_CLICKED_EVENT,
    SYSTEM_QUIT_CLICKED_EVENT,
    WALLPAPER_SETTING_CLICKED_EVENT,
    WALLPAPER_RESET_CLICKED_EVENT,
    MAX_CLICKED_EVENT
}MOUSE_CLICK_EVENT;

typedef enum 
{
    CLICK_INIT_STATE = 0,
    MENU_CLICKED_STATE,
    MY_COMPUTER_CLICKED_STATE,
    MEMORY_INFORMATION_CLICKED_STATE,
    SYSTEM_SETTING_CLICKED_STATE,
    SYSTEM_LOG_STATE,
    MY_COMPUTER_PARTITION_CLICKED_STATE,
    MY_COMPUTER_FOLDER_CLICKED_STATE,
    SYSTEM_QUIT_STATE
}START_MENU_STATE;

extern START_MENU_STATE 	StartMenuNextState;


typedef struct
{
    GRAPHICS_LAYER_ID           GraphicsLayerID;
    MOUSE_CLICK_EVENT           (*pClickEventGet)(); 
    VOID                        (*pClickEventHandle)(MOUSE_CLICK_EVENT); 
}GRAPHICS_LAYER_EVENT_GET;


typedef struct
{
    START_MENU_STATE                    CurrentState;
    MOUSE_CLICK_EVENT            		event;
    START_MENU_STATE                    NextState;
    VOID                          (*pFunc)(); 
}START_MENU_STATE_TRANSFORM;




void L2_GRAPHICS_ParameterInit();

VOID EFIAPI L2_DEBUG_Print1 (UINT16 x, UINT16 y,  IN  CONST CHAR8  *Format, ...);

VOID EFIAPI L2_DEBUG_Print3 (UINT16 x, UINT16 y, WINDOW_LAYER_ITEM layer, IN  CONST CHAR8  *Format, ...);

EFI_STATUS L2_GRAPHICS_ScreenInit();

EFI_STATUS L2_GRAPHICS_SystemSettingInit();

EFI_STATUS L2_GRAPHICS_StartMenuInit();

EFI_STATUS L2_GRAPHICS_SayGoodBye();

UINT16 L2_MOUSE_ClickEventHandle();


VOID L2_MOUSE_Click();

EFI_STATUS L2_GRAPHICS_DeskInit();

EFI_STATUS L2_GRAPHICS_AsciiCharDraw3(UINT8 *pBuffer,
        IN UINTN x0, UINTN y0,UINT8 c,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL BorderColor, UINT16 AreaWidth);

EFI_STATUS L2_GRAPHICS_ChineseHalfDraw(UINT8 *pBuffer,UINT8 d,
        IN UINTN x0, UINTN y0,
        UINT8 width,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color, UINT8 fontWidth);

VOID L2_GRAPHICS_LayerCompute(UINT16 iMouseX, UINT16 iMouseY, UINT8 MouseClickFlag);

VOID L2_STORE_FolderItemsPrint();

void L2_GRAPHICS_BootScreenInit();

void L2_GRAPHICS_Init();

void L2_GRAPHICS_CopyBufferFromWindowsToDesk();

VOID L2_STORE_PartitionItemsPrint(UINT16 Index);

EFI_STATUS L2_GRAPHICS_AsciiCharDraw(UINT8 *pBufferDest,
        IN UINTN x0, UINTN y0, UINT8 c,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color);
        
EFI_STATUS L2_GRAPHICS_ChineseCharDraw12(UINT8 *pBuffer,
        IN UINTN x0, UINTN y0, UINT32 offset,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color , UINT16 AreaWidth);

EFI_STATUS L2_GRAPHICS_AsciiCharDraw2(WINDOW_LAYER_ITEM layer,
        IN UINTN x0, UINTN y0, UINT8 c,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color);

VOID L2_GRAPHICS_RightClickMenu(UINT16 iMouseX, UINT16 iMouseY);

VOID L2_MOUSE_MoveOver();

VOID L2_MOUSE_Move();

VOID
EFIAPI
L2_TIMER_Print (
  IN EFI_EVENT Event,
  IN VOID      *Context
  );

EFI_STATUS L2_GRAPHICS_ChineseCharDraw(UINT8 *pBuffer,
        IN UINTN x0, UINTN y0, UINT32 offset,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color , UINT16 AreaWidth);

void L1_MEMORY_CopyColor1(UINT8 *pBuffer, EFI_GRAPHICS_OUTPUT_BLT_PIXEL color, UINT16 x0, UINT16 y0, UINT16 AreaWidth);

void L1_MEMORY_CopyColor2(UINT8 *pBuffer, EFI_GRAPHICS_OUTPUT_BLT_PIXEL color, UINT16 x0, UINT16 y0);

void L1_MEMORY_CopyColor3(UINT8 *pBuffer, EFI_GRAPHICS_OUTPUT_BLT_PIXEL color, UINT16 x0, UINT16 y0, UINT8 AreaWidth);


