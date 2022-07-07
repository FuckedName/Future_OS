
/*************************************************
    .
    File name:      	*.*
    Author��	        	������
    ID��					00001
    Date:          		202107
    Description:    	
    Others:         	��

    History:        	��
	    1.  Date:
		    Author: 
		    ID:
		    Modification:
		    
	    2.  Date:
		    Author: 
		    ID:
		    Modification:
*************************************************/





#include <Library/UefiRuntimeLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <L2_GRAPHICS.h>
#include <string.h>
#include <Libraries/Math/L1_LIBRARY_Math.h>
#include <Libraries/Network/L1_LIBRARY_Network.h>
#include <Libraries/Memory/L1_LIBRARY_Memory.h>

#include <Devices/Screen/L2_DEVICE_Screen.h>

#include <Devices/Store/L2_DEVICE_Store.h>
#include <Global/Global.h>

#include <Libraries/String/L2_LIBRARY_String.h>
#include <Partitions/FAT32/L2_PARTITION_FAT32.h>
#include <Partitions/NTFS/L2_PARTITION_NTFS.h>
#include <Partitions/L2_PARTITION.h>

#include <Devices/Mouse/L1_DEVICE_Mouse.h>

UINTN ScreenWidth, ScreenHeight;

UINT16 LogStatusErrorCount = 0;

UINT8 *sChineseChar = NULL; // Default chinese char size is 16 * 16 pixel
UINT8 *sChineseChar12 = NULL; // For chinese char size is 12 * 12 pixel

UINT8 *pDeskBuffer = NULL; //only Desk layer include wallpaper and button : 1
UINT8 *pDeskDisplayBuffer = NULL; //desk display after multi graphicses layers compute
UINT8 *pDeskWallpaperBuffer = NULL;
UINT8 *pDeskWallpaperZoomedBuffer = NULL;
UINT8 *pMemoryInformationBuffer = NULL; // MyComputer layer: 2
UINT8 *pTerminalWindowBuffer = NULL; // MyComputer layer: 2
UINT8 *pMouseBuffer = NULL; //Mouse layer: 4
UINT8 *pMouseRightButtonClickWindowBuffer = NULL; // for mouse click 
UINT8 *pMouseSelectedBuffer = NULL;  // after mouse selected
UINT8 *pMyComputerBuffer = NULL; // MyComputer layer: 2

UINT8 *pReadFileDestBuffer = NULL;

UINT8 *pStartMenuBuffer = NULL;
UINT8 *pSystemIconBuffer[SYSTEM_ICON_MAX]; //desk display after multi graphicses layers compute
UINT8 *pSystemIconFolderBuffer = NULL; //after zoom in or zoom out
UINT8 *pSystemIconMySettingBuffer = NULL; //after zoom in or zoom out
UINT8 *pSystemIconRecycleBuffer = NULL; //after zoom in or zoom out
UINT8 *pSystemIconTempBuffer2 = NULL;
UINT8 *pSystemIconTextBuffer = NULL; //after zoom in or zoom out

UINT16 StartMenuWidth = 16 * 10;
UINT16 StartMenuHeight = 16 * 20;
UINT16 StatusErrorCount = 0;
UINT16 SystemLogWindowWidth = 16 * 48;
UINT16 SystemLogWindowHeight = 16 * 26;
UINT16 TerminalWindowWidth = 16 * 48;
UINT16 TerminalWindowHeight = 16 * 48;
UINT16 SystemSettingWindowWidth = 16 * 10;
UINT16 SystemSettingWindowHeight = 16 * 10;

UINT16 MemoryInformationWindowHeight = 16 * 60;
UINT16 MemoryInformationWindowWidth = 16 * 40;
UINT16 LayerID = 0;


EFI_GRAPHICS_OUTPUT_BLT_PIXEL WhiteColor;
EFI_GRAPHICS_OUTPUT_BLT_PIXEL BlackColor;


#define WINDOW_DEFAULT_WIDTH  (ScreenWidth / 2)
#define WINDOW_DEFAULT_HEIGHT (ScreenHeight / 2)


EFI_GRAPHICS_OUTPUT_BLT_PIXEL MouseMoveoverObjectDrawColor;


EFI_GRAPHICS_OUTPUT_BLT_PIXEL MouseColor;

INT16 iMouseRightClickX = 0;
INT16 iMouseRightClickY = 0;

UINT8 *pSystemLogBuffer = NULL; // Save log data

//Need to allocate buffer in this module.
#define SYSTEM_LOG_DATA_WIDTH 200
#define SYSTEM_LOG_DATA_LINE 40

#define MY_COMPUTER_PARTITION_CLICKED_FLAG -1

UINT16 date_time_count_increase_flag = 0;
UINT16 date_time_count = 0;


UINT8 BufferMFT[DISK_BUFFER_SIZE * 2];

UINT16 PartitionItemID = 0xffff; // invalid
UINT16 FolderItemID = 0xffff; // invalid

//ϵͳ�ػ�
INT8 SystemQuitFlag = FALSE;

// ��ǰ��Ϊ��Ŀ¼���ļ��ż�¼Ϊ��Ч�ļ�����;
UINT16 FolderItemValidIndexArray[10] = {0};

DESKTOP_ITEM_CLICKED_EVENT    DesktopItemClickEvent = DESKTOP_ITEM_INIT_CLICKED_EVENT;

//��ʼ�˵�״̬ת�������ڼ�¼�ϴ�״̬��״̬
START_MENU_STATE 	 StartMenuNextState = START_MENU_ITEM_INIT_EVENT;

//�ҵĵ���״̬ת�������ڼ�¼�ϴ�״̬��״̬
MY_COMPUTER_STATE 	 MyComputerNextState = MY_COMPUTER_INIT_STATE;

// FAT32 �ļ�����Ŀ¼
FAT32_ROOTPATH_SHORT_FILE_ITEM pItems[32];


NTFS_FILE_SWITCHED NTFSFileSwitched = {0};


//Line 0
#define DISPLAY_DESK_HEIGHT_WEIGHT_X (date_time_count % 30) 
#define DISPLAY_DESK_HEIGHT_WEIGHT_Y (ScreenHeight - 16 * 3)

#define FAT32_FILE_SYSTEM_ATTRIBUTE_DIRECTORY   0x10
#define FAT32_FILE_SYSTEM_ATTRIBUTE_FILE  		0x20


MOUSE_MOVEOVER_OBJECT MouseMoveoverObjectOld;



