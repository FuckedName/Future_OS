
/*************************************************
    .
    File name:      	*.*
    Author：	        	任启红
    ID：					00001
    Date:          		202107
    Description:    	图形处理的一些基础算法，不依赖其他模块
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

#include <Library/BaseLib.h>

#define LAYER_QUANTITY 20 


typedef struct
{
    UINT16 Name[40]; // graphics layer
    UINT8  DisplayFlag; // FALSE: do not display, TRUE: display
    UINT8 *pBuffer;
    UINT16 StartX;
    UINT16 StartY;
    UINT16 WindowWidth;
    UINT16 WindowHeight;
    UINT16 LayerID;
	UINT16 Step;
}WINDOW_LAYER_ITEM;

typedef struct
{
    WINDOW_LAYER_ITEM item[LAYER_QUANTITY];
    UINT16 LayerCount;
    UINT16 ActiveWindowCount;
    UINT16 LayerSequences[LAYER_QUANTITY];
}WINDOW_LAYERS;



VOID L3_APPLICATION_MyComputerWindow(UINT16 StartX, UINT16 StartY);

VOID L3_APPLICATION_MemoryInformationWindow(UINT16 StartX, UINT16 StartY);

VOID L3_APPLICATION_SystemLogWindow(UINT16 StartX, UINT16 StartY);

VOID L3_APPLICATION_WindowsInitial();


EFI_STATUS L2_GRAPHICS_ButtonDraw2(WINDOW_LAYER_ITEM *pWindowLayerItem, UINT16 StartX, UINT16 StartY, UINT16 Width, UINT16 Height);

