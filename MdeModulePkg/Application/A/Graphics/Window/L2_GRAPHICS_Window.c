
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


#include <Protocol/GraphicsOutput.h>
#include <Library/BaseLib.h>
#include <Global/Global.h>
#include <Libraries/Languages/L1_LIBRARY_Language.h>

#include <L1_GRAPHICS.h>

#include <L2_GRAPHICS_Window.h>


UINT8 *pSystemLogWindowBuffer = NULL; // MyComputer layer: 2
UINT8 *pSystemSettingWindowBuffer = NULL;



UINT16 MyComputerWidth = 16 * 50;
UINT16 MyComputerHeight = 16 * 60;

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
	UINT16 CurrentWidth;
	UINT16 CurrentHeight;
	UINT8 *pBuffer;
	UINT16 LayerID;
	UINT16 Step;
	UINT16 CurrentX; //Current Bar StartX, a window may be have many Bars.
	UINT16 CurrentY; //Current Bar StartY, a window may be have many Bars.
	UINT16 CurrentBarHeight;
}WINDOW_CURRENT_POSITION;


MY_COMPUTER_WINDOW MyComputerWindow;


WINDOW_LAYERS WindowLayers = {0};

/****************************************************************************
*
*  描述: 将汉字字符串显示到指定内存Buffer
*
*  参数 pBuffer： 需要写入的目标内存块
*  参数 ChineseChar： 汉字字符串
	声明汉字字符串的时候可以采用如下方式声明
	int ChineseChars[5] = {0};
		
	ChineseChars[0] = L'我';
	ChineseChars[1] = L'的';
	ChineseChars[2] = L'电';
	ChineseChars[3] = L'脑';
	
*  参数 FontWidth： 字体宽度，当前我们字体大小是12 *12像素
*       StartX：写入的起始X轴坐标
*       StartY：写入的起始Y轴坐标
		Color:写入的颜色
		AreaWidth：目标图层内存块，图层宽度
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L3_WINDOW_ChineseCharsDraw(UINT8 *pBuffer, INT16 *ChineseChar, UINT16 FontWidth, UINTN *StartX, UINTN StartY, EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color , UINT16 AreaWidth)
{		
	//int word3[] = {L'脑'};
	GBK_Code Code = {0};
	
	for (int i = 0; i < sizeof(ChineseChar) / sizeof(INT16); i++)
	{
		//这里的2 * i是因为汉字的占用两个字节，需要取第一个字节
		L1_LIBRARY_QueryAreaCodeBitCodeByChineseChar(ChineseChar[2 * i], &Code); 	
		L2_GRAPHICS_ChineseCharDraw12(pBuffer, *StartX, StartY, Code.AreaCode, Code.BitCode, Color, AreaWidth);
		*StartX += FontWidth + 1;
	}
}

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
	UINT16 Step = pWindowLayerItem->Step;
	
	//g.setColor(new Color(191,191,191));
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
	
	UINT8 *pBuffer= pWindowLayerItem->pBuffer;
	
	for (UINT16 height = 0; height < pWindowLayerItem->WindowHeight; height++)
	{
		for (UINT16 width = 0; width < pWindowLayerItem->WindowWidth; width++)	
		{
			pBuffer[(height * pWindowLayerItem->WindowWidth + width) * 4 + 0] = 188;
			pBuffer[(height * pWindowLayerItem->WindowWidth + width) * 4 + 1] = 188;
			pBuffer[(height * pWindowLayerItem->WindowWidth + width) * 4 + 2] = 188;
			pBuffer[(height * pWindowLayerItem->WindowWidth + width) * 4 + 3] = pWindowLayerItem->LayerID;
		}
	}
	
	//�����һ������
	Color.Red = 100;
	Color.Green = 100;
	Color.Blue = 100;
	Color.Reserved = pWindowLayerItem->LayerID;
	L1_MEMORY_RectangleFillInrease(pBuffer, 0, 0, Step, pWindowLayerItem->WindowHeight - 1, pWindowLayerItem->WindowWidth, Color); //left
	L1_MEMORY_RectangleFillInrease(pBuffer, 0, 0, pWindowLayerItem->WindowWidth - 1, Step, pWindowLayerItem->WindowWidth, Color); //top
	
	//Black
	Color.Red = BlackColor.Red;
	Color.Green = BlackColor.Green;
	Color.Blue = BlackColor.Blue;
	L1_MEMORY_RectangleFillInrease(pBuffer, pWindowLayerItem->WindowWidth - Step - 1, 0, Step, pWindowLayerItem->WindowHeight - 1, pWindowLayerItem->WindowWidth, Color); //right
	L1_MEMORY_RectangleFillInrease(pBuffer, 0, pWindowLayerItem->WindowHeight - Step - 1, pWindowLayerItem->WindowWidth - 1,  Step, pWindowLayerItem->WindowWidth, Color); //down
		
	//����ڶ�������
	//white
	Color.Red = WhiteColor.Red;
	Color.Green = WhiteColor.Green;
	Color.Blue = WhiteColor.Blue;
	L1_MEMORY_RectangleFillInrease(pWindowLayerItem->pBuffer, Step, Step, Step, pWindowLayerItem->WindowHeight - 1 - 2 * Step, pWindowLayerItem->WindowWidth, Color); //left
	L1_MEMORY_RectangleFillInrease(pWindowLayerItem->pBuffer, Step, Step, pWindowLayerItem->WindowWidth - 1 - 2 * Step,  Step, pWindowLayerItem->WindowWidth, Color); //Top
	
	//g.setColor(new Color(128,128,128));
	Color.Red = 128;
	Color.Green = 128;
	Color.Blue = 128;
	L1_MEMORY_RectangleFillInrease(pWindowLayerItem->pBuffer, Step, pWindowLayerItem->WindowHeight - 2 * Step - 1, pWindowLayerItem->WindowWidth - 2 * Step - 1, Step, pWindowLayerItem->WindowWidth, Color); //Down
	L1_MEMORY_RectangleFillInrease(pWindowLayerItem->pBuffer, pWindowLayerItem->WindowWidth - 2 * Step - 1, Step, Step, pWindowLayerItem->WindowHeight - 2 * Step - 1, pWindowLayerItem->WindowWidth, Color); //Right
		
	//�������������
	//Step = windowParameter.DefaultStep * 2;
	//g.setColor(new Color(191,191,191));
	Color.Red = 186;
	Color.Green = 188;
	Color.Blue = 189;

	for (UINT16 i = 0; i < Step * 2; i++)
	{
		L2_GRAPHICS_RectangleDraw(pWindowLayerItem->pBuffer, 2 * Step + i, 2 * Step + i, pWindowLayerItem->WindowWidth - 2 * Step - 1 - i, pWindowLayerItem->WindowHeight - 2 * Step - 1 - i, 0, Color, pWindowLayerItem->WindowWidth);
	}	

	//��߿�ռ�õ�����
	Position->CurrentX += 4 * Step;
	Position->CurrentY += 4 * Step;

	Position->CurrentHeight -= 2 * 4 * Step;
	Position->CurrentWidth -=  4 * Step;

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

VOID L3_APPLICATION_TitleBarCreate2(WINDOW_LAYER_ITEM *pWindowLayerItem, UINT16 *TitleName, WINDOW_CURRENT_POSITION *Position)
{
}


VOID L3_APPLICATION_TitleBarCreate(WINDOW_LAYER_ITEM *pWindowLayerItem, UINT16 *TitleName, WINDOW_CURRENT_POSITION *Position)
{
	//蓝色背景，标题“我的电脑”，颜色白色，右边是最小化，最大化，关闭窗口    
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
	
    UINT16 CurrentX;
	UINT16 CurrentY;
    UINT16 CurrentWidth;
	UINT16 CurrentHeight;
	UINT16 WindowWidth; //���ڱ�����С�����ƴ���ʱ�����޸Ĵ�С
	UINT16 Height;
	UINT16 LayerID;
	UINT16 TitleBarHeight;
	UINT16 FontSize = 12;
    UINT16 AreaCode;
	UINT16 BitCode;
	UINT16 Step;

	TitleBarHeight = pWindowLayerItem->TitleBarHeight;
    
    Color.Blue  = WhiteColor.Blue;
    Color.Red   = WhiteColor.Blue;
    Color.Green = WhiteColor.Blue;
    Color.Reserved = pWindowLayerItem->LayerID;
		
    CurrentX = Position->CurrentX;
    CurrentY = Position->CurrentY;
	CurrentWidth = Position->CurrentWidth;
	CurrentHeight = Position->CurrentHeight;
	WindowWidth = pWindowLayerItem->WindowWidth;
	Height = pWindowLayerItem->WindowHeight;
	LayerID = pWindowLayerItem->LayerID;
	Step = pWindowLayerItem->Step;
	
	//Blue: RGB:9，70，168

	//这里减4是因为窗口最外边4 * Step个像素被边框占用
	for (UINT16 i = CurrentY; i <  CurrentY + TitleBarHeight; i++)
	{
		for (UINT16 j = CurrentX; j < CurrentWidth; j++)
		{
			
			pWindowLayerItem->pBuffer[(i * WindowWidth + j) * 4 + 0] = 168; //Blue
			pWindowLayerItem->pBuffer[(i * WindowWidth + j) * 4 + 1] = 70; //Green
			pWindowLayerItem->pBuffer[(i * WindowWidth + j) * 4 + 2] = 9; //Red
		}
	}
	
	//Step * 4是窗口边框所占像素
	CurrentY += (TitleBarHeight - FontSize - 4 * Step) / 2; 

	//��Ϊ��ʾ�ҵĵ��ԣ�������Ҫ�����ƶ�Щ
	UINT16 TempX = Position->CurrentX + 4 * Step;

	//灰色背景，文件、编辑、查看、转到、收藏、帮助
	//这里不能写成{L'我',L'我',L'我',L'我'}，因为EDK2代码里边编译选项不让一次性定义多个，如果定义，需要修改编译选项，修改编译选项后，原来EDK2代码会编译不通过，有点蛋疼，
	
	int ChineseChars[] = {L'我', L'的', L'电', L'脑'};
	L3_WINDOW_ChineseCharsDraw(pWindowLayerItem->pBuffer, ChineseChars, FontSize, &TempX, CurrentY, Color, WindowWidth);

	//������ռ�õĸ߶�
	Position->CurrentY += WindowLayers.item[LayerID].TitleBarHeight;

	Color.Red = 189;
	Color.Green = 195;
	Color.Blue = 195;
	L1_MEMORY_RectangleFillInrease(pWindowLayerItem->pBuffer, Position->CurrentX, Position->CurrentY, CurrentWidth - 4 * Step, Step, WindowWidth, Color); //a line
							
	//�������±߻���һ����
	Position->CurrentY += Step;

	Position->CurrentHeight-= TitleBarHeight;

	Position->CurrentHeight-= Step;

	Position->CurrentWidth -= Step;
	
}


/****************************************************************************
*
*  ����: �������ڵ�XX�������繤�������˵����ȵ�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*
*****************************************************************************/
VOID L3_APPLICATION_WindowBarCreate(WINDOW_LAYER_ITEM *pWindowLayerItem, UINT16 *TitleName, WINDOW_CURRENT_POSITION *Position)
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
	UINT16 FontSize;
	
	UINT16 CurrentX = Position->CurrentX;
	UINT16 CurrentY = Position->CurrentY;
	UINT16 TempWidth = Position->CurrentWidth;
	UINT16 Step = 2;
	UINT16 WindowWidth = pWindowLayerItem->WindowWidth;
	Position->CurrentBarHeight = pWindowLayerItem->TitleBarHeight;

	FontSize = 12;
	Color.Red   = 192;
	Color.Green = 192;
	Color.Blue  = 192;
	Color.Reserved = pWindowLayerItem->LayerID;
	
	//�����4����Ϊ���������4 * Step�����ر��߿�ռ��
	for (UINT16 i = CurrentY; i <  CurrentY + Position->CurrentBarHeight; i++)
	{
		for (UINT16 j = CurrentX; j < TempWidth; j++)
		{
			pWindowLayerItem->pBuffer[(i * WindowWidth + j) * 4 + 0] = 180;
			pWindowLayerItem->pBuffer[(i * WindowWidth + j) * 4 + 1] = 180;
			pWindowLayerItem->pBuffer[(i * WindowWidth + j) * 4 + 2] = 180;
		}
	}
	
	TempWidth -= 12 * Step;
	
	Color.Red = WhiteColor.Red;
	Color.Green =  WhiteColor.Green;
	Color.Blue =  WhiteColor.Blue;	
	L1_MEMORY_RectangleFillInrease(pWindowLayerItem->pBuffer, CurrentX, CurrentY, Step, Position->CurrentBarHeight, WindowWidth, Color); //left
	L1_MEMORY_RectangleFillInrease(pWindowLayerItem->pBuffer, CurrentX, CurrentY, TempWidth, Step, WindowWidth, Color); //Top

	CurrentX += Step;
	CurrentY += Step;	
	
	Color.Red = 100;
	Color.Green =  100;
	Color.Blue =  100;	
	L1_MEMORY_RectangleFillInrease(pWindowLayerItem->pBuffer, CurrentX, CurrentY + Position->CurrentBarHeight - Step, TempWidth, Step, WindowWidth, Color); //down
	L1_MEMORY_RectangleFillInrease(pWindowLayerItem->pBuffer, TempWidth + 14 * Step, CurrentY, Step, Position->CurrentBarHeight, WindowWidth, Color); //right

	Position->CurrentY += pWindowLayerItem->TitleBarHeight + Step;
	
}

