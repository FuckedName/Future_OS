
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





#include <L2_GRAPHICS.h>
#include <string.h>

#include <Libraries/String/L2_LIBRARY_String.h>
#include <Partitions/FAT32/L2_PARTITION_FAT32.h>
#include <Global/Global.h>
#include <Memory/L1_MEMORY.h>

UINT16 MyComputerWidth = 16 * 40;
UINT16 MyComputerHeight = 16 * 80;




/****************************************************************************
*
*  描述: 创建窗口
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L3_WINDOW_Create(UINT8 *pBuffer, UINT8 *pParent, UINT16 Width, UINT16 Height, UINT16 LayerID, CHAR8 *pWindowTitle)
{   
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Width: %d \n", __LINE__, Width);
    
    UINT16 i, j;

    if (NULL == pBuffer)
    {
        //DEBUG ((EFI_D_INFO, "NULL == pBuffer"));
        return EFI_SUCCESS;
    }   
    
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    
    Color.Blue  = 0xff;
    Color.Red   = 0xff;
    Color.Green = 0xff;
    Color.Reserved = LayerID;

    L2_GRAPHICS_ChineseCharDraw(pBuffer, Width - 3 * 16 - 3, 6, (12 - 1) * 94 + 58 - 1, Color, Width);
    L2_GRAPHICS_ChineseCharDraw(pBuffer, Width - 2 * 16 - 3, 6, (01 - 1) * 94 + 85 - 1, Color, Width);
    L2_GRAPHICS_ChineseCharDraw(pBuffer, Width - 1 * 16 - 3, 6, (14 - 1) * 94 + 21 - 1, Color, Width);

    
    // 窗口最顶部标题栏
    for (i = 0; i < 23; i++)
    {
        for (j = 0; j < Width; j++)
        {
            pBuffer[(i * Width + j) * 4 + 3] = LayerID;
        }
    }

    // The Left of Window
    for (i = 23; i < Height; i++)
    {
        for (j = 0; j < Width / 3; j++)
        {
            pBuffer[(i * Width + j) * 4]     = 235;
            pBuffer[(i * Width + j) * 4 + 1] = 235;
            pBuffer[(i * Width + j) * 4 + 2] = 235;
            pBuffer[(i * Width + j) * 4 + 3] = LayerID;
        }
    }

    // The right of Window
    for (i = 23; i < Height; i++)
    {
        for (j = Width / 3 - 1; j < Width; j++)
        {
            pBuffer[(i * Width + j) * 4]     = 0xff;
            pBuffer[(i * Width + j) * 4 + 1] = 0xff;
            pBuffer[(i * Width + j) * 4 + 2] = 0xff;
            pBuffer[(i * Width + j) * 4 + 3] = LayerID;
        }
    }

    return EFI_SUCCESS;
}






/****************************************************************************
*
*  描述:   创建我的电脑窗口
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID L3_APPLICATION_MyComputerWindow(UINT16 StartX, UINT16 StartY)
{
    UINT8 *pParent;
    UINT16 Type;
    CHAR8 *pWindowTitle;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    
    Color.Blue  = 0xff;
    Color.Red   = 0xff;
    Color.Green = 0xff;
    Color.Reserved = GRAPHICS_LAYER_MY_COMPUTER_WINDOW;
    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MyComputerWidth: %d \n", __LINE__, MyComputerWidth);
    L3_WINDOW_Create(pMyComputerBuffer, pParent, MyComputerWidth, MyComputerHeight, GRAPHICS_LAYER_MY_COMPUTER_WINDOW, pWindowTitle);

    UINT8 *pBuffer = pMyComputerBuffer;

    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartX = StartX;
    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartY = StartY;
    
    int x, y;
    x = 3;
    y = 6;
    
    // wo de dian nao
    L2_GRAPHICS_ChineseCharDraw(pMyComputerBuffer, x, y, (46 - 1) * 94 + 50 - 1, Color, MyComputerWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pMyComputerBuffer, x, y, (21 - 1) * 94 + 36 - 1, Color, MyComputerWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pMyComputerBuffer, x, y, (21 - 1) * 94 + 71 - 1, Color, MyComputerWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pMyComputerBuffer, x, y, (36 - 1) * 94 + 52 - 1, Color, MyComputerWidth);

    
    Color.Blue  = 0x00;
    Color.Red   = 0x00;
    Color.Green = 0x00;

    UINT16 HeightNew = SYSTEM_ICON_HEIGHT / 8;
    UINT16 WidthNew = SYSTEM_ICON_WIDTH / 8;

    //Skip bmp header.
    for (UINT32 i = 0; i < 384000; i++)
        pSystemIconTempBuffer2[i] = pSystemIconBuffer[SYSTEM_ICON_FOLDER][0x36 + i];

    L1_GRAPHICS_ZoomImage(pSystemIconFolderBuffer, WidthNew, HeightNew, pSystemIconTempBuffer2, SYSTEM_ICON_WIDTH, SYSTEM_ICON_HEIGHT);
    
    //WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartX = StartX;
    //WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartY = StartY;
    /*
    for (int j = 0; j < HeightNew; j++)
    {
        for (int k = 0; k < WidthNew; k++)
        {
            pBuffer[((30 + j) * MyComputerWidth + 100 + k) * 4 ]     = pSystemIconFolderBuffer[((HeightNew - j) * WidthNew + k) * 3 ];
            pBuffer[((30 + j) * MyComputerWidth + 100 + k) * 4 + 1 ] = pSystemIconFolderBuffer[((HeightNew - j) * WidthNew + k) * 3 + 1 ];
            pBuffer[((30 + j) * MyComputerWidth + 100 + k) * 4 + 2 ] = pSystemIconFolderBuffer[((HeightNew - j) * WidthNew + k) * 3 + 2 ];
        }
    }*/


    //Skip bmp header.
    for (UINT32 i = 0; i < 384000; i++)
        pSystemIconTempBuffer2[i] = pSystemIconBuffer[SYSTEM_ICON_TEXT][0x36 + i];

    L1_GRAPHICS_ZoomImage(pSystemIconTextBuffer, WidthNew, HeightNew, pSystemIconTempBuffer2, SYSTEM_ICON_WIDTH, SYSTEM_ICON_HEIGHT);
    
    //WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartX = StartX;
    //WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartY = StartY;
    /*
    for (int j = 0; j < HeightNew; j++)
    {
        for (int k = 0; k < WidthNew; k++)
        {
            pBuffer[((130 + j) * MyComputerWidth + 100 + k) * 4 ]     = pSystemIconTextBuffer[((HeightNew - j) * WidthNew + k) * 3 ];
            pBuffer[((130 + j) * MyComputerWidth + 100 + k) * 4 + 1 ] = pSystemIconTextBuffer[((HeightNew - j) * WidthNew + k) * 3 + 1 ];
            pBuffer[((130 + j) * MyComputerWidth + 100 + k) * 4 + 2 ] = pSystemIconTextBuffer[((HeightNew - j) * WidthNew + k) * 3 + 2 ];
        }
    }
    */

    for (UINT16 i = 0 ; i < PartitionCount; i++)
    {
        x = 100;
        y = i * 18 + 16 * 2;        

        if (device[i].DeviceType == 2)
        {
            //
            L2_GRAPHICS_ChineseCharDraw(pBuffer, x, y,          (20 - 1 ) * 94 + 37 - 1, Color, MyComputerWidth); 
            x += 16;
        }
        else
        {
            // U pan
            L2_GRAPHICS_ChineseCharDraw(pBuffer, x, y,          (51 - 1 ) * 94 + 37 - 1, Color, MyComputerWidth); 
            x += 16;
        }   
        //pan
        L2_GRAPHICS_ChineseCharDraw(pBuffer, x, y,          (37 - 1 ) * 94 + 44 - 1, Color, MyComputerWidth); 
        x += 16;

        //  2354 分 3988 区
        L2_GRAPHICS_ChineseCharDraw(pBuffer, x, y,          (23 - 1 ) * 94 + 54 - 1, Color, MyComputerWidth);    
        x += 16;
        
        L2_GRAPHICS_ChineseCharDraw(pBuffer, x, y,          (39 - 1 ) * 94 + 88 - 1, Color, MyComputerWidth);   
        x += 16;

        // 5027
        // 一
        // 2294
        // 二
        // 4093
        // 三
        if (device[i].PartitionID == 1)
        {
            L2_GRAPHICS_ChineseCharDraw(pBuffer, x, y,          (50 - 1 ) * 94 + 27 - 1, Color, MyComputerWidth);   
            x += 16;
        }
        else if (device[i].PartitionID == 2)
        {
            L2_GRAPHICS_ChineseCharDraw(pBuffer, x, y,          (22 - 1 ) * 94 + 94 - 1, Color, MyComputerWidth);   
            x += 16;
        }
        else if (device[i].PartitionID == 3)
        {
            L2_GRAPHICS_ChineseCharDraw(pBuffer, x, y,          (40 - 1 ) * 94 + 93 - 1, Color, MyComputerWidth);   
            x += 16;
        }
        
        UINT32 size = (UINT32)device[i].SectorCount / 2; //K
        
        char sizePostfix[3] = "MB";
        size /= 1024.0; //M
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: size: %llu \n", __LINE__, size);
        if (size > 1024.0)
        {
            //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: size: %llu \n", __LINE__, size);
            size /= 1024;   
            sizePostfix[0] = 'G';
        }

        //2083
        //大
        //4801
        //小
        L2_GRAPHICS_ChineseCharDraw(pBuffer, x, y,          (20 - 1 ) * 94 + 83 - 1, Color, MyComputerWidth);  
        x += 16;
        
        L2_GRAPHICS_ChineseCharDraw(pBuffer, x, y,          (48 - 1 ) * 94 + 1 - 1, Color, MyComputerWidth);  
        x += 16;

        L2_DEBUG_Print3(x, y, WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW], "%d%a", size, sizePostfix);
        x += 16;

        x += 64;

        char type[10] = "OTHER";
        //UINT8 FileSystemType = L2_FILE_PartitionTypeAnalysis(i);
        if (FILE_SYSTEM_FAT32 == device[i].FileSystemType)
        {
            type[0] = 'F';
            type[1] = 'A';
            type[2] = 'T';
            type[3] = '3';
            type[4] = '2';
            type[5] = '\0';
        }
        else if(FILE_SYSTEM_NTFS == device[i].FileSystemType) 
        {   
            type[0] = 'N';
            type[1] = 'T';
            type[2] = 'F';
            type[3] = 'S';
            type[4] = '\0';
        }
		//L2_FILE_PartitionNameGet(i);
		
		L2_DEBUG_Print3(0, y, WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW], "%a", device[i].PartitionName);

        L2_DEBUG_Print3(x, y, WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW], "%a", type);
        
    }
    
    y += 16;
    y += 16;
    
    x = 100;
    //3658
    //内
    //2070
    //存
    /*
    L2_GRAPHICS_ChineseCharDraw(pBuffer, x, y,          (36 - 1 ) * 94 + 58 - 1, Color, MyComputerWidth);  
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pBuffer, x, y,          (20 - 1 ) * 94 + 70 - 1, Color, MyComputerWidth);  
    x += 16;
    
    // Get memory infomation
    //x = 0;
    // Note: the other class memory can not use
    MemorySize = MemorySize * 4;
    MemorySize = MemorySize / (1024 * 1024);
    CHAR8 buf[7] = {0};
    char sizePostfix2[3] = "GB";
    L2_DEBUG_Print3(x, y, WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW], "%a%a", L1_STRING_FloatToString(MemorySize, 3, buf), sizePostfix2);
    x += 5 * 8;
    */
}