/****************************************************************************
*  ���Դ�����������ͼ���¼�������ڣ��ҳ���Ҫ
*  ����:   �������������ͼ�㣬�ҵ�ͼ�����¼��������ݵ���¼��ҵ���Ӧ����Ӧ��������
*  ��һ�У���ͬͼ����
*  �ڶ��У�����ͼ��ID��ȡ��ͬͼ��ĵ���¼�XXXXXClickEventGet����ͬ�����¼���ȡ��
*  �����У�����ͬ�¼�XXXXXClickEventHandle����ͬ�����¼�������ڣ��Ƚ���Ҫ��
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
CHINESE_FONT_DRAW_FUNCTION_GET ChineseDrawFunctionGet[] =
{
    {CHINESE_FONT_SIZE_0,   NULL},
    {CHINESE_FONT_SIZE_1,   NULL},
    {CHINESE_FONT_SIZE_2,   NULL},
    {CHINESE_FONT_SIZE_3,   NULL},
    {CHINESE_FONT_SIZE_4,   NULL},
    {CHINESE_FONT_SIZE_5,   NULL},
    {CHINESE_FONT_SIZE_6,   NULL},
    {CHINESE_FONT_SIZE_7,   NULL},
    {CHINESE_FONT_SIZE_8,   NULL},
    {CHINESE_FONT_SIZE_9,   NULL},
    {CHINESE_FONT_SIZE_10,   NULL},
    {CHINESE_FONT_SIZE_11,   NULL},
    {CHINESE_FONT_SIZE_12,   L2_GRAPHICS_ChineseCharDraw12},
    {CHINESE_FONT_SIZE_13,   NULL},
    {CHINESE_FONT_SIZE_14,   NULL},
    {CHINESE_FONT_SIZE_15,   NULL},
    {CHINESE_FONT_SIZE_16,   L2_GRAPHICS_ChineseCharDraw16}
};



const UINT8 sASCII[][16] =
{   
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},       //0x00
    {0x00,0x00,0x7E,0x81,0xA5,0x81,0x81,0xBD,0x99,0x81,0x81,0x7E,0x00,0x00,0x00,0x00},       //0x01
    {0x00,0x00,0x7E,0xFF,0xDB,0xFF,0xFF,0xC3,0xE7,0xFF,0xFF,0x7E,0x00,0x00,0x00,0x00},       //0x02
    {0x00,0x00,0x00,0x00,0x6C,0xFE,0xFE,0xFE,0xFE,0x7C,0x38,0x10,0x00,0x00,0x00,0x00},       //0x03
    {0x00,0x00,0x00,0x00,0x10,0x38,0x7C,0xFE,0x7C,0x38,0x10,0x00,0x00,0x00,0x00,0x00},       //0x04
    {0x00,0x00,0x00,0x18,0x3C,0x3C,0xE7,0xE7,0xE7,0x18,0x18,0x3C,0x00,0x00,0x00,0x00},       //0x05
    {0x00,0x00,0x00,0x18,0x3C,0x7E,0xFF,0xFF,0x7E,0x18,0x18,0x3C,0x00,0x00,0x00,0x00},       //0x06
    {0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x3C,0x3C,0x18,0x00,0x00,0x00,0x00,0x00,0x00},       //0x07
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xE7,0xC3,0xC3,0xE7,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},       //0x08
    {0x00,0x00,0x00,0x00,0x00,0x3C,0x66,0x42,0x42,0x66,0x3C,0x00,0x00,0x00,0x00,0x00},       //0x09
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xC3,0x99,0xBD,0xBD,0x99,0xC3,0xFF,0xFF,0xFF,0xFF,0xFF},       //0x0A
    {0x00,0x00,0x1E,0x0E,0x1A,0x32,0x78,0xCC,0xCC,0xCC,0xCC,0x78,0x00,0x00,0x00,0x00},       //0x0B
    {0x00,0x00,0x3C,0x66,0x66,0x66,0x66,0x3C,0x18,0x7E,0x18,0x18,0x00,0x00,0x00,0x00},       //0x0C
    {0x00,0x00,0x3F,0x33,0x3F,0x30,0x30,0x30,0x30,0x70,0xF0,0xE0,0x00,0x00,0x00,0x00},       //0x0D
    {0x00,0x00,0x7F,0x63,0x7F,0x63,0x63,0x63,0x63,0x67,0xE7,0xE6,0xC0,0x00,0x00,0x00},       //0x0E
    {0x00,0x00,0x00,0x18,0x18,0xDB,0x3C,0xE7,0x3C,0xDB,0x18,0x18,0x00,0x00,0x00,0x00},       //0x0F
    {0x00,0x80,0xC0,0xE0,0xF0,0xF8,0xFE,0xF8,0xF0,0xE0,0xC0,0x80,0x00,0x00,0x00,0x00},       //0x10
    {0x00,0x02,0x06,0x0E,0x1E,0x3E,0xFE,0x3E,0x1E,0x0E,0x06,0x02,0x00,0x00,0x00,0x00},       //0x11
    {0x00,0x00,0x18,0x3C,0x7E,0x18,0x18,0x18,0x7E,0x3C,0x18,0x00,0x00,0x00,0x00,0x00},       //0x12
    {0x00,0x00,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x00,0x66,0x66,0x00,0x00,0x00,0x00},       //0x13
    {0x00,0x00,0x7F,0xDB,0xDB,0xDB,0x7B,0x1B,0x1B,0x1B,0x1B,0x1B,0x00,0x00,0x00,0x00},       //0x14
    {0x00,0x7C,0xC6,0x60,0x38,0x6C,0xC6,0xC6,0x6C,0x38,0x0C,0xC6,0x7C,0x00,0x00,0x00},       //0x15
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0xFE,0xFE,0xFE,0x00,0x00,0x00,0x00},       //0x16
    {0x00,0x00,0x18,0x3C,0x7E,0x18,0x18,0x18,0x7E,0x3C,0x18,0x7E,0x00,0x00,0x00,0x00},       //0x17
    {0x00,0x00,0x18,0x3C,0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x00,0x00,0x00,0x00},       //0x18
    {0x00,0x00,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x7E,0x3C,0x18,0x00,0x00,0x00,0x00},       //0x19
    {0x00,0x00,0x00,0x00,0x00,0x18,0x0C,0xFE,0x0C,0x18,0x00,0x00,0x00,0x00,0x00,0x00},       //0x1A
    {0x00,0x00,0x00,0x00,0x00,0x30,0x60,0xFE,0x60,0x30,0x00,0x00,0x00,0x00,0x00,0x00},       //0x1B
    {0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xC0,0xC0,0xFE,0x00,0x00,0x00,0x00,0x00,0x00},       //0x1C
    {0x00,0x00,0x00,0x00,0x00,0x28,0x6C,0xFE,0x6C,0x28,0x00,0x00,0x00,0x00,0x00,0x00},       //0x1D
    {0x00,0x00,0x00,0x00,0x10,0x38,0x38,0x7C,0x7C,0xFE,0xFE,0x00,0x00,0x00,0x00,0x00},       //0x1E
    {0x00,0x00,0x00,0x00,0xFE,0xFE,0x7C,0x7C,0x38,0x38,0x10,0x00,0x00,0x00,0x00,0x00},       //0x1F
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},       //0x20' '
    {0x00,0x00,0x18,0x3C,0x3C,0x3C,0x18,0x18,0x18,0x00,0x18,0x18,0x00,0x00,0x00,0x00},       //0x21'!'
    {0x00,0x66,0x66,0x66,0x24,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},       //0x22'"'
    {0x00,0x00,0x00,0x6C,0x6C,0xFE,0x6C,0x6C,0x6C,0xFE,0x6C,0x6C,0x00,0x00,0x00,0x00},       //0x23'#'
    {0x18,0x18,0x7C,0xC6,0xC2,0xC0,0x7C,0x06,0x06,0x86,0xC6,0x7C,0x18,0x18,0x00,0x00},       //0x24'$'
    {0x00,0x00,0x00,0x00,0xC2,0xC6,0x0C,0x18,0x30,0x60,0xC6,0x86,0x00,0x00,0x00,0x00},       //0x25'%'
    {0x00,0x00,0x38,0x6C,0x6C,0x38,0x76,0xDC,0xCC,0xCC,0xCC,0x76,0x00,0x00,0x00,0x00},       //0x26'&'
    {0x00,0x30,0x30,0x30,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},       //0x27'''
    {0x00,0x00,0x0C,0x18,0x30,0x30,0x30,0x30,0x30,0x30,0x18,0x0C,0x00,0x00,0x00,0x00},       //0x28'('
    {0x00,0x00,0x30,0x18,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x18,0x30,0x00,0x00,0x00,0x00},       //0x29')'
    {0x00,0x00,0x00,0x00,0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00,0x00,0x00,0x00,0x00},       //0x2A'*'
    {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00,0x00,0x00,0x00,0x00},       //0x2B'+'
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x18,0x30,0x00,0x00,0x00},       //0x2C','
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},       //0x2D'-'
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00},       //0x2E'.'
    {0x00,0x00,0x00,0x00,0x02,0x06,0x0C,0x18,0x30,0x60,0xC0,0x80,0x00,0x00,0x00,0x00},       //0x2F'/'
    {0x00,0x00,0x38,0x6C,0xC6,0xC6,0xD6,0xD6,0xC6,0xC6,0x6C,0x38,0x00,0x00,0x00,0x00},       //0x30'0'
    {0x00,0x00,0x18,0x38,0x78,0x18,0x18,0x18,0x18,0x18,0x18,0x7E,0x00,0x00,0x00,0x00},       //0x31'1'
    {0x00,0x00,0x7C,0xC6,0x06,0x0C,0x18,0x30,0x60,0xC0,0xC6,0xFE,0x00,0x00,0x00,0x00},       //0x32'2'
    {0x00,0x00,0x7C,0xC6,0x06,0x06,0x3C,0x06,0x06,0x06,0xC6,0x7C,0x00,0x00,0x00,0x00},       //0x33'3'
    {0x00,0x00,0x0C,0x1C,0x3C,0x6C,0xCC,0xFE,0x0C,0x0C,0x0C,0x1E,0x00,0x00,0x00,0x00},       //0x34'4'
    {0x00,0x00,0xFE,0xC0,0xC0,0xC0,0xFC,0x06,0x06,0x06,0xC6,0x7C,0x00,0x00,0x00,0x00},       //0x35'5'
    {0x00,0x00,0x38,0x60,0xC0,0xC0,0xFC,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00},       //0x36'6'
    {0x00,0x00,0xFE,0xC6,0x06,0x06,0x0C,0x18,0x30,0x30,0x30,0x30,0x00,0x00,0x00,0x00},       //0x37'7'
    {0x00,0x00,0x7C,0xC6,0xC6,0xC6,0x7C,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00},       //0x38'8'
    {0x00,0x00,0x7C,0xC6,0xC6,0xC6,0x7E,0x06,0x06,0x06,0x0C,0x78,0x00,0x00,0x00,0x00},       //0x39'9'
    {0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00,0x00},       //0x3A':'
    {0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x18,0x18,0x30,0x00,0x00,0x00,0x00},       //0x3B';'
    {0x00,0x00,0x00,0x06,0x0C,0x18,0x30,0x60,0x30,0x18,0x0C,0x06,0x00,0x00,0x00,0x00},       //0x3C'<'
    {0x00,0x00,0x00,0x00,0x00,0x7E,0x00,0x00,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,0x00},       //0x3D'='
    {0x00,0x00,0x00,0x60,0x30,0x18,0x0C,0x06,0x0C,0x18,0x30,0x60,0x00,0x00,0x00,0x00},       //0x3E'>'
    {0x00,0x00,0x7C,0xC6,0xC6,0x0C,0x18,0x18,0x18,0x00,0x18,0x18,0x00,0x00,0x00,0x00},       //0x3F'?'
    {0x00,0x00,0x00,0x7C,0xC6,0xC6,0xDE,0xDE,0xDE,0xDC,0xC0,0x7C,0x00,0x00,0x00,0x00},       //0x40'@'
    {0x00,0x00,0x10,0x38,0x6C,0xC6,0xC6,0xFE,0xC6,0xC6,0xC6,0xC6,0x00,0x00,0x00,0x00},       //0x41'A' //https://blog.csdn.net/czg13548930186/article/details/79861914
    {0x00,0x00,0xFC,0x66,0x66,0x66,0x7C,0x66,0x66,0x66,0x66,0xFC,0x00,0x00,0x00,0x00},       //0x42'B'
    {0x00,0x00,0x3C,0x66,0xC2,0xC0,0xC0,0xC0,0xC0,0xC2,0x66,0x3C,0x00,0x00,0x00,0x00},       //0x43'C'
    {0x00,0x00,0xF8,0x6C,0x66,0x66,0x66,0x66,0x66,0x66,0x6C,0xF8,0x00,0x00,0x00,0x00},       //0x44'D'
    {0x00,0x00,0xFE,0x66,0x62,0x68,0x78,0x68,0x60,0x62,0x66,0xFE,0x00,0x00,0x00,0x00},       //0x45'E'
    {0x00,0x00,0xFE,0x66,0x62,0x68,0x78,0x68,0x60,0x60,0x60,0xF0,0x00,0x00,0x00,0x00},       //0x46'F'
    {0x00,0x00,0x3C,0x66,0xC2,0xC0,0xC0,0xDE,0xC6,0xC6,0x66,0x3A,0x00,0x00,0x00,0x00},       //0x47'G'
    {0x00,0x00,0xC6,0xC6,0xC6,0xC6,0xFE,0xC6,0xC6,0xC6,0xC6,0xC6,0x00,0x00,0x00,0x00},       //0x48'H'
    {0x00,0x00,0x3C,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00},       //0x49'I'
    {0x00,0x00,0x1E,0x0C,0x0C,0x0C,0x0C,0x0C,0xCC,0xCC,0xCC,0x78,0x00,0x00,0x00,0x00},       //0x4A'J'
    {0x00,0x00,0xE6,0x66,0x66,0x6C,0x78,0x78,0x6C,0x66,0x66,0xE6,0x00,0x00,0x00,0x00},       //0x4B'K'
    {0x00,0x00,0xF0,0x60,0x60,0x60,0x60,0x60,0x60,0x62,0x66,0xFE,0x00,0x00,0x00,0x00},       //0x4C'L'
    {0x00,0x00,0xC6,0xEE,0xFE,0xFE,0xD6,0xC6,0xC6,0xC6,0xC6,0xC6,0x00,0x00,0x00,0x00},       //0x4D'M'
    {0x00,0x00,0xC6,0xE6,0xF6,0xFE,0xDE,0xCE,0xC6,0xC6,0xC6,0xC6,0x00,0x00,0x00,0x00},       //0x4E'N'
    {0x00,0x00,0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00},       //0x4F'O'
    {0x00,0x00,0xFC,0x66,0x66,0x66,0x7C,0x60,0x60,0x60,0x60,0xF0,0x00,0x00,0x00,0x00},       //0x50'P'
    {0x00,0x00,0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0xD6,0xDE,0x7C,0x0C,0x0E,0x00,0x00},       //0x51'Q'
    {0x00,0x00,0xFC,0x66,0x66,0x66,0x7C,0x6C,0x66,0x66,0x66,0xE6,0x00,0x00,0x00,0x00},       //0x52'R'
    {0x00,0x00,0x7C,0xC6,0xC6,0x60,0x38,0x0C,0x06,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00},       //0x53'S'
    {0x00,0x00,0x7E,0x7E,0x5A,0x18,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00},       //0x54'T'
    {0x00,0x00,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00},       //0x55'U'
    {0x00,0x00,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x6C,0x38,0x10,0x00,0x00,0x00,0x00},       //0x56'V'
    {0x00,0x00,0xC6,0xC6,0xC6,0xC6,0xD6,0xD6,0xD6,0xFE,0xEE,0x6C,0x00,0x00,0x00,0x00},       //0x57'W'
    {0x00,0x00,0xC6,0xC6,0x6C,0x7C,0x38,0x38,0x7C,0x6C,0xC6,0xC6,0x00,0x00,0x00,0x00},       //0x58'X'
    {0x00,0x00,0x66,0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00},       //0x59'Y'
    {0x00,0x00,0xFE,0xC6,0x86,0x0C,0x18,0x30,0x60,0xC2,0xC6,0xFE,0x00,0x00,0x00,0x00},       //0x5A'Z'
    {0x00,0x00,0x3C,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x3C,0x00,0x00,0x00,0x00},       //0x5B'['
    {0x00,0x00,0x00,0x80,0xC0,0xE0,0x70,0x38,0x1C,0x0E,0x06,0x02,0x00,0x00,0x00,0x00},       //0x5C'\'
    {0x00,0x00,0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00,0x00,0x00,0x00},       //0x5D']'
    {0x10,0x38,0x6C,0xC6,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},       //0x5E'^'
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00},       //0x5F'_'
    {0x30,0x30,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},       //0x60'`'
    {0x00,0x00,0x00,0x00,0x00,0x78,0x0C,0x7C,0xCC,0xCC,0xCC,0x76,0x00,0x00,0x00,0x00},       //0x61'a'
    {0x00,0x00,0xE0,0x60,0x60,0x78,0x6C,0x66,0x66,0x66,0x66,0x7C,0x00,0x00,0x00,0x00},       //0x62'b'
    {0x00,0x00,0x00,0x00,0x00,0x7C,0xC6,0xC0,0xC0,0xC0,0xC6,0x7C,0x00,0x00,0x00,0x00},       //0x63'c'
    {0x00,0x00,0x1C,0x0C,0x0C,0x3C,0x6C,0xCC,0xCC,0xCC,0xCC,0x76,0x00,0x00,0x00,0x00},       //0x64'd'
    {0x00,0x00,0x00,0x00,0x00,0x7C,0xC6,0xFE,0xC0,0xC0,0xC6,0x7C,0x00,0x00,0x00,0x00},       //0x65'e'
    {0x00,0x00,0x38,0x6C,0x64,0x60,0xF0,0x60,0x60,0x60,0x60,0xF0,0x00,0x00,0x00,0x00},       //0x66'f'
    {0x00,0x00,0x00,0x00,0x00,0x76,0xCC,0xCC,0xCC,0xCC,0xCC,0x7C,0x0C,0xCC,0x78,0x00},       //0x67'g'
    {0x00,0x00,0xE0,0x60,0x60,0x6C,0x76,0x66,0x66,0x66,0x66,0xE6,0x00,0x00,0x00,0x00},       //0x68'h'
    {0x00,0x00,0x18,0x18,0x00,0x38,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00},       //0x69'i'
    {0x00,0x00,0x06,0x06,0x00,0x0E,0x06,0x06,0x06,0x06,0x06,0x06,0x66,0x66,0x3C,0x00},       //0x6A'j'
    {0x00,0x00,0xE0,0x60,0x60,0x66,0x6C,0x78,0x78,0x6C,0x66,0xE6,0x00,0x00,0x00,0x00},       //0x6B'k'
    {0x00,0x00,0x38,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00},       //0x6C'l'
    {0x00,0x00,0x00,0x00,0x00,0xEC,0xFE,0xD6,0xD6,0xD6,0xD6,0xC6,0x00,0x00,0x00,0x00},       //0x6D'm'
    {0x00,0x00,0x00,0x00,0x00,0xDC,0x66,0x66,0x66,0x66,0x66,0x66,0x00,0x00,0x00,0x00},       //0x6E'n'
    {0x00,0x00,0x00,0x00,0x00,0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00},       //0x6F'o'
    {0x00,0x00,0x00,0x00,0x00,0xDC,0x66,0x66,0x66,0x66,0x66,0x7C,0x60,0x60,0xF0,0x00},       //0x70'p'
    {0x00,0x00,0x00,0x00,0x00,0x76,0xCC,0xCC,0xCC,0xCC,0xCC,0x7C,0x0C,0x0C,0x1E,0x00},       //0x71'q'
    {0x00,0x00,0x00,0x00,0x00,0xDC,0x76,0x66,0x60,0x60,0x60,0xF0,0x00,0x00,0x00,0x00},       //0x72'r'
    {0x00,0x00,0x00,0x00,0x00,0x7C,0xC6,0x60,0x38,0x0C,0xC6,0x7C,0x00,0x00,0x00,0x00},       //0x73's'
    {0x00,0x00,0x10,0x30,0x30,0xFC,0x30,0x30,0x30,0x30,0x36,0x1C,0x00,0x00,0x00,0x00},       //0x74't'
    {0x00,0x00,0x00,0x00,0x00,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x76,0x00,0x00,0x00,0x00},       //0x75'u'
    {0x00,0x00,0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x66,0x3C,0x18,0x00,0x00,0x00,0x00},       //0x76'v'
    {0x00,0x00,0x00,0x00,0x00,0xC6,0xC6,0xD6,0xD6,0xD6,0xFE,0x6C,0x00,0x00,0x00,0x00},       //0x77'w'
    {0x00,0x00,0x00,0x00,0x00,0xC6,0x6C,0x38,0x38,0x38,0x6C,0xC6,0x00,0x00,0x00,0x00},       //0x78'x'
    {0x00,0x00,0x00,0x00,0x00,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x7E,0x06,0x0C,0xF8,0x00},       //0x79'y'
    {0x00,0x00,0x00,0x00,0x00,0xFE,0xCC,0x18,0x30,0x60,0xC6,0xFE,0x00,0x00,0x00,0x00},       //0x7A'z'
    {0x00,0x00,0x0E,0x18,0x18,0x18,0x70,0x18,0x18,0x18,0x18,0x0E,0x00,0x00,0x00,0x00},       //0x7B'{'
    {0x00,0x00,0x18,0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x18,0x18,0x00,0x00,0x00,0x00},       //0x7C'|'
    {0x00,0x00,0x70,0x18,0x18,0x18,0x0E,0x18,0x18,0x18,0x18,0x70,0x00,0x00,0x00,0x00},       //0x7D'}'
    {0x00,0x00,0x76,0xDC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},       //0x7E'~'
    {0x00,0x00,0x00,0x00,0x10,0x38,0x6C,0xC6,0xC6,0xC6,0xFE,0x00,0x00,0x00,0x00,0x00},       //0x7F''
    {0x00,0x00,0x3C,0x66,0xC2,0xC0,0xC0,0xC0,0xC2,0x66,0x3C,0x0C,0x06,0x7C,0x00,0x00},       //0x80
    {0x00,0x00,0xCC,0x00,0x00,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x76,0x00,0x00,0x00,0x00},       //0x81
    {0x00,0x0C,0x18,0x30,0x00,0x7C,0xC6,0xFE,0xC0,0xC0,0xC6,0x7C,0x00,0x00,0x00,0x00},       //0x82
    {0x00,0x10,0x38,0x6C,0x00,0x78,0x0C,0x7C,0xCC,0xCC,0xCC,0x76,0x00,0x00,0x00,0x00},       //0x83
    {0x00,0x00,0xCC,0x00,0x00,0x78,0x0C,0x7C,0xCC,0xCC,0xCC,0x76,0x00,0x00,0x00,0x00},       
};

typedef struct 
{
	UINT8 Path[20];
	UINT16 PartitionID;
	UINT16 ItemID;
	UINT16 MyComputerNextState;
	UINT64 SectorStartOld;  //���ڼ�¼��һ�η����ļ�����Ŀ¼��Ӧ�������Ż��߿��
	UINT64 LastVisitedItemAttribute;  //���ڼ�¼��һ�η����ļ�����Ŀ¼��Ӧ�������Ż��߿��
	MY_COMPUTER_WINDOW_CLICKED_EVENT CurrentEvent; //��ǰ�������ҵĵ��ԡ������¼�
}MY_COMPUTER_CURRENT_STATE;

//���ڱ����ҵĵ��Դ��ڵ�һЩ�¼���Ϣ
MY_COMPUTER_CURRENT_STATE MyComputerCurrentState;



/****************************************************************************
*
*  ����:     ����ӿ�ֻ���ڷ��������ʱĿ¼չʾ����������ӿ�ֻ���Ƿ������������ʹ��
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_GRAPHICS_PathPushByName(MY_COMPUTER_CURRENT_STATE *pMyComputerCurrentState, UINT8 *pName)
{
	UINT16 i = 0;
	pMyComputerCurrentState->Path[0] = '/' ;

	while(L1_STRING_IsValidNameChar(pName[i]))
	{
		pMyComputerCurrentState->Path[i + 1] = pName[i];
		i++;
	}

	pMyComputerCurrentState->Path[i + 1] = '\0';

	pMyComputerCurrentState->SectorStartOld = MY_COMPUTER_PARTITION_CLICKED_FLAG;
	
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: PartitionName: %a\n",  __LINE__, pName);
    
}


/****************************************************************************
*
*  ����:     ���·��ʵ�Ŀ¼�����ļ���Ϣ��ӵ��ҵĵ��Ե�ǰ����·��
*          ����ϴη��ʵ����ļ�������Ҫ���ļ����滻��
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_GRAPHICS_PathPush(MY_COMPUTER_CURRENT_STATE *pMyComputerCurrentState, UINT16 ItemIndex)
{
	UINT16 PathLength = 0;	
	UINT16 i = 0;

	//8���ļ������ȣ�3����׺�����ȣ�1����Ź�12������һ��'\0'
	UINT8  FileName[14] = {0};

	if (ItemIndex > 32)
	{		
		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d ItemIndex > 32: %d\n", __LINE__, ItemIndex);
		return;
	}

	//��ȡ��ǰ��·�����ȣ���Ҫ�ڳ���·�������������·������
	PathLength = L1_STRING_Length(pMyComputerCurrentState->Path);

	L1_FILE_NameGetUseItem(pItems[ItemIndex], FileName);
	
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d pMyComputerCurrentState->LastVisitedItemAttribute: %d\n", __LINE__, pMyComputerCurrentState->LastVisitedItemAttribute);
		
	//���ǰ����ʵ������ļ�������Ҫ���ļ�ǰ���/��ʼд�µ�������
	if (pMyComputerCurrentState->LastVisitedItemAttribute == FAT32_FILE_SYSTEM_ATTRIBUTE_FILE) 
	{
		for (i = PathLength - 1; i > 0; i--)
		{
			if (pMyComputerCurrentState->Path[i] == '/')
			{
				PathLength = i;
				break;
			}
		}
	}

	//��Ϊ��Ҫ������Ŀ¼��������Ҫ������/
	pMyComputerCurrentState->Path[PathLength] = '/';
			
	//��Ϊ�ϱ��Ѿ������/������������Ҫ��1��ʼд���µ�·���ַ���
	i = 1;	
	while(L1_STRING_IsValidNameChar(FileName[i - 1]))
	{
		pMyComputerCurrentState->Path[PathLength + i] = FileName[i - 1];
		i++;
	}
	
	pMyComputerCurrentState->Path[PathLength + i]  = '\0';

	pMyComputerCurrentState->LastVisitedItemAttribute = pItems[ItemIndex].Attribute[0];

}


/****************************************************************************
*
*  ����:   ��ԭ�ڴ濽����Ŀ���ڴ��ָ��λ�á���ǰ��ȱ��һЩ�Ϸ����ж�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L3_GRAPHICS_ItemPrint(UINT8 *pDestBuffer, UINT8 *pSourceBuffer, UINT16 pDestWidth, UINT16 pDestHeight, 
                              UINT16 pSourceWidth, UINT16 pSourceHeight, UINT16 x, UINT16 y, CHAR8 *pNameString, CHAR16 StringType, UINT16 DestLayerID)
{
	UINT16 WindowLayerID = 0;
	WindowLayers.item[WindowLayerID];
    
    for (int j = 0; j < pSourceHeight; j++)
    {
        for (int k = 0; k < pSourceWidth; k++)
        {
            pDestBuffer[((y + j) * pDestWidth + x + k) * 4 ]     = pSourceBuffer[((pSourceHeight - j) * pSourceWidth + k) * 3 ];
            pDestBuffer[((y + j) * pDestWidth + x + k) * 4 + 1 ] = pSourceBuffer[((pSourceHeight - j) * pSourceWidth + k) * 3 + 1 ];
            pDestBuffer[((y + j) * pDestWidth + x + k) * 4 + 2 ] = pSourceBuffer[((pSourceHeight - j) * pSourceWidth + k) * 3 + 2 ];
            pDestBuffer[((y + j) * pDestWidth + x + k) * 4 + 3 ] = DestLayerID;
        }
    }

    //if (2 == StringType)
    //    L2_DEBUG_Print2(x, y + pDestHeight, pDestBuffer, "%a ", pNameString);
}


/****************************************************************************
*
*  ����:   ͼƬ���ڴ濽�����о���L3_GRAPHICS_ItemPrint��Щ���ظ��ġ�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_GRAPHICS_MouseMoveoverObjectSetZero()
{    
    bMouseMoveoverObject = FALSE;
   
}


/****************************************************************************
*
*  ����:   ͼƬ���ڴ濽�����о���L3_GRAPHICS_ItemPrint��Щ���ظ��ġ�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_GRAPHICS_Copy(UINT8 *pDest, UINT8 *pSource, 
                           UINT16 DestWidth, UINT16 DestHeight, 
                           UINT16 SourceWidth, UINT16 SourceHeight, 
                           UINT16 StartX, UINT16 StartY)
{
    UINT16 i, j;
    for(i = 0; i < SourceHeight; i++)
    {
        for (j = 0; j < SourceWidth; j++)
        {
            pDest[((StartY + i) * DestWidth + StartX + j) * 4] = pSource[(i * SourceWidth + j) * 4];
            pDest[((StartY + i) * DestWidth + StartX + j) * 4 + 1] = pSource[(i * SourceWidth + j) * 4 + 1];
            pDest[((StartY + i) * DestWidth + StartX + j) * 4 + 2] = pSource[(i * SourceWidth + j) * 4 + 2];
            pDest[((StartY + i) * DestWidth + StartX + j) * 4 + 3] = pSource[(i * SourceWidth + j) * 4 + 3];
        }
    }
}






/****************************************************************************
*
*  ����:   ��ֱ��
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_GRAPHICS_LineDraw(UINT8 *pBuffer,
        IN UINTN x0, UINTN y0, UINTN x1, UINTN y1, 
        IN UINTN BorderWidth,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL BorderColor, UINT16 AreaWidth)
{
    INT32 dx  = L1_MATH_ABS((int)(x1 - x0));
    INT32 sx  = x0 < x1 ? 1 : -1;
    INT32 dy  = L1_MATH_ABS((int)(y1 - y0)); 
    INT32 sy  = y0 < y1 ? 1 : -1;
    INT32 err = ( dx > dy ? dx : -dy) / 2, e2;
    
    for(;;)
    {    
        L1_MEMORY_CopyColor1(pBuffer, BorderColor, x0, y0, AreaWidth);
        
        if (x0==x1 && y0==y1) break;
    
        e2 = err;
    
        if (e2 > -dx) { err -= dy; x0 += sx; }
        if (e2 <  dy) { err += dx; y0 += sy; }
    }
    return EFI_SUCCESS;
}


// draw rectangle borders



/****************************************************************************
*
*  ����:   �����Σ���ǰ�ߵĿ�Ȼ�δʹ�ã��Ƚϴֲ�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
void L2_GRAPHICS_RectangleDraw(UINT8 *pBuffer,
        IN UINTN x0, UINTN y0, UINTN x1, UINTN y1, 
        IN UINTN BorderWidth,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color, UINT16 AreaWidth)
{    
    //�������жϺ���Ҫ����Ȼ����Ĳ������ܴܺ�Ȼ���ֱ�������� - -
    if (x0 > ScreenWidth || y0 > ScreenHeight || x1 > ScreenWidth || y1 > ScreenHeight)
        return;
        
    if (NULL == pBuffer)
    {
        //DEBUG ((EFI_D_INFO, "NULL == pBuffer"));
        return ;
    }
    
    L2_GRAPHICS_LineDraw(pBuffer, x0, y0, x0, y1, 1, Color, AreaWidth);
    L2_GRAPHICS_LineDraw(pBuffer, x0, y0, x1, y0, 1, Color, AreaWidth);
    L2_GRAPHICS_LineDraw(pBuffer, x0, y1, x1, y1, 1, Color, AreaWidth);
    L2_GRAPHICS_LineDraw(pBuffer, x1, y0, x1, y1, 1, Color, AreaWidth);
}


// fill into rectangle



/****************************************************************************
*
*  ����:   �������
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
void L1_MEMORY_RectangleFill(UINT8 *pBuffer,
        UINTN x0, UINTN y0, UINTN x1, UINTN y1, 
        UINTN DestWidth,
        EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color)
{    
    if (NULL == pBuffer)
    {
        //DEBUG ((EFI_D_INFO, "NULL == pBuffer"));
        return ;
    }

	if (x0 >= x1 || y0 >= y1)
	{
		return;
	}
    

    UINT32 i = 0;
    UINT32 j = 0;
    for (j = y0; j <= y1; j++) 
    {
        for (i = x0; i <= x1; i++) 
        {
            pBuffer[(j * DestWidth + i) * 4]     =  Color.Blue; //Blue   
            pBuffer[(j * DestWidth + i) * 4 + 1] =  Color.Green; //Green 
            pBuffer[(j * DestWidth + i) * 4 + 2] =  Color.Red; //Red  
            pBuffer[(j * DestWidth + i) * 4 + 3] =  Color.Reserved; //Red  
        }
    }

}


/****************************************************************************
*
*  ����:   ������䣬x1,y1�ǻ���x0,y0������
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
void L1_MEMORY_RectangleFillInrease(UINT8 *pBuffer,
        UINTN x0, UINTN y0, UINTN IncreaseX, UINTN IncreaseY, 
        UINTN DestWidth,
        EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color)
{    
    if (NULL == pBuffer)
    {
        //DEBUG ((EFI_D_INFO, "NULL == pBuffer"));
        return ;
    }

	if ( IncreaseX == 0 || 0 == IncreaseY)
	{
		return;
	}
    

    UINT32 i = 0;
    UINT32 j = 0;
    for (j = y0; j <= y0 + IncreaseY; j++) 
    {
        for (i = x0; i <= x0 + IncreaseX; i++) 
        {
            pBuffer[(j * DestWidth + i) * 4]     =  Color.Blue; //Blue   
            pBuffer[(j * DestWidth + i) * 4 + 1] =  Color.Green; //Green 
            pBuffer[(j * DestWidth + i) * 4 + 2] =  Color.Red; //Red  
            pBuffer[(j * DestWidth + i) * 4 + 3] =  Color.Reserved; //Red  
        }
    }

}


/****************************************************************************
*
*  ����:   �������
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
void L1_MEMORY_RectangleFill2(UINT8 *pBuffer,
        IN UINTN x0, UINTN y0, UINTN x1, UINTN y1, 
        IN UINTN DestBufferWidth,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color)
{    
    if (NULL == pBuffer)
    {
        //DEBUG ((EFI_D_INFO, "NULL == pBuffer"));
        return ;
    }
    

    UINT32 i = 0;
    UINT32 j = 0;
    for (j = y0; j <= y1; j++) 
    {
        for (i = x0; i <= x1; i++) 
        {
            pBuffer[(j * DestBufferWidth + i) * 4]     =  Color.Blue; //Blue   
            pBuffer[(j * DestBufferWidth + i) * 4 + 1] =  Color.Green; //Green 
            pBuffer[(j * DestBufferWidth + i) * 4 + 2] =  Color.Red; //Red  
            pBuffer[(j * DestBufferWidth + i) * 4 + 3] =  Color.Reserved; //Red  
        }
    }

}


VOID L2_MOUSE_TerminalWindowCloseClicked()
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_MyComputerCloseClicked\n", __LINE__);
    //DisplayMyComputerFlag = 0;
    //WindowLayers.item[3].DisplayFlag = 0;


	//L3_APPLICATION_MyComputerWindow(0, 50);
	
    if (TRUE == WindowLayers.item[GRAPHICS_LAYER_TERMINAL_WINDOW].DisplayFlag)
    {
        WindowLayers.ActiveWindowCount--;        
        WindowLayers.item[GRAPHICS_LAYER_TERMINAL_WINDOW].DisplayFlag = FALSE;
    }
}



/****************************************************************************
*
*  ����:   �ҵĵ��Դ��ڵ���¼��йرմ����¼���Ӧ��ע��δ�����ڴ��ͷţ����Ż��Ŀռ�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_MOUSE_MyComputerCloseClicked()
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_MyComputerCloseClicked\n", __LINE__);
    //DisplayMyComputerFlag = 0;
    //WindowLayers.item[3].DisplayFlag = 0;

	MyComputerNextState = MY_COMPUTER_INIT_STATE;
	PartitionItemID = 0xffff; // invalid
	FolderItemID = 0xffff; // invalid
	L1_MEMORY_Memset(&pItems, 0, sizeof(pItems));

	L3_APPLICATION_MyComputerWindow(0, 50);
	
    if (TRUE == WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].DisplayFlag)
    {
        WindowLayers.ActiveWindowCount--;        
        WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].DisplayFlag = FALSE;
    }
}




/****************************************************************************
*
*  ����:   ����������¼�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_MOUSE_MyComputerPartitionItemClicked()
{
	UINT16 i = 0;
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_MOUSE_MyComputerPartitionItemClicked, PartitionItemID: %d\n",  __LINE__, PartitionItemID);
    
    // this code may be have some problems, because my USB file system is FAT32, my Disk file system is NTFS.
    // others use this code must be careful...
    //UINT8 FileSystemType = L2_FILE_PartitionTypeAnalysis(PartitionItemID);

	L2_GRAPHICS_PathPushByName(&MyComputerCurrentState, device[PartitionItemID].PartitionName);

	MyComputerCurrentState.LastVisitedItemAttribute = FAT32_FILE_SYSTEM_ATTRIBUTE_DIRECTORY;

    L2_DEBUG_Print3(16 * 23, 32, WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW], "%a",
                                    MyComputerCurrentState.Path);		

    L2_FILE_PartitionTypeAnalysis(PartitionItemID);

    if (device[PartitionItemID].FileSystemType == FILE_SYSTEM_FAT32)
    {
        //��ȡ��Ŀ¼��Ŀ
        EFI_STATUS Status = L2_FILE_FAT32_DataSectorHandle(PartitionItemID);
        if (0 != Status)
            return;
            
        L2_STORE_FolderItemsPrint();
    }
    else if (device[PartitionItemID].FileSystemType == FILE_SYSTEM_NTFS)
    {
    	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%a %d: %d\n",  __FUNCTION__,  __LINE__, device[PartitionItemID].StartSectorNumber + MFT_ITEM_DOLLAR_ROOT * 2);
		
        // get MFT $ROOT item. 
        L2_FILE_NTFS_MFT_Item_Read(PartitionItemID, device[PartitionItemID].StartSectorNumber + MFT_ITEM_DOLLAR_ROOT * 2);

        // get data runs
        //L2_FILE_NTFS_MFTDollarRootFileAnalysis(BufferMFT);      

		L1_MEMORY_Memset(&NTFSFileSwitched, 0, sizeof(NTFSFileSwitched)); 
		
		//��ǰ���ԣ�ֻ��ʾһ���豸����ʾ����豸���Ի�Ƚ��鷳
		//if (3 == DeviceID)
		//L2_PARTITION_FileContentPrint(BufferMFT);

		L2_FILE_NTFS_FileItemBufferAnalysis(BufferMFT, &NTFSFileSwitched);

		for (UINT16 i = 0; i < 10; i++)
		{
			//�ҵ�A0����
			if (NTFSFileSwitched.NTFSFileAttributeHeaderSwitched[i].Type == MFT_ATTRIBUTE_DOLLAR_INDEX_ALLOCATION)
			{
				// Analysis data runs
				L2_FILE_NTFS_DollarRootA0DatarunAnalysis(NTFSFileSwitched.NTFSFileAttributeHeaderSwitched[i].Data);
				
				L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: %02X %02X %02X %02X %02X  %02X\n",  __LINE__, NTFSFileSwitched.NTFSFileAttributeHeaderSwitched[i].Data[0],
								NTFSFileSwitched.NTFSFileAttributeHeaderSwitched[i].Data[1],
								NTFSFileSwitched.NTFSFileAttributeHeaderSwitched[i].Data[2],
								NTFSFileSwitched.NTFSFileAttributeHeaderSwitched[i].Data[3],
								NTFSFileSwitched.NTFSFileAttributeHeaderSwitched[i].Data[4],
								NTFSFileSwitched.NTFSFileAttributeHeaderSwitched[i].Data[5]);
				break;
			}
		}
		
        // use data run get root path item index
        L2_FILE_NTFS_RootPathItemsRead(PartitionItemID);
		

		L2_STORE_FolderItemsPrint2();
    }
    else if (device[PartitionItemID].FileSystemType == FILE_SYSTEM_MAX)
    {
        return;
    }
}




/****************************************************************************
*
*  ����:   �ļ�������ʾ
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_PARTITION_FileContentPrint(UINT8 *Buffer)
{	
	for (int j = 0; j < 512; j++)
    {
        L2_DEBUG_Print1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", Buffer[j] & 0xff);
    }
}



/****************************************************************************
*
*  ����:   �ļ�������ʾ
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_PARTITION_BufferPrint(UINT8 *Buffer, UINT16 Length)
{	
	for (int j = 0; j < Length; j++)
    {
        L2_DEBUG_Print1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", Buffer[j] & 0xff);
    }
}

EFI_STATUS L2_FILE_NTFS_90AttributeAnalysis()
{
}


// 1. To judge folder item is file or folder
// 2. Get StartClusterHigh2B and StartClusterLow2B and computer the next read sector id number
// 3. Read sector id number sector content from partition
// 4. Analysis content and print result 

/****************************************************************************
*
*  ����:   �ļ������ļ�����Ŀ¼���������ʾ�ļ�������ʾ��Ŀ¼����
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_MOUSE_MyComputerFolderItemClicked()
{
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_MyComputerFolderItemClicked\n", __LINE__);
	//FolderItemID;
	//L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d DeviceID: %d\n", __LINE__, DeviceID);
    //printf( "RootPathAnalysis\n" );
    EFI_STATUS Status ;
    UINT8 Buffer[DISK_BUFFER_SIZE];
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d PartitionItemID: %d FolderItemID: %d \n", __LINE__, PartitionItemID, FolderItemID);
		
	if (0xffff == PartitionItemID || 0xffff == FolderItemID)
	{
		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d 0xffff == PartitionItemID\n", __LINE__);
		return -1;
	}

	//��Ҫ�һ�ȡ��Ч��������
	UINT16 index = FolderItemValidIndexArray[FolderItemID];
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d index: %d\n", __LINE__, index);

	MyComputerCurrentState.LastVisitedItemAttribute = pItems[index].Attribute[0];

	L2_GRAPHICS_PathPush(&MyComputerCurrentState, index);

    L2_DEBUG_Print3(16 * 23, 32, WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW], "%a",
                                    MyComputerCurrentState.Path);	

	//FAT32�ļ�ϵͳ��ʽ
	if (device[PartitionItemID].FileSystemType == FILE_SYSTEM_FAT32)
    {    	
		UINT16 High2B = L1_NETWORK_2BytesToUINT16(pItems[index].StartClusterHigh2B);
		UINT16 Low2B  = L1_NETWORK_2BytesToUINT16(pItems[index].StartClusterLow2B);
		UINT32 StartCluster = (UINT32)High2B << 16 | (UINT32)Low2B;

		// Start cluster id is 2, exclude 0,1
		//����д��8192������BUG
		UINT32 StartSectorNumber = 8192 + (StartCluster - 2) * 8;
		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], 
						"%d High2B: %X Low2B: %X StartCluster: %X StartSectorNumber: %X\n", 
						__LINE__, 
						High2B,
						Low2B,
						StartCluster,
						StartSectorNumber);

        //�����efi�ļ�����Ϊ�ǿ�ִ���ļ�
        if (pItems[index].ExtensionName[0] == 'E' && pItems[index].ExtensionName[1] == 'F' && pItems[index].ExtensionName[2] == 'I' )
        {
            pItems[index].ExtensionName[0] = 'e';
            pItems[index].ExtensionName[1] = 'f';
            pItems[index].ExtensionName[2] = 'i';
            
            UINT8 FileName[13] = {0};
            L1_FILE_NameMerge(index, FileName);
            CHAR16 wcFileName[13] = {0};

            for (UINT8 i = 0; '\0' != FileName[i]; i++)
            {
                wcFileName[i] = FileName[i];
            }
                            
            EFI_EVENT       Event;
            L2_ApplicationRun(Event, wcFileName);
        }

		// Read data from partition(disk or USB etc..)					
	    Status = L2_STORE_Read(PartitionItemID, StartSectorNumber, 1, Buffer); 
	    if (EFI_ERROR(Status))
	    {
	        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
	        return Status;
	    }

		switch(pItems[index].Attribute[0])
		{
		    //�����Ŀ¼������ʾ��Ŀ¼
			case FAT32_FILE_SYSTEM_ATTRIBUTE_DIRECTORY:  L1_MEMORY_Memset(&pItems, 0, sizeof(pItems));
					    L1_MEMORY_Copy(&pItems, Buffer, DISK_BUFFER_SIZE);
						L2_STORE_FolderItemsPrint();
						break;

			//������ļ�������ʾ�ļ�����
			case FAT32_FILE_SYSTEM_ATTRIBUTE_FILE: L2_PARTITION_FileContentPrint(Buffer); break;

			default: break;
		}
	    
    }
    else if (device[PartitionItemID].FileSystemType == FILE_SYSTEM_NTFS) //NTFS�ļ�ϵͳ��ʽ
    {
		//���ܻ���BUG 6291456=786432 * 8���е�MFT����786432
		UINT32 StartSectorNumber = 6291456 + pCommonStorageItems[index].FileContentRelativeSector * 2;
		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: StartSector: %llu Sector: %llu",  __LINE__, StartSectorNumber, pCommonStorageItems[index].FileContentRelativeSector);

		// Read data from partition(disk or USB etc..)					
	    Status = L2_STORE_Read(PartitionItemID, StartSectorNumber, 2, BufferMFT); 
	    if (EFI_ERROR(Status))
	    {
	        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
	        return Status;
	    }
		
		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d pNTFSFileSwitched Allocate memory: %d.\n", __LINE__, sizeof(NTFS_FILE_SWITCHED));
		
		L1_MEMORY_Memset(&NTFSFileSwitched, 0, sizeof(NTFSFileSwitched)); 
				
		L2_PARTITION_FileContentPrint(BufferMFT);

		//�ӷ�����ȡ���Ĵ�������FILE��������������������ļ����ļ���ӵ�е��������ͬ
		
		L2_FILE_NTFS_FileItemBufferAnalysis(BufferMFT, &NTFSFileSwitched);

		
		//return;
		for (UINT16 i = 0; i < 20; i++)
		{
			UINT8 type = NTFSFileSwitched.NTFSFileAttributeHeaderSwitched[i].Type;
			
			L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: type: %02X",  __LINE__, type);
		
			if (0 == type || MFT_ATTRIBUTE_INVALID == type)
			{
				break;
			}

			// 90 attribute Only Folder file item have MFT_ATTRIBUTE_DOLLAR_INDEX_ROOT and MFT_ATTRIBUTE_DOLLAR_INDEX_ALLOCATION attribute.
			if (type == MFT_ATTRIBUTE_DOLLAR_INDEX_ROOT)
			{				
				L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: %02X %02X %02X %02X %02X %02X\n",  __LINE__, NTFSFileSwitched.NTFSFileAttributeHeaderSwitched[i].Data[0],
								NTFSFileSwitched.NTFSFileAttributeHeaderSwitched[i].Data[1],
								NTFSFileSwitched.NTFSFileAttributeHeaderSwitched[i].Data[2],
								NTFSFileSwitched.NTFSFileAttributeHeaderSwitched[i].Data[3],
								NTFSFileSwitched.NTFSFileAttributeHeaderSwitched[i].Data[4],
								NTFSFileSwitched.NTFSFileAttributeHeaderSwitched[i].Data[5]);
				
				L2_STORE_FolderItemsPrint2();
			} 
			// Only Folder file item have MFT_ATTRIBUTE_DOLLAR_INDEX_ALLOCATION attribute.
			else if (type == MFT_ATTRIBUTE_DOLLAR_INDEX_ALLOCATION)
			{
				IndexInformation *pA0Indexes = L2_FILE_NTFS_GetA0Indexes();
				if (pA0Indexes[0].Offset != 0)
				{
				    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
					// Read data from partition(disk or USB etc..)					
				    Status = L2_STORE_Read(PartitionItemID, 8 * pA0Indexes[0].Offset, 2, BufferMFT); 
				    if (EFI_ERROR(Status))
				    {
				        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
				        return Status;
				    }
					
    				L2_FILE_NTFS_MFTIndexItemsAnalysis(BufferMFT, PartitionItemID);    
					//L1_MEMORY_Memset(&NTFSFileSwitched, 0, sizeof(NTFSFileSwitched)); 
					//L2_FILE_NTFS_FileItemBufferAnalysis(BufferMFT, &NTFSFileSwitched);
				}				
				
				L2_STORE_FolderItemsPrint2();
				break;
			}
			// Only file item have MFT_ATTRIBUTE_DOLLAR_DATA attribute.
			else if (type == MFT_ATTRIBUTE_DOLLAR_DATA)
			{
				//��ʾ�ļ�����ʱ���������֧��
	            L2_DEBUG_Print3(300, 300, WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW], "%d: %02X %02X %02X %02X", __LINE__, 
                                NTFSFileSwitched.NTFSFileAttributeHeaderSwitched[i].Data[0],
                                NTFSFileSwitched.NTFSFileAttributeHeaderSwitched[i].Data[1],
                                NTFSFileSwitched.NTFSFileAttributeHeaderSwitched[i].Data[2],
                                NTFSFileSwitched.NTFSFileAttributeHeaderSwitched[i].Data[3]);
				break;
			}
			
		}
		
    }
	

	return EFI_SUCCESS;
}


/****************************************************************************
*
*  ����:   �ҵĵ��Դ���״̬ת����״̬�����������ڹرա�����������ļ��е�����ļ�����¼���
   ���״̬����ǰ���������е�����ģ���Ϊ�ڵ�������¼���Ҳ���Լ�����������¼�
   ���κ�״̬�¶�Ӧ�ÿ��Ե�����ڹر��¼�����Ȼ�ڿ�ʼ���Ե�ʱ��������Ҫ��ģ�Խ�����ڿ����������״̬��ͻ�
   �ԵñȽ���
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
STATE_TRANSFORM MyComputerStateTransformTable[] =
{
    {MY_COMPUTER_INIT_STATE,                MY_COMPUTER_WINDOW_CLOSE_WINDOW_CLICKED_EVENT,     MY_COMPUTER_INIT_STATE,               L2_MOUSE_MyComputerCloseClicked},
    {MY_COMPUTER_INIT_STATE,     	        MY_COMPUTER_WINDOW_PARTITION_ITEM_CLICKED_EVENT,   MY_COMPUTER_PARTITION_CLICKED_STATE,  L2_MOUSE_MyComputerPartitionItemClicked},
    {MY_COMPUTER_PARTITION_CLICKED_STATE,   MY_COMPUTER_WINDOW_PARTITION_ITEM_CLICKED_EVENT,   MY_COMPUTER_FOLDER_CLICKED_STATE,     L2_MOUSE_MyComputerPartitionItemClicked},
    {MY_COMPUTER_PARTITION_CLICKED_STATE,   MY_COMPUTER_WINDOW_FOLDER_ITEM_CLICKED_EVENT,      MY_COMPUTER_FOLDER_CLICKED_STATE,     L2_MOUSE_MyComputerFolderItemClicked},
    {MY_COMPUTER_FOLDER_CLICKED_STATE, 	    MY_COMPUTER_WINDOW_FOLDER_ITEM_CLICKED_EVENT,      MY_COMPUTER_FOLDER_CLICKED_STATE,     L2_MOUSE_MyComputerFolderItemClicked},
};




/****************************************************************************
*
*  ����:   ���ڴ�������ڲ�ͬ���ڵ���¼��Ĵ�����������ڻᵽ����ͼ�����ϲ㡣
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L1_GRAPHICS_UpdateWindowLayer(UINT16 layer)
{	
    for (UINT16 i = 0; i < WindowLayers.LayerCount; i++)
    {
        if (layer == WindowLayers.LayerSequences[i])
        {
            for (UINT16 j = i; j < WindowLayers.LayerCount - 1; j++)
            {
                WindowLayers.LayerSequences[j] = WindowLayers.LayerSequences[j + 1];
            }
            WindowLayers.LayerSequences[WindowLayers.LayerCount - 1] = layer;
            break;
        }
    }

    //WindowLayers.LayerSequences[WindowLayers.LayerCount - 1] = layer;
}





/****************************************************************************
*
*  ����:   ������ҵĵ��ԡ��¼�������ʾ�ҵĵ��Դ���
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_MOUSE_MyComputerClicked()
{       
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_MyComputerClicked\n", __LINE__);
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    Color.Red = 0xff;
    Color.Green= 0x00;
    Color.Blue= 0x00;
    Color.Reserved = GRAPHICS_LAYER_MY_COMPUTER_WINDOW;

    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].DisplayFlag = TRUE;
	WindowLayers.item[GRAPHICS_LAYER_START_MENU].DisplayFlag = FALSE;
    MyComputerNextState = MY_COMPUTER_INIT_STATE;
		
    L1_GRAPHICS_UpdateWindowLayer(GRAPHICS_LAYER_MY_COMPUTER_WINDOW);
    
}





/****************************************************************************
*
*  ����:   �����ϵͳ���á��¼�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
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




/****************************************************************************
*
*  ����:   ������ڴ���Ϣ���¼�������ʾ�ڴ���Ϣ����
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
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




/****************************************************************************
*
*  ����:   �����ϵͳ��־���¼�������ʾϵͳ��־���ڣ������ȽϷ���ϵͳ���������ⶨλ
*        ע�⣺ϵͳ��־���ڣ���ʾ����L2_DEBUG_Print3������ӡ����־
*
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
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



/****************************************************************************
*
*  ����:   �����ϵͳ��־���¼�������ʾϵͳ��־���ڣ������ȽϷ���ϵͳ���������ⶨλ
*        ע�⣺ϵͳ��־���ڣ���ʾ����L2_DEBUG_Print3������ӡ����־
*
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_MOUSE_TerminalWindowClicked()
{   
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_TerminalWindowClicked\n", __LINE__);
    //DisplaySystemLogWindowFlag = TRUE;  
    //WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].DisplayFlag = TRUE;
    //if (FALSE == WindowLayers.item[GRAPHICS_LAYER_TERMINAL_WINDOW].DisplayFlag)
    //{
        WindowLayers.ActiveWindowCount++;
        WindowLayers.item[GRAPHICS_LAYER_TERMINAL_WINDOW].DisplayFlag = TRUE;
        
    //}
	WindowLayers.item[GRAPHICS_LAYER_START_MENU].DisplayFlag = FALSE;
    L1_GRAPHICS_UpdateWindowLayer(GRAPHICS_LAYER_TERMINAL_WINDOW);
}



/****************************************************************************
*
*  ����: �����ϵͳ�˳����¼������Ի�ػ�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_MOUSE_SystemQuitClicked()
{   
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_SystemQuitClicked\n", __LINE__);
    SystemQuitFlag = TRUE;  
}





/****************************************************************************
*
*  ����:   ����������ֽ���á��¼��������ֽ���ң���ɫ����������forѭ�������
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_MOUSE_WallpaperSettingClicked()
{   
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_WallpaperSettingClicked\n", __LINE__);
    //DisplaySystemSettingWindowFlag = 1;
    if (TRUE == WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].DisplayFlag)
    {
        WindowLayers.ActiveWindowCount--;
        WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].DisplayFlag = FALSE;
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
    Color.Reserved  = GRAPHICS_LAYER_MY_COMPUTER_WINDOW;
    L1_MEMORY_RectangleFill(pDeskBuffer, 0,     y - 28, x -  1, y - 28, ScreenWidth, Color);

    Color.Red   = 0xFF;
    Color.Green = 0xFF;
    Color.Blue  = 0xFF;
    L1_MEMORY_RectangleFill(pDeskBuffer, 0,     y - 27, x -  1, y - 27, ScreenWidth, Color);
    
    Color.Red   = 0xC6;
    Color.Green = 0xC6;
    Color.Blue  = 0xC6;
    L1_MEMORY_RectangleFill(pDeskBuffer, 0,     y - 26, x -  1, y -  1, ScreenWidth, Color);
    
    Color.Red   = 0xFF;
    Color.Green = 0xFF;
    Color.Blue  = 0xFF;
    L1_MEMORY_RectangleFill(pDeskBuffer, 3,     y - 24, 59,     y - 24, ScreenWidth, Color);
    L1_MEMORY_RectangleFill(pDeskBuffer, 2,     y - 24, 59,     y - 4, ScreenWidth, Color);

    Color.Red   = 0x84;
    Color.Green = 0x84;
    Color.Blue  = 0x84;
    L1_MEMORY_RectangleFill(pDeskBuffer, 3,     y -  4, 59,     y -  4, ScreenWidth, Color);
    L1_MEMORY_RectangleFill(pDeskBuffer, 59,     y - 23, 59,     y -  5, ScreenWidth, Color);

    
    Color.Red   = 0x00;
    Color.Green = 0x00;
    Color.Blue  = 0x00;
    L1_MEMORY_RectangleFill(pDeskBuffer, 2,     y -  3, 59,     y -  3, ScreenWidth, Color);
    L1_MEMORY_RectangleFill(pDeskBuffer, 60,    y - 24, 60,     y -  3, ScreenWidth, Color);

    Color.Red   = 0x84;
    Color.Green = 0x84;
    Color.Blue  = 0x84;
    L1_MEMORY_RectangleFill(pDeskBuffer, x - 163, y - 24, x -  4, y - 24, ScreenWidth, Color);
    L1_MEMORY_RectangleFill(pDeskBuffer, x - 163, y - 23, x - 47, y -  4, ScreenWidth, Color);
    
    Color.Red   = 0xFF;
    Color.Green = 0xFF;
    Color.Blue  = 0xFF;
    L1_MEMORY_RectangleFill(pDeskBuffer, x - 163,    y - 3, x - 4,     y - 3, ScreenWidth, Color);
    L1_MEMORY_RectangleFill(pDeskBuffer, x - 3,     y - 24, x - 3,     y - 3, ScreenWidth, Color);

    Color.Red   = 0x00;
    Color.Green = 0x00;
    Color.Blue   = 0x00;

    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer,  16, ScreenHeight - 21,     (18 - 1) * 94 + 43 - 1, Color, ScreenWidth);
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer,  16 * 2, ScreenHeight - 21, (21 - 1) * 94 + 05 - 1, Color, ScreenWidth);      
}





/****************************************************************************
*
*  ����:   ��������汳��ͼƬ�ָ����¼�������ͼƬ��ָ������ͼƬ
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_MOUSE_WallpaperResetClicked()
{   
    if (TRUE == WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].DisplayFlag)
    {
        WindowLayers.ActiveWindowCount--;
        WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].DisplayFlag = FALSE;
    }
    
    WindowLayers.item[GRAPHICS_LAYER_START_MENU].DisplayFlag = FALSE;
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_WallpaperResetClicked\n", __LINE__);
    
    L2_GRAPHICS_DeskInit();
}


/****************************************************************************
*
*  ����:   ��ʼ�˵�״̬ת��������Ҫ
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
STATE_TRANSFORM StartMenuStateTransformTable[] =
{
    {START_MENU_ITEM_INIT_EVENT,    START_MENU_ITEM_MY_COMPUTER_CLICKED_EVENT,                            START_MENU_CLICK_INIT_STATE,          			L2_MOUSE_MyComputerClicked},
    {START_MENU_ITEM_INIT_EVENT,    START_MENU_ITEM_SYSTEM_SETTING_CLICKED_EVENT,                         START_MENU_SYSTEM_SETTING_CLICKED_STATE,       	L2_MOUSE_SystemSettingClicked},
    {START_MENU_ITEM_INIT_EVENT,    START_MENU_ITEM_MEMORY_INFORMATION_CLICKED_EVENT,   		          START_MENU_MEMORY_INFORMATION_CLICKED_STATE,   	L2_MOUSE_MemoryInformationClicked},
    {START_MENU_ITEM_INIT_EVENT,    START_MENU_ITEM_SYSTEM_LOG_CLICKED_EVENT,           		          START_MENU_SYSTEM_LOG_STATE,                   	L2_MOUSE_SystemLogClicked},
    {START_MENU_ITEM_INIT_EVENT,    START_MENU_ITEM_TERMINAL_CLICKED_EVENT,           		          		START_MENU_TERMINAL_STATE,                   	L2_MOUSE_TerminalWindowClicked},
    {START_MENU_ITEM_INIT_EVENT,    START_MENU_ITEM_SHUTDOWN_CLICKED_EVENT,          		              START_MENU_SYSTEM_QUIT_STATE,                  	L2_MOUSE_SystemQuitClicked},
    {START_MENU_SYSTEM_SETTING_CLICKED_STATE,  START_MENU_SYSTEM_SETTING_SUBITEM_WALLPAPER_SETTING_CLICKED_EVENT,    START_MENU_CLICK_INIT_STATE,                   	L2_MOUSE_WallpaperSettingClicked},
    {START_MENU_SYSTEM_SETTING_CLICKED_STATE,  START_MENU_SYSTEM_SETTING_SUBITEM_WALLPAPER_RESET_CLICKED_EVENT,      START_MENU_CLICK_INIT_STATE,                   	L2_MOUSE_WallpaperResetClicked},
};

void L2_GRAPHICS_Init()
{
    
}


/****************************************************************************
*
*  ����:   ͼ�������ʼ��
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
void L2_GRAPHICS_BootScreenInit()
{
    //������������⼸�У�����ֱ����ʾ�ڴ���Ϣ���������е���ѩ��    
    for (int j = ScreenHeight / 2; j < ScreenHeight; j++)
    {
        for (int i = 0; i < ScreenWidth; i++)
        {
            pDeskBuffer[(j * ScreenWidth + i) * 4]     = i % 255;
            pDeskBuffer[(j * ScreenWidth + i) * 4 + 1] = 0x00;
            pDeskBuffer[(j * ScreenWidth + i) * 4 + 2] = 0x00;
        }
    }

	L2_SCREEN_Draw(pDeskBuffer, 0, 0, 0, 0, ScreenWidth, ScreenHeight);	

}


/****************************************************************************
*
*  ����:   ͼ�������ʼ��
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
void L2_GRAPHICS_ParameterInit()
{
    WindowLayers.LayerCount = 0;

    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].DisplayFlag = TRUE;
    L1_MEMORY_Copy((UINT8 *)WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].Name, "System Log Window", sizeof("System Log Window"));
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].pBuffer = pSystemLogWindowBuffer;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].StartX = 0;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].StartY = 0;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].WindowWidth =  WINDOW_DEFAULT_WIDTH + 10 * 16;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].WindowHeight = WINDOW_DEFAULT_HEIGHT + 10 * 16;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].LayerID = GRAPHICS_LAYER_SYSTEM_LOG_WINDOW;

	SystemLogWindowHeight = WINDOW_DEFAULT_HEIGHT + 10 * 16;
	SystemLogWindowWidth = WINDOW_DEFAULT_WIDTH + 10 * 16;

    WindowLayers.LayerCount++;
    

    WindowLayers.item[GRAPHICS_LAYER_START_MENU].DisplayFlag = FALSE;
    L1_MEMORY_Copy((UINT8 *)WindowLayers.item[GRAPHICS_LAYER_START_MENU].Name, "Start Menu", sizeof("Start Menu"));
    WindowLayers.item[GRAPHICS_LAYER_START_MENU].pBuffer = pStartMenuBuffer;
    WindowLayers.item[GRAPHICS_LAYER_START_MENU].StartX = 0;
    WindowLayers.item[GRAPHICS_LAYER_START_MENU].StartY = ScreenHeight - StartMenuHeight - 25;
    WindowLayers.item[GRAPHICS_LAYER_START_MENU].WindowWidth = StartMenuWidth;
    WindowLayers.item[GRAPHICS_LAYER_START_MENU].WindowHeight= StartMenuHeight;
    WindowLayers.item[GRAPHICS_LAYER_START_MENU].LayerID = GRAPHICS_LAYER_START_MENU;
    
    WindowLayers.LayerCount++;

    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].DisplayFlag = FALSE;
    L1_MEMORY_Copy((UINT8 *)WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].Name, "System Setting Window", sizeof("System Setting Window"));
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].pBuffer = pSystemSettingWindowBuffer;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].StartX = StartMenuWidth;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].StartY = WindowLayers.item[GRAPHICS_LAYER_START_MENU].StartY + 16;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].WindowWidth = SystemSettingWindowWidth;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].WindowHeight= SystemSettingWindowHeight;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].LayerID = GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW;
    
    WindowLayers.LayerCount++;

    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].DisplayFlag = FALSE;
    L1_MEMORY_Copy((UINT8 *)WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].Name, "My Computer", sizeof("My Computer"));
    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].pBuffer = pMyComputerBuffer;
    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartX = 0;
    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartY = 0;
    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].WindowWidth = WINDOW_DEFAULT_WIDTH;
    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].WindowHeight = WINDOW_DEFAULT_HEIGHT;
    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].LayerID = GRAPHICS_LAYER_MY_COMPUTER_WINDOW;
    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].Step = 4;
    
    WindowLayers.LayerCount++;

    WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].DisplayFlag = FALSE;
    L1_MEMORY_Copy((UINT8 *)WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].Name, "Memory Information", sizeof("Memory Information"));
    WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].pBuffer = pMemoryInformationBuffer;
    WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].StartX = 0;
    WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].StartY = 0;
    WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].WindowWidth = MemoryInformationWindowWidth;
    WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].WindowHeight= MemoryInformationWindowHeight;
    WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].LayerID = GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW;

    WindowLayers.LayerCount++;
	
    WindowLayers.item[GRAPHICS_LAYER_DESK].DisplayFlag = TRUE;
    L1_MEMORY_Copy((UINT8 *)WindowLayers.item[GRAPHICS_LAYER_DESK].Name, "Desk layer", sizeof("Desk layer"));
    WindowLayers.item[GRAPHICS_LAYER_DESK].pBuffer = pDeskBuffer;
    WindowLayers.item[GRAPHICS_LAYER_DESK].StartX = 0;
    WindowLayers.item[GRAPHICS_LAYER_DESK].StartY = 0;
    WindowLayers.item[GRAPHICS_LAYER_DESK].WindowWidth = ScreenWidth;
    WindowLayers.item[GRAPHICS_LAYER_DESK].WindowHeight= ScreenHeight;
    WindowLayers.item[GRAPHICS_LAYER_DESK].LayerID = GRAPHICS_LAYER_DESK;

    WindowLayers.LayerCount++;
	
    WindowLayers.item[GRAPHICS_LAYER_TERMINAL_WINDOW].DisplayFlag = FALSE;
    L1_MEMORY_Copy((UINT8 *)WindowLayers.item[GRAPHICS_LAYER_TERMINAL_WINDOW].Name, "Terminal layer", sizeof("Terminal layer"));
    WindowLayers.item[GRAPHICS_LAYER_TERMINAL_WINDOW].pBuffer = pTerminalWindowBuffer;
    WindowLayers.item[GRAPHICS_LAYER_TERMINAL_WINDOW].StartX = 0;
    WindowLayers.item[GRAPHICS_LAYER_TERMINAL_WINDOW].StartY = 0;
    WindowLayers.item[GRAPHICS_LAYER_TERMINAL_WINDOW].WindowWidth = ScreenWidth * 4 / 5;
    WindowLayers.item[GRAPHICS_LAYER_TERMINAL_WINDOW].WindowHeight = WINDOW_DEFAULT_HEIGHT;
    WindowLayers.item[GRAPHICS_LAYER_TERMINAL_WINDOW].LayerID = GRAPHICS_LAYER_TERMINAL_WINDOW;

    WindowLayers.LayerCount++;
    
    WindowLayers.item[GRAPHICS_LAYER_MOUSE_RIGHT_CLICK_WINDOW].DisplayFlag = FALSE;
    L1_MEMORY_Copy((UINT8 *)WindowLayers.item[GRAPHICS_LAYER_MOUSE_RIGHT_CLICK_WINDOW].Name, "Mouse right click window layer", sizeof("Mouse right click window layer"));
    WindowLayers.item[GRAPHICS_LAYER_MOUSE_RIGHT_CLICK_WINDOW].pBuffer = pMouseRightButtonClickWindowBuffer;
    WindowLayers.item[GRAPHICS_LAYER_MOUSE_RIGHT_CLICK_WINDOW].StartX = 0;
    WindowLayers.item[GRAPHICS_LAYER_MOUSE_RIGHT_CLICK_WINDOW].StartY = 0;
    WindowLayers.item[GRAPHICS_LAYER_MOUSE_RIGHT_CLICK_WINDOW].WindowWidth = MouseRightButtonClickWindowWidth;
    WindowLayers.item[GRAPHICS_LAYER_MOUSE_RIGHT_CLICK_WINDOW].WindowHeight= MouseRightButtonClickWindowHeight;
    WindowLayers.item[GRAPHICS_LAYER_MOUSE_RIGHT_CLICK_WINDOW].LayerID = GRAPHICS_LAYER_MOUSE_RIGHT_CLICK_WINDOW;

    WindowLayers.LayerCount++;

    L1_MEMORY_SetValue(WindowLayers.LayerSequences, 0, LAYER_QUANTITY * 2);

    MouseClickFlag = MOUSE_EVENT_TYPE_NO_CLICKED;

	// Active window list, please note: desk layer always display at firstly.
	// So WindowLayers.LayerCount value always one more than WindowLayers.ActiveWindowCount
    WindowLayers.LayerSequences[0] = GRAPHICS_LAYER_START_MENU;
    WindowLayers.LayerSequences[1] = GRAPHICS_LAYER_MY_COMPUTER_WINDOW;
    WindowLayers.LayerSequences[2] = GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW;
    WindowLayers.LayerSequences[3] = GRAPHICS_LAYER_SYSTEM_LOG_WINDOW;
    WindowLayers.LayerSequences[4] = GRAPHICS_LAYER_TERMINAL_WINDOW;
    WindowLayers.LayerSequences[5] = GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW;
	
    WindowLayers.ActiveWindowCount = WindowLayers.LayerCount - 1;

    MouseMoveoverObjectDrawColor.Blue = 0;
    MouseMoveoverObjectDrawColor.Red = 0;
    MouseMoveoverObjectDrawColor.Green = 0;
    MouseMoveoverObjectDrawColor.Reserved = 0;

	WhiteColor.Blue  = 255;
	WhiteColor.Red	 = 255;
	WhiteColor.Green  = 255;
		
	BlackColor.Blue  = 0;
	BlackColor.Red	 = 0;
	BlackColor.Green  = 0;

	

}



/****************************************************************************
*
*  ����: ��ָ��ͼ���ӡָ���ַ���������layer��Ŀ��ͼ��
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID EFIAPI L2_DEBUG_Print3 (UINT16 x, UINT16 y, WINDOW_LAYER_ITEM layer, IN  CONST CHAR8  *Format, ...)
{
    if (y > layer.WindowHeight - 16 || x > layer.WindowWidth- 8)
        return;

    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    UINT32 i = 0;
    
        
    Color.Blue = 0xFF;
    Color.Red = 0xFF;
    Color.Green = 0xFF;
    Color.Reserved = layer.LayerID;

    for (UINT16 i = 0; i < 0x100; i++)
        AsciiBuffer[i] = 0;

    VA_LIST         VaList;
    VA_START (VaList, Format);

	UINT8 *pBuffer = layer.pBuffer;

	if (LogStatusErrorCount % 27 == 0)
	{
		for (int height = 2 * 16; height < SystemLogWindowHeight - 4; height++)
		{
			for (int width = 4;  width < SystemLogWindowWidth - 4; width++)
			{
				pBuffer[(height * SystemLogWindowWidth + width) * 4 + 0] = 235;
				pBuffer[(height * SystemLogWindowWidth + width) * 4 + 1] = 235;
				pBuffer[(height * SystemLogWindowWidth + width) * 4 + 2] = 235;
			}
		}	
		LogStatusErrorCount = 0;
	}
	
	
	if (layer.LayerID == GRAPHICS_LAYER_SYSTEM_LOG_WINDOW)
	{
		L2_STRING_Maker3(x, y, layer, Format, VaList);
	}
    else
    {
    	L2_STRING_Maker2(x, y, layer, Format, VaList);
    }

	
    VA_END (VaList);
	
    for (UINT16 i = 0; i < sizeof(AsciiBuffer) /sizeof(CHAR8); i++)
        L2_GRAPHICS_AsciiCharDraw2(layer, x + i * 8, y, AsciiBuffer[i], Color);
}


/****************************************************************************
*
*  ����:   Ӣ���ַ�����ʾ��������Ҫע��������������ӡ����Ϣ���������ϣ���WindowLayers.item[GRAPHICS_LAYER_DESK]����
*
*  ����x�� ��ʾ����Ļ��X����
*  ����y�� ��ʾ����Ļ��Y����
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID EFIAPI L2_DEBUG_Print1 (UINT16 x, UINT16 y,  IN  CONST CHAR8  *Format, ...)
{
    if (y > ScreenHeight - 16 || x > ScreenWidth - 8)
        return;

    if (NULL == pDeskBuffer)
        return;

    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    UINT32 i = 0;

    for (i = 0; i < 0x100; i++)
        AsciiBuffer[i] = 0;
        
	
    Color.Blue = 0xFF;
    Color.Red = 0xFF;
    Color.Green = 0xFF;
    Color.Reserved = 0;

    VA_LIST         VaList;
    VA_START (VaList, Format);
    L2_STRING_Maker(x, y, Format, VaList);
    VA_END (VaList);

    for (i = 0; i < sizeof(AsciiBuffer) /sizeof(CHAR8); i++)
        L2_GRAPHICS_AsciiCharDraw(WindowLayers.item[GRAPHICS_LAYER_DESK].pBuffer, x + i * 8, y, AsciiBuffer[i], Color);
}




/****************************************************************************
*
*  ����:   ��ʾ����Ļ��ʼ������U����ߵ����汳��ͼƬ���ҵĵ��ԡ�����վ��ϵͳ���á��ļ����ļ���ͼ�궼��ȡ�����湩������ʾ��
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_GRAPHICS_ScreenInit()
{
    EFI_STATUS Status = 0;
    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FAT32\n",  __LINE__);

	L3_APPLICATION_FileReadWithPath("/OS/resource/zhufeng.bmp", pDeskWallpaperBuffer);
	L3_APPLICATION_FileReadWithPath("/OS/resource/computer.bmp", pSystemIconBuffer[SYSTEM_ICON_MYCOMPUTER]);
	L3_APPLICATION_FileReadWithPath("/OS/resource/setting.bmp", pSystemIconBuffer[SYSTEM_ICON_SETTING]);
	L3_APPLICATION_FileReadWithPath("/OS/resource/recycle.bmp", pSystemIconBuffer[SYSTEM_ICON_RECYCLE]);
	L3_APPLICATION_FileReadWithPath("/OS/resource/folder.bmp", pSystemIconBuffer[SYSTEM_ICON_FOLDER]);
	L3_APPLICATION_FileReadWithPath("/OS/resource/text.bmp", pSystemIconBuffer[SYSTEM_ICON_TEXT]);
	L3_APPLICATION_FileReadWithPath("/OS/resource/shutdown.bmp", pSystemIconBuffer[SYSTEM_ICON_SHUTDOWN]);        
    
	L3_APPLICATION_FileReadWithPath("/OS/HZK16", sChineseChar);
	L3_APPLICATION_FileReadWithPath("/OS/HZK12", sChineseChar12);
    
    L2_GRAPHICS_DeskInit();

    // ��ʼ�������ʾ����
    L2_GRAPHICS_ChineseCharDraw(pMouseBuffer, 0, 0, (12 - 1) * 94 + 84 - 1, MouseColor, 16);
    //L2_DEBUG_Print1(DISPLAY_X, DISPLAY_Y, "%d: GraphicsLayerCompute\n", __LINE__);

	//�������ʼ���õĻ�����ʾ����
	L2_SCREEN_Draw(pDeskBuffer, 0, 0, 0, 0, ScreenWidth, ScreenHeight);	   

    // Desk graphics layer, buffer can not free!!
    //FreePool(pDeskBuffer);
    
    return EFI_SUCCESS;
}




/****************************************************************************
*
*  ����:   ��ʼ�˵����������
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_GRAPHICS_StartMenuInit()
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    
    int x = 3, y = 6;
    Color.Red   = 0xff;
    Color.Green = 0x00;
    Color.Blue  = 0x00;
    Color.Reserved  = GRAPHICS_LAYER_START_MENU;

    //��ߵ�������Ҫ��START_MENU_BUTTON_SEQUENCE���ö�ٶ����һ��
    //�ҵĵ���
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (46 - 1 ) * 94 + 50 - 1, Color, StartMenuWidth);    
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (21 - 1) * 94 + 36 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (21 - 1) * 94 + 71 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (36 - 1) * 94 + 52 - 1, Color, StartMenuWidth);   
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw16(pStartMenuBuffer, x , y,     12, 84, Color, StartMenuWidth);   

    //ϵͳ����
    x = 3;
    y += 16;
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (47 - 1 ) * 94 + 21 - 1, Color, StartMenuWidth);    
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (45 - 1) * 94 + 19 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (41 - 1) * 94 + 72 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (54 - 1) * 94 + 35 - 1, Color, StartMenuWidth);   
    

    //�ڴ�鿴
    //����    ��λ�� ����  ��λ�� ����  ��λ�� ����  ��λ��
    //�� 3658    ��       2070    ��       1873    ��       3120
    x = 3;
    y += 16;
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (36 - 1 ) * 94 + 58 - 1, Color, StartMenuWidth);    
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (20 - 1) * 94 + 70 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (18 - 1) * 94 + 73 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (31 - 1) * 94 + 20 - 1, Color, StartMenuWidth);   


    //ϵͳ��־
    //����	��λ��	����	��λ��	����	��λ��	����	��λ��
    //ϵ	4721	ͳ	4519	��	4053	־	5430
    x = 3;
    y += 16;
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (47 - 1 ) * 94 + 21 - 1, Color, StartMenuWidth);    
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (45 - 1) * 94 + 19 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (40 - 1) * 94 + 53 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (54 - 1) * 94 + 30 - 1, Color, StartMenuWidth);  

	int ChineseChars[1][4] = 
	{
		{L'终', L'端', L'窗', L'口'},
	};


	
    //ϵͳ��־
    //����	��λ��	����	��λ��	����	��λ��	����	��λ��
    //ϵ	4721	ͳ	4519	��	4053	־	5430
    x = 3;
    y += 16;
    
	L3_WINDOW_ChineseCharsDraw(pStartMenuBuffer, ChineseChars[0], sizeof(ChineseChars[0])/sizeof(int), 12, &x , y, Color, StartMenuWidth);
	
    //ϵͳ�˳�
    //�� 4543    ��   1986
    x = 3;
    y += 16;
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (47 - 1 ) * 94 + 21 - 1, Color, StartMenuWidth);    
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (45 - 1) * 94 + 19 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (45 - 1) * 94 + 43 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (19 - 1) * 94 + 86 - 1, Color, StartMenuWidth);   


    x = 3;
    y += 16;
    L2_GRAPHICS_ChineseCharDraw12(pStartMenuBuffer, x , y,     47, 21, Color, StartMenuWidth);    
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw12(pStartMenuBuffer, x , y,     45, 19, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw12(pStartMenuBuffer, x , y,     45, 43, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw12(pStartMenuBuffer, x , y,     19, 86, Color, StartMenuWidth);   

    x = 3;
    y += 16;
    L2_GRAPHICS_ChineseCharDraw12(pStartMenuBuffer, x , y,     47, 21, Color, StartMenuWidth);    
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw12(pStartMenuBuffer, x , y,     45, 19, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw12(pStartMenuBuffer, x , y,     45, 43, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw12(pStartMenuBuffer, x , y,     19, 86, Color, StartMenuWidth);   
    
    x = 3;
    y += 16;
    L2_GRAPHICS_ChineseCharDraw12(pStartMenuBuffer, x , y,     47, 21, Color, StartMenuWidth);    
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw12(pStartMenuBuffer, x , y,     45, 19, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw12(pStartMenuBuffer, x , y,     45, 43, Color, StartMenuWidth);
    x += 16;

    UINT8 word[3] = "��";
    
    //L2_GRAPHICS_ChineseCharDraw12(pStartMenuBuffer, x , y,     (word[0] - 0xa0- 1) * 94 + word[1] - 0xa0 - 1, Color, StartMenuWidth);   

	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: word[0]:%d word[1]:%d word[2]:%d \n", __LINE__, word[0], word[1], word[2]);

}





/****************************************************************************
*
*  ����:   ϵͳ���û�������ʼ��
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_GRAPHICS_SystemSettingInit()
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    
    int x = 3, y = 6;
    Color.Red   = 0xff;
    Color.Green = 0x00;
    Color.Blue  = 0x00;
    Color.Reserved  = GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW;

    //��������
    //�� 1719    ��   3016    ��   4172    ��   5435
    L2_GRAPHICS_ChineseCharDraw(pSystemSettingWindowBuffer, x , y,   (17 - 1) * 94 + 19 - 1, Color, SystemSettingWindowWidth);   
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pSystemSettingWindowBuffer, x , y,   (30 - 1) * 94 + 16 - 1, Color, SystemSettingWindowWidth);
    x += 16;
        
    L2_GRAPHICS_ChineseCharDraw(pSystemSettingWindowBuffer, x , y,   (41 - 1) * 94 + 72 - 1, Color, SystemSettingWindowWidth); 
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pSystemSettingWindowBuffer, x , y,   (54 - 1) * 94 + 35 - 1, Color, SystemSettingWindowWidth);
    x += 16;

    //������ԭ
    //�� 2725    ԭ   5213
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




/****************************************************************************
*
*  ����:   �ػ���Ļ
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_GRAPHICS_SystemLogBufferClear()
{
	for (UINT32 i = 23; i < SystemLogWindowHeight - 3; i++)
	{
		for (UINT32 j = 3; j < SystemLogWindowWidth - 3; j++)
		{
			pSystemLogWindowBuffer[4 * (i * SystemLogWindowWidth + j)] = 0;
			pSystemLogWindowBuffer[4 * (i * SystemLogWindowWidth + j) + 1] = 0;
			pSystemLogWindowBuffer[4 * (i * SystemLogWindowWidth + j) + 2] = 0;
		}
	}	

	//��ʼ���󣬴ӵ�1�п�ʼ��ʾ
	LogStatusErrorCount = 0;

}


/****************************************************************************
*
*  ����:   �ػ���Ļ
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
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
    
    // ��    5257    ��   2891    ��   0312    ��   2722    ӭ   5113
    // ��    4734    ��   2046    ��   2756    ��   3220
    // menu chinese
    UINT16 x = ScreenWidth / 2;
    UINT16 y = ScreenHeight / 2;
    
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    Color.Red   = 0xff;
    Color.Green = 0xff;
    Color.Blue  = 0xff;
    Color.Reserved  = GRAPHICS_LAYER_DESK;
    
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

	L2_SCREEN_Draw(pDeskBuffer, 0, 0, 0, 0, ScreenWidth, ScreenHeight);	  
}




/****************************************************************************
*
*  ����:   �жϵ�ǰ��������Ƿ��ھ���������
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
BOOLEAN L1_GRAPHICS_InsideRectangle(UINT16 StartX, UINT16 EndX, UINT16 StartY, UINT16 EndY)
{
    //����Ƿ���ָ������
    BOOLEAN bInArea = iMouseX >= StartX && iMouseX <= EndX && iMouseY >= StartY && iMouseY <= EndY;

    if (!bInArea)
        return bInArea;
    
    //���ͼ��Ϊ��ʾ״̬������������¼��������������ֵ
    if (TRUE == WindowLayers.item[LayerID].DisplayFlag && bInArea)
    {        
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: StartX: %d, EndX: %d, StartY: %d, EndY: %d\n", __LINE__, StartX, EndX, StartY, EndY);
        MouseMoveoverObject.StartX = StartX;
        MouseMoveoverObject.EndX   = EndX;
        MouseMoveoverObject.StartY = StartY;
        MouseMoveoverObject.EndY   = EndY;
        MouseMoveoverObject.GraphicsLayerID = LayerID;
        
        bMouseMoveoverObject = TRUE;
    }

    //��Ϊÿһ��ͼ���ÿһ���¼�������Ψһ�ģ�����ֻ��Ҫ�ж�����������ȾͿ���
    if (MouseMoveoverObjectOld.StartX != MouseMoveoverObject.StartX 
       && MouseMoveoverObjectOld.StartY != MouseMoveoverObject.StartY
       && MouseMoveoverObjectOld.GraphicsLayerID != MouseMoveoverObject.GraphicsLayerID)
    {    
        MouseMoveoverObjectDrawColor.Red = 0xff;
        MouseMoveoverObjectDrawColor.Blue = 0;                

        MouseMoveoverObjectOld.StartX = MouseMoveoverObject.StartX;
        MouseMoveoverObjectOld.StartY = MouseMoveoverObject.StartY;
        MouseMoveoverObjectOld.EndX   = MouseMoveoverObject.EndX;
        MouseMoveoverObjectOld.EndY   = MouseMoveoverObject.EndY;
        MouseMoveoverObjectOld.GraphicsLayerID = MouseMoveoverObject.GraphicsLayerID;        
    }
    else
    {   
        MouseMoveoverObjectDrawColor.Red = 0;
        MouseMoveoverObjectDrawColor.Blue = 0xff;
    }

	return bInArea;
}





/****************************************************************************
*
*  ����:   ����ͼ�����¼�����ǰֻ�����½ǵĿ�ʼ�˵�����¼�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
DESKTOP_ITEM_CLICKED_EVENT L2_GRAPHICS_DeskLayerClickEventGet()
{
	//L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_GRAPHICS_DeskLayerClickEventGet\n", __LINE__);

	if (L1_GRAPHICS_InsideRectangle(0, 16 + 16 * 2, ScreenHeight - 21, ScreenHeight))
	{	    
		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d:1 DESKTOP_ITEM_START_MENU_CLICKED_EVENT\n", __LINE__);
        return DESKTOP_ITEM_START_MENU_CLICKED_EVENT;
	}

	
    UINT16 ItemHeight = SYSTEM_ICON_HEIGHT / 4;
    UINT16 ItemWidth = SYSTEM_ICON_WIDTH / 4;
    UINT16 x1 = 20;
    UINT16 y1 = 20;

    UINT16 ystep = ItemHeight + 2 * 16;

    //�����Ҫע�⣬��һ��i��Ϊ0����1
    for (UINT16 i = DESKTOP_ITEM_MY_COMPUTER_CLICKED_EVENT; i < DESKTOP_ITEM_MAX_CLICKED_EVENT; i++)
    {
    	if (L1_GRAPHICS_InsideRectangle(x1, x1 + ItemWidth, y1 + (i - 2) * ystep, y1 + (i - 2) * ystep + ItemHeight))
    	{	    
    		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d:2 DESKTOP_ITEM_MY_COMPUTER_CLICKED_EVENT\n", __LINE__);
            return i;
    	}    	
    }
    
	y1 += ItemHeight + 2 * 16;
    L2_GRAPHICS_MouseMoveoverObjectSetZero();

	return DESKTOP_ITEM_MAX_CLICKED_EVENT;    
}




/****************************************************************************
*
*  ����:   ����ͼ�����¼�����ǰֻ�����½ǵĿ�ʼ�˵�����¼�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
DESKTOP_ITEM_CLICKED_EVENT L2_GRAPHICS_MouseRightButtonClickEventGet()
{	
    UINT16 x = 10 + WindowLayers.item[GRAPHICS_LAYER_MOUSE_RIGHT_CLICK_WINDOW].StartX;
    UINT16 y = 10 + WindowLayers.item[GRAPHICS_LAYER_MOUSE_RIGHT_CLICK_WINDOW].StartY;

    for (UINT16 i = 0; i < MOUSE_RIGHT_MENU_MAX_CLICKED_EVENT; i++)
    {
    	if (L1_GRAPHICS_InsideRectangle(x, x + 4 * 16, y + i * 16, y + (i + 1) * 16))
    	{	    
    		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d:2 L2_GRAPHICS_MouseRightButtonClickEventGet\n", __LINE__);
            return i;
    	}    	
    }
    
    L2_GRAPHICS_MouseMoveoverObjectSetZero();

	return MOUSE_RIGHT_MENU_MAX_CLICKED_EVENT;    
}



/****************************************************************************
*
*  ����:   ��ʼ�˵�ͼ�����¼���ȡ
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
START_MENU_ITEM_CLICKED_EVENT L2_GRAPHICS_StartMenuLayerClickEventGet()
{
	//L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_GRAPHICS_StartMenuLayerClickEventGet\n", __LINE__);

    UINT16 StartMenuPositionX = WindowLayers.item[GRAPHICS_LAYER_START_MENU].StartX;
    UINT16 StartMenuPositionY = WindowLayers.item[GRAPHICS_LAYER_START_MENU].StartY;

    //�����ʼ�˵�û��û����ʾ����ص��˵���ʼ״̬
    if (FALSE == WindowLayers.item[GRAPHICS_LAYER_START_MENU].DisplayFlag)
    {
        return START_MENU_ITEM_MAX_CLICKED_EVENT;
    }

    // Display my computer window
    if (L1_GRAPHICS_InsideRectangle(3 + StartMenuPositionX, 3 + 4 * 16  + StartMenuPositionX, 
                                   3 + StartMenuPositionY + 16 * START_MENU_BUTTON_MY_COMPUTER, 3 + StartMenuPositionY + 16 * (START_MENU_BUTTON_MY_COMPUTER + 1)))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d:5 MY_COMPUTER_CLICKED_EVENT\n", __LINE__);
        return START_MENU_ITEM_MY_COMPUTER_CLICKED_EVENT;
    }
    
    // Display Setting window
    if (L1_GRAPHICS_InsideRectangle(3 + StartMenuPositionX, 3 + 4 * 16  + StartMenuPositionX, 
                                   3 + StartMenuPositionY + 16 * START_MENU_BUTTON_SYSTEM_SETTING, 3 + StartMenuPositionY + 16 * (START_MENU_BUTTON_SYSTEM_SETTING + 1)))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d:6 SETTING_CLICKED_EVENT\n", __LINE__);
        return START_MENU_ITEM_SYSTEM_SETTING_CLICKED_EVENT;
    }

    // Display Memory Information window
    if (L1_GRAPHICS_InsideRectangle(3 + StartMenuPositionX, 3 + 4 * 16  + StartMenuPositionX, 
                                    3 + StartMenuPositionY + 16 * START_MENU_BUTTON_MEMORY_INFORMATION, 3 + StartMenuPositionY + 16 * (START_MENU_BUTTON_MEMORY_INFORMATION + 1)))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d:7 MEMORY_INFORMATION_CLICKED_EVENT\n", __LINE__);
        return START_MENU_ITEM_MEMORY_INFORMATION_CLICKED_EVENT;
    }
    
    // System quit button
    if (L1_GRAPHICS_InsideRectangle(3 + StartMenuPositionX, 3 + 4 * 16  + StartMenuPositionX, 
                                    3 + StartMenuPositionY + 16 * START_MENU_BUTTON_SYSTEM_LOG, 3 + StartMenuPositionY + 16 * (START_MENU_BUTTON_SYSTEM_LOG + 1)))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d:8 SYSTEM_LOG_CLICKED_EVENT\n", __LINE__);
        return START_MENU_ITEM_SYSTEM_LOG_CLICKED_EVENT;
    }

    // System quit button
    if (L1_GRAPHICS_InsideRectangle(3 + StartMenuPositionX, 3 + 4 * 16  + StartMenuPositionX, 
                                    3 + StartMenuPositionY + 16 * START_MENU_BUTTON_TERMINAL, 3 + StartMenuPositionY + 16 * (START_MENU_BUTTON_TERMINAL + 1)))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d:8 START_MENU_ITEM_TERMINAL_CLICKED_EVENT\n", __LINE__);
        return START_MENU_ITEM_TERMINAL_CLICKED_EVENT;
    }

    // System quit button
    if (L1_GRAPHICS_InsideRectangle(3 + StartMenuPositionX, 3 + 4 * 16  + StartMenuPositionX, 
                                    3 + StartMenuPositionY + 16 * START_MENU_BUTTON_SYSTEM_QUIT, 3 + StartMenuPositionY + 16 * (START_MENU_BUTTON_SYSTEM_QUIT + 1)))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d:9 SYSTEM_QUIT_CLICKED_EVENT\n", __LINE__);
        return START_MENU_ITEM_SHUTDOWN_CLICKED_EVENT;
    }

    L2_GRAPHICS_MouseMoveoverObjectSetZero();

	return START_MENU_ITEM_MAX_CLICKED_EVENT;
}




/****************************************************************************
*
*  ����:   ϵͳ����ͼ�����¼���ȡ
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
START_MENU_SYSTEM_SETTING_SUBITEM_CLICKED_EVENT L2_GRAPHICS_SystemSettingLayerClickEventGet()
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_GRAPHICS_SystemSettingLayerClickEventGet\n", __LINE__);
	
    UINT16 SystemSettingWindowPositionX = WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].StartX;
    UINT16 SystemSettingWindowPositionY = WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].StartY;

    //Wall paper setting
    if (L1_GRAPHICS_InsideRectangle(3 + SystemSettingWindowPositionX, 3 + 4 * 16  + SystemSettingWindowPositionX, 
                                   3 + SystemSettingWindowPositionY, 3 + SystemSettingWindowPositionY + 16))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d:10 WALLPAPER_SETTING_CLICKED_EVENT\n", __LINE__);
        return START_MENU_SYSTEM_SETTING_SUBITEM_WALLPAPER_SETTING_CLICKED_EVENT;
    }
    
    //Wall paper Reset
    if (L1_GRAPHICS_InsideRectangle(3 + SystemSettingWindowPositionX, 3 + 4 * 16  + SystemSettingWindowPositionX, 
                                   3 + SystemSettingWindowPositionY + 16, 3 + SystemSettingWindowPositionY + 16 * 2))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d:11 WALLPAPER_RESET_CLICKED_EVENT\n", __LINE__);
        return START_MENU_SYSTEM_SETTING_SUBITEM_WALLPAPER_RESET_CLICKED_EVENT;
    }

    // Hide Memory Information window
    if (L1_GRAPHICS_InsideRectangle(SystemSettingWindowPositionX + SystemSettingWindowWidth - 20, SystemSettingWindowPositionX + SystemSettingWindowWidth - 4, 
                                   SystemSettingWindowPositionY+ 0, SystemSettingWindowPositionY + 16))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d:12 SYSTEM_SETTING_CLOSE_CLICKED_EVENT\n", __LINE__);
        return START_MENU_SYSTEM_SETTING_SUBITEM_CLOSE_SUBITEM_CLICKED_EVENT;
    }	

    L2_GRAPHICS_MouseMoveoverObjectSetZero();

	return START_MENU_SYSTEM_SETTING_SUBITEM_MAX_CLICKED_EVENT;

}


/****************************************************************************
*
*  ����:   ͨ�������λ�û�ȡ�ҵĵ���ͼ�����¼�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
MY_COMPUTER_WINDOW_CLICKED_EVENT L2_GRAPHICS_TerminalLayerClickEventGet()
{
	//L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_GRAPHICS_TerminalLayerClickEventGet\n", __LINE__);
		
    UINT16 PositionX = WindowLayers.item[GRAPHICS_LAYER_TERMINAL_WINDOW].StartX;
    UINT16 PositionY = WindowLayers.item[GRAPHICS_LAYER_TERMINAL_WINDOW].StartY;
    UINT16 WindowWidth = WindowLayers.item[GRAPHICS_LAYER_TERMINAL_WINDOW].WindowWidth;
    UINT16 WindowHeight = WindowLayers.item[GRAPHICS_LAYER_TERMINAL_WINDOW].WindowHeight;
    
    if (FALSE == WindowLayers.item[GRAPHICS_LAYER_TERMINAL_WINDOW].DisplayFlag)
    {
        return START_MENU_ITEM_INIT_EVENT;
    }
    
    if (L1_GRAPHICS_InsideRectangle(PositionX + WindowWidth - 20, PositionX + WindowWidth - 4, 
                                       PositionY + 0, PositionY + 16))  
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_TERMINAL_WINDOW], "%d: TERMINAL_WINDOW_CLOSE_WINDOW_CLICKED_EVENT\n", __LINE__);
        return TERMINAL_WINDOW_CLOSE_WINDOW_CLICKED_EVENT;
    }

    L2_GRAPHICS_MouseMoveoverObjectSetZero();

	return TERMINAL_WINDOW_MAX_CLICKED_EVENT;
}


/****************************************************************************
*
*  ����:   ͨ�������λ�û�ȡ�ҵĵ���ͼ�����¼�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
MY_COMPUTER_WINDOW_CLICKED_EVENT L2_GRAPHICS_MyComputerLayerClickEventGet()
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_GRAPHICS_MyComputerLayerClickEventGet\n", __LINE__);
		
    UINT16 MyComputerPositionX = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartX;
    UINT16 MyComputerPositionY = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartY;
    UINT16 WindowWidth = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].WindowWidth;
    UINT16 WindowHeight = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].WindowHeight;
    
    if (FALSE == WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].DisplayFlag)
    {
        return START_MENU_ITEM_INIT_EVENT;
    }
    
    if (L1_GRAPHICS_InsideRectangle(MyComputerPositionX + WindowWidth - 20, MyComputerPositionX + MyComputerWidth - 4, 
                                       MyComputerPositionY + 0, MyComputerPositionY + 16))  
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MY_COMPUTER_CLOSE_CLICKED_EVENT\n", __LINE__);
        return MY_COMPUTER_WINDOW_CLOSE_WINDOW_CLICKED_EVENT;
    }

	UINT16 FontSize = 12;
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: StartX: %d StartY: %d Width: %d Height:%d\n", __LINE__,
					MyComputerWindowState.PartitionStartX,
					MyComputerWindowState.PartitionStartY,
					MyComputerWindowState.PartitionWidth,
					MyComputerWindowState.PartitionHeight);
			
	//����������¼�
    for (UINT16 i = 0 ; i < PartitionCount; i++)
    {
        //��Ҫ��L3_APPLICATION_MyComputerWindow���屣��һ��
		UINT16 StartX = MyComputerWindowState.PartitionStartX + WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartX;
		UINT16 StartY = MyComputerWindowState.PartitionStartY + i * 18 + WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartY;
		
		if (L1_GRAPHICS_InsideRectangle(StartX, StartX + MyComputerWindowState.PartitionWidth, 
                                   StartY + 0, StartY + MyComputerWindowState.PartitionHeight))
		{
			PartitionItemID = i;
			L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d:14 MY_COMPUTER_PARTITION_ITEM_CLICKED_EVENT PartitionItemID: %d\n", __LINE__, PartitionItemID);
			return MY_COMPUTER_WINDOW_PARTITION_ITEM_CLICKED_EVENT;
		}
	}

    UINT16 HeightNew = SYSTEM_ICON_HEIGHT / 8;
    UINT16 WidthNew = SYSTEM_ICON_WIDTH / 8;
	
	//Only 6 item, need to fix after test.
	//�������ļ����ļ��б�����¼�
    for (UINT16 i = 0 ; i < 11; i++)
    {
		UINT16 StartX = MyComputerWindowState.ItemStartX + WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartX;
		UINT16 StartY = MyComputerWindowState.ItemStartY + i  * (HeightNew + 16 * 2) + WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartY;
		
		if (L1_GRAPHICS_InsideRectangle(StartX, StartX + WidthNew, 
                                   StartY + 0, StartY + HeightNew))
		{
			FolderItemID = i;
			L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d:15 MY_COMPUTER_PARTITION_ITEM_CLICKED_EVENT FolderItemID: %d\n", __LINE__, FolderItemID);
			return MY_COMPUTER_WINDOW_FOLDER_ITEM_CLICKED_EVENT;
		}
	}

    L2_GRAPHICS_MouseMoveoverObjectSetZero();

	return MY_COMPUTER_WINDOW_MAX_CLICKED_EVENT;
}





/****************************************************************************
*
*  ����:   ϵͳ��־ͼ�����¼���ȡ
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
SYSTEM_LOG_WINDOW_CLICKED_EVENT L2_GRAPHICS_SystemLogLayerClickEventGet()
{	
	//L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_GRAPHICS_SystemLogLayerClickEventGet\n", __LINE__);
    UINT16 SystemLogWindowPositionX = WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].StartX;
    UINT16 SystemLogWindowPositionY = WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].StartY;

    // Hide System Log window
    if (L1_GRAPHICS_InsideRectangle(SystemLogWindowPositionX + SystemLogWindowWidth - 20, SystemLogWindowPositionX + SystemLogWindowWidth - 4, 
                                       SystemLogWindowPositionY+ 0, SystemLogWindowPositionY + 16))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: SYSTEM_LOG_CLOSE_CLICKED_EVENT\n", __LINE__);
        return SYSTEM_LOG_WINDOW_CLOSE_WINDOW_CLICKED_EVENT;
    }    

    L2_GRAPHICS_MouseMoveoverObjectSetZero();

	return SYSTEM_LOG_WINDOW_MAX_CLICKED_EVENT;			
}





/****************************************************************************
*
*  ����:   �ڴ���Ϣ����ͼ�����¼���ȡ
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
MEMORY_INFORMATION_WINDOW_CLICKED_EVENT L2_GRAPHICS_MemoryInformationLayerClickEventGet()
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_GRAPHICS_MemoryInformationLayerClickEventGet\n", __LINE__);

    UINT16 MemoryInformationWindowPositionX = WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].StartX;
    UINT16 MemoryInformationWindowPositionY = WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].StartY;

    if (L1_GRAPHICS_InsideRectangle(MemoryInformationWindowPositionX + MemoryInformationWindowWidth - 20, MemoryInformationWindowPositionX + MemoryInformationWindowWidth - 4, 
                                   MemoryInformationWindowPositionY+ 0, MemoryInformationWindowPositionY + 16))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MEMORY_INFORMATION_CLOSE_CLICKED_EVENT\n", __LINE__);
        return MEMORY_INFORMATION_WINDOW_CLOSE_WINDOW_CLICKED_EVENT;
    }
			
    L2_GRAPHICS_MouseMoveoverObjectSetZero();
	return MEMORY_INFORMATION_WINDOW_MAX_CLICKED_EVENT;
}




/****************************************************************************
*
*  ����:   ��ʼ�˵��������������
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L3_GRAPHICS_StartMenuClicked()
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L3_GRAPHICS_StartMenuClicked\n", __LINE__);
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    Color.Red   = 0xff;
    Color.Green = 0x00;
    Color.Blue   = 0x00;
    Color.Reserved  = GRAPHICS_LAYER_START_MENU;
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: iMouseX: %d iMouseY: %d \n",  __LINE__, iMouseX, iMouseY);
    //L2_GRAPHICS_RectangleDraw(pMouseSelectedBuffer, 0,  0, 31, 15, 1,  Color, 32);
    //MenuButtonClickResponse();
    Color.Red   = 0xFF;
    Color.Green = 0xFF;
    Color.Blue  = 0xFF;
    //L1_MEMORY_RectangleFill(pDeskBuffer, 3,     ScreenHeight - 21, 13,     ScreenHeight - 11, 1, Color);
    L2_GRAPHICS_Copy(pDeskDisplayBuffer, pMouseSelectedBuffer, ScreenWidth, ScreenHeight, 32, 16, 3, ScreenHeight - 21);
        
    //L2_GRAPHICS_Copy(pDeskDisplayBuffer, pStartMenuBuffer, ScreenWidth, ScreenHeight, StartMenuWidth, StartMenuHeight, StartMenuPositionX, StartMenuPositionY);

	//Update state
    StartMenuNextState = START_MENU_ITEM_INIT_EVENT;

    if (FALSE == WindowLayers.item[GRAPHICS_LAYER_START_MENU].DisplayFlag)
    {
        WindowLayers.ActiveWindowCount++;
        WindowLayers.item[GRAPHICS_LAYER_START_MENU].DisplayFlag = TRUE;        
    }
    L1_GRAPHICS_UpdateWindowLayer(GRAPHICS_LAYER_START_MENU);

}




/****************************************************************************
*
*  ����:   ����ͼ�㱻���������
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L3_GRAPHICS_DeskClickEventHandle(DESKTOP_ITEM_CLICKED_EVENT event)
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: event: %d\n", __LINE__, event);

	switch(event)
	{
		case DESKTOP_ITEM_START_MENU_CLICKED_EVENT:
			L3_GRAPHICS_StartMenuClicked(); break;

        case DESKTOP_ITEM_MY_COMPUTER_CLICKED_EVENT:
            L2_MOUSE_MyComputerClicked(); break;

        case DESKTOP_ITEM_SYSTEM_SETTING_CLICKED_EVENT:
            L2_MOUSE_SystemSettingClicked(); break;

        case DESKTOP_ITEM_RECYCLE_BIN_CLICKED_EVENT:
            L2_MOUSE_SystemLogClicked(); break;

        case DESKTOP_ITEM_SHUTDOWN_CLICKED_EVENT:
            L2_System_Shutdown(); break;

		//�����ʵ��Ҫע���£���Ҫ���¼���ȡ��״̬����һ�£���ʵ���������¼����ҵĵ��ԡ�ϵͳ���á�����վ��
		default: break;
	}
}





/****************************************************************************
*
*  ����:   ��ʼ�˵��¼�����״̬��
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L3_GRAPHICS_StartMenuClickEventHandle(START_MENU_ITEM_CLICKED_EVENT event)
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
            StartMenuStateTransformTable[i].pAction();
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MouseClickEvent: %d StartMenuNextState: %d\n", __LINE__, event, StartMenuNextState);
            break;
        }   
    }

}





/****************************************************************************
*
*  ����:   xxxxx
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L3_GRAPHICS_SystemSettingClickEventHandle(START_MENU_SYSTEM_SETTING_SUBITEM_CLICKED_EVENT event)
{

	switch(event)
	{
		case START_MENU_SYSTEM_SETTING_SUBITEM_CLOSE_SUBITEM_CLICKED_EVENT:
			L2_MOUSE_SystemSettingCloseClicked(); break;
			
        case START_MENU_SYSTEM_SETTING_SUBITEM_WALLPAPER_SETTING_CLICKED_EVENT:
            L2_MOUSE_WallpaperSettingClicked(); break;
            
        case START_MENU_SYSTEM_SETTING_SUBITEM_WALLPAPER_RESET_CLICKED_EVENT:
            L2_MOUSE_WallpaperResetClicked(); break;
            
		default: break;
	}

}



/****************************************************************************
*
*  ����:   �ҵĵ��Ա��������ʾ�ҵĴ��ڣ����ҽ����ҵĵ��Դ���״̬��
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L3_GRAPHICS_MyComputerClickEventHandle(MY_COMPUTER_WINDOW_CLICKED_EVENT event)
{    
	MyComputerCurrentState;
	
	switch(event)
	{
		case MY_COMPUTER_WINDOW_CLOSE_WINDOW_CLICKED_EVENT:
			L2_MOUSE_MyComputerCloseClicked(); break;

		default: break;
	}
	
    for (int i = 0; i <  sizeof(MyComputerStateTransformTable)/sizeof(MyComputerStateTransformTable[0]); i++ )
    {
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MyComputerStateTransformTable[i].CurrentState: %d\n", __LINE__, MyComputerStateTransformTable[i].CurrentState);
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: i: %d\n", __LINE__, i);
        if (MyComputerStateTransformTable[i].CurrentState == MyComputerNextState 
            && event == MyComputerStateTransformTable[i].event )
        {
            MyComputerNextState = MyComputerStateTransformTable[i].NextState;

            // need to check the return value after function runs..... 
            MyComputerStateTransformTable[i].pAction();
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MouseClickEvent: %d StartMenuNextState: %d\n", __LINE__, event, StartMenuNextState);
            break;
        }   
    }

}


/****************************************************************************
*
*  ����:   �ҵĵ��Ա��������ʾ�ҵĴ��ڣ����ҽ����ҵĵ��Դ���״̬��
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L3_GRAPHICS_MyComupterClickEventHandle(MY_COMPUTER_WINDOW_CLICKED_EVENT event)
{    	
	switch(event)
	{
		case MY_COMPUTER_WINDOW_CLOSE_WINDOW_CLICKED_EVENT:
			L2_MOUSE_MyComputerCloseClicked(); break;

		default: break;
	}
	
    for (int i = 0; i <  sizeof(MyComputerStateTransformTable)/sizeof(MyComputerStateTransformTable[0]); i++ )
    {
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MyComputerStateTransformTable[i].CurrentState: %d\n", __LINE__, MyComputerStateTransformTable[i].CurrentState);
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: i: %d\n", __LINE__, i);
        if (MyComputerStateTransformTable[i].CurrentState == MyComputerNextState 
            && event == MyComputerStateTransformTable[i].event )
        {
            MyComputerNextState = MyComputerStateTransformTable[i].NextState;

            // need to check the return value after function runs..... 
            MyComputerStateTransformTable[i].pAction();
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MouseClickEvent: %d StartMenuNextState: %d\n", __LINE__, event, StartMenuNextState);
            break;
        }   
    }

}


/****************************************************************************
*
*  ����:   �ҵĵ��Ա��������ʾ�ҵĴ��ڣ����ҽ����ҵĵ��Դ���״̬��
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L3_GRAPHICS_TerminalClickEventHandle(MY_COMPUTER_WINDOW_CLICKED_EVENT event)
{    	
	switch(event)
	{
		case TERMINAL_WINDOW_CLOSE_WINDOW_CLICKED_EVENT:
			L2_MOUSE_TerminalWindowCloseClicked(); break;

		default: break;
	}
}




/****************************************************************************
*
*  ����:   �ҵĵ��Դ��ڵ���¼��йرմ����¼���Ӧ��ע��δ�����ڴ��ͷţ����Ż��Ŀռ�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_MOUSE_SystemSettingCloseClicked()
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_SystemSettingCloseClicked\n", __LINE__);

    if (TRUE == WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].DisplayFlag)
    {
        WindowLayers.ActiveWindowCount--;        
        WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].DisplayFlag = FALSE;
    }
}



/****************************************************************************
*
*  ����:   �ҵĵ��Դ��ڵ���¼��йرմ����¼���Ӧ��ע��δ�����ڴ��ͷţ����Ż��Ŀռ�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_MOUSE_MemoryInformationCloseClicked()
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_SystemLogCloseClicked\n", __LINE__);

    if (TRUE == WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].DisplayFlag)
    {
        WindowLayers.ActiveWindowCount--;        
        WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].DisplayFlag = FALSE;
    }
}



/****************************************************************************
*
*  ����:   �ҵĵ��Դ��ڵ���¼��йرմ����¼���Ӧ��ע��δ�����ڴ��ͷţ����Ż��Ŀռ�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_MOUSE_SystemLogCloseClicked()
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_SystemLogCloseClicked\n", __LINE__);
    //DisplayMyComputerFlag = 0;
    //WindowLayers.item[3].DisplayFlag = 0;
    if (TRUE == WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].DisplayFlag)
    {
        WindowLayers.ActiveWindowCount--;        
        WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].DisplayFlag = FALSE;
    }
}


/****************************************************************************
*
*  ����:   xxxxx
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L3_GRAPHICS_SystemLogClickEventHandle(SYSTEM_LOG_WINDOW_CLICKED_EVENT event)
{

	switch(event)
	{
		case SYSTEM_LOG_WINDOW_CLOSE_WINDOW_CLICKED_EVENT:
			L2_MOUSE_SystemLogCloseClicked(); break;

		//�����ʵ��Ҫע���£���Ҫ���¼���ȡ��״̬����һ�£���ʵ���������¼����ҵĵ��ԡ�ϵͳ���á�����վ��
		default: break;
	}

}


/****************************************************************************
*
*  ����:   xxxxx
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L3_GRAPHICS_MouseRightButtonClickEventHandle(MOUSE_RIGHT_MENU_CLICKED_EVENT event)
{    
    UINT8 *pPathName;
    UINT8 *pBuffer;
	switch(event)
	{
		case MOUSE_RIGHT_MENU_OPEN_CLICKED_EVENT:
		    L2_FILE_FAT32_FileOpen();
			L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d MOUSE_RIGHT_MENU_OPEN_CLICKED_EVENT\n", __LINE__); 
			break;

		case MOUSE_RIGHT_MENU_DELETE_CLICKED_EVENT:
		    L2_FILE_FAT32_FileDelete();
			L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d MOUSE_RIGHT_MENU_DELETE_CLICKED_EVENT\n", __LINE__); 
			break;

		case MOUSE_RIGHT_MENU_ADD_CLICKED_EVENT:
		    TestFlag = TRUE;
			UINT8 Buffer[512] = {0};
			L3_APPLICATION_FileReadWithPath("/TEST/2.TEXT", Buffer);    
		    L2_FILE_FAT32_FileAdd();
			L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d MOUSE_RIGHT_MENU_ADD_CLICKED_EVENT\n", __LINE__); 
			break;

		case MOUSE_RIGHT_MENU_MODIFY_CLICKED_EVENT:
		    L2_FILE_FAT32_FileModify();
			L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d MOUSE_RIGHT_MENU_MODIFY_CLICKED_EVENT\n", __LINE__); 
			break;

		default: break;
	}

}



/****************************************************************************
*
*  ����:   xxxxx
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L3_GRAPHICS_MemoryInformationClickEventHandle(MEMORY_INFORMATION_WINDOW_CLICKED_EVENT event)
{    
	switch(event)
	{
		case MEMORY_INFORMATION_WINDOW_CLOSE_WINDOW_CLICKED_EVENT:
			L2_MOUSE_MemoryInformationCloseClicked(); break;

		default: break;
	}

}


/****************************************************************************
*  ���Դ�����������ͼ���¼�������ڣ��ҳ���Ҫ
*  ����:   �������������ͼ�㣬�ҵ�ͼ�����¼��������ݵ���¼��ҵ���Ӧ����Ӧ��������
*  ��һ�У���ͬͼ����
*  �ڶ��У�����ͼ��ID��ȡ��ͬͼ��ĵ���¼�XXXXXClickEventGet����ͬ�����¼���ȡ��
*  �����У�����ͬ�¼�XXXXXClickEventHandle����ͬ�����¼�������ڣ��Ƚ���Ҫ��
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
GRAPHICS_LAYER_EVENT_GET GraphicsLayerEventHandle[] =
{
    {GRAPHICS_LAYER_DESK,       					 L2_GRAPHICS_DeskLayerClickEventGet, 				L3_GRAPHICS_DeskClickEventHandle},
    {GRAPHICS_LAYER_START_MENU,            			 L2_GRAPHICS_StartMenuLayerClickEventGet, 			L3_GRAPHICS_StartMenuClickEventHandle},
    {GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW,           L2_GRAPHICS_SystemSettingLayerClickEventGet, 		L3_GRAPHICS_SystemSettingClickEventHandle},
    {GRAPHICS_LAYER_MY_COMPUTER_WINDOW,            	 L2_GRAPHICS_MyComputerLayerClickEventGet, 			L3_GRAPHICS_MyComupterClickEventHandle},
    {GRAPHICS_LAYER_TERMINAL_WINDOW,            	 L2_GRAPHICS_TerminalLayerClickEventGet, 			L3_GRAPHICS_TerminalClickEventHandle},
    {GRAPHICS_LAYER_SYSTEM_LOG_WINDOW,            	 L2_GRAPHICS_SystemLogLayerClickEventGet, 			L3_GRAPHICS_SystemLogClickEventHandle},
    {GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW,       L2_GRAPHICS_MemoryInformationLayerClickEventGet, 	L3_GRAPHICS_MemoryInformationClickEventHandle},
    {GRAPHICS_LAYER_MOUSE_RIGHT_CLICK_WINDOW,        L2_GRAPHICS_MouseRightButtonClickEventGet, 	    L3_GRAPHICS_MouseRightButtonClickEventHandle},
};

/****************************************************************************
*
*  ����:   ����������¼�����
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_MOUSE_RightClick(UINT16 LayerID, UINT16 event)
{
    WindowLayers.item[GRAPHICS_LAYER_MOUSE_RIGHT_CLICK_WINDOW].StartX = iMouseX;
    WindowLayers.item[GRAPHICS_LAYER_MOUSE_RIGHT_CLICK_WINDOW].StartY = iMouseY;

    WindowLayers.item[GRAPHICS_LAYER_MOUSE_RIGHT_CLICK_WINDOW].DisplayFlag = TRUE;

    MouseClickFlag = MOUSE_EVENT_TYPE_NO_CLICKED;
    
}


/****************************************************************************
*
*  ����:   ����������¼�����
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_MOUSE_LeftClick(UINT16 LayerID, UINT16 event)
{
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: iMouseX: %d iMouseY: %d \n",  __LINE__, iMouseX, iMouseY);
    //����϶������ƶ�
    
    if ( MouseClickFlag != MOUSE_EVENT_TYPE_LEFT_CLICKED)
    {
        return;
    }

    //�����������ͼ�㣬���ƶ�����������Ҳ��Щ���⣬ͼ����߰���ϵͳ����ͼ�㣬��������£������ǲ˵���ɲ��֣���Ӧ�ÿ����ƶ�
    if (GRAPHICS_LAYER_DESK != LayerID)
    {
        WindowLayers.item[LayerID].StartX += x_move * 3;
        WindowLayers.item[LayerID].StartY += y_move * 3;

        if (WindowLayers.item[LayerID].StartX >= ScreenWidth )
            WindowLayers.item[LayerID].StartX = ScreenWidth / 2;

        //���������Ļ����Ѵ���Ų����
        if (WindowLayers.item[LayerID].StartY >= ScreenHeight )
            WindowLayers.item[LayerID].StartY =  2;
            
        L1_GRAPHICS_UpdateWindowLayer(LayerID);
    }
    
    x_move = 0;
    y_move = 0;

	//L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: iMouseX: %d iMouseY: %d ClickFlag: %d, LayerID: %d\n", __LINE__, iMouseX, iMouseY, MouseClickFlag, LayerID);
      	
    MouseClickFlag = MOUSE_EVENT_TYPE_NO_CLICKED;

	// Handle click event
	GraphicsLayerEventHandle[LayerID].pClickEventHandle(event);
	
}






/****************************************************************************
*
*  ����:   ���ư�ť
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_GRAPHICS_ButtonDraw()
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    UINT32 x = ScreenWidth;
    UINT32 y = ScreenHeight;
    
    // Button
    // white
    Color.Red   = 0xFF;
    Color.Green = 0xFF;
    Color.Blue  = 0xFF;
    Color.Reserved  = GRAPHICS_LAYER_DESK;
    
    L1_MEMORY_RectangleFill(pDeskBuffer, 3,  y - 24, 59, y - 24, ScreenWidth, Color); //line top (3,  y - 24) (59, y - 24)
    L1_MEMORY_RectangleFill(pDeskBuffer, 2,  y - 24, 59, y - 4,  ScreenWidth, Color); //area center(2,  y - 24) (59, y - 4)

    Color.Red   = 0x84;
    Color.Green = 0x84;
    Color.Blue  = 0x84;
    L1_MEMORY_RectangleFill(pDeskBuffer, 3,  y -  4, 59, y -  4, ScreenWidth, Color); // line button (3,  y -  4) (59, y -  4)
    L1_MEMORY_RectangleFill(pDeskBuffer, 59, y - 23, 59, y -  5, ScreenWidth, Color); // line right(59, y - 23) (59, y -  5)

    // Black
    Color.Red   = 0x00;
    Color.Green = 0x00;
    Color.Blue  = 0x00;
    L1_MEMORY_RectangleFill(pDeskBuffer, 2,  y -  3, 59, y -  3, ScreenWidth, Color); // line button(2,  y -  3) (59, y -  3)
    L1_MEMORY_RectangleFill(pDeskBuffer, 60, y - 24, 60, y -  3, ScreenWidth, Color); // line right(60, y - 24) (60, y -  3)
}


//                                                 16 * 4,        16 * 7,       16 * 4,        16 * 2


/****************************************************************************
*
*  ����:   �����ʼ�������Ƚϴֲ�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_GRAPHICS_DeskInit()
{
    
    EFI_STATUS status = 0;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    UINT32 x = ScreenWidth;
    UINT32 y = ScreenHeight;

	Color.Reserved = GRAPHICS_LAYER_DESK;

    //��ֽ�ֱ���1920*1080��ÿ�����ص�ռ�����ֽڣ�0x36��ָBMP��ʽͼƬ�ļ���ͷ
    for (int i = 0; i < 1920 * 1080 * 3; i++)
    	pDeskWallpaperBuffer[i] = pDeskWallpaperBuffer[0x36 + i];

    UINT8 *pWallpaperBuffer = pDeskWallpaperBuffer;
    
	if (ScreenHeight != 1080 || ScreenWidth != 1920)
	{
	    //Ĭ���ṩ��BMPͼ��̫����������ʾ֮ǰ��ͼƬ��С����
        L1_GRAPHICS_ZoomImage(pDeskWallpaperZoomedBuffer, ScreenWidth, ScreenHeight, pDeskWallpaperBuffer, 1920, 1080);  
        pWallpaperBuffer = pDeskWallpaperZoomedBuffer;

	}
		    
    for (int i = 0; i < ScreenHeight; i++)
    {
        for (int j = 0; j < ScreenWidth; j++)
        {
            // BMP 3bits, and desk buffer 4bits
            pDeskBuffer[(i * ScreenWidth + j) * 4]     = pWallpaperBuffer[((ScreenHeight - i) * ScreenWidth + j) * 3 ];
            pDeskBuffer[(i * ScreenWidth + j) * 4 + 1] = pWallpaperBuffer[((ScreenHeight - i) * ScreenWidth + j) * 3 + 1];
            pDeskBuffer[(i * ScreenWidth + j) * 4 + 2] = pWallpaperBuffer[((ScreenHeight - i) * ScreenWidth + j) * 3 + 2];
            pDeskBuffer[(i * ScreenWidth + j) * 4 + 3] = GRAPHICS_LAYER_DESK;

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

    UINT16 icon_chinese_name[SYSTEM_ICON_MAX][8] = 
    {
        {46,50,21,36,21,71,36,52}, //�ҵĵ���
        {47,21,45,19,41,72,54,35}, //ϵͳ����
        {27,56,42,53,53,30,0,0},//����վ
        {46,36,28,94,28,48,0,0},//��	����
        {46,36,28,94,0,0,0,0},//�ļ�
        {25,56,27,90,0,0,0,0},//�ػ�
    };
    
    Color.Blue  = 0xff;
    Color.Red   = 0xff;
    Color.Green = 0xff;
    
    UINT16 x1, y1;
    y1 = 20;
    
    for (UINT32 j = 0; j < SYSTEM_ICON_MAX; j++)
    {
        x1 = 20;
        
        for (UINT32 i = 0; i < 384000; i++)
            pSystemIconTempBuffer2[i] = pSystemIconBuffer[j][0x36 + i];
        
    	//Ĭ���ṩ��BMPͼ��̫����������ʾ֮ǰ��ͼƬ��С����
        L1_GRAPHICS_ZoomImage(pSystemIconMyComputerBuffer, WidthNew, HeightNew, pSystemIconTempBuffer2, SYSTEM_ICON_WIDTH, SYSTEM_ICON_HEIGHT);

	    //��������ʾ�ҵĵ���ͼ��
        L3_GRAPHICS_ItemPrint(pDeskBuffer, pSystemIconMyComputerBuffer, ScreenWidth, ScreenHeight, WidthNew, HeightNew, x1, y1, "", 1, GRAPHICS_LAYER_DESK);

        
        y1 += HeightNew;

        for (UINT16 i = 0; i < 4; i++)
        {
            UINT16 AreaCode = icon_chinese_name[j][2 * i];
            UINT16 BitCode = icon_chinese_name[j][2 * i + 1];
            
            if (0 != AreaCode && 0 != BitCode)
                L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x1, y1, (AreaCode - 1) * 94 + BitCode - 1, Color, ScreenWidth);
                
            x1 += 16;
        }        

        y1 += 16;
        y1 += 16;
    }

    // line
    Color.Red   = 0xC6;
    Color.Green = 0xC6;
    Color.Blue  = 0xC6;
    L1_MEMORY_RectangleFill(pDeskBuffer, 0,     y - 28, x -  1, y - 28, ScreenWidth, Color); // area top

    // line
    Color.Red   = 0xFF;
    Color.Green = 0xFF;
    Color.Blue  = 0xFF;
    L1_MEMORY_RectangleFill(pDeskBuffer, 0,     y - 27, x -  1, y - 27, ScreenWidth, Color); // line top

    // rectangle
    Color.Red   = 0xC6;
    Color.Green = 0xC6;
    Color.Blue  = 0xC6;
    L1_MEMORY_RectangleFill(pDeskBuffer, 0,     y - 26, x -  1, y -  1, ScreenWidth, Color); // area task bar

    // Menu Button
    L2_GRAPHICS_ButtonDraw();

    L2_GRAPHICS_ButtonDraw2(&WindowLayers.item[GRAPHICS_LAYER_DESK], 16 * 6, ScreenHeight - 22, 16 * 4, 16);

    L2_GRAPHICS_ButtonDraw2(&WindowLayers.item[GRAPHICS_LAYER_DESK], 16 * 11, ScreenHeight - 22, 16 * 4, 16);

    L2_GRAPHICS_ButtonDraw2(&WindowLayers.item[GRAPHICS_LAYER_DESK], 16 * 16, ScreenHeight - 22, 16 * 4, 16);

    Color.Red   = 0x84;
    Color.Green = 0x84;
    Color.Blue  = 0x84;
    L1_MEMORY_RectangleFill(pDeskBuffer, DISPLAY_DESK_DATE_TIME_X, y - 24, x -  4, y - 24, ScreenWidth, Color); // line
    //L1_MEMORY_RectangleFill(pDeskBuffer, DISPLAY_DESK_DATE_TIME_X, y - 23, x - 47, y -  4, 1, Color); // area
    
    Color.Red   = 0xFF;
    Color.Green = 0xFF;
    Color.Blue  = 0xFF;
    L1_MEMORY_RectangleFill(pDeskBuffer, DISPLAY_DESK_DATE_TIME_X,    y - 3, x - 4,     y - 3, ScreenWidth, Color); // line
    L1_MEMORY_RectangleFill(pDeskBuffer, x - 3,     y - 24, x - 3,     y - 3, ScreenWidth, Color); //line

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

    x1 = ScreenWidth / 2;
    y1 = ScreenHeight - 21;
    
    //���������½���ʾ���ļ��������죬���͡�����������Ϊ��ϵͳʵ�ֵĹ����У��������������⣬ϣ���Լ��ܼ��������
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer,  x1, y1, (40 - 1) * 94 + 46 - 1, Color, ScreenWidth);
    x1 += 16;    
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer,  x1, y1, (38 - 1) * 94 + 84 - 1, Color, ScreenWidth);
    x1 += 16;
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer,  x1, y1, (26 - 1) * 94 + 76 - 1, Color, ScreenWidth);
    x1 += 16;
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer,  x1, y1, (3 - 1) * 94 + 12 - 1, Color, ScreenWidth);
    x1 += 16;
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer,  x1, y1, (28 - 1) * 94 + 51 - 1, Color, ScreenWidth);
    x1 += 16;
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer,  x1, y1, (51 - 1) * 94 + 45 - 1, Color, ScreenWidth);
    x1 += 16;
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer,  x1, y1, (1 - 1) * 94 + 3 - 1, Color, ScreenWidth);
    x1 += 16;


    //���������½���ʾ���ļ����˵�������
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer,  16, ScreenHeight - 21,     (18 - 1) * 94 + 43 - 1, Color, ScreenWidth);
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer,  16 * 2, ScreenHeight - 21, (21 - 1) * 94 + 05 - 1, Color, ScreenWidth);
}






/****************************************************************************
*
*  ����:   xxxxx
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_GRAPHICS_AsciiCharDraw3(UINT8 *pBuffer,
        IN UINTN x0, UINTN y0,UINT8 c,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL BorderColor, UINT16 AreaWidth)
{
    INT8 i;
    UINT8 d;

    if (pBuffer == NULL)
    {
        return;
    }
    
    for(i = 0; i < 16; i++)
    {
        d = sASCII[c][i];
        
        if ((d & 0x80) != 0) 
            L1_MEMORY_CopyColor1(pBuffer, BorderColor, x0 + 0, y0 + i, AreaWidth); 
        
        if ((d & 0x40) != 0) 
            L1_MEMORY_CopyColor1(pBuffer, BorderColor, x0 + 1, y0 + i, AreaWidth);
        
        if ((d & 0x20) != 0) 
            L1_MEMORY_CopyColor1(pBuffer, BorderColor, x0 + 2, y0 + i, AreaWidth);
        
        if ((d & 0x10) != 0) 
            L1_MEMORY_CopyColor1(pBuffer, BorderColor, x0 + 3, y0 + i, AreaWidth);
        
        if ((d & 0x08) != 0) 
            L1_MEMORY_CopyColor1(pBuffer, BorderColor, x0 + 4, y0 + i, AreaWidth);
        
        if ((d & 0x04) != 0) 
            L1_MEMORY_CopyColor1(pBuffer, BorderColor, x0 + 5, y0 + i, AreaWidth);
        
        if ((d & 0x02) != 0) 
            L1_MEMORY_CopyColor1(pBuffer, BorderColor, x0 + 6, y0 + i, AreaWidth);
        
        if ((d & 0x01) != 0) 
            L1_MEMORY_CopyColor1(pBuffer, BorderColor, x0 + 7, y0 + i, AreaWidth);
        
    }
    
    return EFI_SUCCESS;
}


// Draw 8 X 16 point



/****************************************************************************
*
*  ����:   xxxxx
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_GRAPHICS_ChineseHalfDraw(UINT8 *pBuffer,UINT8 d,
        IN UINTN x0, UINTN y0,
        UINT8 width,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color, UINT8 fontWidth)
{
    if ((d & 0x80) != 0) 
        L1_MEMORY_CopyColor3(pBuffer, Color, x0 + 0, y0, fontWidth); 
    
    if ((d & 0x40) != 0) 
        L1_MEMORY_CopyColor3(pBuffer, Color, x0 + 1, y0, fontWidth );
    
    if ((d & 0x20) != 0) 
        L1_MEMORY_CopyColor3(pBuffer, Color, x0 + 2, y0, fontWidth );
    
    if ((d & 0x10) != 0) 
        L1_MEMORY_CopyColor3(pBuffer, Color, x0 + 3, y0, fontWidth );
    
    if ((d & 0x08) != 0) 
        L1_MEMORY_CopyColor3(pBuffer, Color, x0 + 4, y0, fontWidth );
    
    if ((d & 0x04) != 0) 
        L1_MEMORY_CopyColor3(pBuffer, Color, x0 + 5, y0, fontWidth );
    
    if ((d & 0x02) != 0) 
        L1_MEMORY_CopyColor3(pBuffer, Color, x0 + 6, y0, fontWidth );
    
    if ((d & 0x01) != 0) 
        L1_MEMORY_CopyColor3(pBuffer, Color, x0 + 7, y0, fontWidth );


    return EFI_SUCCESS;
}


// Note: Do not copy Color's reserved member



/****************************************************************************
*
*  ����:   ͼ���ص㿽������reserved�ֶΣ�ǰ�����ֶΰ����졢�̡���������ɫ�����һ����ɫ��ͼ��ID����Ϊ�����ͼ�����ϲ㣬���Բ���Ҫͼ��ID
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_GRAPHICS_CopyNoReserved(UINT8 *pDest, UINT8 *pSource, 
                           UINT16 DestWidth, UINT16 DestHeight, 
                           UINT16 SourceWidth, UINT16 SourceHeight, 
                           UINT16 StartX, UINT16 StartY)
{
    int i, j;
    for(i = 0; i < SourceHeight; i++)
    {
        for (j = 0; j < SourceWidth; j++)
        {
            pDest[((StartY + i) * DestWidth + StartX + j) * 4] = pSource[(i * SourceWidth + j) * 4];
            pDest[((StartY + i) * DestWidth + StartX + j) * 4 + 1] = pSource[(i * SourceWidth + j) * 4 + 1];
            pDest[((StartY + i) * DestWidth + StartX + j) * 4 + 2] = pSource[(i * SourceWidth + j) * 4 + 2];
        }
    }
}

VOID L2_GRAPHICS_DrawMouseToDesk()
{
    //Ϊ���������͸������Ҫ��ͼ���Ӧ�����ص㿽���������ʾ�ڴ滺��
    for (UINT8 i = 0; i < 16; i++)
    {
        for (UINT8 j = 0; j < 16; j++)
        {
			pMouseBuffer[(i * 16 + j) * 4]     = pDeskDisplayBuffer[((iMouseY + i) * ScreenWidth + iMouseX + j) * 4 + 0];
			pMouseBuffer[(i * 16 + j) * 4 + 1] = pDeskDisplayBuffer[((iMouseY + i) * ScreenWidth + iMouseX + j) * 4 + 1];
			pMouseBuffer[(i * 16 + j) * 4 + 2] = pDeskDisplayBuffer[((iMouseY + i) * ScreenWidth + iMouseX + j) * 4 + 2];
        }
	}

	//Ȼ����������
    L2_GRAPHICS_ChineseCharDraw16(pMouseBuffer, 0 , 0,     12, 84, MouseColor, 16);  
    //L2_GRAPHICS_ChineseCharDraw(pMouseBuffer, 0, 0, 12 * 94 + 84, MouseColor, 16);

	//���������ʾ������
    L2_GRAPHICS_CopyNoReserved(pDeskDisplayBuffer, pMouseBuffer, ScreenWidth, ScreenHeight, 16, 16, iMouseX, iMouseY);
    //L2_DEBUG_Print1(DISPLAY_X, DISPLAY_Y, "%d: GraphicsLayerCompute\n", __LINE__);
}


VOID L2_GRAPHICS_TrackMouseMoveoverObject()
{
    UINT16 DrawGraphicsLayerID = MouseMoveoverObject.GraphicsLayerID;
    UINT16 DrawWindowWidth = WindowLayers.item[DrawGraphicsLayerID].WindowWidth;
    UINT8 *pDrawBuffer = WindowLayers.item[DrawGraphicsLayerID].pBuffer;
    UINT16 DrawStartX = MouseMoveoverObject.StartX;
    UINT16 DrawWindowStartX = WindowLayers.item[DrawGraphicsLayerID].StartX;
    UINT16 DrawEndX = MouseMoveoverObject.EndX;
    UINT16 DrawStartY = MouseMoveoverObject.StartY;
    UINT16 DrawWindowStartY = WindowLayers.item[DrawGraphicsLayerID].StartY;
    UINT16 DrawEndY = MouseMoveoverObject.EndY;
    
    
    L2_DEBUG_Print1(0, ScreenHeight - 30 - 9 * 16, "%d: MouseMoveoverObject: LayerID: %u StartX: %u EndX: %u StartY: %u EndY: %u\n", __LINE__, 
                              DrawGraphicsLayerID,
                              DrawStartX,
                              DrawEndX, 
                              DrawStartY, 
                              DrawEndY);

    if (bMouseMoveoverObject == FALSE)
    {
        MouseMoveoverObjectDrawColor.Blue = 0xff;
        MouseMoveoverObjectDrawColor.Red = 0;
    }
    else
    {
        MouseMoveoverObjectDrawColor.Blue = 0;
        MouseMoveoverObjectDrawColor.Red = 0xff;
    }    
                              
    //������û�е������׷�������ָ��Ŀ��
    //��Ϊ����ͼ����û���������ͼ����ʼX��Y��������
    if (0 != MouseMoveoverObject.GraphicsLayerID)
    {        
        DrawStartX -= DrawWindowStartX;
        DrawStartY -= DrawWindowStartY;
        DrawEndX -= DrawWindowStartX;
        DrawEndY -= DrawWindowStartY;  
    }
    
    L2_GRAPHICS_RectangleDraw(pDrawBuffer, 
                              DrawStartX,
                              DrawStartY, 
                              DrawEndX, 
                              DrawEndY, 
                              1,  
                              MouseMoveoverObjectDrawColor, 
                              DrawWindowWidth);

    L2_GRAPHICS_RectangleDraw(pDrawBuffer, 
                              DrawStartX + 1,
                              DrawStartY + 1, 
                              DrawEndX - 1, 
                              DrawEndY - 1, 
                              1,  
                              MouseMoveoverObjectDrawColor, 
                              DrawWindowWidth);

}



/****************************************************************************
*
*  ����:   ������ͼ�����������ʾ
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_GRAPHICS_LayerCompute(UINT16 iMouseX, UINT16 iMouseY, UINT8 MouseClickFlag)
{
    //desk 
    L2_GRAPHICS_Copy(pDeskDisplayBuffer, pDeskBuffer, ScreenWidth, ScreenHeight, ScreenWidth, ScreenHeight, 0, 0);

    //�ػ�
    if (TRUE == SystemQuitFlag)
    {    
        //�ػ���ʱ�򣬻���һ��С�Ķ���д�뵽pDeskBuffer
		L2_SCREEN_Draw(pDeskDisplayBuffer, 0, 0, 0, 0, ScreenWidth, ScreenHeight);	
                    
        return;
    }

    //����ͼ�㿽��
    L2_GRAPHICS_CopyBufferFromWindowsToDesk();
	        
    //����һ��˵���ע�⣬��Ҫ������ȡ�¼�ǰ��������Ϊ����һ��¼�Ҳ��Ҫ����ȡ
    if (WindowLayers.item[GRAPHICS_LAYER_MOUSE_RIGHT_CLICK_WINDOW].StartX != 0 || WindowLayers.item[GRAPHICS_LAYER_MOUSE_RIGHT_CLICK_WINDOW].StartY != 0)
    {        
        L2_GRAPHICS_Copy(pDeskDisplayBuffer, pMouseRightButtonClickWindowBuffer, ScreenWidth, ScreenHeight, MouseRightButtonClickWindowWidth, MouseRightButtonClickWindowHeight, WindowLayers.item[GRAPHICS_LAYER_MOUSE_RIGHT_CLICK_WINDOW].StartX, WindowLayers.item[GRAPHICS_LAYER_MOUSE_RIGHT_CLICK_WINDOW].StartY);
    }    

	//��ȡ��ǰ������ڵ�ͼ�㣬������굱ǰ�ĵ���¼����в���
    L2_MOUSE_Move();

    //׷������¼���Ҫ�ڻ������ͼ��ǰ��ɣ���Ϊ����ͼ����޸���ʾ��ͼ��
    L2_GRAPHICS_TrackMouseMoveoverObject();
    
	L2_GRAPHICS_DrawMouseToDesk();

	//��׼���õ����滺������ʾ����Ļ
	L2_SCREEN_Draw(pDeskDisplayBuffer, 0, 0, 0, 0, ScreenWidth, ScreenHeight);	
}




/****************************************************************************
*
*  ����:   ���ļ���Ŀ¼�µ��ļ����ļ�����ʾ���ҵĵ���ͼ�㣬��ǰ����ߺܶ�̶���ֵ����̫��
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_STORE_FolderItemsPrint()
{
    UINT16 valid_count = 0;
    UINT16 HeightNew = SYSTEM_ICON_HEIGHT / 8;
    UINT16 WidthNew = SYSTEM_ICON_WIDTH / 8;
    UINT16 x;
    UINT16 y;
	
	pMyComputerBuffer = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].pBuffer;

	UINT16 StartX = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartX + 130;
	UINT16 StartY = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartY + 6  * (HeightNew + 16 * 2) + 200;

	UINT16 Width = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].WindowWidth;
	UINT16 Height = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].WindowHeight;
	

	// Clear old display items
	for (UINT16 i = 400; i < Height; i++)
    {
        for (UINT16 j = 430; j < Width; j++)
        {
            pMyComputerBuffer[(i * Width + j) * 4]     = 0xff;
            pMyComputerBuffer[(i * Width + j) * 4 + 1] = 0xff;
            pMyComputerBuffer[(i * Width + j) * 4 + 2] = 0xff;
            pMyComputerBuffer[(i * Width + j) * 4 + 3] = GRAPHICS_LAYER_MY_COMPUTER_WINDOW;
        }
    }	
	
	MyComputerWindowState.ItemStartX = MyComputerWindowState.PartitionStartX + 150;
	MyComputerWindowState.ItemStartY = MyComputerWindowState.PartitionStartY;
	MyComputerWindowState.ItemWidth  = WidthNew;
	MyComputerWindowState.ItemHeight = HeightNew;	
	
    for (UINT16 i = 0; i < 32; i++)
    {       
        //�����һλΪ0��ʾ����������
        if (pItems[i].FileName[0] == 0)
            break;
            
        char name[13] = {0};
        x = MyComputerWindowState.ItemStartX;

        y = valid_count * (HeightNew + 16 * 2) + MyComputerWindowState.ItemStartY;

        for (UINT8 i = 0; i < 12; i++)
            name[i] = '\0';
        
        char ItemType[10] = "OTHER";
        L1_FILE_NameGet(i, name);
                
        if (pItems[i].Attribute[0] == 0x10) //Folder
        {
            //��ʾ�ļ���Сͼ��
            L3_GRAPHICS_ItemPrint(pMyComputerBuffer, pSystemIconFolderBuffer, MyComputerWidth, MyComputerHeight, WidthNew, HeightNew, x, y, "111", 2, GRAPHICS_LAYER_MY_COMPUTER_WINDOW);
            
            L2_DEBUG_Print3(x, y + HeightNew, WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW], "%a %d Bytes",
                                            name,
                                            L1_NETWORK_4BytesToUINT32(pItems[i].FileLength));
            FolderItemValidIndexArray[valid_count] = i;
            valid_count++;
        }
        else if (pItems[i].Attribute[0] == 0x20) //File
        {            
            L2_DEBUG_Print3(x, y + HeightNew, WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW], "%a %d Bytes",
                                            name,
                                            L1_NETWORK_4BytesToUINT32(pItems[i].FileLength));
            
            //��ʾ�ļ�Сͼ��
            L3_GRAPHICS_ItemPrint(pMyComputerBuffer, pSystemIconTextBuffer, MyComputerWidth, MyComputerHeight, WidthNew, HeightNew, x, y, "222", 2, GRAPHICS_LAYER_MY_COMPUTER_WINDOW);
			FolderItemValidIndexArray[valid_count] = i;
            valid_count++;
        }
    }       

}


/****************************************************************************
*
*  ����:   ���ļ���Ŀ¼�µ��ļ����ļ�����ʾ���ҵĵ���ͼ�㣬��ǰ����ߺܶ�̶���ֵ����̫��
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_STORE_FolderItemsPrint2()
{
    UINT16 valid_count = 0;
    UINT16 HeightNew = SYSTEM_ICON_HEIGHT / 8;
    UINT16 WidthNew = SYSTEM_ICON_WIDTH / 8;
    UINT16 x;
    UINT16 y;
	
	pMyComputerBuffer = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].pBuffer;

	UINT16 StartX = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartX + 130;
	UINT16 StartY = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartY + 6  * (HeightNew + 16 * 2) + 200;

	UINT16 Width = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].WindowWidth;
	UINT16 Height = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].WindowHeight;

	// Clear old display items
	for (UINT16 i = 200; i < Height; i++)
    {
        for (UINT16 j = 130; j < Width; j++)
        {
            pMyComputerBuffer[(i * Width + j) * 4]     = 0xff;
            pMyComputerBuffer[(i * Width + j) * 4 + 1] = 0xff;
            pMyComputerBuffer[(i * Width + j) * 4 + 2] = 0xff;
            pMyComputerBuffer[(i * Width + j) * 4 + 3] = GRAPHICS_LAYER_MY_COMPUTER_WINDOW;
        }
    }	

	//��Ŀ¼��ʾ������
    for (UINT16 i = 0; i < 7; i++)
    {                   
    	if (pCommonStorageItems[i].Type == COMMON_STORAGE_ITEM_MAX) //over
        {
        	break;
    	}
        x = 130;

        y = valid_count * (HeightNew + 16 * 2) + 200;
                
        if (pCommonStorageItems[i].Type == COMMON_STORAGE_ITEM_FOLDER) //Folder
        {
            //��ʾ�ļ���Сͼ��
            L3_GRAPHICS_ItemPrint(pMyComputerBuffer, pSystemIconFolderBuffer, MyComputerWidth, MyComputerHeight, WidthNew, HeightNew, x, y, "111", 2, GRAPHICS_LAYER_MY_COMPUTER_WINDOW);
            /*
            for (int j = 0; j < HeightNew; j++)
            {
                for (int k = 0; k < WidthNew; k++)
                {
                    pMyComputerBuffer[((valid_count * (HeightNew + 16 * 2) + 200 + j) * MyComputerWidth + 130 + k) * 4 ]     = pSystemIconFolderBuffer[((HeightNew - j) * WidthNew + k) * 3 ];
                    pMyComputerBuffer[((valid_count * (HeightNew + 16 * 2) + 200 + j) * MyComputerWidth + 130 + k) * 4 + 1 ] = pSystemIconFolderBuffer[((HeightNew - j) * WidthNew + k) * 3 + 1 ];
                    pMyComputerBuffer[((valid_count * (HeightNew + 16 * 2) + 200 + j) * MyComputerWidth + 130 + k) * 4 + 2 ] = pSystemIconFolderBuffer[((HeightNew - j) * WidthNew + k) * 3 + 2 ];
                }
            }
            */
            L2_DEBUG_Print3(x, y + HeightNew, WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW], "%a %d Bytes",
                                            pCommonStorageItems[i].Name,
                                            L1_NETWORK_4BytesToUINT32(pCommonStorageItems[i].Size));
            FolderItemValidIndexArray[valid_count] = i;
            valid_count++;
        }
        else if (pCommonStorageItems[i].Type == COMMON_STORAGE_ITEM_FILE) //File
        {
            
            /*for (int j = 0; j < HeightNew; j++)
            {
                for (int k = 0; k < WidthNew; k++)
                {
                    pMyComputerBuffer[((valid_count * (HeightNew + 16 * 2) + 200 + j) * MyComputerWidth + 130 + k) * 4 ]     = pSystemIconTextBuffer[((HeightNew - j) * WidthNew + k) * 3 ];
                    pMyComputerBuffer[((valid_count * (HeightNew + 16 * 2) + 200 + j) * MyComputerWidth + 130 + k) * 4 + 1 ] = pSystemIconTextBuffer[((HeightNew - j) * WidthNew + k) * 3 + 1 ];
                    pMyComputerBuffer[((valid_count * (HeightNew + 16 * 2) + 200 + j) * MyComputerWidth + 130 + k) * 4 + 2 ] = pSystemIconTextBuffer[((HeightNew - j) * WidthNew + k) * 3 + 2 ];
                }
            }*/
            L2_DEBUG_Print3(x, y + HeightNew, WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW], "%a %d Bytes",
                                            pCommonStorageItems[i].Name,
                                            pCommonStorageItems[i].Size);
            
            //��ʾ�ļ�Сͼ��
            L3_GRAPHICS_ItemPrint(pMyComputerBuffer, pSystemIconTextBuffer, MyComputerWidth, MyComputerHeight, WidthNew, HeightNew, x, y, "222", 2, GRAPHICS_LAYER_MY_COMPUTER_WINDOW);
			FolderItemValidIndexArray[valid_count] = i;
            valid_count++;
        }
    }       

}



