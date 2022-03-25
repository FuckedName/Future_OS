
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
#include <Libraries/Languages/L1_LIBRARY_Language.h>
#include <Partitions/FAT32/L2_PARTITION_FAT32.h>
#include <Global/Global.h>
#include <Memory/L1_MEMORY.h>

UINT16 MyComputerWidth = 16 * 40;
UINT16 MyComputerHeight = 16 * 80;

typedef struct
{
	UINT8* pWindowBuffer;
	UINT16 StartX;
	UINT16 StartY;
	UINT16 MaxWidth;
	UINT16 MaxHeight;
	UINT16 CurrentWidth;
	UINT16 CurrentHeight;
}MY_COMPUTER_WINDOW;

//窗口当前已经初始化的位置信息，方便下一个模块初始化
typedef struct
{
	UINT16 StartX;
	UINT16 StartY;
}WINDOW_CURRENT_POSITION;


MY_COMPUTER_WINDOW MyComputerWindow;


/****************************************************************************
*
*  描述: 初始化窗口
         图层初始化
         窗口边框初始化
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L3_WINDOW_Initial(WINDOW_LAYER_ITEM *pWindowLayerItem, WINDOW_CURRENT_POSITION *Position)
{	
	UINT16 WindowHeight = pWindowLayerItem->WindowHeight;
	UINT16 WindowWidth = pWindowLayerItem->WindowWidth;

	//初始化图层ID
    for (UINT16 i = 0; i < WindowHeight; i++)
	{
		for (UINT16 j = 0; j < WindowWidth; j++)
		{
			pWindowLayerItem->pBuffer[(i * WindowWidth + j) * 4 + 3] = pWindowLayerItem->LayerID;
		}
	}

	//229 229 202，左
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL GrayColor;
	
    GrayColor.Blue  = 229;
    GrayColor.Red   = 229;
    GrayColor.Green = 202;

	//初始化窗口边框    
	//上边第一像素行
	for (UINT16 j = 0; j < WindowWidth; j++)
	{
		pWindowLayerItem->pBuffer[j * 4 + 0] = GrayColor.Blue;
		pWindowLayerItem->pBuffer[j * 4 + 1] = GrayColor.Red;
		pWindowLayerItem->pBuffer[j * 4 + 2] = GrayColor.Green;
	}
	
	//上边第二像素行
	for (UINT16 j = 0; j < WindowWidth; j++)
	{
		pWindowLayerItem->pBuffer[(j + WindowWidth) * 4 + 0] = WhiteColor.Blue;
		pWindowLayerItem->pBuffer[(j + WindowWidth) * 4 + 1] = WhiteColor.Green;
		pWindowLayerItem->pBuffer[(j + WindowWidth) * 4 + 2] = WhiteColor.Red;
	}
	
	//左边第一像素列
	for (UINT16 j = 0; j < WindowHeight; j++)
	{
		pWindowLayerItem->pBuffer[j * WindowWidth * 4 + 0] = 229;
		pWindowLayerItem->pBuffer[j * WindowWidth * 4 + 1] = 229;
		pWindowLayerItem->pBuffer[j * WindowWidth * 4 + 2] = 202;
	}
	
	//左边第二像素列
	for (UINT16 j = 0; j < WindowHeight; j++)
	{
		pWindowLayerItem->pBuffer[(j + WindowHeight) * WindowWidth * 4 + 0] = WhiteColor.Blue;
		pWindowLayerItem->pBuffer[(j + WindowHeight) * WindowWidth * 4 + 1] = WhiteColor.Green;
		pWindowLayerItem->pBuffer[(j + WindowHeight) * WindowWidth * 4 + 2] = WhiteColor.Red;
	}

	//上边第三、四像素行
	for (UINT16 i = 2; i <= 3; i++)
	{
		for (UINT16 j = 0; j < WindowWidth; j++)
		{
			pWindowLayerItem->pBuffer[(i * WindowWidth + j) * 4 + 0] = WhiteColor.Blue;
			pWindowLayerItem->pBuffer[(i * WindowWidth + j) * 4 + 1] = WhiteColor.Green;
			pWindowLayerItem->pBuffer[(i * WindowWidth + j) * 4 + 2] = WhiteColor.Red;
		}
	}
	
	//左边第三、四像素列
	for (UINT16 i = 2; i <= 3; i++)
	{
		for (UINT16 j = 0; j < WindowHeight; j++)
		{
			pWindowLayerItem->pBuffer[(WindowHeight * WindowWidth + i) * 4 + 0] = WhiteColor.Blue;
			pWindowLayerItem->pBuffer[(WindowHeight * WindowWidth + i) * 4 + 1] = WhiteColor.Blue;
			pWindowLayerItem->pBuffer[(WindowHeight * WindowWidth + i) * 4 + 2] = WhiteColor.Blue;
		}
	}

	Position->StartX = 4;
	Position->StartY = 4;
	return  EFI_SUCCESS;
}


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

	for (i = 0; i < Height; i++)
	{
		for (j = 0; j < Width; j++)
		{
			pBuffer[(i * Width + j) * 4 + 0] = 0;
			pBuffer[(i * Width + j) * 4 + 1] = 0;
			pBuffer[(i * Width + j) * 4 + 2] = 0;
		}
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

	

	//返回窗口已占用像素；
    return EFI_SUCCESS;
}


VOID L3_APPLICATION_TitleBarCreate(WINDOW_LAYER_ITEM *pWindowLayerItem, UINT16 *TitleName, WINDOW_CURRENT_POSITION *Position)
{
	//蓝色背景，标题“我的电脑”，颜色白色，右边是最小化，最大化，关闭窗口    
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
	
    UINT16 StartX;
	UINT16 StartY;
	UINT16 Width;
	UINT16 Height;
	UINT16 LayerID;
	UINT16 TitleBarHeight = 30;
	UINT16 FontSize = 12;
    UINT16 AreaCode;
	UINT16 BitCode;
    
    Color.Blue  = WhiteColor.Blue;
    Color.Red   = WhiteColor.Blue;
    Color.Green = WhiteColor.Blue;
    Color.Reserved = GRAPHICS_LAYER_MY_COMPUTER_WINDOW;
		
    StartY = pWindowLayerItem->StartY;
    StartX = pWindowLayerItem->StartX;
	Width   = pWindowLayerItem->WindowWidth;
	Height = pWindowLayerItem->WindowHeight;
	LayerID = pWindowLayerItem->LayerID;
	
	StartX = Position->StartX;
	StartY = Position->StartY;

	//Blue: RGB:9，70，168

	//这里减4是因为窗口最外边4个像素被边框占用
	for (UINT16 i = StartY; i <  StartY + TitleBarHeight; i++)
	{
		for (UINT16 j = StartX + 4; j < StartX + Width - 4; j++)
		{
			pWindowLayerItem->pBuffer[(i * Width + j) * 4 + 0] = 168;
			pWindowLayerItem->pBuffer[(i * Width + j) * 4 + 1] = 70;
			pWindowLayerItem->pBuffer[(i * Width + j) * 4 + 2] = 9;
		}
	}
	
	StartY = StartY + (TitleBarHeight - FontSize) / 2;
	
    for (UINT16 i = 0; i < 4; i++)
    {
        AreaCode = TitleName[2 * i];
        BitCode  = TitleName[2 * i + 1];
        
        if (0 != AreaCode && 0 != BitCode)
            L2_GRAPHICS_ChineseCharDraw12(pWindowLayerItem->pBuffer, StartX, StartY, AreaCode, BitCode, Color, MyComputerWidth);
            
        StartX += FontSize + 1; //因为字体宽度是12，所以加13，多一点像素
    }
	
	//灰色背景，文件、编辑、查看、转到、收藏、帮助
	int word[] = {L'文', L'件', L'编', L'辑', L'查', L'看', L'转', L'到', L'收', L'藏', L'帮', L'助'};
	    
	UINT16 Code[12] = {0};
	L1_LIBRARY_QueryAreaCodeBitCodeByChineseChar(word, Code);//这里一定要有个数组取数据的操作
	for (UINT16 i = 0; i < sizeof(word)/sizeof(int) - 1; i++)
	{
		AreaCode = (Code[i] & 0xff00) >> 8;
		BitCode  = Code[i] & 0xff;
	
		if (0 <= AreaCode && AreaCode <= 94 && 0 <= BitCode && 94 >= BitCode)
			L2_GRAPHICS_ChineseCharDraw12(pWindowLayerItem->pBuffer, StartX, StartY, AreaCode, BitCode, Color, MyComputerWidth);
		StartX += FontSize + 1;	
	}
	
	Position->StartY += TitleBarHeight;
}


VOID L3_APPLICATION_MenuBarCreate(WINDOW_LAYER_ITEM *pWindowLayerItem, UINT16 *TitleName, WINDOW_CURRENT_POSITION *Position)
{
	//灰色背景，文件、编辑、查看、转到、收藏、帮助
	int word[] = {L'文'};
	UINT16 Code[12] = {0};
	L1_LIBRARY_QueryAreaCodeBitCodeByChineseChar(word, Code);
	
    UINT16 AreaCode;
	UINT16 BitCode;
	
	AreaCode = (Code[0] & 0xff00) >> 8;
	BitCode  = Code[0] & 0xff;
	
	if (0 != AreaCode && 0 != BitCode)
		L2_GRAPHICS_ChineseCharDraw12(pWindowLayerItem->pBuffer, Position->StartX, Position->StartY, AreaCode, BitCode, BlackColor, MyComputerWidth);
		
}


VOID L3_APPLICATION_ToolBarCreate(UINT16 StartX, UINT16 StartY, UINT16 Model)
{
	//后退、进进、向上、剪切、复制、粘贴、撤消、删除属性、查看
}


VOID L3_APPLICATION_AddressBarCreate(UINT16 StartX, UINT16 StartY, UINT16 Model)
{
	//地址：空白区域，可点击编辑
}


VOID L3_APPLICATION_WorkSpaceCreate(UINT16 StartX, UINT16 StartY, UINT16 Model)
{
	//工作空间，白色背景，里边放读取的文件夹、文件项
}

VOID L3_APPLICATION_StateBarCreate(UINT16 StartX, UINT16 StartY, UINT16 Model)
{
	//项目数量、选中项目数量、选中项目大小
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
	WINDOW_CURRENT_POSITION WindowCurrentPosition;
    
    Color.Blue  = 0xff;
    Color.Red   = 0xff;
    Color.Green = 0xff;
    Color.Reserved = GRAPHICS_LAYER_MY_COMPUTER_WINDOW;
    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MyComputerWidth: %d \n", __LINE__, MyComputerWidth);
	
    int x, y;
    x = 3;
    y = 6;

	UINT16 LayerID = GRAPHICS_LAYER_MY_COMPUTER_WINDOW;
    
    L3_WINDOW_Initial(&WindowLayers.item[LayerID], &WindowCurrentPosition);
	
	UINT16 TitleChineseName[] = {46,50,21,36,21,71,36,52}; //我的电脑
    
	//标题栏 TitleBar, 20 高度，宽度比窗口小些
	L3_APPLICATION_TitleBarCreate(&WindowLayers.item[LayerID], TitleChineseName, &WindowCurrentPosition);
	
	//菜单栏, //灰色背景，文件、编辑、查看、转到、收藏、帮助
	//UINT16 MenusChineseName[] = "文件";
	L3_APPLICATION_MenuBarCreate(&WindowLayers.item[LayerID], TitleChineseName, &WindowCurrentPosition);

	//工具栏
	//L3_APPLICATION_ToolBarCreate();
	
	//工作区
	//L3_APPLICATION_WorkSpaceCreate();	

	//状态栏
	//L3_APPLICATION_StateBarCreate();

	//窗口边框

    UINT8 *pBuffer = pMyComputerBuffer;

    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartX = StartX;
    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartY = StartY;
    
    
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

		//原来这里是X坐标0，显示效果比较靠窗口左边，现在改为10
		L2_DEBUG_Print3(10, y, WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW], "%a", device[i].PartitionName);

        L2_DEBUG_Print3(x + 10, y, WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW], "%a", type);
        
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
*  描述:   创建我的电脑窗口
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID L3_APPLICATION_MyComputerWindowNew(UINT16 StartX, UINT16 StartY)
{
    UINT8 *pParent;
    UINT16 Type;
    CHAR8 *pWindowTitle;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    
    Color.Blue  = 0xff;
    Color.Red   = 0xff;
    Color.Green = 0xff;
    Color.Reserved = GRAPHICS_LAYER_MY_COMPUTER_NEW_WINDOW;
    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MyComputerWidth: %d \n", __LINE__, MyComputerWidth);
	
    
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
    //L3_APPLICATION_MyComputerWindowUpdate(50, 50);
	
    L3_APPLICATION_MyComputerWindow(0, 50);
        
    L3_APPLICATION_SystemLogWindow(300, 10);
    
    L3_APPLICATION_MemoryInformationWindow(600, 100);
	
}