/****************************************************************************
*
*  描述:   创建内存详细使用信息窗口，当前显示只包含L2_MEMORY_Allocate分配的内存信息
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID L3_APPLICATION_MemoryInformationWindow(UINT16 StartX, UINT16 StartY)
{
    UINT8 *pParent;
    UINT16 Type;
    CHAR8 *pWindowTitle;
    UINT16 i = 0;
    UINT16 j = 0;
    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MemoryInformationWindowWidth: %d \n", __LINE__, MemoryInformationWindowWidth);
    L3_WINDOW_Create(pMemoryInformationBuffer, pParent, MemoryInformationWindowWidth, MemoryInformationWindowHeight, GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW, pWindowTitle);

    UINT8 *pBuffer = pMemoryInformationBuffer;
    UINT16 Width = MemoryInformationWindowWidth;
    UINT16 Height = MemoryInformationWindowHeight;
    
    WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].StartX = StartX;
    WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].StartY = StartY;
    
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    
    Color.Blue  = 0xff;
    Color.Red   = 0xff;
    Color.Green = 0xff;
    Color.Reserved = GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW;
    
    //3658
    //内
    //2070
    //存
    //汉字    区位码 汉字  区位码
    //详 4774    细   4724
    UINT16 TitleX = 3;
    UINT16 TitleY = 6;
    // wo de dian nao
    L2_GRAPHICS_ChineseCharDraw(pBuffer, TitleX, TitleY, (36 - 1) * 94 + 58 - 1, Color, Width); 

    TitleX += 16;
    L2_GRAPHICS_ChineseCharDraw(pBuffer, TitleX, TitleY, (20 - 1) * 94 + 70 - 1, Color, Width);
    
    TitleX += 16;
    L2_GRAPHICS_ChineseCharDraw(pBuffer, TitleX, TitleY, (47 - 1) * 94 + 74 - 1, Color, Width);
    
    TitleX += 16;
    L2_GRAPHICS_ChineseCharDraw(pBuffer, TitleX, TitleY, (47 - 1) * 94 + 24 - 1, Color, Width);
    
    /*
    L2_GRAPHICS_ChineseCharDraw(pBuffer, MyComputerWidth - 3 * 16 - 3, 6, (12 - 1) * 94 + 58 - 1, Color, Width);
    L2_GRAPHICS_ChineseCharDraw(pBuffer, MyComputerWidth - 2 * 16 - 3, 6, (01 - 1) * 94 + 85 - 1, Color, Width);
    L2_GRAPHICS_ChineseCharDraw(pBuffer, MyComputerWidth - 1 * 16 - 3, 6, (14 - 1) * 94 + 21 - 1, Color, Width);
    */
    
    int x = 0, y = 0;
    
    y += 16;
    y += 16;
    
    x = 50;
    //3658
    //内
    //2070
    //存
    L2_GRAPHICS_ChineseCharDraw(pBuffer, x, y,          (36 - 1 ) * 94 + 58 - 1, Color, Width);  
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pBuffer, x, y,          (20 - 1 ) * 94 + 70 - 1, Color, Width);  
    x += 16;
        
    y += 16;
    x = 3;
    L2_DEBUG_Print3(x, y, WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW], "AllocatedPageSum: %llu", MemoryAllocatedCurrent.AllocatedPageSum);
    
    y += 16;
    L2_DEBUG_Print3(x, y, WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW], "AllocatedSizeSum: %llu", MemoryAllocatedCurrent.AllocatedSizeSum);
    
    y += 16;
    L2_DEBUG_Print3(x, y, WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW], "AllocatedSliceCount: %llu", MemoryAllocatedCurrent.AllocatedSliceCount);

    for (UINT16 i = 0; i < MemoryAllocatedCurrent.AllocatedSliceCount; i++)
    {       
        y += 16;
        L2_DEBUG_Print3(x, y, WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW], "Name: %a, Size: %llu, Type: %d", 
                                        MemoryAllocatedCurrent.items[i].ApplicationName,
                                        MemoryAllocatedCurrent.items[i].AllocatedSize,
                                        MemoryAllocatedCurrent.items[i].MemoryType);
    }
    
    return EFI_SUCCESS;
}