/****************************************************************************
*
*  ����:   ���ͼ����ӣ�����ͼ��������ϣ����ε�������������������漰��ͼ�㣬Ч�ʻ�Ƚϵף����⣬δ�ж�ͼ��δ�޸ĵĲ��ֲ���Ҫ���µ��ӡ�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
void L2_GRAPHICS_CopyBufferFromWindowsToDesk()
{
    for (UINT16 i = 0; i < WindowLayers.LayerCount; i++)
    {
        UINT16 j = WindowLayers.LayerSequences[i];
        if (TRUE == WindowLayers.item[j].DisplayFlag)
        {
			/*L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], 
							"%d LayerID: %d pBuffer: %X StartX: %d StartY: %d WindowWidth: %d WindowHeight: %d j: %d\n", 
							__LINE__, 
							WindowLayers.item[j].LayerID,
							WindowLayers.item[j].pBuffer,
							WindowLayers.item[j].StartX,
							WindowLayers.item[j].StartY,
							WindowLayers.item[j].WindowWidth,
							WindowLayers.item[j].WindowHeight,
							j);*/
            L2_GRAPHICS_Copy(pDeskDisplayBuffer, WindowLayers.item[j].pBuffer, ScreenWidth, ScreenHeight, WindowLayers.item[j].WindowWidth, WindowLayers.item[j].WindowHeight, WindowLayers.item[j].StartX, WindowLayers.item[j].StartY);
        }
    }
}
							  