VOID L3_APPLICATION_MenuBarCreate(WINDOW_LAYER_ITEM *pWindowLayerItem, UINT16 *TitleName, WINDOW_CURRENT_POSITION *Position)
{
	UINT16 Step = 2;
	UINT16 CurrentX = Position->CurrentX;
	UINT16 CurrentY = Position->CurrentY;
	UINT16 CurrentWidth = Position->CurrentWidth;
	UINT16 CurrentHeight = Position->CurrentHeight;
	UINT16 WindowWidth = pWindowLayerItem->WindowWidth;
	UINT16 FontSize = 12;
	UINT16 TempX = CurrentX + FontSize;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
	
	Color.Red = BlackColor.Red;
	Color.Red = BlackColor.Red;
	Color.Red = BlackColor.Red;
	
	L3_APPLICATION_WindowBarCreate(pWindowLayerItem, NULL, Position);

	//int ChineseChars[5] = {L'文件、编辑、查看、转到、收藏、帮助', L'的', L'电', L'脑'};
	int ChineseChars[6][2] = 
	{
		{L'文', L'件'},
		{L'编', L'辑'},
		{L'查', L'看'},
		{L'转', L'到'},
		{L'收', L'藏'},
		{L'帮', L'助'}
	};

	UINT16 TempY = (Position->CurrentBarHeight - FontSize) / 2;
	
	L3_WINDOW_ChineseCharsDraw(pWindowLayerItem->pBuffer, ChineseChars[0], FontSize, &TempX, CurrentY + TempY, Color, WindowWidth);

	TempX += 2 * FontSize;
	L3_WINDOW_ChineseCharsDraw(pWindowLayerItem->pBuffer, ChineseChars[1], FontSize, &TempX, CurrentY + TempY, Color, WindowWidth);
	
	TempX += 2 * FontSize;
	L3_WINDOW_ChineseCharsDraw(pWindowLayerItem->pBuffer, ChineseChars[2], FontSize, &TempX, CurrentY + TempY, Color, WindowWidth);
		
	TempX += 2 * FontSize;
	L3_WINDOW_ChineseCharsDraw(pWindowLayerItem->pBuffer, ChineseChars[3], FontSize, &TempX, CurrentY + TempY, Color, WindowWidth);
		
	TempX += 2 * FontSize;
	L3_WINDOW_ChineseCharsDraw(pWindowLayerItem->pBuffer, ChineseChars[4], FontSize, &TempX, CurrentY + TempY, Color, WindowWidth);
		
	TempX += 2 * FontSize;
	L3_WINDOW_ChineseCharsDraw(pWindowLayerItem->pBuffer, ChineseChars[5], FontSize, &TempX, CurrentY + TempY, Color, WindowWidth);
	Position->CurrentY += Step;
}


