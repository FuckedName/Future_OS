#pragma once

#include <Library/BaseLib.h>


typedef struct
{
    UINT16 Name[40]; // graphics layer
    UINT8  DisplayFlag; // 0: do not display, 1: display
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
    GRAPHICS_LAYER_MOUSE,
    GRAPHICS_LAYER_MAX
}GRAPHICS_LAYER_ID;