/****************************************************************************
*
*  ����:   ��ʾ������Ŀ¼�µ��ļ���Ŀ¼����ǰ֧��FAT32��NTFS���ָ�ʽ
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_STORE_PartitionItemsPrint(UINT16 PartitionItemID)
{
}






/****************************************************************************
*
*  ����:   ���ڴ��������Ascii�ַ�������ʹ�ã�L2_GRAPHICS_AsciiCharDraw2
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_GRAPHICS_AsciiCharDraw(UINT8 *pBufferDest,
        IN UINTN x0, UINTN y0, UINT8 c,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color)
{
    INT16 i;
    UINT8 d;
    UINT8 pBuffer[16 * 8 * 4];

    if ('\0' == c || '\n' == c)
        return;

    for(i = 0; i < 16 * 8 * 4; i++)
    {
        pBuffer[i] = 0x0;
    }
    
    for(i = 0; i < 16; i++)
    {
        d = sASCII[c][i];
        
        if ((d & 0x80) != 0) 
            L1_MEMORY_CopyColor2(pBuffer, Color, 0, i); 
        
        if ((d & 0x40) != 0) 
            L1_MEMORY_CopyColor2(pBuffer, Color, 1, i);
        
        if ((d & 0x20) != 0) 
            L1_MEMORY_CopyColor2(pBuffer, Color, 2, i);
        
        if ((d & 0x10) != 0) 
            L1_MEMORY_CopyColor2(pBuffer, Color, 3, i);
        
        if ((d & 0x08) != 0) 
            L1_MEMORY_CopyColor2(pBuffer, Color, 4, i);
        
        if ((d & 0x04) != 0) 
            L1_MEMORY_CopyColor2(pBuffer, Color, 5, i);
        
        if ((d & 0x02) != 0) 
            L1_MEMORY_CopyColor2(pBuffer, Color, 6, i);
        
        if ((d & 0x01) != 0) 
            L1_MEMORY_CopyColor2(pBuffer, Color, 7, i);
        
    }

    L2_GRAPHICS_Copy(pBufferDest, pBuffer, ScreenWidth, ScreenHeight, 8, 16, x0, y0);

	//ע���±�ע�͵��⼸���ִ��������漰̫��IO��������Ч��Ӱ��ϸߣ�����������
    /*
    GraphicsOutput->Blt(GraphicsOutput, 
                        (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) pBuffer,
                        EfiBltBufferToVideo,
                        0, 0, 
                        x0, y0, 
                        8, 16, 
                        0);   
      */          
    return EFI_SUCCESS;
}