VOID L3_APPLICATION_ToolBarCreate(WINDOW_LAYER_ITEM *pWindowLayerItem, UINT16 *TitleName, WINDOW_CURRENT_POSITION *Position)
{
	UINT16 Step = 2;
	
	L3_APPLICATION_WindowBarCreate(pWindowLayerItem, NULL, Position);

	Position->CurrentY += Step;
}


VOID L3_APPLICATION_AddressBarCreate(WINDOW_LAYER_ITEM *pWindowLayerItem, UINT16 *TitleName, WINDOW_CURRENT_POSITION *Position)
{
	//地址：空白区域，可点击编辑
	
	UINT16 Step = 2;
	
	L3_APPLICATION_WindowBarCreate(pWindowLayerItem, NULL, Position);

	
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
	UINT16 AddressBarHeight;
	UINT16 FontSize;

	Position->CurrentY += Step;

	AddressBarHeight = 20;
	FontSize = 12;
	Color.Red   = WhiteColor.Red;
	Color.Green = WhiteColor.Green;
	Color.Blue  = WhiteColor.Blue;
	Color.Reserved = GRAPHICS_LAYER_MY_COMPUTER_WINDOW;
	
	//L2_GRAPHICS_ButtonDraw2(pWindowLayerItem, Position->StartX + 5 * FontSize, Position->StartY, pWindowLayerItem->WindowWidth - 6, Position->StartY + FontSize * 2);
	
	//L1_MEMORY_RectangleFill(pWindowLayerItem->pBuffer, Position->StartX + 5 * FontSize, Position->StartY, pWindowLayerItem->WindowWidth - 6, Position->StartY + FontSize, pWindowLayerItem->WindowWidth, Color); // line top
	
	//灰色背景，文件、编辑、查看、转到、收藏、帮助
	int word[] = {L'文'};
	GBK_Code Code = {0};
	L1_LIBRARY_QueryAreaCodeBitCodeByChineseChar(word[0], &Code);

	//L2_GRAPHICS_ChineseCharDraw12(pWindowLayerItem->pBuffer, Position->StartX + 4, Position->StartY, Code.AreaCode, Code.BitCode, WhiteColor, MyComputerWidth);

	//Position->StartY += AddressBarHeight + 4;
}


