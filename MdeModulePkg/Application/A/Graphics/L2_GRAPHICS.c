
#include <L2_GRAPHICS.h>
#include <Global/Global.h>
#include <string.h>

#include <Libraries/String/L2_LIBRARY_String.h>


UINT16 PartitionItemID = 0xffff; // invalid
UINT16 FolderItemID = 0xffff; // invalid

INT8 SystemQuitFlag = FALSE;

// because part items of  pItems is not valid;
UINT16 FolderItemValidIndexArray[10] = {0};

MOUSE_CLICK_EVENT    MouseClickEvent = START_MENU_INIT_CLICKED_EVENT;
START_MENU_STATE 	StartMenuNextState = CLICK_INIT_STATE;


START_MENU_STATE 	 MyComputerNextState = CLICK_INIT_STATE;

FAT32_ROOTPATH_SHORT_FILE_ITEM pItems[32];

VOID L2_MOUSE_MyComputerCloseClicked()
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_MyComputerCloseClicked\n", __LINE__);
    //DisplayMyComputerFlag = 0;
    //WindowLayers.item[3].DisplayFlag = 0;
    if (TRUE  == WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].DisplayFlag)
    {
        WindowLayers.ActiveWindowCount--;
        WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].DisplayFlag = FALSE;
    }
}

VOID L2_MOUSE_MyComputerPartitionItemClicked()
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_MyComputerPartitionItemClicked\n", __LINE__);
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
	
    Color.Red = 0xff;
    Color.Green= 0x00;
    Color.Blue= 0x00;
	
    L2_GRAPHICS_RectangleDraw(pMouseSelectedBuffer, 0,  0, 31, 15, 1,  Color, 32);
	L2_GRAPHICS_Copy(pDeskDisplayBuffer, pMouseSelectedBuffer, ScreenWidth, ScreenHeight, 32, 16, WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartX + 50, WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartY  + PartitionItemID * (16 + 2) + 16 * 2);   
    L2_STORE_PartitionItemsPrint(PartitionItemID);
}

// 1. To judge folder item is file or folder
// 2. Get StartClusterHigh2B and StartClusterLow2B and computer the next read sector id number
// 3. Read sector id number sector content from partition
// 4. Analysis content and print result 
VOID L2_MOUSE_MyComputerFolderItemClicked()
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_MyComputerFolderItemClicked\n", __LINE__);
	//FolderItemID;
	//L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d DeviceID: %d\n", __LINE__, DeviceID);
    //printf( "RootPathAnalysis\n" );
    EFI_STATUS Status ;
    UINT8 Buffer[DISK_BUFFER_SIZE];
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d PartitionItemID: %d FolderItemID: %d \n", __LINE__, PartitionItemID, FolderItemID);
		
	if (0xffff == PartitionItemID || 0xffff == FolderItemID)
	{
		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d 0xffff == PartitionItemID\n", __LINE__);
		return;
	}

	UINT16 index = FolderItemValidIndexArray[FolderItemID];

	UINT16 High2B = L1_NETWORK_2BytesToUINT16(pItems[index].StartClusterHigh2B);
	UINT16 Low2B  = L1_NETWORK_2BytesToUINT16(pItems[index].StartClusterLow2B);
	UINT32 StartCluster = High2B * 16 * 16 * 16 * 16 + Low2B;

	// Start cluster id is 2, exclude 0,1
	UINT32 StartSectorNumber = 8192 + (StartCluster - 2) * 8;
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], 
					"%d High2B: %X Low2B: %X StartCluster: %X StartSectorNumber: %X\n", 
					__LINE__, 
					High2B,
					Low2B,
					StartCluster,
					StartSectorNumber);

	// Read data from partition(disk or USB etc..)					
    Status = L1_STORE_READ(PartitionItemID, StartSectorNumber, 1, Buffer); 
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
        return Status;
    }

	switch(pItems[index].Attribute[0])
	{
		case 0x10:  L1_MEMORY_Memset(&pItems, 0, sizeof(pItems));
				    memcpy(&pItems, Buffer, DISK_BUFFER_SIZE);
					L2_STORE_FolderItemsPrint();
					break;
		
		case 0x20: L2_PARTITION_FileContentPrint(Buffer); break;

		default: break;
	}

	
}


// For my computer window state transform.
START_MENU_STATE_TRANSFORM MyComputerStateTransformTable[] =
{
    {MY_COMPUTER_CLICKED_STATE,           MY_COMPUTER_CLOSE_CLICKED_EVENT,    	   	  CLICK_INIT_STATE,                   	L2_MOUSE_MyComputerCloseClicked},
    {MY_COMPUTER_CLICKED_STATE,     	  MY_COMPUTER_PARTITION_ITEM_CLICKED_EVENT,   MY_COMPUTER_PARTITION_CLICKED_STATE,  L2_MOUSE_MyComputerPartitionItemClicked},
    {MY_COMPUTER_PARTITION_CLICKED_STATE, MY_COMPUTER_FOLDER_ITEM_CLICKED_EVENT,      MY_COMPUTER_FOLDER_CLICKED_STATE,     L2_MOUSE_MyComputerFolderItemClicked},
    {MY_COMPUTER_FOLDER_CLICKED_STATE, 	  MY_COMPUTER_FOLDER_ITEM_CLICKED_EVENT,      MY_COMPUTER_FOLDER_CLICKED_STATE,     L2_MOUSE_MyComputerFolderItemClicked},
};