/****************************************************************************
*
*  ����:   ���ڴ��������Ascii�ַ�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_GRAPHICS_AsciiCharDraw2(WINDOW_LAYER_ITEM layer,
        IN UINTN x0, UINTN y0, UINT8 c,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color)
{
    INT16 i;
    UINT8 d;
    UINT8 pBuffer[16 * 8 * 4];

    if (0 == c || 10 == c)
        return;

    for(i = 0; i < 16 * 8 * 4; i++)
    {
        pBuffer[i] = 0;
    }
    
    for(i = 0; i < 16; i++)
    {
        d = sASCII[c][i];
        
        if ((d & 0x80) != 0) 
            L1_MEMORY_CopyColor2(pBuffer, Color, 0, i); 
        
        if ((d & 0x40) != 0) 
            L1_MEMORY_CopyColor2(pBuffer, Color, 1, i);
        
        if ((d & 0x20) != 0) 
            L1_MEMORY_CopyColor2(pBuffer, Color, 2, i);
        
        if ((d & 0x10) != 0) 
            L1_MEMORY_CopyColor2(pBuffer, Color, 3, i);
        
        if ((d & 0x08) != 0) 
            L1_MEMORY_CopyColor2(pBuffer, Color, 4, i);
        
        if ((d & 0x04) != 0) 
            L1_MEMORY_CopyColor2(pBuffer, Color, 5, i);
        
        if ((d & 0x02) != 0) 
            L1_MEMORY_CopyColor2(pBuffer, Color, 6, i);
        
        if ((d & 0x01) != 0) 
            L1_MEMORY_CopyColor2(pBuffer, Color, 7, i);
        
    }

    L2_GRAPHICS_Copy(layer.pBuffer, pBuffer, layer.WindowWidth, layer.WindowWidth, 8, 16, x0, y0);
    
    /*
    GraphicsOutput->Blt(GraphicsOutput, 
                        (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) pBuffer,
                        EfiBltBufferToVideo,
                        0, 0, 
                        x0, y0, 
                        8, 16, 
                        0);   
      */          
    return EFI_SUCCESS;
}

