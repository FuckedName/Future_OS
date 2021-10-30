
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

EFI_STATUS L2_GRAPHICS_ScreenInit()
{
    EFI_STATUS Status = 0;
    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FAT32\n",  __LINE__);

    Status = L3_APPLICATION_ReadFile("ZHUFENGBMP", 10, pDeskWallpaperBuffer);
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: ReadFileSelf error\n", __LINE__);
    }
    
    Status = L3_APPLICATION_ReadFile("COMPUTERBMP", 11, pSystemIconBuffer[SYSTEM_ICON_MYCOMPUTER]);
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: ReadFileSelf error\n", __LINE__);
    }
    
    Status = L3_APPLICATION_ReadFile("SETTINGBMP", 10, pSystemIconBuffer[SYSTEM_ICON_SETTING]);
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: ReadFileSelf error\n", __LINE__);
    }

    Status = L3_APPLICATION_ReadFile("RECYCLEBMP", 10, pSystemIconBuffer[SYSTEM_ICON_RECYCLE]);
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: ReadFileSelf error\n", __LINE__);
    }

    Status = L3_APPLICATION_ReadFile("FOLDERBMP", 9, pSystemIconBuffer[SYSTEM_ICON_FOLDER]);
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: ReadFileSelf error\n", __LINE__);
    }

    Status = L3_APPLICATION_ReadFile("TEXTBMP", 7, pSystemIconBuffer[SYSTEM_ICON_TEXT]);
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: ReadFileSelf error\n", __LINE__);
    }
    
    Status = L3_APPLICATION_ReadFile("HZK16", 5, sChineseChar);
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: ReadFileSelf error\n", __LINE__);
    }
    
    L2_GRAPHICS_DeskInit();

    
    // init mouse buffer with cursor
    L2_GRAPHICS_ChineseCharDraw(pMouseBuffer, 0, 0, 11 * 94 + 42, MouseColor, 16);
    //L2_DEBUG_Print1(DISPLAY_X, DISPLAY_Y, "%d: GraphicsLayerCompute\n", __LINE__);
    
    GraphicsOutput->Blt(
                GraphicsOutput, 
                (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) pDeskBuffer,
                EfiBltBufferToVideo,
                0, 0, 
                0, 0, 
                ScreenWidth, ScreenHeight, 0);   

    // Desk graphics layer, buffer can not free!!
    //FreePool(pDeskBuffer);
    
    return EFI_SUCCESS;
}

EFI_STATUS L2_GRAPHICS_StartMenuInit()
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    
    int x = 3, y = 6;
    Color.Red   = 0xff;
    Color.Green = 0x00;
    Color.Blue  = 0x00;
    Color.Reserved  = GRAPHICS_LAYER_START_MENU;

    //这边的序列需要跟START_MENU_BUTTON_SEQUENCE这个枚举定义的一致
    //我的电脑
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (46 - 1 ) * 94 + 50 - 1, Color, StartMenuWidth);    
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (21 - 1) * 94 + 36 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (21 - 1) * 94 + 71 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (36 - 1) * 94 + 52 - 1, Color, StartMenuWidth);   

    //系统设置
    x = 3;
    y += 16;
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (47 - 1 ) * 94 + 21 - 1, Color, StartMenuWidth);    
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (45 - 1) * 94 + 19 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (41 - 1) * 94 + 72 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (54 - 1) * 94 + 35 - 1, Color, StartMenuWidth);   
    

    //内存查看
    //汉字    区位码 汉字  区位码 汉字  区位码 汉字  区位码
    //内 3658    存       2070    查       1873    看       3120
    x = 3;
    y += 16;
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (36 - 1 ) * 94 + 58 - 1, Color, StartMenuWidth);    
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (20 - 1) * 94 + 70 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (18 - 1) * 94 + 73 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (31 - 1) * 94 + 20 - 1, Color, StartMenuWidth);   
    
    //系统日志
    //汉字	区位码	汉字	区位码	汉字	区位码	汉字	区位码
    //系	4721	统	4519	日	4053	志	5430
    x = 3;
    y += 16;
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (47 - 1 ) * 94 + 21 - 1, Color, StartMenuWidth);    
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (45 - 1) * 94 + 19 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (40 - 1) * 94 + 53 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (54 - 1) * 94 + 30 - 1, Color, StartMenuWidth);   
    
    //系统退出
    //退 4543    出   1986
    x = 3;
    y += 16;
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (47 - 1 ) * 94 + 21 - 1, Color, StartMenuWidth);    
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (45 - 1) * 94 + 19 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (45 - 1) * 94 + 43 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (19 - 1) * 94 + 86 - 1, Color, StartMenuWidth);   
}


