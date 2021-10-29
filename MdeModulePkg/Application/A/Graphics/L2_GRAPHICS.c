
#include <L2_GRAPHICS.h>
#include <Global/Global.h>

#include <Libraries/String/L2_LIBRARY_String.h>



void L2_GRAPHICS_ParameterInit()
{
    WindowLayers.LayerCount = 0;

    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].DisplayFlag = TRUE;
    L1_MEMORY_Copy(WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].Name, "System Log Window", sizeof("System Log Window"));
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].pBuffer = pSystemLogWindowBuffer;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].StartX = 0;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].StartY = 0;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].WindowWidth  = SystemLogWindowWidth;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].WindowHeight = SystemLogWindowHeight;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].LayerID = GRAPHICS_LAYER_SYSTEM_LOG_WINDOW;

    WindowLayers.LayerCount++;
    

    WindowLayers.item[GRAPHICS_LAYER_START_MENU].DisplayFlag = FALSE;
    L1_MEMORY_Copy(WindowLayers.item[GRAPHICS_LAYER_START_MENU].Name, "Start Menu", sizeof("Start Menu"));
    WindowLayers.item[GRAPHICS_LAYER_START_MENU].pBuffer = pStartMenuBuffer;
    WindowLayers.item[GRAPHICS_LAYER_START_MENU].StartX = 0;
    WindowLayers.item[GRAPHICS_LAYER_START_MENU].StartY = ScreenHeight - StartMenuHeight - 25;
    WindowLayers.item[GRAPHICS_LAYER_START_MENU].WindowWidth = StartMenuWidth;
    WindowLayers.item[GRAPHICS_LAYER_START_MENU].WindowHeight= StartMenuHeight;
    WindowLayers.item[GRAPHICS_LAYER_START_MENU].LayerID = GRAPHICS_LAYER_START_MENU;
    
    WindowLayers.LayerCount++;

    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].DisplayFlag = FALSE;
    L1_MEMORY_Copy(WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].Name, "System Setting Window", sizeof("System Setting Window"));
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].pBuffer = pSystemSettingWindowBuffer;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].StartX = StartMenuWidth;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].StartY = WindowLayers.item[GRAPHICS_LAYER_START_MENU].StartY + 16;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].WindowWidth = SystemSettingWindowWidth;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].WindowHeight= SystemSettingWindowHeight;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].LayerID = GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW;
    
    WindowLayers.LayerCount++;

    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].DisplayFlag = FALSE;
    L1_MEMORY_Copy(WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].Name, "My Computer", sizeof("My Computer"));
    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].pBuffer = pMyComputerBuffer;
    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartX = 0;
    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartY = 0;
    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].WindowWidth = MyComputerWidth;
    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].WindowHeight = MyComputerHeight;
    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].LayerID = GRAPHICS_LAYER_MY_COMPUTER_WINDOW;
    
    WindowLayers.LayerCount++;

    WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].DisplayFlag = FALSE;
    L1_MEMORY_Copy(WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].Name, "Memory Information", sizeof("Memory Information"));
    WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].pBuffer = pMemoryInformationBuffer;
    WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].StartX = 0;
    WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].StartY = 0;
    WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].WindowWidth = MemoryInformationWindowWidth;
    WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].WindowHeight= MemoryInformationWindowHeight;
    WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].LayerID = GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW;

    WindowLayers.LayerCount++;
	
    WindowLayers.item[GRAPHICS_LAYER_DESK].DisplayFlag = FALSE;
    L1_MEMORY_Copy(WindowLayers.item[GRAPHICS_LAYER_DESK].Name, "Desk layer", sizeof("Desk layer"));
    WindowLayers.item[GRAPHICS_LAYER_DESK].pBuffer = pDeskBuffer;
    WindowLayers.item[GRAPHICS_LAYER_DESK].StartX = 0;
    WindowLayers.item[GRAPHICS_LAYER_DESK].StartY = 0;
    WindowLayers.item[GRAPHICS_LAYER_DESK].WindowWidth = ScreenWidth;
    WindowLayers.item[GRAPHICS_LAYER_DESK].WindowHeight= ScreenHeight;
    WindowLayers.item[GRAPHICS_LAYER_DESK].LayerID = GRAPHICS_LAYER_DESK;

    WindowLayers.LayerCount++;

    L1_MEMORY_SetValue(WindowLayers.LayerSequences, 10 * 2, 0);

    WindowLayers.ActiveWindowCount = 0;

    MouseClickFlag = MOUSE_NO_CLICKED;

	// Active window list, please note: desk layer always display at firstly.
	// So WindowLayers.LayerCount value always one more than WindowLayers.ActiveWindowCount
    WindowLayers.LayerSequences[0] = GRAPHICS_LAYER_START_MENU;
    WindowLayers.LayerSequences[1] = GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW;
    WindowLayers.LayerSequences[2] = GRAPHICS_LAYER_MY_COMPUTER_WINDOW;
    WindowLayers.LayerSequences[3] = GRAPHICS_LAYER_SYSTEM_LOG_WINDOW;
    WindowLayers.LayerSequences[4] = GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW;
    WindowLayers.ActiveWindowCount = WindowLayers.LayerCount - 1;
    
}

/* Display a string */
VOID EFIAPI L2_DEBUG_Print3 (UINT16 x, UINT16 y, WINDOW_LAYER_ITEM layer, IN  CONST CHAR8  *Format, ...)
{
    if (y > layer.WindowHeight - 16 || x > layer.WindowWidth- 8)
        return;

    VA_LIST         VaList;
    VA_START (VaList, Format);
    L2_STRING_Maker2(x, y, layer, Format, VaList);
    VA_END (VaList);
}


/* Display a string */
VOID EFIAPI L2_DEBUG_Print1 (UINT16 x, UINT16 y,  IN  CONST CHAR8  *Format, ...)
{
    if (y > ScreenHeight - 16 || x > ScreenWidth - 8)
        return;

    if (NULL == pDeskBuffer)
        return;

    VA_LIST         VaList;
    VA_START (VaList, Format);
    L2_STRING_Maker(x, y, Format, VaList);
    VA_END (VaList);
}