// iMouseX: left top
// iMouseY: left top



/****************************************************************************
*
*  ����:   ����һ���ʾһ��С�Ĵ���
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_GRAPHICS_RightClickMenuInit(UINT16 iMouseX, UINT16 iMouseY, UINT16 LayerID)
{
	INT16 i;	
	UINT16 width = 100;
	UINT16 height = 300;
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;

	Color.Blue = 0xFF;
	Color.Red  = 0xFF;
	Color.Green= 0xFF;
		 
	Color.Reserved = LayerID;
	Color.Blue = 0;
	
    //L2_GRAPHICS_ChineseCharDraw(pMouseClickBuffer, 0 , 0,     (47 - 1 ) * 94 + 21 - 1, Color, MouseClickWindowWidth);  
    
	/*
	GraphicsOutput->Blt(GraphicsOutput, 
						(EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) pMouseClickBuffer,
						EfiBltBufferToVideo,
						0, 0, 
						iMouseX, iMouseY, 
						width, height, 0);	
    */
	//FreePool(pBuffer);
	return ;

}



/****************************************************************************
*
*  ����:   �ڴ濽������ɫ�ڱ�����������Ҳ�����ڴ�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
void L1_MEMORY_CopyColor1(UINT8 *pBuffer, EFI_GRAPHICS_OUTPUT_BLT_PIXEL color, UINT16 x0, UINT16 y0, UINT16 AreaWidth)
{
    pBuffer[y0 * AreaWidth * 4 + x0 * 4]     = color.Blue;
    pBuffer[y0 * AreaWidth * 4 + x0 * 4 + 1] = color.Green;
    pBuffer[y0 * AreaWidth * 4 + x0 * 4 + 2] = color.Red;

    //��Ϊ��ʱ�����δ��ʼ���ڴ沿�ֹ���������ֱ�ӻ�����
    if (color.Reserved < GRAPHICS_LAYER_MOUSE)
        pBuffer[y0 * AreaWidth * 4 + x0 * 4 + 3] = color.Reserved;

}




/****************************************************************************
*
*  ����:   �ڴ濽������ɫ�ڱ�����������Ҳ�����ڴ�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� Ŀ���ڴ������ȹ̶�
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
void L1_MEMORY_CopyColor2(UINT8 *pBuffer, EFI_GRAPHICS_OUTPUT_BLT_PIXEL color, UINT16 x0, UINT16 y0)
{
    pBuffer[y0 * 8 * 4 + x0 * 4]     = color.Blue;
    pBuffer[y0 * 8 * 4 + x0 * 4 + 1] = color.Green;
    pBuffer[y0 * 8 * 4 + x0 * 4 + 2] = color.Red;
    pBuffer[y0 * 8 * 4 + x0 * 4 + 3] = color.Reserved;
}




/****************************************************************************
*
*  ����:   �ڴ濽������ɫ�ڱ�����������Ҳ�����ڴ�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� AreaWidth��Ŀ���ڴ�����Ŀ��
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
void L1_MEMORY_CopyColor3(UINT8 *pBuffer, EFI_GRAPHICS_OUTPUT_BLT_PIXEL color, UINT16 x0, UINT16 y0, UINT8 AreaWidth)
{
    pBuffer[y0 * AreaWidth * 4 + x0 * 4]     = color.Blue;
    pBuffer[y0 * AreaWidth * 4 + x0 * 4 + 1] = color.Green;
    pBuffer[y0 * AreaWidth * 4 + x0 * 4 + 2] = color.Red;
    pBuffer[y0 * AreaWidth * 4 + x0 * 4 + 3] = 0;
}

// Draw 8 X 16 point



/****************************************************************************
*
*  ����:   �����ַ�����
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_GRAPHICS_ChineseHalfDraw2(UINT8 *pBuffer,UINT8 d,
        IN UINTN x0, UINTN y0,
        UINT8 width,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color, UINT16 AreaWidth)
{
    //L2_DEBUG_Print1(10, 10, "%X %X %X %X", x0, y0, AreaWidth);
    if (NULL == pBuffer)
    {
        //DEBUG ((EFI_D_INFO, "pBuffer is NULL\n"));
        return EFI_SUCCESS;
    }
        
    if ((d & 0x80) != 0) 
        L1_MEMORY_CopyColor1(pBuffer, Color, x0 + 0, y0, AreaWidth ); 
    
    if ((d & 0x40) != 0) 
        L1_MEMORY_CopyColor1(pBuffer, Color, x0 + 1, y0, AreaWidth );
    
    if ((d & 0x20) != 0) 
        L1_MEMORY_CopyColor1(pBuffer, Color, x0 + 2, y0, AreaWidth );
    
    if ((d & 0x10) != 0) 
        L1_MEMORY_CopyColor1(pBuffer, Color, x0 + 3, y0, AreaWidth );
    
    if ((d & 0x08) != 0) 
        L1_MEMORY_CopyColor1(pBuffer, Color, x0 + 4, y0, AreaWidth );
    
    if ((d & 0x04) != 0) 
        L1_MEMORY_CopyColor1(pBuffer, Color, x0 + 5, y0, AreaWidth );
    
    if ((d & 0x02) != 0) 
        L1_MEMORY_CopyColor1(pBuffer, Color, x0 + 6, y0, AreaWidth );
    
    if ((d & 0x01) != 0) 
        L1_MEMORY_CopyColor1(pBuffer, Color, x0 + 7, y0, AreaWidth );

    return EFI_SUCCESS;
}



/****************************************************************************
*
*  ����:   �����ַ����ƺ��������ƽ����16*16���ش�С
*
*  ����pBuffer�� 		�������ַ�д����Ŀ�껺��
*  ����x0�� 			�������ַ�д����XĿ��
*  ����y0�� 			�������ַ�д����YĿ��
*  ����offset�� 		���ֿ����λ��
*  ����Color�� 		������ɫ
*  ����AreaWidth�� 	Ŀ�껺���ȣ����磺�������ϻ��ƴ�����Ŀ�ȣ����ҵĵ��Ի��ƴ��ҵĵ��Կ�ȵȵ�
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_GRAPHICS_ChineseCharDraw(UINT8 *pBuffer,
        IN UINTN x0, UINTN y0, UINT32 offset,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color , UINT16 AreaWidth)
{
    INT8 i;
    //L2_DEBUG_Print1(10, 10, "%X %X %X %X", x0, y0, offset, AreaWidth);
    ////DEBUG ((EFI_D_INFO, "%X %X %X %X", x0, y0, offset, AreaWidth));

    if (NULL == pBuffer)
    {
        //DEBUG ((EFI_D_INFO, "NULL == pBuffer"));
        return EFI_SUCCESS;
    }

    if (offset < 1)
    {
        //DEBUG ((EFI_D_INFO, "offset < 1 \n"));
        return EFI_SUCCESS;
    }
    
    for(i = 0; i < 32; i += 2)
    {
        L2_GRAPHICS_ChineseHalfDraw2(pBuffer, sChineseChar[offset * 32 + i ],     x0,     y0 + i / 2, 1, Color, AreaWidth);              
        L2_GRAPHICS_ChineseHalfDraw2(pBuffer, sChineseChar[offset * 32 + i + 1],  x0 + 8, y0 + i / 2, 1, Color, AreaWidth);      
    }
    
    ////DEBUG ((EFI_D_INFO, "\n"));
    
    return EFI_SUCCESS;
}



/****************************************************************************
*
*  ����:   �����ַ����ƺ��������ƽ����16*16���ش�С
*        ������ƿ����Ż��£����ڴ���׼���ã�ֱ�ӿ���������CPUʹ�û���Щ������Ҫÿ�δ�ӡ���ĵ�ʱ���ٱȽ�һ��
*
*  ����pBuffer�� 		�������ַ�д����Ŀ�껺��
*  ����x0�� 			�������ַ�д����XĿ��
*  ����y0�� 			�������ַ�д����YĿ��
*  ����AreaCode�� 		���ֿ�����
*  ����BitCode�� 		���ֿ�λ��
*  ����Color�� 		������ɫ
*  ����AreaWidth�� 	Ŀ�껺���ȣ����磺�������ϻ��ƴ�����Ŀ�ȣ����ҵĵ��Ի��ƴ��ҵĵ��Կ�ȵȵ�
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_GRAPHICS_ChineseCharDraw16(UINT8 *pBuffer,
        IN UINTN x0, UINTN y0, UINT32 AreaCode, UINT32 BitCode,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color , UINT16 AreaWidth)
{
    INT8 i;
    //L2_DEBUG_Print1(10, 10, "%X %X %X %X", x0, y0, offset, AreaWidth);
    ////DEBUG ((EFI_D_INFO, "%X %X %X %X", x0, y0, offset, AreaWidth));
    
    UINT32 offset = (AreaCode - 1) * 94 + BitCode - 1;
    
    if (NULL == pBuffer)
    {
        //DEBUG ((EFI_D_INFO, "NULL == pBuffer"));
        return EFI_SUCCESS;
    }

    if (offset < 1)
    {
        //DEBUG ((EFI_D_INFO, "offset < 1 \n"));
        return EFI_SUCCESS;
    }
    
    for(i = 0; i < 32; i += 2)
    {
        L2_GRAPHICS_ChineseHalfDraw2(pBuffer, sChineseChar[offset * 32 + i ],     x0,     y0 + i / 2, 1, Color, AreaWidth);              
        L2_GRAPHICS_ChineseHalfDraw2(pBuffer, sChineseChar[offset * 32 + i + 1],  x0 + 8, y0 + i / 2, 1, Color, AreaWidth);      
    }
        
    return EFI_SUCCESS;
}


/****************************************************************************
*
*  ����:   �����ַ����ƺ��������ƽ����12*12���ش�С
*
*  ����pBuffer�� 		�������ַ�д����Ŀ�껺��
*  ����x0�� 			�������ַ�д����XĿ��
*  ����y0�� 			�������ַ�д����YĿ��
*  ����offset�� 		���ֿ����λ��
*  ����Color�� 		������ɫ
*  ����AreaWidth�� 	Ŀ�껺���ȣ����磺�������ϻ��ƴ�����Ŀ�ȣ����ҵĵ��Ի��ƴ��ҵĵ��Կ�ȵȵ�
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_GRAPHICS_ChineseCharDraw12(UINT8 *pBuffer,
        UINTN x0, UINTN y0, UINT16 AreaCode,	UINT16 BitCode,
        EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color , UINT16 AreaWidth)
{
    INT8 i;
    //L2_DEBUG_Print1(10, 10, "%X %X %X %X", x0, y0, offset, AreaWidth);
    ////DEBUG ((EFI_D_INFO, "%X %X %X %X", x0, y0, offset, AreaWidth));

	UINT32 offset = (AreaCode - 1) * 94 + BitCode - 1;

    if (NULL == pBuffer)
    {
        //DEBUG ((EFI_D_INFO, "NULL == pBuffer"));
        return EFI_SUCCESS;
    }

    if (offset < 1)
    {
        //DEBUG ((EFI_D_INFO, "offset < 1 \n"));
        return EFI_SUCCESS;
    }

    //12*12
    for(i = 0; i < 24; i += 2)
    {
        //12*12/8=18
        L2_GRAPHICS_ChineseHalfDraw2(pBuffer, sChineseChar12[offset * 24 + i ],     x0,     y0 + i / 2, 1, Color, AreaWidth);              
        L2_GRAPHICS_ChineseHalfDraw2(pBuffer, sChineseChar12[offset * 24 + i + 1],  x0 + 8, y0 + i / 2, 1, Color, AreaWidth);      
    }
    
    ////DEBUG ((EFI_D_INFO, "\n"));
    
    return EFI_SUCCESS;
}



UINT16 GraphicsLayerIDCount = 0;



/****************************************************************************
*
*  ����:   ����ƶ��¼�����ʵ�����������ƶ����˵�����Ч��������ǰ��ʱû�����
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_MOUSE_Move()
{   
    //��ȡ��������ڵ�ͼ�㣬���ڡ�ͼ���ڳ�ʼ����ʱ��ѵ�4���ֽ����ڴ��ͼ��ID    
	LayerID = pDeskDisplayBuffer[(iMouseY * ScreenWidth + iMouseX) * 4 + 3];
	
    L2_DEBUG_Print1(0, ScreenHeight - 30 -  7 * 16, "%d: iMouseX: %d iMouseY: %d MouseClickFlag: %d Graphics Layer id: %d GraphicsLayerIDCount: %u", __LINE__, iMouseX, iMouseY, MouseClickFlag, LayerID, GraphicsLayerIDCount++);

        
    //��ȡ��ͬͼ��Ĳ�ͬ������������Ӧ���¼�
	UINT16 event = GraphicsLayerEventHandle[LayerID].pClickEventGet();

    //���û�е���¼����򲻻�ȡ����¼���
    if (MOUSE_EVENT_TYPE_NO_CLICKED == MouseClickFlag)
        return;

	switch (MouseClickFlag)
	{
	    case MOUSE_EVENT_TYPE_LEFT_CLICKED: 
	        L2_MOUSE_LeftClick(LayerID, event); break;
	    
	    case MOUSE_EVENT_TYPE_RIGHT_CLICKED: 
	        L2_MOUSE_RightClick(LayerID, event); break;
	        
	    default: break;
	}
    
    
}


/****************************************************************************
*
*  ����:     ��ʾ���ڡ�ʱ�䡢���ڼ�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID EFIAPI L2_TIMER_Print (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{   
    EFI_TIME EFITime;

	
	if (date_time_count_increase_flag == 0)
    {
    	date_time_count++;
	}
	else if (date_time_count_increase_flag == 1)
	{
		date_time_count--;	
	}
		
	if (date_time_count == 0 || date_time_count == 30)
		date_time_count_increase_flag = (date_time_count_increase_flag == 0) ?  1 : 0;

	//L2_TCP4_Send();
	
	//L2_TCP4_Receive();
	
    gRT->GetTime(&EFITime, NULL);
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    UINT16 x, y;
    
    Color.Blue = 0x00;
    Color.Red = 0xFF;
    Color.Green = 0x00;
    Color.Reserved  = GRAPHICS_LAYER_DESK;

    x = DISPLAY_DESK_DATE_TIME_X;
    y = DISPLAY_DESK_DATE_TIME_Y;

    //Display system date time weekday.
    L2_DEBUG_Print1(x, y, "%04d-%02d-%02d %02d:%02d:%02d ", 
                  EFITime.Year, EFITime.Month, EFITime.Day, EFITime.Hour, EFITime.Minute, EFITime.Second);

	//���ʵ�����п����Ż��Ŀռ䣬��Ϊ�����պ����ڼ�ÿ��ֻ��Ҫ����һ�ξ���
	//  ��   4839    ��   3858
    x += 21 * 8 + 3;
    //L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x, y,  (48 - 1) * 94 + 39 - 1, Color, ScreenWidth); 
    L2_GRAPHICS_ChineseCharDraw16(pDeskBuffer, x, y, 48, 39, Color, ScreenWidth);
    
    x += 16;
    //L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x, y,  (38 - 1) * 94 + 58 - 1, Color, ScreenWidth);
    L2_GRAPHICS_ChineseCharDraw16(pDeskBuffer, x, y, 38, 58, Color, ScreenWidth);

    x += 16;

    UINT8 DayOfWeek = L1_MATH_DayOfWeek(EFITime.Year, EFITime.Month, EFITime.Day);
	
    UINT8 AreaCode = 0;
    UINT8 BitCode = 0;
    
    // �� 4053 һ 5027 �� 2294 �� 4093 �� 4336 �� 4669 �� 3389
    switch (DayOfWeek)
    {
        case 0: AreaCode = 40; BitCode = 53; break;
        case 1: AreaCode = 50; BitCode = 27; break;
        case 2: AreaCode = 22; BitCode = 94; break;
        case 3: AreaCode = 40; BitCode = 93; break;
        case 4: AreaCode = 43; BitCode = 36; break;
        case 5: AreaCode = 46; BitCode = 69; break;
        case 6: AreaCode = 33; BitCode = 89; break;
        default: AreaCode = 16; BitCode = 01; break;
    }

    //L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x, y, (AreaCode - 1 ) * 94 + BitCode - 1, Color, ScreenWidth);
    L2_GRAPHICS_ChineseCharDraw16(pDeskBuffer, x, y, AreaCode, BitCode, Color, ScreenWidth);
    
   L2_DEBUG_Print1(DISPLAY_DESK_HEIGHT_WEIGHT_X, DISPLAY_DESK_HEIGHT_WEIGHT_Y, "%d: ScreenWidth:%d, ScreenHeight:%d\n", __LINE__, ScreenWidth, ScreenHeight);
   /*
   GraphicsOutput->Blt(GraphicsOutput, 
                        (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) pDateTimeBuffer,
                        EfiBltBufferToVideo,
                        0, 0, 
                        0, 16 * 8, 
                        8 * 50, 16, 0);*/
}

