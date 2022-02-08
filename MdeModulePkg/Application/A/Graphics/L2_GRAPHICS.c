
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
UINT8 *pMouseBuffer = NULL; //Mouse layer: 4
UINT8 *pMouseClickBuffer = NULL; // for mouse click 
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
UINT8 *pSystemLogWindowBuffer = NULL; // MyComputer layer: 2
UINT8 *pSystemSettingWindowBuffer = NULL;

UINT16 StartMenuWidth = 16 * 10;
UINT16 StartMenuHeight = 16 * 20;
UINT16 StatusErrorCount = 0;
UINT16 SystemLogWindowWidth = 16 * 48;
UINT16 SystemLogWindowHeight = 16 * 60;
UINT16 SystemSettingWindowWidth = 16 * 10;
UINT16 SystemSettingWindowHeight = 16 * 10;

UINT16 MemoryInformationWindowHeight = 16 * 60;
UINT16 MemoryInformationWindowWidth = 16 * 40;
UINT16 LayerID = 0;

EFI_GRAPHICS_OUTPUT_BLT_PIXEL MouseMoveoverObjectDrawColor;


EFI_GRAPHICS_OUTPUT_BLT_PIXEL MouseColor;

UINT8 *pSystemLogBuffer = NULL; // Save log data

//Need to allocate buffer in this module.
#define SYSTEM_LOG_DATA_WIDTH 200
#define SYSTEM_LOG_DATA_LINE 40


// Line 22
#define DISK_READ_BUFFER_X (0) 
#define DISK_READ_BUFFER_Y (6 * 56)


UINT16 date_time_count_increase_flag = 0;
UINT16 date_time_count = 0;


UINT8 BufferMFT[DISK_BUFFER_SIZE * 2];

UINT16 PartitionItemID = 0xffff; // invalid
UINT16 FolderItemID = 0xffff; // invalid

INT8 SystemQuitFlag = FALSE;

// because part items of  pItems is not valid;
UINT16 FolderItemValidIndexArray[10] = {0};

DESKTOP_ITEM_CLICKED_EVENT    DesktopItemClickEvent = DESKTOP_ITEM_INIT_CLICKED_EVENT;

//开始菜单状态转换，用于记录上次状态机状态
START_MENU_STATE 	 StartMenuNextState = START_MENU_ITEM_INIT_EVENT;

//我的电脑状态转换，用于记录上次状态机状态
MY_COMPUTER_STATE 	 MyComputerNextState = MY_COMPUTER_INIT_STATE;

// FAT32 文件或者目录
FAT32_ROOTPATH_SHORT_FILE_ITEM pItems[32];


NTFS_FILE_SWITCHED NTFSFileSwitched = {0};


//Line 0
#define DISPLAY_DESK_HEIGHT_WEIGHT_X (date_time_count % 30) 
#define DISPLAY_DESK_HEIGHT_WEIGHT_Y (ScreenHeight - 16 * 3)


MOUSE_MOVEOVER_OBJECT MouseMoveoverObjectOld;


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