VOID L2_MOUSE_MyComputerClicked()
{       
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_MyComputerClicked\n", __LINE__);
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    Color.Red = 0xff;
    Color.Green= 0x00;
    Color.Blue= 0x00;

    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].DisplayFlag = TRUE;
	WindowLayers.item[GRAPHICS_LAYER_START_MENU].DisplayFlag = FALSE;
    MyComputerNextState = MY_COMPUTER_CLICKED_STATE;
		
    L1_GRAPHICS_UpdateWindowLayer(GRAPHICS_LAYER_MY_COMPUTER_WINDOW);
    
}


VOID L2_MOUSE_SystemSettingClicked()
{   
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_SettingClicked\n", __LINE__);
    //DisplaySystemSettingWindowFlag = 1; 
    if (FALSE == WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].DisplayFlag)
    {
        WindowLayers.ActiveWindowCount++;
        WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].DisplayFlag = TRUE;
    }

    L1_GRAPHICS_UpdateWindowLayer(GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW);
}

VOID L2_MOUSE_MemoryInformationClicked()
{   
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_MemoryInformationClicked\n", __LINE__);
    //DisplayMemoryInformationWindowFlag = TRUE;
    if (FALSE == WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].DisplayFlag)
    {
        WindowLayers.ActiveWindowCount++;
        WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].DisplayFlag = TRUE;
    }
	WindowLayers.item[GRAPHICS_LAYER_START_MENU].DisplayFlag = FALSE;
    L1_GRAPHICS_UpdateWindowLayer(GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW);
}

VOID L2_MOUSE_SystemLogClicked()
{   
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_SystemLogClicked\n", __LINE__);
    //DisplaySystemLogWindowFlag = TRUE;  
    //WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].DisplayFlag = TRUE;
    if (FALSE == WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].DisplayFlag)
    {
        WindowLayers.ActiveWindowCount++;
        WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].DisplayFlag = TRUE;
        
    }
	WindowLayers.item[GRAPHICS_LAYER_START_MENU].DisplayFlag = FALSE;
    L1_GRAPHICS_UpdateWindowLayer(GRAPHICS_LAYER_SYSTEM_LOG_WINDOW);
}

VOID L2_MOUSE_SystemQuitClicked()
{   
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_SystemQuitClicked\n", __LINE__);
    SystemQuitFlag = TRUE;  
}


VOID L2_MOUSE_WallpaperSettingClicked()
{   
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_WallpaperSettingClicked\n", __LINE__);
    //DisplaySystemSettingWindowFlag = 1;
    if (FALSE == WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].DisplayFlag)
    {
        WindowLayers.ActiveWindowCount++;
        WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].DisplayFlag = TRUE;
    }
	WindowLayers.item[GRAPHICS_LAYER_START_MENU].DisplayFlag = FALSE;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    UINT32 x = ScreenWidth;
    UINT32 y = ScreenHeight;
        
    for (int i = 0; i < ScreenHeight; i++)
        for (int j = 0; j < ScreenWidth; j++)
        {
            // BMP 3bits, and desk buffer 4bits
            pDeskBuffer[(i * ScreenWidth + j) * 4]     = 0x33;
            pDeskBuffer[(i * ScreenWidth + j) * 4 + 1] = 0x33;
            pDeskBuffer[(i * ScreenWidth + j) * 4 + 2] = 0x33;
        }

    Color.Red   = 0xC6;
    Color.Green = 0xC6;
    Color.Blue  = 0xC6;
    L1_MEMORY_RectangleFill(pDeskBuffer, 0,     y - 28, x -  1, y - 28, 1, Color);

    Color.Red   = 0xFF;
    Color.Green = 0xFF;
    Color.Blue  = 0xFF;
    L1_MEMORY_RectangleFill(pDeskBuffer, 0,     y - 27, x -  1, y - 27, 1, Color);
    
    Color.Red   = 0xC6;
    Color.Green = 0xC6;
    Color.Blue  = 0xC6;
    L1_MEMORY_RectangleFill(pDeskBuffer, 0,     y - 26, x -  1, y -  1, 1, Color);
    
    Color.Red   = 0xFF;
    Color.Green = 0xFF;
    Color.Blue  = 0xFF;
    L1_MEMORY_RectangleFill(pDeskBuffer, 3,     y - 24, 59,     y - 24, 1, Color);
    L1_MEMORY_RectangleFill(pDeskBuffer, 2,     y - 24, 59,     y - 4, 1, Color);

    Color.Red   = 0x84;
    Color.Green = 0x84;
    Color.Blue  = 0x84;
    L1_MEMORY_RectangleFill(pDeskBuffer, 3,     y -  4, 59,     y -  4, 1, Color);
    L1_MEMORY_RectangleFill(pDeskBuffer, 59,     y - 23, 59,     y -  5, 1, Color);

    
    Color.Red   = 0x00;
    Color.Green = 0x00;
    Color.Blue  = 0x00;
    L1_MEMORY_RectangleFill(pDeskBuffer, 2,     y -  3, 59,     y -  3, 1, Color);
    L1_MEMORY_RectangleFill(pDeskBuffer, 60,    y - 24, 60,     y -  3, 1, Color);

    Color.Red   = 0x84;
    Color.Green = 0x84;
    Color.Blue  = 0x84;
    L1_MEMORY_RectangleFill(pDeskBuffer, x - 163, y - 24, x -  4, y - 24, 1, Color);
    L1_MEMORY_RectangleFill(pDeskBuffer, x - 163, y - 23, x - 47, y -  4, 1, Color);
    
    Color.Red   = 0xFF;
    Color.Green = 0xFF;
    Color.Blue  = 0xFF;
    L1_MEMORY_RectangleFill(pDeskBuffer, x - 163,    y - 3, x - 4,     y - 3, 1, Color);
    L1_MEMORY_RectangleFill(pDeskBuffer, x - 3,     y - 24, x - 3,     y - 3, 1, Color);

    Color.Red   = 0x00;
    Color.Green = 0x00;
    Color.Blue   = 0x00;

    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer,  16, ScreenHeight - 21,     (18 - 1) * 94 + 43 - 1, Color, ScreenWidth);
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer,  16 * 2, ScreenHeight - 21, (21 - 1) * 94 + 05 - 1, Color, ScreenWidth);      
}