EFI_STATUS L2_GRAPHICS_SystemSettingInit()
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    
    int x = 3, y = 6;
    Color.Red   = 0xff;
    Color.Green = 0x00;
    Color.Blue  = 0x00;

    //背景设置
    //背 1719    景   3016    设   4172    置   5435
    L2_GRAPHICS_ChineseCharDraw(pSystemSettingWindowBuffer, x , y,   (17 - 1) * 94 + 19 - 1, Color, SystemSettingWindowWidth);   
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pSystemSettingWindowBuffer, x , y,   (30 - 1) * 94 + 16 - 1, Color, SystemSettingWindowWidth);
    x += 16;
        
    L2_GRAPHICS_ChineseCharDraw(pSystemSettingWindowBuffer, x , y,   (41 - 1) * 94 + 72 - 1, Color, SystemSettingWindowWidth); 
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pSystemSettingWindowBuffer, x , y,   (54 - 1) * 94 + 35 - 1, Color, SystemSettingWindowWidth);
    x += 16;

    //背景还原
    //还 2725    原   5213
    x = 3;
    y += 16;
    L2_GRAPHICS_ChineseCharDraw(pSystemSettingWindowBuffer, x , y,   (17 - 1) * 94 + 19 - 1, Color, SystemSettingWindowWidth);   
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pSystemSettingWindowBuffer, x , y,   (30 - 1) * 94 + 16 - 1, Color, SystemSettingWindowWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pSystemSettingWindowBuffer, x , y,   (27 - 1) * 94 + 25 - 1, Color, SystemSettingWindowWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pSystemSettingWindowBuffer, x , y,   (52 - 1) * 94 + 13 - 1, Color, SystemSettingWindowWidth);
    

}


EFI_STATUS L2_GRAPHICS_SayGoodBye()
{
    for (int i = 0; i < ScreenHeight; i++)
    {
        for (int j = 0; j < ScreenWidth; j++)
        {
            // BMP 3bits, and desk buffer 4bits
            pDeskBuffer[(i * ScreenWidth + j) * 4]     = 0xff;
            pDeskBuffer[(i * ScreenWidth + j) * 4 + 1] = 0x00;
            pDeskBuffer[(i * ScreenWidth + j) * 4 + 2] = 0x00;
        }
    }
    
    // 再    5257    见   2891    ，   0312    欢   2722    迎   5113
    // 下    4734    次   2046    回   2756    来   3220
    // menu chinese
    UINT16 x = ScreenWidth / 2;
    UINT16 y = ScreenHeight / 2;
    
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    Color.Red   = 0xff;
    Color.Green = 0xff;
    Color.Blue  = 0xff;
    
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer,  x, y, (52 - 1) * 94 + 57 - 1, Color, ScreenWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer,  x, y, (28 - 1) * 94 + 91 - 1, Color, ScreenWidth);
    x += 16;

    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer,  x, y, (3 - 1) * 94 + 12 - 1, Color, ScreenWidth);
    x += 16;

    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer,  x, y, (27 - 1) * 94 + 22 - 1, Color, ScreenWidth);
    x += 16;

    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer,  x, y, (51 - 1) * 94 + 13 - 1, Color, ScreenWidth);
    x += 16;

    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer,  x, y, (47 - 1) * 94 + 34 - 1, Color, ScreenWidth);
    x += 16;

    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer,  x, y, (20 - 1) * 94 + 46 - 1, Color, ScreenWidth);
    x += 16;

    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer,  x, y, (27 - 1) * 94 + 56 - 1, Color, ScreenWidth);
    x += 16;

    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer,  x, y, (32 - 1) * 94 + 20 - 1, Color, ScreenWidth);
    x += 16;

    GraphicsOutput->Blt(
                GraphicsOutput, 
                (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) pDeskBuffer,
                EfiBltBufferToVideo,
                0, 0, 
                0, 0, 
                ScreenWidth, ScreenHeight, 0);   

}