/****************************************************************************
*
*  描述:   把原内存拷贝到目的内存的指定位置。当前还缺少一些合法性判断
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
*  描述:   图片的内存拷贝，感觉跟L3_GRAPHICS_ItemPrint有些是重复的。
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID L2_GRAPHICS_Copy(UINT8 *pDest, UINT8 *pSource, 
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
            pDest[((StartY + i) * DestWidth + StartX + j) * 4 + 3] = pSource[(i * SourceWidth + j) * 4 + 3];
        }
    }
}






/****************************************************************************
*
*  描述:   画直线
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
*  描述:   画矩形，当前线的宽度还未使用，比较粗糙
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
void L2_GRAPHICS_RectangleDraw(UINT8 *pBuffer,
        IN UINTN x0, UINTN y0, UINTN x1, UINTN y1, 
        IN UINTN BorderWidth,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color, UINT16 AreaWidth)
{    
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
*  描述:   矩形填充
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
void L1_MEMORY_RectangleFill(UINT8 *pBuffer,
        IN UINTN x0, UINTN y0, UINTN x1, UINTN y1, 
        IN UINTN BorderWidth,
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
            pBuffer[(j * ScreenWidth + i) * 4]     =  Color.Blue; //Blue   
            pBuffer[(j * ScreenWidth + i) * 4 + 1] =  Color.Green; //Green 
            pBuffer[(j * ScreenWidth + i) * 4 + 2] =  Color.Red; //Red  
            pBuffer[(j * ScreenWidth + i) * 4 + 3] =  Color.Reserved; //Red  
        }
    }

}





/****************************************************************************
*
*  描述:   我的电脑窗口点击事件中关闭窗口事件响应，注：未进行内存释放，有优化的空间
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID L2_MOUSE_MyComputerCloseClicked()
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_MyComputerCloseClicked\n", __LINE__);
    //DisplayMyComputerFlag = 0;
    //WindowLayers.item[3].DisplayFlag = 0;
    if (TRUE == WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].DisplayFlag)
    {
        WindowLayers.ActiveWindowCount--;        
        WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].DisplayFlag = FALSE;
    }
}




/****************************************************************************
*
*  描述:   分区被点击事件
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID L2_MOUSE_MyComputerPartitionItemClicked()
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_MyComputerPartitionItemClicked\n", __LINE__);
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
	
    Color.Red   = 0xff;
    Color.Green = 0x00;
    Color.Blue  = 0x00;
    Color.Reserved = GRAPHICS_LAYER_MY_COMPUTER_WINDOW;
	
    L2_GRAPHICS_RectangleDraw(pMouseSelectedBuffer, 0,  0, 31, 15, 1,  Color, 32);
	L2_GRAPHICS_Copy(pDeskDisplayBuffer, pMouseSelectedBuffer, ScreenWidth, ScreenHeight, 32, 16, WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartX + 50, WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartY  + PartitionItemID * (16 + 2) + 16 * 2);   
    L2_STORE_PartitionItemsPrint(PartitionItemID);
}




/****************************************************************************
*
*  描述:   文件内容显示
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
*  描述:   文件内容显示
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
*  描述:   文件夹中文件或者目录被点击后显示文件或者显示子目录操作
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L2_MOUSE_MyComputerFolderItemClicked()
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
		return -1;
	}

	//需要找获取有效的项索引
	UINT16 index = FolderItemValidIndexArray[FolderItemID];
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d index: %d\n", __LINE__, index);
		
	//FAT32文件系统格式
	if (device[PartitionItemID].FileSystemType == FILE_SYSTEM_FAT32)
    {    	

		UINT16 High2B = L1_NETWORK_2BytesToUINT16(pItems[index].StartClusterHigh2B);
		UINT16 Low2B  = L1_NETWORK_2BytesToUINT16(pItems[index].StartClusterLow2B);
		UINT32 StartCluster = (UINT32)High2B << 64 | (UINT32)Low2B;

		// Start cluster id is 2, exclude 0,1
		//这样写死8192，会有BUG
		UINT32 StartSectorNumber = 8192 + (StartCluster - 2) * 8;
		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], 
						"%d High2B: %X Low2B: %X StartCluster: %X StartSectorNumber: %X\n", 
						__LINE__, 
						High2B,
						Low2B,
						StartCluster,
						StartSectorNumber);

        //如果是efi文件，认为是可执行文件
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
	    Status = L1_STORE_READ(PartitionItemID, StartSectorNumber, 1, Buffer); 
	    if (EFI_ERROR(Status))
	    {
	        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
	        return Status;
	    }

		switch(pItems[index].Attribute[0])
		{
		    //如果是目录，则显示子目录
			case 0x10:  L1_MEMORY_Memset(&pItems, 0, sizeof(pItems));
					    L1_MEMORY_Copy(&pItems, Buffer, DISK_BUFFER_SIZE);
						L2_STORE_FolderItemsPrint();
						break;

			//如果是文件，则显示文件内容
			case 0x20: L2_PARTITION_FileContentPrint(Buffer); break;

			default: break;
		}
	    
    }
    else if (device[PartitionItemID].FileSystemType == FILE_SYSTEM_NTFS) //NTFS文件系统格式
    {
		//可能会有BUG 6291456=786432 * 8，有的MFT不在786432
		UINT32 StartSectorNumber = 6291456 + pCommonStorageItems[index].FileContentRelativeSector * 2;
		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: StartSector: %llu Sector: %llu",  __LINE__, StartSectorNumber, pCommonStorageItems[index].FileContentRelativeSector);

		// Read data from partition(disk or USB etc..)					
	    Status = L1_STORE_READ(PartitionItemID, StartSectorNumber, 2, BufferMFT); 
	    if (EFI_ERROR(Status))
	    {
	        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
	        return Status;
	    }
		
		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d pNTFSFileSwitched Allocate memory: %d.\n", __LINE__, sizeof(NTFS_FILE_SWITCHED));
		
		L1_MEMORY_Memset(&NTFSFileSwitched, 0, sizeof(NTFSFileSwitched)); 
				
		L2_PARTITION_FileContentPrint(BufferMFT);

		//从分区读取到的磁盘用于FILE分析，分析所有属性项，文件和文件夹拥有的属性项不相同
		
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
				    Status = L1_STORE_READ(PartitionItemID, 8 * pA0Indexes[0].Offset, 2, BufferMFT); 
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
				//显示文件内容时会走这个分支。
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
*  描述:   我的电脑窗口状态转换的状态机，包含窗口关闭、分区点击、文件夹点击、文件点击事件。
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
STATE_TRANSFORM MyComputerStateTransformTable[] =
{
    {MY_COMPUTER_INIT_STATE,                MY_COMPUTER_WINDOW_CLOSE_WINDOW_CLICKED_EVENT,     MY_COMPUTER_INIT_STATE,         L2_MOUSE_MyComputerCloseClicked},
    {MY_COMPUTER_INIT_STATE,     	        MY_COMPUTER_WINDOW_PARTITION_ITEM_CLICKED_EVENT,   MY_COMPUTER_PARTITION_CLICKED_STATE,  L2_MOUSE_MyComputerPartitionItemClicked},
    {MY_COMPUTER_PARTITION_CLICKED_STATE,   MY_COMPUTER_WINDOW_FOLDER_ITEM_CLICKED_EVENT,      MY_COMPUTER_FOLDER_CLICKED_STATE,     L2_MOUSE_MyComputerFolderItemClicked},
    {MY_COMPUTER_FOLDER_CLICKED_STATE, 	    MY_COMPUTER_WINDOW_FOLDER_ITEM_CLICKED_EVENT,      MY_COMPUTER_FOLDER_CLICKED_STATE,     L2_MOUSE_MyComputerFolderItemClicked},
};




/****************************************************************************
*
*  描述:   用于处理鼠标在不同窗口点击事件的处理，被点击窗口会到所有图层最上层。
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
*  描述:   点击“我的电脑”事件，会显示我的电脑窗口
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
*  描述:   点击“系统设置”事件
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
*  描述:   点击“内存信息”事件，会显示内存信息窗口
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
*  描述:   点击“系统日志”事件，会显示系统日志窗口，这样比较方便系统开发，问题定位
*        注意：系统日志窗口，显示的是L2_DEBUG_Print3函数打印的日志
*
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
*  描述: 点击“系统退出”事件，电脑会关机
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID L2_MOUSE_SystemQuitClicked()
{   
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_SystemQuitClicked\n", __LINE__);
    SystemQuitFlag = TRUE;  
}





/****************************************************************************
*
*  描述:   点击“桌面壁纸设置”事件，桌面壁纸会变灰，颜色可以自已在for循环里控制
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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





/****************************************************************************
*
*  描述:   点击“桌面背景图片恢复”事件，背景图片会恢复成珠峰图片
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
*  描述:   开始菜单状态转换表，很重要
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
STATE_TRANSFORM StartMenuStateTransformTable[] =
{
    {START_MENU_ITEM_INIT_EVENT,    START_MENU_ITEM_MY_COMPUTER_CLICKED_EVENT,                            START_MENU_CLICK_INIT_STATE,          			L2_MOUSE_MyComputerClicked},
    {START_MENU_ITEM_INIT_EVENT,    START_MENU_ITEM_SYSTEM_SETTING_CLICKED_EVENT,                         START_MENU_SYSTEM_SETTING_CLICKED_STATE,       	L2_MOUSE_SystemSettingClicked},
    {START_MENU_ITEM_INIT_EVENT,    START_MENU_ITEM_MEMORY_INFORMATION_CLICKED_EVENT,   		          START_MENU_MEMORY_INFORMATION_CLICKED_STATE,   	L2_MOUSE_MemoryInformationClicked},
    {START_MENU_ITEM_INIT_EVENT,    START_MENU_ITEM_SYSTEM_LOG_CLICKED_EVENT,           		          START_MENU_SYSTEM_LOG_STATE,                   	L2_MOUSE_SystemLogClicked},
    {START_MENU_ITEM_INIT_EVENT,    START_MENU_ITEM_SHUTDOWN_CLICKED_EVENT,          		              START_MENU_SYSTEM_QUIT_STATE,                  	L2_MOUSE_SystemQuitClicked},
    {START_MENU_SYSTEM_SETTING_CLICKED_STATE,  START_MENU_SYSTEM_SETTING_SUBITEM_WALLPAPER_SETTING_CLICKED_EVENT,    START_MENU_CLICK_INIT_STATE,                   	L2_MOUSE_WallpaperSettingClicked},
    {START_MENU_SYSTEM_SETTING_CLICKED_STATE,  START_MENU_SYSTEM_SETTING_SUBITEM_WALLPAPER_RESET_CLICKED_EVENT,      START_MENU_CLICK_INIT_STATE,                   	L2_MOUSE_WallpaperResetClicked},
};

void L2_GRAPHICS_Init()
{

}


/****************************************************************************
*
*  描述:   图层参数初始化
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
void L2_GRAPHICS_BootScreenInit()
{
    //如果不加下面这几行，则是直接显示内存信息，看起来有点像雪花    
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
*  描述:   图层参数初始化
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].WindowWidth  = SystemLogWindowWidth;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].WindowHeight = SystemLogWindowHeight;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].LayerID = GRAPHICS_LAYER_SYSTEM_LOG_WINDOW;

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
    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].WindowWidth = MyComputerWidth;
    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].WindowHeight = MyComputerHeight;
    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].LayerID = GRAPHICS_LAYER_MY_COMPUTER_WINDOW;
    
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

    L1_MEMORY_SetValue(WindowLayers.LayerSequences, 0, 10 * 2);

    WindowLayers.ActiveWindowCount = 0;

    MouseClickFlag = MOUSE_EVENT_TYPE_NO_CLICKED;

	// Active window list, please note: desk layer always display at firstly.
	// So WindowLayers.LayerCount value always one more than WindowLayers.ActiveWindowCount
    WindowLayers.LayerSequences[0] = GRAPHICS_LAYER_START_MENU;
    WindowLayers.LayerSequences[1] = GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW;
    WindowLayers.LayerSequences[2] = GRAPHICS_LAYER_MY_COMPUTER_WINDOW;
    WindowLayers.LayerSequences[3] = GRAPHICS_LAYER_SYSTEM_LOG_WINDOW;
    WindowLayers.LayerSequences[4] = GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW;
    WindowLayers.ActiveWindowCount = WindowLayers.LayerCount - 1;


    MouseMoveoverObjectDrawColor.Blue = 0;
    MouseMoveoverObjectDrawColor.Red = 0;
    MouseMoveoverObjectDrawColor.Green = 0;
    MouseMoveoverObjectDrawColor.Reserved = 0;
}



/****************************************************************************
*
*  描述: 在指定图层打印指定字符串。其中layer是目标图层
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
*  描述:   英文字符串显示函数，需要注意的是这个函数打印的信息都在桌面上，由WindowLayers.item[GRAPHICS_LAYER_DESK]控制
*
*  参数x： 显示的屏幕的X坐标
*  参数y： 显示到屏幕的Y坐标
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
*  描述:   显示器屏幕初始化，把U盘里边的桌面背景图片，我的电脑、回收站、系统设置、文件、文件夹图标都读取到缓存供桌面显示用
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
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
    
    Status = L3_APPLICATION_ReadFile("HZK12", 5, sChineseChar12);
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L3_APPLICATION_ReadFile error\n", __LINE__);
    }
    L2_GRAPHICS_DeskInit();

    // 初始化鼠标显示缓存
    L2_GRAPHICS_ChineseCharDraw(pMouseBuffer, 0, 0, (12 - 1) * 94 + 84 - 1, MouseColor, 16);
    //L2_DEBUG_Print1(DISPLAY_X, DISPLAY_Y, "%d: GraphicsLayerCompute\n", __LINE__);

	//把上面初始化好的缓冲显示出来
	L2_SCREEN_Draw(pDeskBuffer, 0, 0, 0, 0, ScreenWidth, ScreenHeight);	   

    // Desk graphics layer, buffer can not free!!
    //FreePool(pDeskBuffer);
    
    return EFI_SUCCESS;
}




/****************************************************************************
*
*  描述:   开始菜单内容项添加
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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

    //这边的序列需要跟START_MENU_BUTTON_SEQUENCE这个枚举定义的一致
    //我的电脑
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (46 - 1 ) * 94 + 50 - 1, Color, StartMenuWidth);    
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (21 - 1) * 94 + 36 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (21 - 1) * 94 + 71 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pStartMenuBuffer, x , y,     (36 - 1) * 94 + 52 - 1, Color, StartMenuWidth);   
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pStartMenuBuffer, x , y,     12, 84, Color, StartMenuWidth);   

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


    x = 3;
    y += 16;
    L2_GRAPHICS_ChineseCharDraw12(pStartMenuBuffer, x , y,     (47 - 1 ) * 94 + 21 - 1, Color, StartMenuWidth);    
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw12(pStartMenuBuffer, x , y,     (45 - 1) * 94 + 19 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw12(pStartMenuBuffer, x , y,     (45 - 1) * 94 + 43 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw12(pStartMenuBuffer, x , y,     (19 - 1) * 94 + 86 - 1, Color, StartMenuWidth);   

    x = 3;
    y += 16;
    L2_GRAPHICS_ChineseCharDraw12(pStartMenuBuffer, x , y,     (47 - 1 ) * 94 + 21 - 1, Color, StartMenuWidth);    
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw12(pStartMenuBuffer, x , y,     (45 - 1) * 94 + 19 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw12(pStartMenuBuffer, x , y,     (45 - 1) * 94 + 43 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw12(pStartMenuBuffer, x , y,     (19 - 1) * 94 + 86 - 1, Color, StartMenuWidth);   
    
    x = 3;
    y += 16;
    L2_GRAPHICS_ChineseCharDraw12(pStartMenuBuffer, x , y,     (47 - 1 ) * 94 + 21 - 1, Color, StartMenuWidth);    
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw12(pStartMenuBuffer, x , y,     (45 - 1) * 94 + 19 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw12(pStartMenuBuffer, x , y,     (45 - 1) * 94 + 43 - 1, Color, StartMenuWidth);
    x += 16;

    UINT8 word[3] = "啊";
    
    //L2_GRAPHICS_ChineseCharDraw12(pStartMenuBuffer, x , y,     (word[0] - 0xa0- 1) * 94 + word[1] - 0xa0 - 1, Color, StartMenuWidth);   

	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: word[0]:%d word[1]:%d word[2]:%d \n", __LINE__, word[0], word[1], word[2]);

}





/****************************************************************************
*
*  描述:   系统设置缓存区初始化
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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





/****************************************************************************
*
*  描述:   关机屏幕
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
    
    // 再    5257    见   2891    ，   0312    欢   2722    迎   5113
    // 下    4734    次   2046    回   2756    来   3220
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
*  描述:   判断当前鼠标坐标是否在矩形区域内
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
BOOLEAN L1_GRAPHICS_InsideRectangle(UINT16 StartX, UINT16 EndX, UINT16 StartY, UINT16 EndY)
{
    BOOLEAN bInArea = iMouseX >= StartX && iMouseX <= EndX && iMouseY >= StartY && iMouseY <= EndY;

    if (!bInArea)
        return bInArea;
    
    //如果图层为显示状态，并且鼠标在事件触发区域，则更新值
    if (TRUE == WindowLayers.item[LayerID].DisplayFlag && bInArea)
    {        
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: StartX: %d, EndX: %d, StartY: %d, EndY: %d\n", __LINE__, StartX, EndX, StartY, EndY);
        MouseMoveoverObject.StartX = StartX;
        MouseMoveoverObject.EndX   = EndX;
        MouseMoveoverObject.StartY = StartY;
        MouseMoveoverObject.EndY   = EndY;
        MouseMoveoverObject.GraphicsLayerID = LayerID;
    }

    //因为每一个图层的每一个事件区域是唯一的，所以只需要判断三个都不相等就可以
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
*  描述:   桌面图层点击事件，当前只有左下角的开始菜单点击事件
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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

    
    //下面这几个判断，其实可以写成for循环这种，不用写多次
	if (L1_GRAPHICS_InsideRectangle(x1, x1 + ItemWidth, y1, y1 + ItemHeight))
	{	    
		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d:2 DESKTOP_ITEM_MY_COMPUTER_CLICKED_EVENT\n", __LINE__);
        return DESKTOP_ITEM_MY_COMPUTER_CLICKED_EVENT;
	}

	y1 += ItemHeight + 2 * 16;

	if (L1_GRAPHICS_InsideRectangle(x1, x1 + ItemWidth, y1, y1 + ItemHeight))
	{	    
		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d:3 DESKTOP_ITEM_SYSTEM_SETTING_CLICKED_EVENT\n", __LINE__);
        return DESKTOP_ITEM_SYSTEM_SETTING_CLICKED_EVENT;
	}

	y1 += ItemHeight + 2 * 16;

	if (L1_GRAPHICS_InsideRectangle(x1, x1 + ItemWidth, y1, y1 + ItemHeight))
	{	    
		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d:4 DESKTOP_ITEM_RECYCLE_BIN_CLICKED_EVENT\n", __LINE__);
        return DESKTOP_ITEM_RECYCLE_BIN_CLICKED_EVENT;
	}

	y1 += ItemHeight + 2 * 16;

	return DESKTOP_ITEM_MAX_CLICKED_EVENT;
}






/****************************************************************************
*
*  描述:   开始菜单图层点击事件获取
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
START_MENU_ITEM_CLICKED_EVENT L2_GRAPHICS_StartMenuLayerClickEventGet()
{
	//L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_GRAPHICS_StartMenuLayerClickEventGet\n", __LINE__);

    UINT16 StartMenuPositionX = WindowLayers.item[GRAPHICS_LAYER_START_MENU].StartX;
    UINT16 StartMenuPositionY = WindowLayers.item[GRAPHICS_LAYER_START_MENU].StartY;

    //如果开始菜单没有没有显示，则回到菜单初始状态
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
                                    3 + StartMenuPositionY + 16 * START_MENU_BUTTON_SYSTEM_QUIT, 3 + StartMenuPositionY + 16 * (START_MENU_BUTTON_SYSTEM_QUIT + 1)))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d:9 SYSTEM_QUIT_CLICKED_EVENT\n", __LINE__);
        return START_MENU_ITEM_SHUTDOWN_CLICKED_EVENT;
    }

	return START_MENU_ITEM_MAX_CLICKED_EVENT;
}




/****************************************************************************
*
*  描述:   系统设置图层点击事件获取
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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

	return START_MENU_SYSTEM_SETTING_SUBITEM_MAX_CLICKED_EVENT;

}

/****************************************************************************
*
*  描述:   通过鼠标光标位置获取我的电脑图层点击事件
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
MY_COMPUTER_WINDOW_CLICKED_EVENT L2_GRAPHICS_MyComputerLayerClickEventGet()
{
	//L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_GRAPHICS_MyComputerLayerClickEventGet\n", __LINE__);
		
    UINT16 MyComputerPositionX = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartX;
    UINT16 MyComputerPositionY = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartY;
    
    if (FALSE == WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].DisplayFlag)
    {
        return START_MENU_ITEM_INIT_EVENT;
    }
    
    if (L1_GRAPHICS_InsideRectangle(MyComputerPositionX + MyComputerWidth - 20, MyComputerPositionX + MyComputerWidth - 4, 
                                       MyComputerPositionY + 0, MyComputerPositionY + 16))  
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MY_COMPUTER_CLOSE_CLICKED_EVENT\n", __LINE__);
        return MY_COMPUTER_WINDOW_CLOSE_WINDOW_CLICKED_EVENT;
    }

	//分区被点击事件
    for (UINT16 i = 0 ; i < PartitionCount; i++)
    {
		UINT16 StartX = MyComputerPositionX + 100;
		UINT16 StartY = MyComputerPositionY + i * 18 + 16 * 2;
		
		if (L1_GRAPHICS_InsideRectangle(StartX, StartX + 16 * 4, 
                                   StartY + 0, StartY + 16))
		{
			PartitionItemID = i;
			L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d:14 MY_COMPUTER_PARTITION_ITEM_CLICKED_EVENT PartitionItemID: %d\n", __LINE__, PartitionItemID);
			return MY_COMPUTER_WINDOW_PARTITION_ITEM_CLICKED_EVENT;
		}
	}

    UINT16 HeightNew = SYSTEM_ICON_HEIGHT / 8;
    UINT16 WidthNew = SYSTEM_ICON_WIDTH / 8;

	//Only 6 item, need to fix after test.
	//分区的文件或文件夹被点击事件
    for (UINT16 i = 0 ; i < 11; i++)
    {
		UINT16 StartX = MyComputerPositionX + 130;
		UINT16 StartY = MyComputerPositionY + i  * (HeightNew + 16 * 2) + 200;
		
		if (L1_GRAPHICS_InsideRectangle(StartX, StartX + WidthNew, 
                                   StartY + 0, StartY + HeightNew))
		{
			FolderItemID = i;
			L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d:15 MY_COMPUTER_PARTITION_ITEM_CLICKED_EVENT FolderItemID: %d\n", __LINE__, FolderItemID);
			return MY_COMPUTER_WINDOW_FOLDER_ITEM_CLICKED_EVENT;
		}
	}

	return MY_COMPUTER_WINDOW_MAX_CLICKED_EVENT;
}





/****************************************************************************
*
*  描述:   系统日志图层点击事件获取
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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

	return SYSTEM_LOG_WINDOW_MAX_CLICKED_EVENT;			
}





/****************************************************************************
*
*  描述:   内存信息窗口图层点击事件获取
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
			
	return MEMORY_INFORMATION_WINDOW_MAX_CLICKED_EVENT;
}




/****************************************************************************
*
*  描述:   开始菜单被点击，处理函数
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
    L2_GRAPHICS_RectangleDraw(pMouseSelectedBuffer, 0,  0, 31, 15, 1,  Color, 32);
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
*  描述:   桌面图层被点击处理函数
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
            L2_MOUSE_MyComputerClicked(); break;

		//这边其实需要注意下，需要跟事件获取的状态保持一致，其实少了三个事件，我的电脑、系统设置、回收站。
		default: break;
	}
}





/****************************************************************************
*
*  描述:   开始菜单事件处理状态机
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
            StartMenuStateTransformTable[i].pFunc();
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MouseClickEvent: %d StartMenuNextState: %d\n", __LINE__, event, StartMenuNextState);
            break;
        }   
    }

}





/****************************************************************************
*
*  描述:   xxxxx
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
*  描述:   我的电脑被点击，显示我的窗口，并且进入我的电脑窗口状态机
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID L3_GRAPHICS_MyComupterClickEventHandle(MY_COMPUTER_WINDOW_CLICKED_EVENT event)
{    
	switch(event)
	{
		case MY_COMPUTER_WINDOW_CLOSE_WINDOW_CLICKED_EVENT:
			L2_MOUSE_MyComputerCloseClicked(); break;

		//这边其实需要注意下，需要跟事件获取的状态保持一致，其实少了三个事件，我的电脑、系统设置、回收站。
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
            MyComputerStateTransformTable[i].pFunc();
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MouseClickEvent: %d StartMenuNextState: %d\n", __LINE__, event, StartMenuNextState);
            break;
        }   
    }

}




/****************************************************************************
*
*  描述:   我的电脑窗口点击事件中关闭窗口事件响应，注：未进行内存释放，有优化的空间
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
*  描述:   我的电脑窗口点击事件中关闭窗口事件响应，注：未进行内存释放，有优化的空间
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
*  描述:   我的电脑窗口点击事件中关闭窗口事件响应，注：未进行内存释放，有优化的空间
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
*  描述:   xxxxx
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID L3_GRAPHICS_SystemLogClickEventHandle(SYSTEM_LOG_WINDOW_CLICKED_EVENT event)
{

	switch(event)
	{
		case SYSTEM_LOG_WINDOW_CLOSE_WINDOW_CLICKED_EVENT:
			L2_MOUSE_SystemLogCloseClicked(); break;

		//这边其实需要注意下，需要跟事件获取的状态保持一致，其实少了三个事件，我的电脑、系统设置、回收站。
		default: break;
	}

}





/****************************************************************************
*
*  描述:   xxxxx
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
*  可以处理桌面所有图层事件，总入口，灰常重要
*  描述:   根据鼠标光标所在图层，找到图层点击事件，并根据点击事件找到对应的响应处理函数。
*  第一列：不同图层编号
*  第二列：根据图层ID获取不同图层的点击事件XXXXXClickEventGet（不同窗口事件提取）
*  第三列：处理不同事件XXXXXClickEventHandle（不同窗口事件处理入口，比较重要）
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
GRAPHICS_LAYER_EVENT_GET GraphicsLayerEventHandle[] =
{
    {GRAPHICS_LAYER_DESK,       					 L2_GRAPHICS_DeskLayerClickEventGet, 				L3_GRAPHICS_DeskClickEventHandle},
    {GRAPHICS_LAYER_START_MENU,            			 L2_GRAPHICS_StartMenuLayerClickEventGet, 			L3_GRAPHICS_StartMenuClickEventHandle},
    {GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW,           L2_GRAPHICS_SystemSettingLayerClickEventGet, 		L3_GRAPHICS_SystemSettingClickEventHandle},
    {GRAPHICS_LAYER_MY_COMPUTER_WINDOW,            	 L2_GRAPHICS_MyComputerLayerClickEventGet, 			L3_GRAPHICS_MyComupterClickEventHandle},
    {GRAPHICS_LAYER_SYSTEM_LOG_WINDOW,            	 L2_GRAPHICS_SystemLogLayerClickEventGet, 			L3_GRAPHICS_SystemLogClickEventHandle},
    {GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW,       L2_GRAPHICS_MemoryInformationLayerClickEventGet, 	L3_GRAPHICS_MemoryInformationClickEventHandle},
};

/****************************************************************************
*
*  描述:   鼠标左键点击事件处理
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID L2_MOUSE_RightClick(UINT16 LayerID, UINT16 event)
{
    if ( MouseClickFlag != MOUSE_EVENT_TYPE_RIGHT_CLICKED)
    {
        return;
    }
    

    if (MouseClickFlag == MOUSE_EVENT_TYPE_RIGHT_CLICKED)
        L2_GRAPHICS_CopyNoReserved(pDeskDisplayBuffer, pMouseClickBuffer, ScreenWidth, ScreenHeight, MouseClickWindowWidth, MouseClickWindowHeight, iMouseX, iMouseY);
        
    //L2_GRAPHICS_RightClickMenuInit(iMouseX, iMouseY, LayerID);
    
}


/****************************************************************************
*
*  描述:   鼠标左键点击事件处理
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID L2_MOUSE_LeftClick(UINT16 LayerID, UINT16 event)
{
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: iMouseX: %d iMouseY: %d \n",  __LINE__, iMouseX, iMouseY);
    //鼠标拖动窗口移动
    
    if ( MouseClickFlag != MOUSE_EVENT_TYPE_LEFT_CLICKED)
    {
        return;
    }

    //如果不是桌面图层，则移动，不过这里也有些问题，图层里边包含系统设置图层，正常情况下，这里是菜单组成部分，不应该可以移动
    if (GRAPHICS_LAYER_DESK != LayerID)
    {
        WindowLayers.item[LayerID].StartX += x_move * 3;
        WindowLayers.item[LayerID].StartY += y_move * 3;
       
        L1_GRAPHICS_UpdateWindowLayer(LayerID);
    }
    
    x_move = 0;
    y_move = 0;

	//获取鼠标光标所在的图层，窗口、图层在初始化的时候把第4个字节用于存放图层ID
	LayerID = pDeskDisplayBuffer[(iMouseY * ScreenWidth + iMouseX) * 4 + 3];

	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: iMouseX: %d iMouseY: %d ClickFlag: %d, LayerID: %d\n", __LINE__, iMouseX, iMouseY, MouseClickFlag, LayerID);
      	
    MouseClickFlag = MOUSE_EVENT_TYPE_NO_CLICKED;

	// Handle click event
	GraphicsLayerEventHandle[LayerID].pClickEventHandle(event);
	
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
    
    L1_MEMORY_RectangleFill(pDeskBuffer, 3,  y - 24, 59, y - 24, 1, Color); //line top (3,  y - 24) (59, y - 24)
    L1_MEMORY_RectangleFill(pDeskBuffer, 2,  y - 24, 59, y - 4,  1, Color); //area center(2,  y - 24) (59, y - 4)

    Color.Red   = 0x84;
    Color.Green = 0x84;
    Color.Blue  = 0x84;
    L1_MEMORY_RectangleFill(pDeskBuffer, 3,  y -  4, 59, y -  4, 1, Color); // line button (3,  y -  4) (59, y -  4)
    L1_MEMORY_RectangleFill(pDeskBuffer, 59, y - 23, 59, y -  5, 1, Color); // line right(59, y - 23) (59, y -  5)

    // Black
    Color.Red   = 0x00;
    Color.Green = 0x00;
    Color.Blue  = 0x00;
    L1_MEMORY_RectangleFill(pDeskBuffer, 2,  y -  3, 59, y -  3, 1, Color); // line button(2,  y -  3) (59, y -  3)
    L1_MEMORY_RectangleFill(pDeskBuffer, 60, y - 24, 60, y -  3, 1, Color); // line right(60, y - 24) (60, y -  3)
}


//                                                 16 * 4,        16 * 7,       16 * 4,        16 * 2



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
EFI_STATUS L2_GRAPHICS_ButtonDraw2(UINT16 StartX, UINT16 StartY, UINT16 Width, UINT16 Height)
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    
    // Button
    // white
    Color.Red   = 0xFF;
    Color.Green = 0xFF;
    Color.Blue  = 0xFF;
    Color.Reserved  = GRAPHICS_LAYER_DESK;
    L1_MEMORY_RectangleFill(pDeskBuffer, StartX,  StartY, StartX + Width, StartY, 1, Color); //line top 
    L1_MEMORY_RectangleFill(pDeskBuffer, StartX,  StartY, StartX, StartY + Height,  1, Color); //line left

    Color.Red   = 214;
    Color.Green = 211;
    Color.Blue  = 206;
    L1_MEMORY_RectangleFill(pDeskBuffer, StartX + 1, StartY + 1, StartX + Width, StartY + Height, 1, Color); // Area
    
    // Black
    Color.Red   = 0x00;
    Color.Green = 0x00;
    Color.Blue  = 0x00;
    L1_MEMORY_RectangleFill(pDeskBuffer, StartX,  StartY + Height + 1, StartX + Width, StartY + Height + 2, 1, Color); // line button
    L1_MEMORY_RectangleFill(pDeskBuffer, StartX + Width + 1, StartY + 1 , StartX + Width + 2, StartY + Height + 1, 1, Color); // line right
}




/****************************************************************************
*
*  描述:   桌面初始化，还比较粗糙
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L2_GRAPHICS_DeskInit()
{
    
    EFI_STATUS status = 0;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    UINT32 x = ScreenWidth;
    UINT32 y = ScreenHeight;

	Color.Reserved = GRAPHICS_LAYER_DESK;

    //壁纸分辨率1920*1080，每个像素点占三个字节，0x36是指BMP格式图片文件的头
    for (int i = 0; i < 1920 * 1080 * 3; i++)
    	pDeskWallpaperBuffer[i] = pDeskWallpaperBuffer[0x36 + i];

    UINT8 *pWallpaperBuffer = pDeskWallpaperBuffer;
    
	if (ScreenHeight != 1080 || ScreenWidth != 1920)
	{
	    //默认提供的BMP图标太大，所以在显示之前把图片缩小了下
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

    //Skip bmp header.
    for (UINT32 i = 0; i < 384000; i++)
        pSystemIconTempBuffer2[i] = pSystemIconBuffer[SYSTEM_ICON_MYCOMPUTER][0x36 + i];

	//默认提供的BMP图标太大，所以在显示之前把图片缩小了下
    L1_GRAPHICS_ZoomImage(pSystemIconMyComputerBuffer, WidthNew, HeightNew, pSystemIconTempBuffer2, SYSTEM_ICON_WIDTH, SYSTEM_ICON_HEIGHT);
    
    int x1, y1;
    x1 = 20;
    y1 = 20;
	//在桌面显示我的电脑图标
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
	//在桌面显示系统设置图标
    L3_GRAPHICS_ItemPrint(pDeskBuffer, pSystemIconMySettingBuffer, ScreenWidth, ScreenHeight, WidthNew, HeightNew, x1, y1, "", 1, GRAPHICS_LAYER_DESK);

    
    y1 += HeightNew;
    // wo de dian nao
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x1, y1, (47 - 1) * 94 + 21 - 1, Color, ScreenWidth);
    x1 += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x1, y1, (45 - 1) * 94 + 19 - 1, Color, ScreenWidth);
    x1 += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x1, y1, (41 - 1) * 94 + 72 - 1, Color, ScreenWidth);
    x1 += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x1, y1, (54 - 1) * 94 + 35 - 1, Color, ScreenWidth);

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
	//在桌面显示回收站图标
    L3_GRAPHICS_ItemPrint(pDeskBuffer, pSystemIconRecycleBuffer, ScreenWidth, ScreenHeight, WidthNew, HeightNew, x1, y1, "", 1, GRAPHICS_LAYER_DESK);

    
    y1 += HeightNew;
    // wo de dian nao
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x1, y1, (27 - 1) * 94 + 56 - 1, Color, ScreenWidth);
    x1 += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x1, y1, (42 - 1) * 94 + 53 - 1, Color, ScreenWidth);
    x1 += 16;
    
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x1, y1, (53 - 1) * 94 + 30 - 1, Color, ScreenWidth);
    x1 += 16;
    
    
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

    x1 = ScreenWidth / 2;
    y1 = ScreenHeight - 21;
    
    //在桌面左下角显示中文件“任启红，加油。”字样，因为在系统实现的过程中，碰到无数的问题，希望自己能坚持下来。
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


    //在桌面左下角显示中文件“菜单”字样
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer,  16, ScreenHeight - 21,     (18 - 1) * 94 + 43 - 1, Color, ScreenWidth);
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer,  16 * 2, ScreenHeight - 21, (21 - 1) * 94 + 05 - 1, Color, ScreenWidth);
}






/****************************************************************************
*
*  描述:   xxxxx
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
*  描述:   xxxxx
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
*  描述:   图像素点拷贝不带reserved字段，前三个字段包含红、绿、蓝三种颜色，最后一种颜色是图层ID，因为鼠标是图层最上层，所以不需要图层ID
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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




/****************************************************************************
*
*  描述:   把所有图层叠加起来显示
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID L2_GRAPHICS_LayerCompute(UINT16 iMouseX, UINT16 iMouseY, UINT8 MouseClickFlag)
{
    /*L2_DEBUG_Print1(DISPLAY_X, DISPLAY_Y, "%d: pDeskDisplayBuffer: %X pDeskBuffer: %X ScreenWidth: %d ScreenHeight: %d pMouseBuffer: %X\n", __LINE__, 
                                                                pDeskDisplayBuffer,
                                                                pDeskBuffer,
                                                                ScreenWidth,
                                                                ScreenHeight,
                                                                pMouseBuffer);
    */
    //desk 
    L2_GRAPHICS_Copy(pDeskDisplayBuffer, pDeskBuffer, ScreenWidth, ScreenHeight, ScreenWidth, ScreenHeight, 0, 0);
    //L2_DEBUG_Print1(DISPLAY_X, DISPLAY_Y, "%d: GraphicsLayerCompute\n", __LINE__);

    if (TRUE == SystemQuitFlag)
    {
    
		L2_SCREEN_Draw(pDeskDisplayBuffer, 0, 0, 0, 0, ScreenWidth, ScreenHeight);	
                    
        return;
    }

    L2_GRAPHICS_CopyBufferFromWindowsToDesk();
        
	//这行代码为啥添加，不太记得了
    L2_MOUSE_Move();

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
                              
    //如果鼠标没有点击，则追踪鼠标所指的目标
    //因为桌面图层是没有相对桌面图层起始X，Y方向坐标
    if (0 == MouseMoveoverObject.GraphicsLayerID)
    {
        L2_GRAPHICS_RectangleDraw(pDrawBuffer, 
                                  DrawStartX,
                                  DrawStartY, 
                                  DrawEndX, 
                                  DrawEndY, 
                                  1,  
                                  MouseMoveoverObjectDrawColor, 
                                  ScreenWidth);
    }
    else
    {        
        L2_GRAPHICS_RectangleDraw(pDrawBuffer, 
                                  DrawStartX - DrawWindowStartX,
                                  DrawStartY - DrawWindowStartY, 
                                  DrawEndX - DrawWindowStartX, 
                                  DrawEndY - DrawWindowStartY,  
                                  1,  
                                  MouseMoveoverObjectDrawColor, 
                                  DrawWindowWidth);
    }
    
	//为了让鼠标光标透明，需要把图层对应的像素点拷贝到鼠标显示内存缓冲
    for (UINT8 i = 0; i < 16; i++)
    {
        for (UINT8 j = 0; j < 16; j++)
        {
			pMouseBuffer[(i * 16 + j) * 4]     = pDeskDisplayBuffer[((iMouseY + i) * ScreenWidth + iMouseX + j) * 4 + 0];
			pMouseBuffer[(i * 16 + j) * 4 + 1] = pDeskDisplayBuffer[((iMouseY + i) * ScreenWidth + iMouseX + j) * 4 + 1];
			pMouseBuffer[(i * 16 + j) * 4 + 2] = pDeskDisplayBuffer[((iMouseY + i) * ScreenWidth + iMouseX + j) * 4 + 2];
        }
	}

	//然后绘制鼠标光标
    L2_GRAPHICS_ChineseCharDraw(pMouseBuffer, 0, 0, 12 * 94 + 84, MouseColor, 16);

	//把鼠标光标显示到桌面
    L2_GRAPHICS_CopyNoReserved(pDeskDisplayBuffer, pMouseBuffer, ScreenWidth, ScreenHeight, 16, 16, iMouseX, iMouseY);
    //L2_DEBUG_Print1(DISPLAY_X, DISPLAY_Y, "%d: GraphicsLayerCompute\n", __LINE__);

	//把准备好的桌面缓冲区显示到屏幕
	L2_SCREEN_Draw(pDeskDisplayBuffer, 0, 0, 0, 0, ScreenWidth, ScreenHeight);	
}