VOID L2_MOUSE_WallpaperResetClicked()
{   
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_WallpaperResetClicked\n", __LINE__);
    WindowLayers.item[GRAPHICS_LAYER_START_MENU].DisplayFlag = FALSE;
    L2_GRAPHICS_DeskInit();
}


// For start menu state transform
START_MENU_STATE_TRANSFORM StartMenuStateTransformTable[] =
{
    {MENU_CLICKED_STATE,            MY_COMPUTER_CLICKED_EVENT,          		CLICK_INIT_STATE,          				L2_MOUSE_MyComputerClicked},
    {MENU_CLICKED_STATE,            SETTING_CLICKED_EVENT,              		SYSTEM_SETTING_CLICKED_STATE,       	L2_MOUSE_SystemSettingClicked},
    {MENU_CLICKED_STATE,            MEMORY_INFORMATION_CLICKED_EVENT,   		MEMORY_INFORMATION_CLICKED_STATE,   	L2_MOUSE_MemoryInformationClicked},
    {MENU_CLICKED_STATE,            SYSTEM_LOG_CLICKED_EVENT,           		SYSTEM_LOG_STATE,                   	L2_MOUSE_SystemLogClicked},
    {MENU_CLICKED_STATE,            SYSTEM_QUIT_CLICKED_EVENT,          		SYSTEM_QUIT_STATE,                  	L2_MOUSE_SystemQuitClicked},
    {SYSTEM_SETTING_CLICKED_STATE,  WALLPAPER_SETTING_CLICKED_EVENT,    		CLICK_INIT_STATE,                   	L2_MOUSE_WallpaperSettingClicked},
    {SYSTEM_SETTING_CLICKED_STATE,  WALLPAPER_RESET_CLICKED_EVENT,      		CLICK_INIT_STATE,                   	L2_MOUSE_WallpaperResetClicked},
};


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


MOUSE_CLICK_EVENT L2_GRAPHICS_DeskLayerClickEventGet()
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_GRAPHICS_DeskLayerClickEventGet\n", __LINE__);

	if (L1_GRAPHICS_InsideRectangle(0, 16 + 16 * 2, ScreenHeight - 21, ScreenHeight))
	{
		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: START_MENU_CLICKED_EVENT\n", __LINE__);
        return START_MENU_CLICKED_EVENT;
	}

	return MAX_CLICKED_EVENT;
}



MOUSE_CLICK_EVENT L2_GRAPHICS_StartMenuLayerClickEventGet()
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_GRAPHICS_StartMenuLayerClickEventGet\n", __LINE__);

    UINT16 StartMenuPositionX = WindowLayers.item[GRAPHICS_LAYER_START_MENU].StartX;
    UINT16 StartMenuPositionY = WindowLayers.item[GRAPHICS_LAYER_START_MENU].StartY;

    // Display my computer window
    if (iMouseX >= 3 + StartMenuPositionX && iMouseX <= 3 + 4 * 16  + StartMenuPositionX  
         && iMouseY >= 3 + StartMenuPositionY + 16 * START_MENU_BUTTON_MY_COMPUTER && iMouseY <= 3 + StartMenuPositionY + 16 * (START_MENU_BUTTON_MY_COMPUTER + 1))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MY_COMPUTER_CLICKED_EVENT\n", __LINE__);
        return MY_COMPUTER_CLICKED_EVENT;
    }

    // Display Setting window
    if (iMouseX >= 3 + StartMenuPositionX && iMouseX <= 3 + 4 * 16  + StartMenuPositionX  
         && iMouseY >= 3 + StartMenuPositionY + 16 * START_MENU_BUTTON_SYSTEM_SETTING && iMouseY <= 3 + StartMenuPositionY + 16 * (START_MENU_BUTTON_SYSTEM_SETTING + 1))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: SETTING_CLICKED_EVENT\n", __LINE__);
        return SETTING_CLICKED_EVENT;
    }

    // Display Memory Information window
    if (iMouseX >= 3 + StartMenuPositionX && iMouseX <= 3 + 4 * 16  + StartMenuPositionX  
         && iMouseY >= 3 + StartMenuPositionY + 16 * START_MENU_BUTTON_MEMORY_INFORMATION && iMouseY <= 3 + StartMenuPositionY + 16 * (START_MENU_BUTTON_MEMORY_INFORMATION + 1))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MEMORY_INFORMATION_CLICKED_EVENT\n", __LINE__);
        return MEMORY_INFORMATION_CLICKED_EVENT;
    }
    
    // System quit button
    if (iMouseX >= 3 + StartMenuPositionX && iMouseX <= 3 + 4 * 16  + StartMenuPositionX  
         && iMouseY >= 3 + StartMenuPositionY + 16 * START_MENU_BUTTON_SYSTEM_LOG && iMouseY <= 3 + StartMenuPositionY + 16 * (START_MENU_BUTTON_SYSTEM_LOG + 1))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: SYSTEM_LOG_CLICKED_EVENT\n", __LINE__);
        return SYSTEM_LOG_CLICKED_EVENT;
    }

    // System quit button
    if (iMouseX >= 3 + StartMenuPositionX && iMouseX <= 3 + 4 * 16  + StartMenuPositionX  
         && iMouseY >= 3 + StartMenuPositionY + 16 * START_MENU_BUTTON_SYSTEM_QUIT && iMouseY <= 3 + StartMenuPositionY + 16 * (START_MENU_BUTTON_SYSTEM_QUIT + 1))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: SYSTEM_QUIT_CLICKED_EVENT\n", __LINE__);
        return SYSTEM_QUIT_CLICKED_EVENT;
    }

	return MAX_CLICKED_EVENT;
}

