
/*************************************************
    .
    File name:      	A.c
    Author：	        任启红
    ID：		00001
    Version: Date:  	202107
    Description:    	整个模块的主入口函数
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




#include <L1_MEMORY.h>

#include <Global/Global.h>

UINT8 *pSystemIconMyComputerBuffer; //after zoom in or zoom out

float MemorySize = 0;
MEMORY_ALLOCATED_CURRENT MemoryAllocatedCurrent;
UINT64 FreeNumberOfPages = ALL_PAGE_COUNT;
UINT64 SystemAllPagesAllocated = 0;
UINT8 *pMapper = (UINT8 *)(PHYSICAL_ADDRESS_START);

UINT8 *FAT32_Table = NULL;

UINT8 *pDateTimeBuffer = NULL; //Mouse layer: 3

// size: the one unit size is Bytes 
UINT8 *L2_MEMORY_Allocate(char *pApplicationName, UINT16 type, UINT32 SizeRequired)
{
    // Allocate minimize unit is 4K
    UINT32 PagesRequired = SizeRequired / (ALLOCATE_UNIT_SIZE);
    UINT8  AddOne = (SizeRequired % (ALLOCATE_UNIT_SIZE) != 0) ? 1 : 0;
    
    PagesRequired += AddOne;
    UINT16 currentCount= MemoryAllocatedCurrent.AllocatedSliceCount;
    L1_MEMORY_Copy(MemoryAllocatedCurrent.items[currentCount].ApplicationName, pApplicationName, L1_STRING_Length(pApplicationName));
    MemoryAllocatedCurrent.items[currentCount].AllocatedSize = SizeRequired;
    MemoryAllocatedCurrent.items[currentCount].MemoryType = type;

    MemoryAllocatedCurrent.AllocatedPageSum += PagesRequired;
    MemoryAllocatedCurrent.AllocatedSizeSum += SizeRequired;
    
    MemoryAllocatedCurrent.AllocatedSliceCount++;
    
    INFO_SELF(L"Name: %a, Size: 0x%X, Pages: 0x%X \r\n", pApplicationName, SizeRequired, PagesRequired);  

    UINT64 j = 0;
    UINT64 k = 0;

    //use 2G memory, page size: 1024 * 4B, Mapper use size: 2 * 1024 * 1024 * 1024 / (1024 * 4 ) =  524288 = 0x80000
    UINT8 *pData = (UINT8 *)(PHYSICAL_ADDRESS_START + ALL_PAGE_COUNT);      
    //all pages 
    for (j = 0; j < ALL_PAGE_COUNT; j++)
    {           
        //INFO_SELF(L"j: %d\r\n", j);  
        //find first page is no use 
        //mapper 0: no use, 1: using
        if (pMapper[j] == 0)
        {
            // then check the next n - 1 pages
            for (k = 1; k < PagesRequired; k++)
            {
                if (pMapper[j + k] != 0)
                {
                    j += k;
                    break;
                }
            }

            // found 
            if (k >= PagesRequired)
            {
                //INFO_SELF(L"k: %d \r\n", k);  
                INFO_SELF(L"Block j: 0x%X, k: 0x%X, Allocated: 0x%X \r\n", j, k, SystemAllPagesAllocated); 
                SystemAllPagesAllocated += k;

                // Update mapper array from j to j + PagesRequired - 1
                for (UINT64 m = j; m < PagesRequired + j; m++)
                    pMapper[m] = 1;

                FreeNumberOfPages -= PagesRequired;
                
                INFO_SELF(L"Success: 0x%X, Start: 0x%X, Required: 0x%X, Physical: 0x%X, Free: 0x%X\r\n", 
                            pData, 
                            j, 
                            PagesRequired, 
                            pData + j * ALLOCATE_UNIT_SIZE, 
                            FreeNumberOfPages);  

                L1_MEMORY_Memset(pData + j * ALLOCATE_UNIT_SIZE, 0, SizeRequired);
                
                INFO_SELF(L"\r\n");

                // start physical address
                return pData + j * ALLOCATE_UNIT_SIZE;
            }
        }
    }

    if (ALL_PAGE_COUNT <= j)
    {
        return NULL;
    }
}



UINT8 *L2_MEMORY_MapperInitial()
{

    for (UINT64 temp = 0; temp < ALL_PAGE_COUNT; temp++)
    {
        pMapper[temp] = 0; // no use
    }
}


extern UINT8 BufferMFT[DISK_BUFFER_SIZE * 2];


EFI_STATUS L2_COMMON_MemoryAllocate()
{
    EFI_STATUS  Status;
    
    INFO_SELF(L"\r\n");

    L2_MEMORY_MapperInitial();
    
    L2_MEMORY_CountInitial();

    pDeskBuffer = L2_MEMORY_Allocate("Desk Buffer", MEMORY_TYPE_GRAPHICS, ScreenWidth * ScreenHeight * 4);
    
    pDeskDisplayBuffer = L2_MEMORY_Allocate("Desk Display Buffer", MEMORY_TYPE_GRAPHICS, ScreenWidth * ScreenHeight * 4);

    pDeskWallpaperBuffer = L2_MEMORY_Allocate("Desk Wall paper Buffer", MEMORY_TYPE_GRAPHICS, ScreenWidth * ScreenHeight * 3 + 0x36);

    UINT32 size = 267616;
    
    sChineseChar = (UINT8 *)L2_MEMORY_Allocate("Chinese Char Buffer", MEMORY_TYPE_GRAPHICS, size);
    if (NULL == sChineseChar)
    {
        //DEBUG ((EFI_D_INFO, "ChineseCharArrayInit AllocateZeroPool Failed: %x!\n "));
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: ChineseCharArrayInit AllocateZeroPool failed\n",  __LINE__);
        return -1;
    }   

    for (UINT8 i = 0; i < SYSTEM_ICON_MAX; i++)
        pSystemIconBuffer[i] = L2_MEMORY_Allocate("System Icon Buffer", MEMORY_TYPE_GRAPHICS, 384054);

    
    pSystemIconFolderBuffer = L2_MEMORY_Allocate("System pSystemIconFolder Buffer", MEMORY_TYPE_GRAPHICS, 384054);
    pSystemIconTextBuffer   = L2_MEMORY_Allocate("System pSystemIconText Buffer", MEMORY_TYPE_GRAPHICS, 384054);
    pSystemIconTempBuffer2  = L2_MEMORY_Allocate("System Icon temp2 Buffer", MEMORY_TYPE_GRAPHICS, 384054);
        
    pMouseClickBuffer = (UINT8 *)L2_MEMORY_Allocate("Mouse Click Buffer", MEMORY_TYPE_GRAPHICS, MouseClickWindowWidth * MouseClickWindowHeight * 4); 
    if (pMouseClickBuffer == NULL)
    {
        return -1;
    }   

    pMemoryInformationBuffer = (UINT8 *)L2_MEMORY_Allocate("Date Time Buffer", MEMORY_TYPE_GRAPHICS, MemoryInformationWindowWidth * MemoryInformationWindowHeight * 4); 
    if (pMemoryInformationBuffer == NULL)
    {
        return -1;
    }  
    
    pDateTimeBuffer = (UINT8 *)L2_MEMORY_Allocate("Date Time Buffer", MEMORY_TYPE_GRAPHICS, 8 * 16 * 50 * 4); 
    if (pDateTimeBuffer == NULL)
    {
        return -1;
    }   
    
    pMouseBuffer = (UINT8 *)L2_MEMORY_Allocate("Mouse Buffer", MEMORY_TYPE_GRAPHICS, 16 * 16 * 4);
    if (NULL == pMouseBuffer)
    {
        //DEBUG ((EFI_D_INFO, "MultiProcessInit pMouseBuffer pDeskDisplayBuffer NULL\n"));
        return -1;
    }
    
    pMouseSelectedBuffer = (UINT8 *)L2_MEMORY_Allocate("Mouse Selected Buffer", MEMORY_TYPE_GRAPHICS, 16 * 16 * 2 * 4);
    if (NULL == pMouseSelectedBuffer)
    {
        //DEBUG ((EFI_D_INFO, "ScreenInit AllocatePool pDeskDisplayBuffer NULL\n"));
        return -1;
    }
    
    pStartMenuBuffer = (UINT8 *)L2_MEMORY_Allocate("Start Menu Buffer", MEMORY_TYPE_GRAPHICS, StartMenuWidth * StartMenuHeight * 4);
    if (NULL == pStartMenuBuffer)
    {
        //DEBUG ((EFI_D_INFO, "ChineseCharArrayInit pStartMenuBuffer Failed: %x!\n "));
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: pStartMenuBuffer AllocateZeroPool failed\n",  __LINE__);
        return -1;
    }   
    
    pMyComputerBuffer = (UINT8 *)L2_MEMORY_Allocate("My Computer Buffer", MEMORY_TYPE_GRAPHICS, MyComputerWidth * MyComputerHeight * 4); 
    if (pMyComputerBuffer == NULL)
    {
        //DEBUG ((EFI_D_INFO, "MyComputer , AllocateZeroPool failed... "));
        return -1;
    }       

    pSystemIconMyComputerBuffer = L2_MEMORY_Allocate("System pSystemIconMyComputerBuffer", MEMORY_TYPE_GRAPHICS, 384054);
    pSystemIconMySettingBuffer  = L2_MEMORY_Allocate("System pSystemIconMySettingBuffer", MEMORY_TYPE_GRAPHICS, 384054);
    pSystemIconRecycleBuffer    = L2_MEMORY_Allocate("System pSystemIconRecycleBuffer", MEMORY_TYPE_GRAPHICS, 384054);
    
    pSystemLogWindowBuffer = (UINT8 *)L2_MEMORY_Allocate("System Log Window Buffer", MEMORY_TYPE_GRAPHICS, SystemLogWindowWidth * SystemLogWindowHeight * 4);
    if (pSystemLogWindowBuffer == NULL)
    {
        //DEBUG ((EFI_D_INFO, "pSystemLogWindowBuffer , AllocateZeroPool failed... "));
        return -1;
    }
    
    MouseColor.Blue  = 0xff;
    MouseColor.Red   = 0xff;
    MouseColor.Green = 0xff;

    for (int i = 0; i < StartMenuHeight; i++)
        for (int j = 0; j < StartMenuWidth; j++)
        {
            pStartMenuBuffer[(i * StartMenuWidth + j) * 4 + 0] = 237;
            pStartMenuBuffer[(i * StartMenuWidth + j) * 4 + 1] = 234;
            pStartMenuBuffer[(i * StartMenuWidth + j) * 4 + 2] = 231;
            pStartMenuBuffer[(i * StartMenuWidth + j) * 4 + 3] = GRAPHICS_LAYER_START_MENU;
        }

    pSystemSettingWindowBuffer = (UINT8 *)L2_MEMORY_Allocate("System Setting Window Buffer", MEMORY_TYPE_GRAPHICS, SystemSettingWindowWidth * SystemSettingWindowHeight * 4);
    if (NULL == pSystemSettingWindowBuffer)
    {
        //DEBUG ((EFI_D_INFO, "ChineseCharArrayInit pStartMenuBuffer Failed: %x!\n "));
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: pStartMenuBuffer AllocateZeroPool failed\n",  __LINE__);
        return -1;
    }   
    
    for (int i = 0; i < SystemSettingWindowHeight; i++)
        for (int j = 0; j < SystemSettingWindowWidth; j++)
        {
            pSystemSettingWindowBuffer[(i * SystemSettingWindowWidth + j) * 4 + 0] = 237;
            pSystemSettingWindowBuffer[(i * SystemSettingWindowWidth + j) * 4 + 1] = 234;
            pSystemSettingWindowBuffer[(i * SystemSettingWindowWidth + j) * 4 + 2] = 0;
            pSystemSettingWindowBuffer[(i * SystemSettingWindowWidth + j) * 4 + 3] = GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW;
        }
    
    iMouseX = ScreenWidth / 2;
    iMouseY = ScreenHeight / 2;

    FileReadCount = 0;
    FAT32_Table = NULL;
    
    INFO_SELF(L"\r\n");

    return EFI_SUCCESS;
}
