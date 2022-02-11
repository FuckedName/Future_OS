
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
}WINDOW_LAYER_ITEM;


typedef struct
{
    WINDOW_LAYER_ITEM item[10];
    UINT16 LayerCount;
    UINT16 ActiveWindowCount;
    UINT16 LayerSequences[10];
}WINDOW_LAYERS;

typedef enum
{
    GRAPHICS_LAYER_DESK = 0,
    GRAPHICS_LAYER_START_MENU,
    GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW,
    GRAPHICS_LAYER_MY_COMPUTER_WINDOW,
    GRAPHICS_LAYER_SYSTEM_LOG_WINDOW,
    GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW,
    GRAPHICS_LAYER_MOUSE_RIGHT_CLICK_WINDOW,
    GRAPHICS_LAYER_MOUSE, //永远不会触发这个事件层，因为鼠标上边没有其他事件。
    GRAPHICS_LAYER_MAX
}GRAPHICS_LAYER_ID;

double L1_GRAPHICS_Bilinear(double a, double blue, int uv, int u1v, int uv1, int u1v1);

void L1_GRAPHICS_ZoomImage(UINT8* pDest, unsigned int DestWidth, unsigned int DestHeight, UINT8* pSource, unsigned int SourceWidth, unsigned int SourceHeight );