VOID L3_APPLICATION_WorkSpaceCreate(WINDOW_LAYER_ITEM *pWindowLayerItem, UINT16 *TitleName, WINDOW_CURRENT_POSITION *Position)
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
	UINT16 FontSize;
	
	UINT16 CurrentX = Position->CurrentX;
	UINT16 CurrentY = Position->CurrentY;
	UINT16 TempWidth = Position->CurrentWidth;
	UINT16 Step = 2;
	UINT16 WindowWidth = pWindowLayerItem->WindowWidth;
	Position->CurrentBarHeight = pWindowLayerItem->TitleBarHeight;
	UINT8 *pBuffer = pWindowLayerItem->pBuffer;

	FontSize = 12;
	Color.Red   = 192;
	Color.Green = 192;
	Color.Blue  = 192;
	Color.Reserved = pWindowLayerItem->LayerID;

	//工作空间，白色背景，里边放读取的文件夹、文件项	
	for (UINT16 height = CurrentY + 2 * Step; height < Position->CurrentHeight - 2 * Step; height++)
	{
		for (UINT16 width = CurrentX + 2 * Step; width < Position->CurrentWidth - 2 * Step; width++)	
		{
			pBuffer[(height * WindowWidth + width) * 4 + 0] = WhiteColor.Red;
			pBuffer[(height * WindowWidth + width) * 4 + 1] = WhiteColor.Green;
			pBuffer[(height * WindowWidth + width) * 4 + 2] = WhiteColor.Blue;
			pBuffer[(height * WindowWidth + width) * 4 + 3] = pWindowLayerItem->LayerID;
		}
	}
	
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
	UINT16 Step = WindowLayers.item[LayerID].Step;
	
	WindowLayers.item[LayerID].TitleBarHeight = 30;

	UINT16 TitleChineseName[] = {46,50,21,36,21,71,36,52}; //我的电脑
    	
	WindowCurrentPosition.LayerID = WindowLayers.item[LayerID].LayerID;
	WindowCurrentPosition.pBuffer = WindowLayers.item[LayerID].pBuffer;
	
	WindowCurrentPosition.CurrentX = 0;
	WindowCurrentPosition.CurrentY = 0;
	WindowCurrentPosition.CurrentWidth = WindowLayers.item[LayerID].WindowWidth; //��߿�+�ұ߿��4��
	WindowCurrentPosition.CurrentHeight = WindowLayers.item[LayerID].WindowHeight; //�ϱ߿�+�±߿��4��


	
    //����������ʼ��
    L3_WINDOW_Initial(&WindowLayers.item[LayerID], &WindowCurrentPosition);



	//��������ʼ��
	//标题栏 TitleBar, 20 高度，宽度比窗口小些
	L3_APPLICATION_TitleBarCreate(&WindowLayers.item[LayerID], TitleChineseName, &WindowCurrentPosition);
	


	
	//������Ҫ��һ�������ڱ߿�
	//gray
	Color.Red = 50;
	Color.Green = 50;
	Color.Blue = 50;
	L1_MEMORY_RectangleFillInrease(WindowLayers.item[LayerID].pBuffer, WindowCurrentPosition.CurrentX, WindowCurrentPosition.CurrentY, Step, WindowCurrentPosition.CurrentHeight, WindowLayers.item[LayerID].WindowWidth, Color); //left
	L1_MEMORY_RectangleFillInrease(WindowLayers.item[LayerID].pBuffer, WindowCurrentPosition.CurrentX, WindowCurrentPosition.CurrentY, WindowCurrentPosition.CurrentWidth - 4 * Step,  Step, WindowLayers.item[LayerID].WindowWidth, Color); //Top
	
	//white
	Color.Red = WhiteColor.Red;
	Color.Green = WhiteColor.Green;
	Color.Blue = WhiteColor.Blue;
	L1_MEMORY_RectangleFillInrease(WindowLayers.item[LayerID].pBuffer, WindowCurrentPosition.CurrentX, WindowCurrentPosition.CurrentHeight - 1, WindowCurrentPosition.CurrentWidth, Step, WindowLayers.item[LayerID].WindowWidth, Color); //Down
	L1_MEMORY_RectangleFillInrease(WindowLayers.item[LayerID].pBuffer, WindowCurrentPosition.CurrentWidth + Step, WindowCurrentPosition.CurrentY, Step, WindowCurrentPosition.CurrentHeight - 1, WindowLayers.item[LayerID].WindowWidth, Color); //Right
				
	WindowCurrentPosition.CurrentX += 2 * Step;
	WindowCurrentPosition.CurrentY += 2 * Step;
	//WindowCurrentPosition.CurrentWidth -= Step;

	L3_APPLICATION_MenuBarCreate(&WindowLayers.item[LayerID], TitleChineseName, &WindowCurrentPosition);

	L3_APPLICATION_ToolBarCreate(&WindowLayers.item[LayerID], TitleChineseName, &WindowCurrentPosition);

	L3_APPLICATION_AddressBarCreate(&WindowLayers.item[LayerID], TitleChineseName, &WindowCurrentPosition);
	
	L3_APPLICATION_WorkSpaceCreate(&WindowLayers.item[LayerID], TitleChineseName, &WindowCurrentPosition);
	
	return;
	
	for (UINT16 i = 0; i < 4; i++)
	{
		L3_APPLICATION_WindowBarCreate(&WindowLayers.item[LayerID], TitleChineseName, &WindowCurrentPosition);

		WindowCurrentPosition.CurrentY += Step;
	}


	//工具栏
	L3_APPLICATION_ToolBarCreate(&WindowLayers.item[LayerID], TitleChineseName, &WindowCurrentPosition);


	L3_APPLICATION_AddressBarCreate(&WindowLayers.item[LayerID], TitleChineseName, &WindowCurrentPosition);


	
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
        x = 500;
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
*  描述:   绘制按钮
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L2_GRAPHICS_ButtonDraw2(WINDOW_LAYER_ITEM *pWindowLayerItem, UINT16 StartX, UINT16 StartY, UINT16 Width, UINT16 Height)
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    
    // Button
    // white
    Color.Red   = 0xFF;
    Color.Green = 0xFF;
    Color.Blue  = 0xFF;
    Color.Reserved  = pWindowLayerItem->LayerID;
    L1_MEMORY_RectangleFill(pWindowLayerItem->pBuffer, StartX,  StartY, StartX + Width, StartY, pWindowLayerItem->WindowWidth, Color); //line top 
    L1_MEMORY_RectangleFill(pWindowLayerItem->pBuffer, StartX,  StartY, StartX, StartY + Height,  pWindowLayerItem->WindowWidth, Color); //line left

    Color.Red   = 214;
    Color.Green = 211;
    Color.Blue  = 206;
    L1_MEMORY_RectangleFill(pWindowLayerItem->pBuffer, StartX + 1, StartY + 1, StartX + Width, StartY + Height, pWindowLayerItem->WindowWidth, Color); // Area
    
    // Black
    Color.Red   = 0x00;
    Color.Green = 0x00;
    Color.Blue  = 0x00;
    L1_MEMORY_RectangleFill(pWindowLayerItem->pBuffer, StartX,  StartY + Height + 1, StartX + Width, StartY + Height + 2, pWindowLayerItem->WindowWidth, Color); // line button
    L1_MEMORY_RectangleFill(pWindowLayerItem->pBuffer, StartX + Width + 1, StartY + 1 , StartX + Width + 2, StartY + Height + 1, pWindowLayerItem->WindowWidth, Color); // line right
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