/****************************************************************************
*
*  描述:   创建系统日志窗口，用于显示系统运行时调试日志显示，对应的图层：WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW]
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID L3_APPLICATION_SystemLogWindow(UINT16 StartX, UINT16 StartY)
{
    UINT8 *pParent;
    UINT16 Type;
    CHAR8 *pWindowTitle;
    UINT16 i = 0;
    UINT16 j = 0;
    
    UINT16 Width = SystemLogWindowWidth;
    UINT16 Height = SystemLogWindowHeight;
	
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: SystemLogWindow: %d \n", __LINE__, Width);
    L3_WINDOW_Create(pSystemLogWindowBuffer, pParent, Width, Height, GRAPHICS_LAYER_SYSTEM_LOG_WINDOW, pWindowTitle);

    UINT8 *pBuffer = pSystemLogWindowBuffer;

	WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].StartX = StartX;
	WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].StartY = StartY;
        
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    
    Color.Blue  = 0xff;
    Color.Red   = 0xff;
    Color.Green = 0xff;
    Color.Reserved = GRAPHICS_LAYER_SYSTEM_LOG_WINDOW;
    
    //汉字	区位码	汉字	区位码	汉字	区位码	汉字	区位码
    //系	4721	统	4519	日	4053	志	5430
    UINT16 TitleX = 3;
    UINT16 TitleY = 6;
    
    // 系统日志
    L2_GRAPHICS_ChineseCharDraw(pBuffer, TitleX, TitleY, (47 - 1) * 94 + 21 - 1, Color, Width); 

    TitleX += 16;
    L2_GRAPHICS_ChineseCharDraw(pBuffer, TitleX, TitleY, (45 - 1) * 94 + 19 - 1, Color, Width);
    
    TitleX += 16;
    L2_GRAPHICS_ChineseCharDraw(pBuffer, TitleX, TitleY, (40 - 1) * 94 + 53 - 1, Color, Width);
    
    TitleX += 16;
    L2_GRAPHICS_ChineseCharDraw(pBuffer, TitleX, TitleY, (54 - 1) * 94 + 30 - 1, Color, Width);

    L2_GRAPHICS_ChineseCharDraw(pBuffer, Width - 3 * 16 - 3, 6, (12 - 1) * 94 + 58 - 1, Color, Width);
    L2_GRAPHICS_ChineseCharDraw(pBuffer, Width - 2 * 16 - 3, 6, (01 - 1) * 94 + 85 - 1, Color, Width);
    L2_GRAPHICS_ChineseCharDraw(pBuffer, Width - 1 * 16 - 3, 6, (14 - 1) * 94 + 21 - 1, Color, Width);
            
    return EFI_SUCCESS;
}





/****************************************************************************
*
*  描述:   窗口显示初始化，包含我的电脑窗口、系统日志窗口、内存信息窗口
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID L3_APPLICATION_WindowsInitial()
{    
    L3_APPLICATION_MyComputerWindow(0, 50);
        
    L3_APPLICATION_SystemLogWindow(300, 10);
    
    L3_APPLICATION_MemoryInformationWindow(600, 100);
}