MOUSE_CLICK_EVENT L2_GRAPHICS_SystemSettingLayerClickEventGet()
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_GRAPHICS_SystemSettingLayerClickEventGet\n", __LINE__);
	
    UINT16 SystemSettingWindowPositionX = WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].StartX;
    UINT16 SystemSettingWindowPositionY = WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].StartY;

    //Wall paper setting
    if (iMouseX >= 3 + SystemSettingWindowPositionX && iMouseX <= 3 + 4 * 16  + SystemSettingWindowPositionX  
         && iMouseY >= 3 + SystemSettingWindowPositionY && iMouseY <= 3 + SystemSettingWindowPositionY + 16)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: WALLPAPER_SETTING_CLICKED_EVENT\n", __LINE__);
        return WALLPAPER_SETTING_CLICKED_EVENT;
    }
    
    //Wall paper Reset
    if (iMouseX >= 3 + SystemSettingWindowPositionX && iMouseX <= 3 + 4 * 16  + SystemSettingWindowPositionX  
         && iMouseY >= 3 + SystemSettingWindowPositionY + 16 && iMouseY <= 3 + SystemSettingWindowPositionY + 16 * 2)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: WALLPAPER_RESET_CLICKED_EVENT\n", __LINE__);
        return WALLPAPER_RESET_CLICKED_EVENT;
    }

    // Hide Memory Information window
    if (iMouseX >= SystemSettingWindowPositionX + SystemSettingWindowWidth - 20 && iMouseX <=  SystemSettingWindowPositionX + SystemSettingWindowWidth - 4 
            && iMouseY >= SystemSettingWindowPositionY+ 0 && iMouseY <= SystemSettingWindowPositionY + 16)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: SYSTEM_SETTING_CLOSE_CLICKED_EVENT\n", __LINE__);
        return SYSTEM_SETTING_CLOSE_CLICKED_EVENT;
    }	

	return MAX_CLICKED_EVENT;

}



MOUSE_CLICK_EVENT L2_GRAPHICS_MyComputerLayerClickEventGet()
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_GRAPHICS_MyComputerLayerClickEventGet\n", __LINE__);
		
    UINT16 MyComputerPositionX = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartX;
    UINT16 MyComputerPositionY = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartY;

    // Hide My computer window
    if (iMouseX >= MyComputerPositionX + MyComputerWidth - 20 && iMouseX <=  MyComputerPositionX + MyComputerWidth - 4 
            && iMouseY >= MyComputerPositionY + 0 && iMouseY <= MyComputerPositionY + 16)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MY_COMPUTER_CLOSE_CLICKED_EVENT\n", __LINE__);
        return MY_COMPUTER_CLOSE_CLICKED_EVENT;
    }
	
    for (UINT16 i = 0 ; i < PartitionCount; i++)
    {
		UINT16 StartX = MyComputerPositionX + 50;
		UINT16 StartY = MyComputerPositionY + i * 18 + 16 * 2;
		
		if (iMouseX >= StartX && iMouseX <=  StartX + 16 * 4 
            && iMouseY >= StartY + 0 && iMouseY <= StartY + 16)
		{
			PartitionItemID = i;
			L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MY_COMPUTER_PARTITION_ITEM_CLICKED_EVENT PartitionItemID: %d\n", __LINE__, PartitionItemID);
			return MY_COMPUTER_PARTITION_ITEM_CLICKED_EVENT;
		}
	}

    UINT16 HeightNew = SYSTEM_ICON_HEIGHT / 8;
    UINT16 WidthNew = SYSTEM_ICON_WIDTH / 8;

	//Only 6 item, need to fix after test.
    for (UINT16 i = 0 ; i < 6; i++)
    {
		UINT16 StartX = MyComputerPositionX + 130;
		UINT16 StartY = MyComputerPositionY + i  * (HeightNew + 16 * 2) + 200;
		
		if (iMouseX >= StartX && iMouseX <=  StartX + WidthNew 
            && iMouseY >= StartY && iMouseY <= StartY + HeightNew)
		{
			FolderItemID = i;
			L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MY_COMPUTER_PARTITION_ITEM_CLICKED_EVENT FolderItemID: %d\n", __LINE__, FolderItemID);
			return MY_COMPUTER_FOLDER_ITEM_CLICKED_EVENT;
		}
	}

	return MAX_CLICKED_EVENT;
}