/****************************************************************************
*
*  描述:   把文件夹目录下的文件和文件夹显示到我的电脑图层，当前这里边很多固定的值，不太好
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
	
    for (UINT16 i = 0; i < 32; i++)
    {       
        if (pItems[i].FileName[0] == 0)
            break;
            
        char name[13] = {0};
        x = 130;

        y = valid_count * (HeightNew + 16 * 2) + 200;

        for (UINT8 i = 0; i < 12; i++)
            name[i] = '\0';
        
        char ItemType[10] = "OTHER";
        L1_FILE_NameGet(i, name);
                
        if (pItems[i].Attribute[0] == 0x10) //Folder
        {
            //显示文件夹小图标
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
                                            name,
                                            L1_NETWORK_4BytesToUINT32(pItems[i].FileLength));
            FolderItemValidIndexArray[valid_count] = i;
            valid_count++;
        }
        else if (pItems[i].Attribute[0] == 0x20) //File
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
                                            name,
                                            L1_NETWORK_4BytesToUINT32(pItems[i].FileLength));
            
            //显示文件小图标
            L3_GRAPHICS_ItemPrint(pMyComputerBuffer, pSystemIconTextBuffer, MyComputerWidth, MyComputerHeight, WidthNew, HeightNew, x, y, "222", 2, GRAPHICS_LAYER_MY_COMPUTER_WINDOW);
			FolderItemValidIndexArray[valid_count] = i;
            valid_count++;
        }
    }       

}


/****************************************************************************
*
*  描述:   把文件夹目录下的文件和文件夹显示到我的电脑图层，当前这里边很多固定的值，不太好
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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

	//根目录显示的项数
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
            //显示文件夹小图标
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
            
            //显示文件小图标
            L3_GRAPHICS_ItemPrint(pMyComputerBuffer, pSystemIconTextBuffer, MyComputerWidth, MyComputerHeight, WidthNew, HeightNew, x, y, "222", 2, GRAPHICS_LAYER_MY_COMPUTER_WINDOW);
			FolderItemValidIndexArray[valid_count] = i;
            valid_count++;
        }
    }       

}



/****************************************************************************
*
*  描述:   多个图层叠加，根据图层从下往上，依次叠加起来，不过这里边涉及多图层，效率会比较底，另外，未判断图层未修改的部分不需要重新叠加。
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
void L2_GRAPHICS_CopyBufferFromWindowsToDesk()
{
    for (UINT16 i = 0; i < WindowLayers.LayerCount; i++)
    {
        UINT16 j = WindowLayers.LayerSequences[i];
        if (TRUE == WindowLayers.item[j].DisplayFlag)
        {
            L2_GRAPHICS_Copy(pDeskDisplayBuffer, WindowLayers.item[j].pBuffer, ScreenWidth, ScreenHeight, WindowLayers.item[j].WindowWidth, WindowLayers.item[j].WindowHeight, WindowLayers.item[j].StartX, WindowLayers.item[j].StartY);
        }
    }
}
							  



/****************************************************************************
*
*  描述:   显示分区根目录下的文件和目录，当前支持FAT32和NTFS两种格式
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID L2_STORE_PartitionItemsPrint(UINT16 PartitionItemID)
{
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: \n",  __LINE__);
    
    // this code may be have some problems, because my USB file system is FAT32, my Disk file system is NTFS.
    // others use this code must be careful...
    //UINT8 FileSystemType = L2_FILE_PartitionTypeAnalysis(PartitionItemID);

    if (device[PartitionItemID].FileSystemType == FILE_SYSTEM_FAT32)
    {
        L2_FILE_FAT32_DataSectorHandle(PartitionItemID);
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
		
		//当前测试，只显示一个设备，显示多个设备测试会比较麻烦
		//if (3 == DeviceID)
		//L2_PARTITION_FileContentPrint(BufferMFT);

		L2_FILE_NTFS_FileItemBufferAnalysis(BufferMFT, &NTFSFileSwitched);

		for (UINT16 i = 0; i < 10; i++)
		{
			//找到A0属性
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
*  描述:   在内存区域绘制Ascii字符，建议使用：L2_GRAPHICS_AsciiCharDraw2
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L2_GRAPHICS_AsciiCharDraw(UINT8 *pBufferDest,
        IN UINTN x0, UINTN y0, UINT8 c,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color)
{
    INT16 i;
    UINT8 d;
    UINT8 pBuffer[16 * 8 * 4];

    if ('\0' == c)
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

	//注：下边注释掉这几部分代码由于涉及太多IO操作，对效率影响较高，基本废弃。
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
*  描述:   在内存区域绘制Ascii字符
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L2_GRAPHICS_AsciiCharDraw2(WINDOW_LAYER_ITEM layer,
        IN UINTN x0, UINTN y0, UINT8 c,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color)
{
    INT16 i;
    UINT8 d;
    UINT8 pBuffer[16 * 8 * 4];

    if ('\0' == c)
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
*  描述:   鼠标右击显示一个小的窗口
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
		 
	for (i = 0; i < MouseClickWindowWidth * MouseClickWindowHeight; i++)
	{
		pMouseClickBuffer[i * 4] = 160;
		pMouseClickBuffer[i * 4 + 1] = 160;
		pMouseClickBuffer[i * 4 + 2] = 160;
		pMouseClickBuffer[i * 4 + 3] = LayerID;
	}
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
*  描述:   如果鼠标处于左击，并且鼠标在移动，并且鼠标所在的图层不在桌面层，那么移动鼠标所在的图层，类似窗口移动。
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
UINT16 L2_MOUSE_MoveOver(UINT16 LayerID)                                        
{            
	// Get click event
	UINT16 event = GraphicsLayerEventHandle[LayerID].pClickEventGet();


    return event;
}




/****************************************************************************
*
*  描述:   内存拷贝，颜色在本质上来讲，也就是内存
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
void L1_MEMORY_CopyColor1(UINT8 *pBuffer, EFI_GRAPHICS_OUTPUT_BLT_PIXEL color, UINT16 x0, UINT16 y0, UINT16 AreaWidth)
{
    pBuffer[y0 * AreaWidth * 4 + x0 * 4]     = color.Blue;
    pBuffer[y0 * AreaWidth * 4 + x0 * 4 + 1] = color.Green;
    pBuffer[y0 * AreaWidth * 4 + x0 * 4 + 2] = color.Red;
    pBuffer[y0 * AreaWidth * 4 + x0 * 4 + 3] = 0;

}




/****************************************************************************
*
*  描述:   内存拷贝，颜色在本质上来讲，也就是内存
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： 目标内存区域宽度固定
*
*  返回值： 成功：XXXX，失败：XXXXX
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
*  描述:   内存拷贝，颜色在本质上来讲，也就是内存
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： AreaWidth：目标内存区域的宽度
*
*  返回值： 成功：XXXX，失败：XXXXX
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
*  描述:   中文字符绘制
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
*  描述:   中文字符绘制函数，绘制结果是16*16像素大小
*
*  参数pBuffer： 		把中文字符写到的目标缓存
*  参数x0： 			把中文字符写到的X目标
*  参数y0： 			把中文字符写到的Y目标
*  参数offset： 		汉字库编码位移
*  参数Color： 		字体颜色
*  参数AreaWidth： 	目标缓存宽度，比如：在桌面上绘制传桌面的宽度，在我的电脑绘制传我的电脑宽度等等
*
*  返回值： 成功：XXXX，失败：XXXXX
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
*  描述:   中文字符绘制函数，绘制结果是16*16像素大小
*
*  参数pBuffer： 		把中文字符写到的目标缓存
*  参数x0： 			把中文字符写到的X目标
*  参数y0： 			把中文字符写到的Y目标
*  参数offset： 		汉字库编码位移
*  参数Color： 		字体颜色
*  参数AreaWidth： 	目标缓存宽度，比如：在桌面上绘制传桌面的宽度，在我的电脑绘制传我的电脑宽度等等
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L2_GRAPHICS_ChineseCharDraw2(UINT8 *pBuffer,
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
    
    ////DEBUG ((EFI_D_INFO, "\n"));
    
    return EFI_SUCCESS;
}


/****************************************************************************
*
*  描述:   中文字符绘制函数，绘制结果是12*12像素大小
*
*  参数pBuffer： 		把中文字符写到的目标缓存
*  参数x0： 			把中文字符写到的X目标
*  参数y0： 			把中文字符写到的Y目标
*  参数offset： 		汉字库编码位移
*  参数Color： 		字体颜色
*  参数AreaWidth： 	目标缓存宽度，比如：在桌面上绘制传桌面的宽度，在我的电脑绘制传我的电脑宽度等等
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L2_GRAPHICS_ChineseCharDraw12(UINT8 *pBuffer,
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
*  描述:   鼠标移动事件，其实可以添加鼠标移动到菜单的特效，不过当前暂时没空添加
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID L2_MOUSE_Move()
{   
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: GraphicsLayerMouseMove\n",  __LINE__);
    
    // display graphics layer id mouse over, for mouse click event.
    //L2_DEBUG_Print1(DISPLAY_X, DISPLAY_Y, "%d: Graphics Layer id: %d ", __LINE__, pDeskDisplayBuffer[(iMouseY * ScreenWidth + iMouseX) * 4 + 3]);
    L2_DEBUG_Print1(0, ScreenHeight - 30 -  7 * 16, "%d: iMouseX: %d iMouseY: %d MouseClickFlag: %d Graphics Layer id: %d GraphicsLayerIDCount: %u", __LINE__, iMouseX, iMouseY, MouseClickFlag, pDeskDisplayBuffer[(iMouseY * ScreenWidth + iMouseX) * 4 + 3], GraphicsLayerIDCount++);
    
	//获取鼠标光标所在的图层，窗口、图层在初始化的时候把第4个字节用于存放图层ID
	LayerID = pDeskDisplayBuffer[(iMouseY * ScreenWidth + iMouseX) * 4 + 3];

    UINT16 event = L2_MOUSE_MoveOver(LayerID);

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
*  描述:     显示日期、时间、星期几
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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

	//这边实际上有可以优化的空间，因为年月日和星期几每天只需要更新一次就行
	//  星   4839    期   3858
    x += 21 * 8 + 3;
    //L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x, y,  (48 - 1) * 94 + 39 - 1, Color, ScreenWidth); 
    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer, x, y, 48, 39, Color, ScreenWidth);
    
    x += 16;
    //L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x, y,  (38 - 1) * 94 + 58 - 1, Color, ScreenWidth);
    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer, x, y, 38, 58, Color, ScreenWidth);

    x += 16;

    UINT8 DayOfWeek = L1_MATH_DayOfWeek(EFITime.Year, EFITime.Month, EFITime.Day);
	
    UINT8 AreaCode = 0;
    UINT8 BitCode = 0;
    
    // 日 4053 一 5027 二 2294 三 4093 四 4336 五 4669 六 3389
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
    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer, x, y, AreaCode, BitCode, Color, ScreenWidth);
    
   L2_DEBUG_Print1(DISPLAY_DESK_HEIGHT_WEIGHT_X, DISPLAY_DESK_HEIGHT_WEIGHT_Y, "%d: ScreenWidth:%d, ScreenHeight:%d\n", __LINE__, ScreenWidth, ScreenHeight);
   /*
   GraphicsOutput->Blt(GraphicsOutput, 
                        (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) pDateTimeBuffer,
                        EfiBltBufferToVideo,
                        0, 0, 
                        0, 16 * 8, 
                        8 * 50, 16, 0);*/
}