MOUSE_CLICK_EVENT L2_GRAPHICS_SystemLogLayerClickEventGet()
{	
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_GRAPHICS_SystemLogLayerClickEventGet\n", __LINE__);
    UINT16 SystemLogWindowPositionX = WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].StartX;
    UINT16 SystemLogWindowPositionY = WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].StartY;

    // Hide System Log window
    if (iMouseX >= SystemLogWindowPositionX + SystemLogWindowWidth - 20 && iMouseX <=  SystemLogWindowPositionX + SystemLogWindowWidth - 4 
            && iMouseY >= SystemLogWindowPositionY+ 0 && iMouseY <= SystemLogWindowPositionY + 16)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: SYSTEM_LOG_CLOSE_CLICKED_EVENT\n", __LINE__);
        return SYSTEM_LOG_CLOSE_CLICKED_EVENT;
    }    

	return MAX_CLICKED_EVENT;			
}


MOUSE_CLICK_EVENT L2_GRAPHICS_MemoryInformationLayerClickEventGet()
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_GRAPHICS_MemoryInformationLayerClickEventGet\n", __LINE__);

    UINT16 MemoryInformationWindowPositionX = WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].StartX;
    UINT16 MemoryInformationWindowPositionY = WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].StartY;
    
    // Hide Memory Information window
    if (iMouseX >= MemoryInformationWindowPositionX + MemoryInformationWindowWidth - 20 && iMouseX <=  MemoryInformationWindowPositionX + MemoryInformationWindowWidth - 4 
            && iMouseY >= MemoryInformationWindowPositionY+ 0 && iMouseY <= MemoryInformationWindowPositionY + 16)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MEMORY_INFORMATION_CLOSE_CLICKED_EVENT\n", __LINE__);
        return MEMORY_INFORMATION_CLOSE_CLICKED_EVENT;
    }
			
	return MAX_CLICKED_EVENT;
}

VOID L3_GRAPHICS_DeskClickEventHandle(MOUSE_CLICK_EVENT event)
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: event: %d\n", __LINE__, event);

	switch(event)
	{
		case START_MENU_CLICKED_EVENT: 
			L3_GRAPHICS_StartMenuClicked(); break;
		
		default: break;
	}
}


VOID L3_GRAPHICS_StartMenuClickEventHandle(MOUSE_CLICK_EVENT event)
{ 
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: event: %d StartMenuNextState: %d\n", __LINE__, event, StartMenuNextState);
    for (UINT16 i = 0; i <  sizeof(StartMenuStateTransformTable)/sizeof(StartMenuStateTransformTable[0]); i++ )
    {
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: StartMenuStateTransformTable[i].CurrentState: %d\n", __LINE__, StartMenuStateTransformTable[i].CurrentState);
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: i: %d\n", __LINE__, i);
        if (StartMenuStateTransformTable[i].CurrentState == StartMenuNextState 
            && event == StartMenuStateTransformTable[i].event )
        {
            StartMenuNextState = StartMenuStateTransformTable[i].NextState;

            // need to check the return value after function runs..... 
            StartMenuStateTransformTable[i].pFunc();
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MouseClickEvent: %d StartMenuNextState: %d\n", __LINE__, MouseClickEvent, StartMenuNextState);
            break;
        }   
    }

}


VOID L3_GRAPHICS_SystemSettingClickEventHandle(MOUSE_CLICK_EVENT event)
{

}


VOID L3_GRAPHICS_MyComupterClickEventHandle(MOUSE_CLICK_EVENT event)
{
	
    for (int i = 0; i <  sizeof(MyComputerStateTransformTable)/sizeof(MyComputerStateTransformTable[0]); i++ )
    {
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MyComputerStateTransformTable[i].CurrentState: %d\n", __LINE__, MyComputerStateTransformTable[i].CurrentState);
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: i: %d\n", __LINE__, i);
        if (MyComputerStateTransformTable[i].CurrentState == MyComputerNextState 
            && event == MyComputerStateTransformTable[i].event )
        {
            MyComputerNextState = MyComputerStateTransformTable[i].NextState;

            // need to check the return value after function runs..... 
            MyComputerStateTransformTable[i].pFunc();
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MouseClickEvent: %d StartMenuNextState: %d\n", __LINE__, MouseClickEvent, StartMenuNextState);
            break;
        }   
    }

}



VOID L3_GRAPHICS_SystemLogClickEventHandle(MOUSE_CLICK_EVENT event)
{

}


VOID L3_GRAPHICS_MemoryInformationClickEventHandle(MOUSE_CLICK_EVENT event)
{

}


//Get click event from different graphics layer.
GRAPHICS_LAYER_EVENT_GET GraphicsLayerEventHandle[] =
{
    {GRAPHICS_LAYER_DESK,       					 L2_GRAPHICS_DeskLayerClickEventGet, 				L3_GRAPHICS_DeskClickEventHandle},
    {GRAPHICS_LAYER_START_MENU,            			 L2_GRAPHICS_StartMenuLayerClickEventGet, 			L3_GRAPHICS_StartMenuClickEventHandle},
    {GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW,           L2_GRAPHICS_SystemSettingLayerClickEventGet, 		L3_GRAPHICS_SystemSettingClickEventHandle},
    {GRAPHICS_LAYER_MY_COMPUTER_WINDOW,            	 L2_GRAPHICS_MyComputerLayerClickEventGet, 			L3_GRAPHICS_MyComupterClickEventHandle},
    {GRAPHICS_LAYER_SYSTEM_LOG_WINDOW,            	 L2_GRAPHICS_SystemLogLayerClickEventGet, 			L3_GRAPHICS_SystemLogClickEventHandle},
    {GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW,       L2_GRAPHICS_MemoryInformationLayerClickEventGet, 	L3_GRAPHICS_MemoryInformationClickEventHandle},
};


UINT16 L2_MOUSE_ClickEventHandle()
{   
	UINT16 LayerID = pDeskDisplayBuffer[(iMouseY * ScreenWidth + iMouseX) * 4 + 3];

	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: iMouseX: %d iMouseY: %d ClickFlag: %d, LayerID: %d\n", __LINE__, iMouseX, iMouseY, MouseClickFlag, LayerID);

	// Get click event
	MOUSE_CLICK_EVENT event = GraphicsLayerEventHandle[LayerID].pClickEventGet();
    MouseClickFlag = MOUSE_NO_CLICKED;

	// Handle click event
	GraphicsLayerEventHandle[LayerID].pClickEventHandle(event);

	return;
	
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].DisplayFlag = FALSE;
    
    if (TRUE  == WindowLayers.item[GRAPHICS_LAYER_START_MENU].DisplayFlag)
    {
        WindowLayers.ActiveWindowCount--;
        WindowLayers.item[GRAPHICS_LAYER_START_MENU].DisplayFlag = FALSE;
    }
    
    StartMenuNextState = CLICK_INIT_STATE;

    return START_MENU_INIT_CLICKED_EVENT;
}


VOID L2_MOUSE_Click()
{
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: iMouseX: %d iMouseY: %d \n",  __LINE__, iMouseX, iMouseY);
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    Color.Red = 0xff;
    Color.Green= 0xff;
    Color.Blue= 0x00;
    
    if ( MouseClickFlag != MOUSE_LEFT_CLICKED)
    {
        return;
    }

    L2_MOUSE_ClickEventHandle();
	
    return;
	
    if (START_MENU_INIT_CLICKED_EVENT == MouseClickEvent)
        
    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MouseClickEvent: %d \n", __LINE__, MouseClickEvent);

    MouseClickFlag = MOUSE_NO_CLICKED;
    EFI_STATUS Status;

    if (MouseClickEvent == MY_COMPUTER_CLOSE_CLICKED_EVENT)
    {
        //DisplayMyComputerFlag = 0;
        if (TRUE  == WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].DisplayFlag)
        {
            WindowLayers.ActiveWindowCount--;
            WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].DisplayFlag = FALSE;
        }
        return;
    }

    if (MouseClickEvent == SYSTEM_LOG_CLOSE_CLICKED_EVENT)
    {
        //DisplaySystemLogWindowFlag = 0;
        
        if (TRUE  == WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].DisplayFlag)
        {
            WindowLayers.ActiveWindowCount--;
            WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].DisplayFlag = FALSE;
        }
    
        return;
    }

    if (MouseClickEvent == SYSTEM_SETTING_CLOSE_CLICKED_EVENT)
    {
        //DisplaySystemSettingWindowFlag = 0;
        if (TRUE  == WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].DisplayFlag)
        {
            WindowLayers.ActiveWindowCount--;
            WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].DisplayFlag = FALSE;
        }
        return;
    }

    if (MouseClickEvent == MEMORY_INFORMATION_CLOSE_CLICKED_EVENT)
    {
        //DisplayMemoryInformationWindowFlag = 0;
        if (TRUE  == WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].DisplayFlag)
        {
            WindowLayers.ActiveWindowCount--;
            WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].DisplayFlag = FALSE;
        }
        return;
    }

    if (MouseClickEvent == MY_COMPUTER_CLICKED_EVENT || MouseClickEvent == MEMORY_INFORMATION_CLICKED_EVENT  || MouseClickEvent == SYSTEM_LOG_CLICKED_EVENT
        || MouseClickEvent == WALLPAPER_RESET_CLICKED_EVENT  || MouseClickEvent == WALLPAPER_SETTING_CLICKED_EVENT)
    {
        //DisplayStartMenuFlag = 0;
        //WindowLayers.item[2].DisplayFlag = 0;
        if (TRUE  == WindowLayers.item[GRAPHICS_LAYER_START_MENU].DisplayFlag)
        {
            WindowLayers.ActiveWindowCount--;
            WindowLayers.item[GRAPHICS_LAYER_START_MENU].DisplayFlag = FALSE;
        }
    }    
}




EFI_STATUS L2_GRAPHICS_DeskInit()
{
    
    EFI_STATUS status = 0;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    UINT32 x = ScreenWidth;
    UINT32 y = ScreenHeight;

	Color.Reserved = GRAPHICS_LAYER_DESK;
    
    for (int i = 0; i < ScreenHeight; i++)
    {
        for (int j = 0; j < ScreenWidth; j++)
        {
            // BMP 3bits, and desk buffer 4bits
            pDeskBuffer[(i * ScreenWidth + j) * 4]     = pDeskWallpaperBuffer[0x36 + ((ScreenHeight - i) * 1920 + j) * 3 ];
            pDeskBuffer[(i * ScreenWidth + j) * 4 + 1] = pDeskWallpaperBuffer[0x36 + ((ScreenHeight - i) * 1920 + j) * 3 + 1];
            pDeskBuffer[(i * ScreenWidth + j) * 4 + 2] = pDeskWallpaperBuffer[0x36 + ((ScreenHeight - i) * 1920 + j) * 3 + 2];
            pDeskBuffer[(i * ScreenWidth + j) * 4 + 3] = GRAPHICS_LAYER_DESK;

            //white
            //pDeskBuffer[(i * ScreenWidth + j) * 4]     = 0xff;
            //pDeskBuffer[(i * ScreenWidth + j) * 4 + 1] = 0xff;
            //pDeskBuffer[(i * ScreenWidth + j) * 4 + 2] = 0xff;
        }
    }
        
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: COMPUTER BMP\n", __LINE__);
    
    //the following three for loop is to draw system icon on desk
    /*
    for (UINT8 i = 0; i < SYSTEM_ICON_MAX; i++)
    {
        for (int j = 0; j < SYSTEM_ICON_HEIGHT; j++)
        {
            for (int k = 0; k < SYSTEM_ICON_WIDTH; k++)
            {
                pDeskBuffer[(j * ScreenWidth + 400 * i + k) * 4 ]     = pSystemIconBuffer[i][0x36 + ((SYSTEM_ICON_HEIGHT - j) * SYSTEM_ICON_WIDTH + k) * 3 ];
                pDeskBuffer[(j * ScreenWidth + 400 * i + k) * 4 + 1 ] = pSystemIconBuffer[i][0x36 + ((SYSTEM_ICON_HEIGHT - j) * SYSTEM_ICON_WIDTH + k) * 3 + 1 ];
                pDeskBuffer[(j * ScreenWidth + 400 * i + k) * 4 + 2 ] = pSystemIconBuffer[i][0x36 + ((SYSTEM_ICON_HEIGHT - j) * SYSTEM_ICON_WIDTH + k) * 3 + 2 ];
            }
        }
    }       
    */
    UINT16 HeightNew = SYSTEM_ICON_HEIGHT / 4;
    UINT16 WidthNew = SYSTEM_ICON_WIDTH / 4;
    //UINT8 *pSource = pSystemIconBuffer[0][0x36];

    //Skip bmp header.
    for (UINT32 i = 0; i < 384000; i++)
        pSystemIconTempBuffer2[i] = pSystemIconBuffer[SYSTEM_ICON_MYCOMPUTER][0x36 + i];

    L1_GRAPHICS_ZoomImage(pSystemIconMyComputerBuffer, WidthNew, HeightNew, pSystemIconTempBuffer2, SYSTEM_ICON_WIDTH, SYSTEM_ICON_HEIGHT);
    
    int x1, y1;
    x1 = 20;
    y1 = 20;
    L3_GRAPHICS_ItemPrint(pDeskBuffer, pSystemIconMyComputerBuffer, ScreenWidth, ScreenHeight, WidthNew, HeightNew, x1, y1, "", 1, GRAPHICS_LAYER_DESK);

    
    y1 += HeightNew;
    Color.Blue  = 0xff;
    Color.Red   = 0xff;
    Color.Green = 0xff;
    // wo de dian nao
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x1, y1, (46 - 1) * 94 + 50 - 1, Color, ScreenWidth);
    x1 += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x1, y1, (21 - 1) * 94 + 36 - 1, Color, ScreenWidth);
    x1 += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x1, y1, (21 - 1) * 94 + 71 - 1, Color, ScreenWidth);
    x1 += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x1, y1, (36 - 1) * 94 + 52 - 1, Color, ScreenWidth);

    y1 += 16;
    y1 += 16;

    /*
    for (int j = 0; j < HeightNew; j++)
    {
        for (int k = 0; k < WidthNew; k++)
        {
            pDeskBuffer[((20 + j) * ScreenWidth + 20 + k) * 4 ]     = pSystemIconMyComputerBuffer[((HeightNew - j) * WidthNew + k) * 3 ];
            pDeskBuffer[((20 + j) * ScreenWidth + 20 + k) * 4 + 1 ] = pSystemIconMyComputerBuffer[((HeightNew - j) * WidthNew + k) * 3 + 1 ];
            pDeskBuffer[((20 + j) * ScreenWidth + 20 + k) * 4 + 2 ] = pSystemIconMyComputerBuffer[((HeightNew - j) * WidthNew + k) * 3 + 2 ];
        }
    }
    */


    //Skip bmp header.
    for (UINT32 i = 0; i < 384000; i++)
        pSystemIconTempBuffer2[i] = pSystemIconBuffer[SYSTEM_ICON_SETTING][0x36 + i];

    L1_GRAPHICS_ZoomImage(pSystemIconMySettingBuffer, WidthNew, HeightNew, pSystemIconTempBuffer2, SYSTEM_ICON_WIDTH, SYSTEM_ICON_HEIGHT);
    
    x1 = 20;
    L3_GRAPHICS_ItemPrint(pDeskBuffer, pSystemIconMySettingBuffer, ScreenWidth, ScreenHeight, WidthNew, HeightNew, x1, y1, "", 1, GRAPHICS_LAYER_DESK);

    
    y1 += HeightNew;
    // wo de dian nao
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x1, y1, (46 - 1) * 94 + 50 - 1, Color, ScreenWidth);
    x1 += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x1, y1, (21 - 1) * 94 + 36 - 1, Color, ScreenWidth);
    x1 += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x1, y1, (21 - 1) * 94 + 71 - 1, Color, ScreenWidth);
    x1 += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x1, y1, (36 - 1) * 94 + 52 - 1, Color, ScreenWidth);

    y1 += 16;
    y1 += 16;

    
    /*
    for (int j = 0; j < HeightNew; j++)
    {
        for (int k = 0; k < WidthNew; k++)
        {
            pDeskBuffer[((120 + j) * ScreenWidth + 20 + k) * 4 ]     = pSystemIconMySettingBuffer[((HeightNew - j) * WidthNew + k) * 3 ];
            pDeskBuffer[((120 + j) * ScreenWidth + 20 + k) * 4 + 1 ] = pSystemIconMySettingBuffer[((HeightNew - j) * WidthNew + k) * 3 + 1 ];
            pDeskBuffer[((120 + j) * ScreenWidth + 20 + k) * 4 + 2 ] = pSystemIconMySettingBuffer[((HeightNew - j) * WidthNew + k) * 3 + 2 ];
        }
    }*/

    //Skip bmp header.
    for (UINT32 i = 0; i < 384000; i++)
        pSystemIconTempBuffer2[i] = pSystemIconBuffer[SYSTEM_ICON_RECYCLE][0x36 + i];

    L1_GRAPHICS_ZoomImage(pSystemIconRecycleBuffer, WidthNew, HeightNew, pSystemIconTempBuffer2, SYSTEM_ICON_WIDTH, SYSTEM_ICON_HEIGHT);
    /*
    for (int j = 0; j < HeightNew; j++)
    {
        for (int k = 0; k < WidthNew; k++)
        {
            pDeskBuffer[((220 + j) * ScreenWidth + 20 + k) * 4 ]     = pSystemIconRecycleBuffer[((HeightNew - j) * WidthNew + k) * 3 ];
            pDeskBuffer[((220 + j) * ScreenWidth + 20 + k) * 4 + 1 ] = pSystemIconRecycleBuffer[((HeightNew - j) * WidthNew + k) * 3 + 1 ];
            pDeskBuffer[((220 + j) * ScreenWidth + 20 + k) * 4 + 2 ] = pSystemIconRecycleBuffer[((HeightNew - j) * WidthNew + k) * 3 + 2 ];
        }
    }
    */
        
    x1 = 20;

    L3_GRAPHICS_ItemPrint(pDeskBuffer, pSystemIconRecycleBuffer, ScreenWidth, ScreenHeight, WidthNew, HeightNew, x1, y1, "", 1, GRAPHICS_LAYER_DESK);

    
    y1 += HeightNew;
    // wo de dian nao
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x1, y1, (46 - 1) * 94 + 50 - 1, Color, ScreenWidth);
    x1 += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x1, y1, (21 - 1) * 94 + 36 - 1, Color, ScreenWidth);
    x1 += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x1, y1, (21 - 1) * 94 + 71 - 1, Color, ScreenWidth);
    x1 += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x1, y1, (36 - 1) * 94 + 52 - 1, Color, ScreenWidth);

    y1 += 16;
    y1 += 16;

    
    // line
    Color.Red   = 0xC6;
    Color.Green = 0xC6;
    Color.Blue  = 0xC6;
    L1_MEMORY_RectangleFill(pDeskBuffer, 0,     y - 28, x -  1, y - 28, 1, Color); // area top

    // line
    Color.Red   = 0xFF;
    Color.Green = 0xFF;
    Color.Blue  = 0xFF;
    L1_MEMORY_RectangleFill(pDeskBuffer, 0,     y - 27, x -  1, y - 27, 1, Color); // line top

    // rectangle
    Color.Red   = 0xC6;
    Color.Green = 0xC6;
    Color.Blue  = 0xC6;
    L1_MEMORY_RectangleFill(pDeskBuffer, 0,     y - 26, x -  1, y -  1, 1, Color); // area task bar

    // Menu Button
    L2_GRAPHICS_ButtonDraw();

    L2_GRAPHICS_ButtonDraw2(16 * 6, ScreenHeight - 22, 16 * 4, 16);

    L2_GRAPHICS_ButtonDraw2(16 * 11, ScreenHeight - 22, 16 * 4, 16);

    L2_GRAPHICS_ButtonDraw2(16 * 16, ScreenHeight - 22, 16 * 4, 16);

    Color.Red   = 0x84;
    Color.Green = 0x84;
    Color.Blue  = 0x84;
    L1_MEMORY_RectangleFill(pDeskBuffer, DISPLAY_DESK_DATE_TIME_X, y - 24, x -  4, y - 24, 1, Color); // line
    //L1_MEMORY_RectangleFill(pDeskBuffer, DISPLAY_DESK_DATE_TIME_X, y - 23, x - 47, y -  4, 1, Color); // area
    
    Color.Red   = 0xFF;
    Color.Green = 0xFF;
    Color.Blue  = 0xFF;
    L1_MEMORY_RectangleFill(pDeskBuffer, DISPLAY_DESK_DATE_TIME_X,    y - 3, x - 4,     y - 3, 1, Color); // line
    L1_MEMORY_RectangleFill(pDeskBuffer, x - 3,     y - 24, x - 3,     y - 3, 1, Color); //line

    //Black
    Color.Red   = 0x00;
    Color.Green = 0x00;
    Color.Blue   = 0x00;

    /*
    for (UINT16 i = 1; i < 10; i++)
    {
        for (UINT16 j = 1; j < 22; j++)
        {
            L2_GRAPHICS_ChineseCharDraw(pDeskBuffer,  (16 + 2) * j, i * (16 + 2) + 322, (i - 1) * 94 + j - 1, Color, ScreenWidth);
        }
    }*/


    for (int j = 0; j < 150; j++)
    {
        //L2_DEBUG_Print1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", sChineseChar[j] & 0xff);
    }

    // menu chinese
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer,  16, ScreenHeight - 21,     (18 - 1) * 94 + 43 - 1, Color, ScreenWidth);
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer,  16 * 2, ScreenHeight - 21, (21 - 1) * 94 + 05 - 1, Color, ScreenWidth);
}

