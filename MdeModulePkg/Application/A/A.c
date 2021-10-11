#define Note ""

/** //by renqihong

ToDo:
1. Read File From Path; OK====> optimize point: when blocks get from FAT table are continuous, can read from disk once, it can reduce, disk io.
2. Fix trigger event with mouse and keyboard; OK
3. NetWork connect baidu.com; ==> driver not found
4. File System Simple Management; ==> current can read and write, Sub directory read,NTFS almost ok.
5. progcess; ==> current mouse move, keyboard input, read file they can similar to process, but the real process is very complex, for example: 
    a.register push and pop, 
    b.progress communicate, 
    c.progress priority,
    d.PCB
    e.semaphoe
    etc.
6. My Computer window(disk partition)  
    ==> finish partly
    ==> read partitions root path items need to save to cache, for fastly get next time, and can reduce disk io...
    ==> partition volumn name
7. Setting window(select file, delete file, modify file) ==> 0%
8. Memory Simple Management 
    ==> can get memory information, but it looks like something wrong.
    ==> get memory information successfully.
9. Multi Windows, button click event. ==> 10%
10. Application. ==>10%
11. How to Automated Testing? ==>0%
12. Graphics run slowly. ===> need to fix the bug.
13. Desk wallpaper display successfully..
14. Need to rule naming, about function name, variable name, struct name, and etc.
    a.function name ruled
    b.
    c.
    d.
    e.


current problems:
    1. display NTFS file system root path items system will go die...
    2. display FAT32 file system root path items almost ok, display format not very beautiful.
    3. keyboard input not ok.

常用网址
1、区位码查询 http://quwei.911cha.com/
2、C语言代码格式化 http://web.chacuo.net/formatc

注意事项
1、存放EFI目录的分区一定要是FAT32格式
2、存放EFI分区大小扇区数一定要替换宏STORE_EFI_PATH_PARTITION_SECTOR_COUNT
3、桌面图片文件和HZK16文件一定要存放在这个分区的根目录
4、
5、

常见问题
1、SecureCRT不能连接LINUX UBUNTU，PING不通，需要把LINUX 网络断开重连下
2、
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Library/BaseLib.h>

#include <Guid/ConsoleInDevice.h>
#include <Guid/ConsoleOutDevice.h>
#include <Guid/FileSystemVolumeLabelInfo.h>
#include <Guid/GlobalVariable.h>
#include <Guid/HiiBootMaintenanceFormset.h>
#include <Guid/MdeModuleHii.h>
#include <Guid/ShellLibHiiGuid.h>
#include <Guid/TtyTerm.h>
#include <IndustryStandard/Pci.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/FileExplorerLib.h>
#include <Library/FileHandleLib.h>
#include <Library/HandleParsingLib.h>
#include <Library/HiiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/PeCoffGetEntryPointLib.h>
#include <Library/PrintLib.h>
#include <Library/ShellCEntryLib.h> 
#include <Library/ShellCommandLib.h>
#include <Library/ShellLib.h>
#include <Library/SortLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootManagerLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Pi/PiFirmwareFile.h>
#include <Pi/PiFirmwareVolume.h>
#include <Protocol/AbsolutePointer.h>
#include <Protocol/BusSpecificDriverOverride.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DevicePathToText.h>
#include <Protocol/DriverDiagnostics2.h>
#include <Protocol/DriverDiagnostics.h>
#include <Protocol/DriverFamilyOverride.h>
#include <Protocol/DriverHealth.h>
#include <Protocol/DriverSupportedEfiVersion.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/FormBrowserEx2.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/LoadFile.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PlatformDriverOverride.h>
#include <Protocol/PlatformToDriverConfiguration.h>
#include <Protocol/SerialIo.h>
#include <Protocol/Shell.h>
#include <Protocol/ShellParameters.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/SimplePointer.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/UnicodeCollation.h>
#include <Protocol/DiskIo.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DiskIo2.h>
#include <Protocol/BlockIo2.h>

#define STORE_EFI_PATH_PARTITION_SECTOR_COUNT 1691648

UINT16 date_time_count = 0;
UINT16 keyboard_count = 0;
UINT16 mouse_count = 0;
UINT16 parameter_count = 0;

int keyboard_input_count = 0;

#define KEYBOARD_BUFFER_LENGTH (30) 
char pKeyboardInputBuffer[KEYBOARD_BUFFER_LENGTH] = {0};

#define INFO_SELF(...)   \
            do {   \
                 Print(L"%d ",__LINE__);  \
                 Print(__VA_ARGS__); \
            }while(0);
            
/*
#define INFO_SELF(...)   \
            do {   \
                  Print(L"%d %a %a: ",__LINE__, __FILE__, __FUNCTION__);  \
                 Print(__VA_ARGS__); \
            }while(0);
*/

//Line 0
#define DISPLAY_DESK_HEIGHT_WEIGHT_X (date_time_count % 30) 
#define DISPLAY_DESK_HEIGHT_WEIGHT_Y (ScreenHeight - 16 * 3)



//Line 1
#define DISPLAY_MOUSE_X (0) 
#define DISPLAY_MOUSE_Y (ScreenHeight - 16 * 2  - 30)

//Line 2
#define DISPLAY_KEYBOARD_X (0) 
#define DISPLAY_KEYBOARD_Y (ScreenHeight - 16 * 3  - 30)

//Line 3
#define TEXT_DEVICE_PATH_X (0) 
#define TEXT_DEVICE_PATH_Y (16 * 4)

//Line 18
#define DISK_READ_X (0) 
#define DISK_READ_Y (16 * 19)

//Line 18
#define DISK_READ_X (0) 
#define DISK_READ_Y (16 * 19)

// Line 22
#define DISK_READ_BUFFER_X (0) 
#define DISK_READ_BUFFER_Y (6 * 56)

#define DISK_MBR_X (0) 
#define DISK_MBR_Y (16 * 62)


//last Line
#define DISPLAY_DESK_DATE_TIME_X (ScreenWidth - 22 * 8 - 16 * 3) 
#define DISPLAY_DESK_DATE_TIME_Y (ScreenHeight - 21)


#define DISK_BUFFER_SIZE (512)
#define DISK_BLOCK_BUFFER_SIZE (512 * 8)
#define DISK_MFT_BUFFER_SIZE (512 * 2 * 15)
#define EXBYTE 4

#define E820_RAM        1
#define E820_RESERVED       2
#define E820_ACPI       3
#define E820_NVS        4
#define E820_UNUSABLE       5

#define LLONG_MIN  (((INT64) -9223372036854775807LL) - 1)
#define LLONG_MAX   ((INT64)0x7FFFFFFFFFFFFFFFULL)

// For exception returned status 
#define DISPLAY_X (0) 
#define DISPLAY_Y (16 * (2 + DisplayCount++ % 52) )

// For exception returned status 
#define DISPLAY_ERROR_STATUS_X (ScreenWidth * 2 / 4) 
#define DISPLAY_ERROR_STATUS_Y (16 * (StatusErrorCount++ % (ScreenHeight / 16 - 3)) )

#define DISPLAY_LOG_ERROR_STATUS_X (4) 
#define DISPLAY_LOG_ERROR_STATUS_Y (16 * (LogStatusErrorCount++ % (SystemLogWindowHeight / 16 + 2)) )

#define FILE_SYSTEM_OTHER   0xff
#define FILE_SYSTEM_FAT32  1
#define FILE_SYSTEM_NTFS   2

#define MOUSE_NO_CLICKED 0
#define MOUSE_LEFT_CLICKED 1
#define MOUSE_RIGHT_CLICKED 2

#define SYSTEM_QUIT_FLAG FALSE

extern EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *gSimpleFileSystem; 

UINT16 LogStatusErrorCount = 0;

UINT16 StatusErrorCount = 0;
UINT16 DisplayCount = 0;
UINTN PartitionCount = 0;

CHAR8 x_move = 0;
CHAR8 y_move = 0;

UINT16 DebugPrintX = 0;
UINT16 DebugPrintY = 0; 

UINT8 MouseClickFlag = MOUSE_NO_CLICKED;
INT8 DisplayRootItemsFlag = 0;
INT8 DisplayMyComputerFlag = 0;
INT8 DisplaySystemLogWindowFlag = 0;
INT8 DisplaySystemSettingWindowFlag = 0;
INT8 DisplayMemoryInformationWindowFlag = 0;
INT8 SystemQuitFlag = FALSE;
INT8 DisplayStartMenuFlag = 0;
UINT8 PreviousItem = -1;


CHAR8    AsciiBuffer[0x100] = {0};


EFI_GRAPHICS_OUTPUT_PROTOCOL       *GraphicsOutput = NULL;
EFI_SIMPLE_POINTER_PROTOCOL        *gMouse;


EFI_HANDLE *handle;

typedef enum
{
    SYSTEM_ICON_MYCOMPUTER = 0,
    SYSTEM_ICON_SETTING,
    SYSTEM_ICON_RECYCLE,
    SYSTEM_ICON_FOLDER,
    SYSTEM_ICON_TEXT,
    SYSTEM_ICON_MAX 
}SYSTEM_ICON_320_400_BMP;

UINT8 *pDeskBuffer = NULL; //only Desk layer include wallpaper and button : 1
UINT8 *pMyComputerBuffer = NULL; // MyComputer layer: 2
UINT8 *pSystemLogWindowBuffer = NULL; // MyComputer layer: 2
UINT8 *pMemoryInformationBuffer = NULL; // MyComputer layer: 2
UINT8 *pDeskDisplayBuffer = NULL; //desk display after multi graphicses layers compute
UINT8 *pSystemIconBuffer[SYSTEM_ICON_MAX]; //desk display after multi graphicses layers compute
UINT8 *pSystemIconMyComputerBuffer = NULL; //after zoom in or zoom out
UINT8 *pSystemIconMySettingBuffer = NULL; //after zoom in or zoom out
UINT8 *pSystemIconRecycleBuffer = NULL; //after zoom in or zoom out
UINT8 *pMouseSelectedBuffer = NULL;  // after mouse selected
UINT8 *pMouseClickBuffer = NULL; // for mouse click 
UINT8 *pDateTimeBuffer = NULL; //Mouse layer: 3
UINT8 *pMouseBuffer = NULL; //Mouse layer: 4
UINT8 *pSystemIconFolderBuffer = NULL; //after zoom in or zoom out
UINT8 *pSystemIconTextBuffer = NULL; //after zoom in or zoom out
UINT8 *pSystemIconTempBuffer2 = NULL;


// Read File 
UINT8 ReadFileName[20];
UINT8 *pReadFileDestBuffer = NULL;
UINT8 ReadFileNameLength = 0;
UINT16 FileReadCount = 0;

UINT8 *pDeskWallpaperBuffer = NULL;
UINT8 *pTestBuffer = NULL;

EFI_EVENT MultiTaskTriggerGroup0Event;
EFI_EVENT MultiTaskTriggerGroup1Event;
EFI_EVENT MultiTaskTriggerGroup2Event;

int Nodei = 0;

UINT8 *FAT32_Table = NULL;

UINT8 *pStartMenuBuffer = NULL;
UINT8 *pSystemSettingWindowBuffer = NULL;

UINT8 *sChineseChar = NULL;
//UINT8 sChineseChar[267616];


static UINTN ScreenWidth, ScreenHeight;
UINT16 MyComputerWidth = 16 * 30;
UINT16 MyComputerHeight = 16 * 40;
UINT16 MyComputerPositionX = 300;
UINT16 MyComputerPositionY = 300;

UINT16 SystemLogWindowWidth = 16 * 30;
UINT16 SystemLogWindowHeight = 16 * 40;
UINT16 SystemLogWindowPositionX = 160;
UINT16 SystemLogWindowPositionY = 160;

UINT16 MemoryInformationWindowWidth = 16 * 30;
UINT16 MemoryInformationWindowHeight = 16 * 40;
UINT16 MemoryInformationWindowPositionX = 200;
UINT16 MemoryInformationWindowPositionY = 200;

UINT16 MouseClickWindowWidth = 300;
UINT16 MouseClickWindowHeight = 400;

UINT16 SystemSettingWindowWidth = 16 * 10;
UINT16 SystemSettingWindowHeight = 16 * 10;
UINT16 SystemSettingWindowPositionX = 50;
UINT16 SystemSettingWindowPositionY = 50;

UINT16 StartMenuWidth = 16 * 10;
UINT16 StartMenuHeight = 16 * 20;
UINT16 StartMenuPositionX;
UINT16 StartMenuPositionY;

UINT64 sector_count = 0;
UINT32 FileBlockStart = 0;
UINT32 BlockSize = 0;
UINT32 FileLength = 0;
UINT32 PreviousBlockNumber = 0;

UINT8 BufferMFT[DISK_BUFFER_SIZE * 2];

UINT16 iMouseX = 0;
UINT16 iMouseY = 0;

EFI_GRAPHICS_OUTPUT_BLT_PIXEL MouseColor;

UINT32 TimerSliceCount = 0;

UINT16 display_sector_number = 0;

#define SYSTEM_ICON_WIDTH 400
#define SYSTEM_ICON_HEIGHT 320

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

/* chinese char */
const UINT8 sChinese[][32] =
{   
    {0x04,0x80,0x0E,0xA0,0x78,0x90,0x08,0x90,0x08,0x84,0xFF,0xFE,0x08,0x80,0x08,0x90,
     0x0A,0x90,0x0C,0x60,0x18,0x40,0x68,0xA0,0x09,0x20,0x0A,0x14,0x28,0x14,0x10,0x0C},
    {0x00,0x02,0xBC,0x20,0x20,0x20,0xBF,0x24,0x24,0xA4,0x24,0x24,0xA4,0x44,0x44,0x84,
     0x08,0x04,0x3F,0x00,0x11,0x0A,0x7F,0x04,0x04,0x3F,0x04,0x15,0x24,0x44,0x14,0x08},
    {0x20,0x20,0xFC,0x24,0xFF,0x24,0xFC,0x20,0xFC,0x20,0x20,0xFE,0x20,0x20,0xFF,0x00,
     0x00,0x00,0x7D,0x04,0x0B,0x08,0x11,0x3C,0x05,0x04,0x24,0x1B,0x08,0x16,0x21,0x40},
    {0x00,0x80,0x80,0xFF,0x08,0x08,0x10,0x10,0x20,0x40,0x80,0x40,0x20,0x10,0x0C,0x03,
     0x01,0x00,0x00,0x7F,0x08,0x08,0x04,0x04,0x02,0x01,0x00,0x01,0x02,0x04,0x18,0x60},
    {0x10,0x10,0x90,0x90,0xFE,0x10,0x10,0x10,0x10,0xFF,0x10,0x10,0x10,0x10,0x10,0x10,
     0x04,0x04,0x04,0x08,0x08,0x19,0x19,0x2A,0x48,0x0B,0x08,0x08,0x08,0x08,0x08,0x08}, 
    {0x01,0x41,0x21,0x37,0x21,0x01,0x01,0xF7,0x11,0x11,0x12,0x12,0x14,0x28,0x47,0x00,
     0x10,0x10,0x10,0xFC,0x10,0x10,0x10,0xFE,0x10,0x10,0x10,0x10,0x10,0x00,0xFE,0x00},
    {0x10,0x08,0x08,0x08,0xC8,0x38,0x08,0x00,0x20,0x38,0x26,0x21,0x20,0x20,0x18,0x00,
     0x00,0x00,0x00,0xFE,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x7F,0x40,0x40,0x40,0x00},
};

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

typedef enum
{
    ALLOCATED_INFORMATION_DOMAIN_PHYSICAL_BLOCK_START = 0,
    ALLOCATED_INFORMATION_DOMAIN_PAGE_START,
    ALLOCATED_INFORMATION_DOMAIN_PAGE_COUNT,
    ALLOCATED_INFORMATION_DOMAIN_PHYSICAL_BLOCK_ID,
    ALLOCATED_INFORMATION_DOMAIN_MAX
}MEMORY_ALLOCATED_INFORMATION_DOMAIN;


// Index Header
typedef struct {
    UINT8 Flag[4]; //固定值 "INDX"
    UINT8 USNOffset[2];//更新序列号偏移
    UINT8 USNSize[2];//更新序列号和更新数组大小
    UINT8 LogSequenceNumber[8]; // 日志文件序列号(LSN)
    UINT8 IndexCacheVCN[8];//本索引缓冲区在索引分配中的VCN
    UINT8 IndexEntryOffset[4];//索引项的偏移 相对于当前位置
    UINT8 IndexEntrySize[4];//索引项的大小
    UINT8 IndexEntryAllocSize[4];//索引项分配的大小
    UINT8 HasLeafNode;//置一 表示有子节点
    UINT8 Fill[3];//填充
    UINT8 USN[2];//更新序列号
    UINT8 USNArray[0];//更新序列数组
}INDEX_HEADER;

typedef enum
{
    MEMORY_TYPE_GRAPHICS,
    MEMORY_TYPE_APPLICATION,
    MEMORY_TYPE_TEST
}MEMORY_TYPE;

typedef struct {
     UINT8 MFTReferNumber[8];//文件的MFT参考号, first 6 Bytes * 2 + MFT table sector = file sector 
     UINT8 IndexEntrySize[2];//索引项的大小
     UINT8 FileNameAttriBodySize[2];//文件名属性体的大小
     UINT8 IndexFlag[2];//索引标志
     UINT8 Fill[2];//填充
     UINT8 FatherDirMFTReferNumber[8];//父目录MFT文件参考号
     UINT8 CreatTime[8];//文件创建时间 8
     UINT8 AlterTime[8];//文件最后修改时间
     UINT8 MFTChgTime[8];//文件记录最后修改时间
     UINT8 ReadTime[8];//文件最后访问时间
     UINT8 FileAllocSize[8];//文件分配大小
     UINT8 FileRealSize[8];//文件实际大小
     UINT8 FileFlag[8];//文件标志
     UINT8 FileNameSize;//文件名长度
     UINT8 FileNamespace;//文件命名空间
     UINT8 FileNameAndFill[0];//文件名和填充
}INDEX_ITEM;

typedef struct{
    UINT32 efi_loader_signature;
    UINT32 efi_systab;
    UINT32 efi_memdesc_size;
    UINT32 efi_memdesc_version;
    UINT32 efi_memmap;
    UINT32 efi_memmap_size;
    UINT32 efi_systab_hi;
    UINT32 efi_memmap_hi;
}efi_info_self;

typedef struct
{
    UINT64 addr;        /* start of memory segment */
    UINT64 size;        /* size of memory segment */
    UINT32 type;        /* type of memory segment */
}e820_entry_self;

//save path and block information.
typedef struct
{
    UINT8  Path[50];
    UINT64 BlockNumber;
}PATH_DETAIL;

typedef struct 
{
    UINT16 ReservedSelector;
    UINT16 SectorsPerFat;   
    UINT16 BootPathStartCluster;
    UINT16 NumFATS;
    UINT16 SectorOfCluster;
}MasterBootRecordSwitched;

typedef struct 
{
    UINT8 FileName[8];
    UINT8 ExtensionName[3];

    // 00000000(Read/Write)
    // 00000001(ReadOnly)
    // 00000010(Hide)
    // 00000100(System)
    // 00001000(Volume name)
    // 00010000(Sub path)
    // 00100000(FILING: guidang)
    UINT8 Attribute[1];  // if 0x0FH then Long path structor
    UINT8 Reserved[1];
    UINT8 CreateTimeLow[1];
    UINT8 CreateTimeHigh[2];
    UINT8 CreateDate[2];
    UINT8 LatestVisitedDate[2];
    UINT8 StartClusterHigh2B[2];
    UINT8 LatestModiedTime[2];
    UINT8 LatestModiedDate[2];
    UINT8 StartClusterLow2B[2]; //*
    UINT8 FileLength[4];
}FAT32_ROOTPATH_SHORT_FILE_ITEM;

typedef struct
{
    UINT16 DeviceType; // 0 Disk, 1: USB, 2: Sata;
    UINT16 PartitionType; // 0 MBR, 1 GPT;
    UINT16 PartitionID; // a physics device consist of Several parts like c: d: e:
    UINT16 PartitionGUID; // like FA458FD2-4FF7-44D8-B542-BA560A5990B3
    UINT16 DeviceSquenceID; //0025384961B47ECD
    char Signare[50]; // MBR:0x077410A0
    long long StartSectorNumber; //0x194000
    long long SectorCount; //0xC93060

    //Partition parameter
    MasterBootRecordSwitched stMBRSwitched;

    // Path Stack
    PATH_DETAIL PathStack[12];

	//Items in folder include some folders and files
	FAT32_ROOTPATH_SHORT_FILE_ITEM pItems[32];
}DEVICE_PARAMETER;

DEVICE_PARAMETER device[10] = {0};

/*
    struct for FAT32 file system
*/

typedef struct
{
    UINT8 JMP[3] ; // 0x00 3 跳转指令（跳过开头一段区域）
    UINT8 OEM[8] ; // 0x03 8 OEM名称常见值是MSDOS5.0.
    UINT8 BitsOfSector[2] ; // 0x0b 2 每个扇区的字节数。取值只能是以下几种：512，1024，2048或是4096。设为512会取得最好的兼容性
    UINT8 SectorOfCluster[1] ; // 0x0d 1 每簇扇区数。 其值必须中2的整数次方，同时还要保证每簇的字节数不能超过32K
    UINT8 ReservedSelector[2] ; // 0x0e 2 保留扇区数（包括启动扇区）此域不能为0，FAT12/FAT16必须为1，FAT32的典型值取为32
    UINT8 NumFATS[1] ; // 0x10 1 文件分配表数目。 NumFATS，任何FAT格式都建议为2
    UINT8 RootPathRecords[2] ; // 0x11 2 最大根目录条目个数, 0 for fat32, 512 for fat16
    UINT8 AllSectors[2] ; // 0x13 2 总扇区数（如果是0，就使用偏移0x20处的4字节值）0 for fat32
    UINT8 Description[1] ; // 0x15 1 介质描述 0xF8 单面、每面80磁道、每磁道9扇区
    UINT8 xxx1[2] ; // 0x16 2 每个文件分配表的扇区（FAT16）,0 for fat32
    UINT8 xxx2[2] ; // 0x18 2 每磁道的扇区, 0x003f
    UINT8 xxx3[2] ; // 0x1a 2 磁头数，0xff
    UINT8 xxx4[4] ; // 0x1c 4 隐藏扇区, 与MBR中地址0x1C6开始的4个字节数值相等
    UINT8 SectorCounts[4] ; // 0x20 4 总扇区数（如果超过65535使用此地址，小于65536参见偏移0x13，对FAT32，此域必须是非0）
    UINT8 SectorsPerFat[4] ; // Sectors count each FAT use
    UINT8 Fat32Flag[2] ; // 0x28 2 Flags (FAT32特有)
    UINT8 FatVersion[2] ; // 0x2a 2 版本号 (FAT32特有)
    UINT8 BootPathStartCluster[4] ; // 0x2c 4 根目录起始簇 (FAT32)，一般为2
    UINT8 ClusterName[11] ; // 0x2b 11 卷标（非FAT32）
    UINT8 BootStrap[2] ; // 0x30 2 FSInfo 扇区 (FAT32) bootstrap
    UINT8 BootSectorBackup[2] ; // 0x32 2 启动扇区备份 (FAT32)如果不为0，表示在保留区中引导记录的备数据所占的扇区数，通常为6同时不建议使用6以外的其他数值
    UINT8 Reserved[2] ; // 0x34 2 保留未使用 (FAT32) 此域用0填充
    UINT8 FileSystemType[8] ; // 0x36 8 FAT文件系统类型（如FAT、FAT12、FAT16）含"FAT"就是PBR,否则就是MBR
    UINT8 SelfBootCode[2] ; // 0x3e 2 操作系统自引导代码
    UINT8 DeviceNumber[1] ; // 0x40 1 BIOS设备代号 (FAT32)
    UINT8 NoUse[1] ; // 0x41 1 未使用 (FAT32)
    UINT8 Flag[1] ; // 0x42 1 标记 (FAT32)
    UINT8 SequeenNumber[4] ; // 0x43 4 卷序号 (FAT32)
    UINT8 juanbiao[11] ; // 0x47 11 卷标（FAT32）
    UINT8 TypeOfFileSystem[8] ; // 0x52 8 FAT文件系统类型（FAT32）
    UINT8 BootAssembleCode[338]; // code
    UINT8 Partition1[16] ; // 0x1be 64 partitions table, DOS_PART_TBL_OFFSET
    UINT8 Partition2[16] ; // 0X1BE ~0X1CD 16 talbe entry for Partition 1
    UINT8 Partition3[16] ; // 0X1CE ~0X1DD 16 talbe entry for Partition 2
    UINT8 Partition4[16] ; // 0X1DE ~0X1ED 16 talbe entry for Partition 3
    UINT8 EndFlag[2] ; // 0x1FE 2 扇区结束符（0x55 0xAA） 结束标志：MBR的结束标志与DBR，EBR的结束标志相同。
}MasterBootRecord;

/*
    struct for NTFS file system
*/

//
// first sector of partition
typedef struct
{
    UINT8 JMP[3] ; // 0x00 3 跳转指令（跳过开头一段区域）
    UINT8 OEM[8] ; // 0x03 8 OEM名称常见值是MSDOS5.0, NTFS.

    // 0x0B
    UINT8 BitsOfSector[2];        //  0x0200　　扇区大小，512B
    UINT8 SectorOfCluster;            //  0x08　　  每簇扇区数，4KB
    UINT8 ReservedSelector[2];            // 　　　　　　保留扇区
    UINT8 NoUse01[5];            //
    
    // 0x15
    UINT8 Description;            //  0xF8     
    
    // 磁盘介质 -- 硬盘
    UINT8 NoUse02[2];            //
    
    // 0x18
    UINT8 SectorPerTrack[2];     // 　0x003F 　每道扇区数 63
    UINT8 Headers[2];           //　 0x00FF  磁头数
    UINT8 SectorsHide[4];          // 　0x3F　　隐藏扇区
    UINT8 NoUse03[8];           //
    
    // 0x28
    UINT8 AllSectorCount[8];        // 卷总扇区数, 高位在前, 低位在后
    
    // 0x30
    UINT8 MFT_StartCluster[8];      // MFT 起始簇
    UINT8 MFT_MirrStartCluster[8];  // MTF 备份 MFTMirr 位置
    
    //0x40
    UINT8 ClusterPerMFT[4];    // 每记录簇数 0xF6
    UINT8 ClusterPerIndex[4];    // 每索引簇数
    
    //0x48
    UINT8 SerialNumber[8];    // 卷序列号
    UINT8 CheckSum[8];    // 校验和
    UINT8 EndFlag[2];    // 0x1FE 2 扇区结束符（0x55 0xAA） 结束标志：MBR的结束标志与DBR，EBR的结束标志相同。
}DOLLAR_BOOT;

// 文件记录头
typedef struct
{
    /*+0x00*/ UINT8 Type[4];    // 固定值'FILE'
    /*+0x04*/ UINT8 USNOffset[2]; // 更新序列号偏移, 与操作系统有关
    /*+0x06*/ UINT8 USNCount[2]; // 固定列表大小Size in words of Update Sequence Number & Array (S)
    /*+0x08*/ UINT8 Lsn[8]; // 日志文件序列号(LSN)
    /*+0x10*/ UINT8 SequenceNumber[2]; // 序列号(用于记录文件被反复使用的次数)
    /*+0x12*/ UINT8 LinkCount[2];// 硬连接数
    /*+0x14*/ UINT8 AttributeOffset[2]; // 第一个属性偏移
    /*+0x16*/ UINT8 Flags[2];// flags, 00表示删除文件,01表示正常文件,02表示删除目录,03表示正常目录
    /*+0x18*/ UINT8 BytesInUse[4]; // 文件记录实时大小(字节) 当前MFT表项长度,到FFFFFF的长度+4
    /*+0x1C*/ UINT8 BytesAllocated[4]; // 文件记录分配大小(字节)
    /*+0x20*/ UINT8 BaseFileRecord[8]; // = 0 基础文件记录 File reference to the base FILE record
    /*+0x28*/ UINT8 NextAttributeNumber[2]; // 下一个自由ID号
    /*+0x2A*/ UINT8 Pading[2]; // 边界
    /*+0x2C*/ UINT8 MFTRecordNumber[4]; // windows xp中使用,本MFT记录号
    /*+0x30*/ UINT8 USN[2]; // 更新序列号
    /*+0x32*/ UINT8 UpdateArray[0]; // 更新数组
 } FILE_HEADER, *pFILE_HEADER; 

typedef struct  
{
    UINT8 Type[4];   //属性类型
    UINT8 Size[4];   //属性头和属性体的总长度
    UINT8 ResidentFlag; //是否是常驻属性（0常驻 1非常驻）
    UINT8 NameSize;   //属性名的长度
    UINT8 NameOffset[2]; //属性名的偏移 相对于属性头
    UINT8 Flags[2]; //标志（0x0001压缩 0x4000加密 0x8000稀疏）
    UINT8 Id[2]; //属性唯一ID
}CommonAttributeHeader;

typedef struct 
{
    UINT8 OccupyCluster;
    UINT64 Offset; //Please Note: The first item is start offset, and next item is relative offset....
}IndexInformation;

// Index of A0 attribute
IndexInformation A0Indexes[10] = {0};

typedef struct 
{
    UINT16 BitsOfSector;
    UINT16 SectorOfCluster; 
    UINT64 AllSectorCount;
    UINT64 MFT_StartCluster;
    UINT64 MFT_MirrStartCluster;
}DollarBootSwitched;


// MFT记录了整个卷的所有文件 (包括MFT本身、数据文件、文件夹等等) 信息，包括空间占用，文件基本属性，文件位置索引，创建时
// MFT 的第一项记录$MFT描述的是主分区表MFT本身，它的编号为0，MFT项的头部都是如下结构：
typedef struct {
     UINT8    mark[4];             // "FILE" 标志 
     UINT8    UsnOffset[2];        // 更新序列号偏移 　　　　30 00
     UINT8    usnSize[2];          // 更新序列数组大小+1 　 03 00
     UINT8    LSN[8];              // 日志文件序列号(每次记录修改后改变)  58 8E 0F 34 00 00 00 00
    // 0x10
     UINT8    SN[2];               // 序列号 随主文件表记录重用次数而增加
     UINT8    linkNum[2];          // 硬连接数 (多少目录指向该文件) 01 00
     UINT8    firstAttr[2];        // 第一个属性的偏移　　38 00
     UINT8    flags[2];            // 0已删除 1正常文件 2已删除目录 3目录正使用
    // 0x18
     UINT8    MftUseLen[4];        // 记录有效长度   　A8 01 00 00
     UINT8    maxLen[4];            // 记录占用长度 　 00 04 00 00
    // 0x20
     UINT8    baseRecordNum[8];     // 索引基本记录, 如果是基本记录则为0
     UINT8    nextAttrId[2];        // 下一属性Id　　07 00
     UINT8    border[2];            //
     UINT8    xpRecordNum[4];       // 用于xp, 记录号
    // 0x30
     UINT8    USN[8];                 // 更新序列号(2B) 和 更新序列数组
}MFT_HEADER;

/*
    MFT 是由一条条 MFT 项(记录)所组成的，而且每项大小是固定的(一般为1KB = 2 * 512)，MFT保留了前16项用于特殊文件记录，称为元数据，
    元数据在磁盘上是物理连续的，编号为0~15；如果$MFT的偏移为0x0C0000000, 那么下一项的偏移就是0x0C0000400，在下一项就是
    0x0C0000800等等；
*/

//------------------  属性头通用结构 ----
typedef struct  //所有偏移量均为相对于属性类型 Type 的偏移量
{
     UINT8 Type[4];           // 属性类型 0x10, 0x20, 0x30, 0x40,...,0xF0,0x100
     UINT8 Length[4];         // 属性的长度
     UINT8 NonResidentFiag;   // 是否是非常驻属性，l 为非常驻属性，0 为常驻属性 00
     UINT8 NameLength;        // 属性名称长度，如果无属性名称，该值为 00
     UINT8 ContentOffset[2];  // 属性内容的偏移量  18 00
     UINT8 CompressedFiag[2]; // 该文件记录表示的文件数据是否被压缩过 00 00
     UINT8 Identify[2];       // 识别标志  00 00
    //--- 0ffset: 0x10 ---
    //--------  常驻属性和非常驻属性的公共部分 ----
    union CCommon
    {
    
        //---- 如果该属性为 常驻 属性时使用该结构 ----
        struct CResident
        {
             UINT8 StreamLength[4];        // 属性值的长度, 即属性具体内容的长度。"48 00 00 00"
             UINT8 StreamOffset[2];        // 属性值起始偏移量  "18 00"
             UINT8 IndexFiag[2];           // 属性是否被索引项所索引，索引项是一个索引(如目录)的基本组成  00 00
        };
        
        //------- 如果该属性为 非常驻 属性时使用该结构 ----
        struct CNonResident
        {
             UINT8 StartVCN[8];            // 起始的 VCN 值(虚拟簇号：在一个文件中的内部簇编号,0起）
             UINT8 LastVCN[8];             // 最后的 VCN 值
             UINT8 RunListOffset[2];       // 运行列表的偏移量
             UINT8 CompressEngineIndex[2]; // 压缩引擎的索引值，指压缩时使用的具体引擎。
             UINT8 Reserved[4];
             UINT8 StreamAiiocSize[8];     // 为属性值分配的空间 ，单位为B，压缩文件分配值小于实际值
             UINT8 StreamRealSize[8];      // 属性值实际使用的空间，单位为B
             UINT8 StreamCompressedSize[8]; // 属性值经过压缩后的大小, 如未压缩, 其值为实际值
        };
    };
}NTFSAttribute;

typedef struct 
{
    UINT8 fileCreateTime[8];    // 文件创建时间
    UINT8 fileChangeTime[8];    // 文件修改时间
    UINT8 MFTChangeTime[8];     // MFT修改时间
    UINT8 fileLatVisited[8];    // 文件最后访问时间
    UINT8 tranAtrribute[4];     // 文件传统属性
    UINT8 otherInfo[28];        // 版本，所有者，配额，安全等等信息(详细略)
    UINT8 updataNum[8];         // 文件更新序列号
}Value0x10;


MasterBootRecordSwitched MBRSwitched;
DollarBootSwitched NTFSBootSwitched;


FAT32_ROOTPATH_SHORT_FILE_ITEM pItems[32];

typedef struct 
{
    UINT8 Reserved[1];
    UINT8 CreateTimeLow[1];
    UINT8 CreateTimeHigh[2];
    UINT8 CreateDate[2];
    UINT8 LatestVisitedDate[2];
    UINT8 StartClusterHigh2B[2];
    UINT8 LatestModiedTime[2];
    UINT8 LatestModiedDate[2];
    UINT8 StartClusterLow2B[2]; //*
    UINT8 FileLength[4];
}FAT32_ROOTPATH_SHORT_FILE_ITEMSwitched;


typedef struct 
{
    UINT8 FileName[8];
    UINT8 ExtensionName[3];
    UINT8 Attribute[1];  // if 0x0FH then Long path structor
    UINT8 Reserved[1];
    UINT8 CreateTimeLow[1];
    UINT8 CreateTimeHigh[2];
    UINT8 CreateDate[2];
    UINT8 LatestVisitedDate[2];
    UINT8 StartClusterHigh2B[2];
    UINT8 LatestModiedTime[2];
    UINT8 LatestModiedDate[2];
    UINT8 StartClusterLow2B[2]; //*
    UINT8 FileLength[4];
}FAT32_ROOTPATH_LONG_FILE_ITEM;

typedef struct 
{
    UINT16 ReservedSelector;
    UINT16 SectorsPerFat;   
    UINT16 BootPathStartCluster;
    UINT16 NumFATS;
}ROOT_PATH;

/*目录项首字节含义*/
typedef enum
{
    DirUnUsed            = 0x00,        /*本表项没有使用*/
    DirCharE5            = 0x05,        /*首字符为0xe5*/
    DirisSubDir          = 0x2e,        /*是一个子目录 .,..为父目录*/
    DirFileisDeleted     = 0xe5        /*文件已删除*/
}DirFirstChar;

typedef struct {
  UINTN               Signature;
  EFI_FILE_PROTOCOL   Handle;
  UINT64              Position;
  BOOLEAN             ReadOnly;
  LIST_ENTRY          Tasks;                  // List of all FAT_TASKs
  LIST_ENTRY          Link;                   // Link to other IFiles
} FAT_IFILE;

typedef struct {
  UINTN               Signature;
  EFI_FILE_IO_TOKEN   *FileIoToken;
  FAT_IFILE           *IFile;
  LIST_ENTRY          Subtasks;               // List of all FAT_SUBTASKs
  LIST_ENTRY          Link;                   // Link to other FAT_TASKs
} FAT_TASK;

typedef struct {
  UINTN               Signature;
  EFI_DISK_IO2_TOKEN  DiskIo2Token;
  FAT_TASK            *Task;
  BOOLEAN             Write;
  UINT64              Offset;
  VOID                *Buffer;
  UINTN               BufferSize;
  LIST_ENTRY          Link;
} DISK_HANDLE_TASK;

DISK_HANDLE_TASK  disk_handle_task;

#pragma  pack(1)
typedef struct     //这个结构体就是对上面那个图做一个封装。
{
    //bmp header
    UINT8  Signatue[2] ;   // B  M
    UINT8 FileSize[4] ;     //文件大小
    UINT8 Reserv1[2] ; 
    UINT8 Reserv2[2] ; 
    UINT8 FileOffset[4] ;   //文件头偏移量

    //DIB header
    UINT8 DIBHeaderSize[4] ; //DIB头大小
    UINT8 ImageWidth[4]   ;  //文件宽度
    UINT8 ImageHight[4]   ;  //文件高度
    UINT8 Planes[2]       ; 
    UINT8 BPP[2]          ;  //每个相素点的位数
    UINT8 Compression[4]  ; 
    UINT8 ImageSize[4]    ;  //图文件大小
    UINT8 XPPM[4] ; 
    UINT8 YPPM[4] ; 
    UINT8 CCT[4] ; 
    UINT8 ICC[4] ;          
}BMP_HEADER;
#pragma  pack()

typedef struct 
{
    UINT16 ButtonStartX;
    UINT16 ButtonStartY;
    UINT16 Width;
    UINT16 Height;
    UINT16 Type;  // Big, Small, 
}BUTTON;

typedef struct 
{
    UINT8 *pBuffer;
    UINT16 Width;
    UINT16 Height;
    UINT16 LastWidth;  // for switch between maximize and normal
    UINT16 LastHeight; // for switch between maximize and normal
    UINT16 CurrentX;   // new input text start with x
    UINT16 CurrentY;   // new input text start with y
    UINT16 TextStartX; // text start with x
    UINT16 TextStartY; // text start with y
    UINT16 WindowStartX; // Window left-top
    UINT16 WindowStartY; // Window left-top
    UINT16 Type;  // Big, Small, 
    
    UINT16 NextState; // maximize; minimize; other
    CHAR8 pTitle[50];
    BUTTON *pButtons;
} WINDOW;

// init -> partition analysised -> root path analysised -> read fat table -> start read file -> reading a file -> read finished
typedef enum 
{
    INIT_STATE = 0,
    GET_PARTITION_INFO_STATE,
    GET_ROOT_PATH_INFO_STATE,
    GET_FAT_TABLE_STATE,
    READ_FILE_STATE,
}STATE;

STATE   NextState = INIT_STATE;

typedef enum 
{
    READ_PATITION_EVENT = 0,
    READ_ROOT_PATH_EVENT,
    READ_FAT_TABLE_EVENT,
    READ_FILE_EVENT,
}EVENT;

int READ_FILE_FSM_Event = READ_PATITION_EVENT;

typedef struct
{
    STATE          CurrentState;
    EVENT          event;
    STATE          NextState;
    EFI_STATUS    (*pFunc)(); 
}STATE_TRANS;

//refer from EFI_MEMORY_DESCRIPTOR
typedef struct 
{
    ///
    /// Physical address of the first byte in the memory region. PhysicalStart must be
    /// aligned on a 4 KiB boundary, and must not be above 0xfffffffffffff000. Type
    /// EFI_PHYSICAL_ADDRESS is defined in the AllocatePages() function description
    ///
    EFI_PHYSICAL_ADDRESS  PhysicalStart;
    ///
    /// NumberOfPagesNumber of 4 KiB pages in the memory region.
    /// NumberOfPages must not be 0, and must not be any value
    /// that would represent a memory page with a start address,
    /// either physical or virtual, above 0xfffffffffffff000.
    ///
    UINT64                NumberOfPages;
    
    UINT64                FreeNumberOfPages;

    // Memory use flag page id 
    // no use: 0
    //    use: 1
    UINT8 *pMapper;
}MEMORY_CONTINUOUS;

typedef struct {
    UINT16 MemorySliceCount;
    MEMORY_CONTINUOUS MemoryContinuous[10];
    UINT64 AllocatedInformation[20][ALLOCATED_INFORMATION_DOMAIN_MAX];
    UINT16 CurrentAllocatedCount;
}MEMORY_INFORMATION;

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

WINDOW_LAYERS WindowLayers;

typedef enum 
{
    INIT_ACCESS_STATE = 0,    //
    ROOT_PATH_STATE,
    FILE_ITEM_ACCESS_STATE,
    FOLDER_ITEM_ACCESS_STATE,
    PARENT_PATH_ACCESS_STATE,
    MAX_ACCESS_STATE
}PARTITION_ITEM_ACCESS_STATE;

typedef enum 
{
    ROOT_PATH_ACCESS_EVENT = 0,
    FOLDER_ACCESS_EVENT,
    FILE_ACCESS_EVENT,
    PARENT_ACCESS_EVENT,
    CLOSE_ACCESS_EVENT,
    MAX_ACCESS_EVENT
}PARTITION_ITEM_ACCESS_EVENT;

typedef struct
{
    PARTITION_ITEM_ACCESS_STATE     CurrentState;
    PARTITION_ITEM_ACCESS_EVENT     event;
    PARTITION_ITEM_ACCESS_STATE     NextState;
    EFI_STATUS                      (*pFunc)(); 
}PARTITION_ITEM_ACCESS_STATE_TRANSFORM;

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

// fill into rectangle
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



// 小端模式
// byte转int  
UINT64 L1_NETWORK_8BytesToUINT64(UINT8 *bytes)
{
    UINT64 s = bytes[0];
    s += ((UINT64)bytes[1]) << 8;
    s += ((UINT64)bytes[2]) << 16;
    s += ((UINT64)bytes[3]) << 24;
    s += ((UINT64)bytes[4]) << 32;
    s += ((UINT64)bytes[5]) << 40;
    s += ((UINT64)bytes[6]) << 48;
    s += ((UINT64)bytes[7]) << 56;
    //printf("%llu\n",  s );

    return s;
}

UINT64 L1_NETWORK_6BytesToUINT64(UINT8 *bytes)
{
    UINT64 s = bytes[0];
    s += ((UINT64)bytes[1]) << 8;
    s += ((UINT64)bytes[2]) << 16;
    s += ((UINT64)bytes[3]) << 24;
    s += ((UINT64)bytes[4]) << 32;
    s += ((UINT64)bytes[5]) << 40;
    //printf("%llu\n",  s );

    return s;
}

UINT32 L1_NETWORK_4BytesToUINT32(UINT8 *bytes)
{
    UINT32 Result = bytes[0] & 0xFF;
    Result |= (bytes[1] << 8 & 0xFF00);
    Result |= ((bytes[2] << 16) & 0xFF0000);
    Result |= ((bytes[3] << 24) & 0xFF000000);
    return Result;
}

UINT32 L1_NETWORK_3BytesToUINT32(UINT8 *bytes)
{
    //INFO_SELF("%x %x %x\n", bytes[0], bytes[1], bytes[2]);
    UINT32 Result = bytes[0] & 0xFF;
    Result |= (bytes[1] << 8 & 0xFF00);
    Result |= ((bytes[2] << 16) & 0xFF0000);
    return Result;
}

UINT16 L1_NETWORK_2BytesToUINT16(UINT8 *bytes)
{
    UINT16 Result = bytes[0] & 0xFF;
    Result |= (bytes[1] << 8 & 0xFF00);
    return Result;
}


void L1_MEMORY_CopyColor1(UINT8 *pBuffer, EFI_GRAPHICS_OUTPUT_BLT_PIXEL color, UINT16 x0, UINT16 y0, UINT16 AreaWidth)
{
    pBuffer[y0 * AreaWidth * 4 + x0 * 4]     = color.Blue;
    pBuffer[y0 * AreaWidth * 4 + x0 * 4 + 1] = color.Green;
    pBuffer[y0 * AreaWidth * 4 + x0 * 4 + 2] = color.Red;
    pBuffer[y0 * AreaWidth * 4 + x0 * 4 + 3] = color.Reserved;

}

void L1_MEMORY_CopyColor2(UINT8 *pBuffer, EFI_GRAPHICS_OUTPUT_BLT_PIXEL color, UINT16 x0, UINT16 y0)
{
    pBuffer[y0 * 8 * 4 + x0 * 4]     = color.Blue;
    pBuffer[y0 * 8 * 4 + x0 * 4 + 1] = color.Green;
    pBuffer[y0 * 8 * 4 + x0 * 4 + 2] = color.Red;
    pBuffer[y0 * 8 * 4 + x0 * 4 + 3] = color.Reserved;
}

void L1_MEMORY_CopyColor3(UINT8 *pBuffer, EFI_GRAPHICS_OUTPUT_BLT_PIXEL color, UINT16 x0, UINT16 y0, UINT8 AreaWidth)
{
    pBuffer[y0 * AreaWidth * 4 + x0 * 4]     = color.Blue;
    pBuffer[y0 * AreaWidth * 4 + x0 * 4 + 1] = color.Green;
    pBuffer[y0 * AreaWidth * 4 + x0 * 4 + 2] = color.Red;
    pBuffer[y0 * AreaWidth * 4 + x0 * 4 + 3] = color.Reserved;
}

//refer from https://www.cnblogs.com/onepixel/articles/7674659.html
VOID L1_SORT_Bubble(UINT8 *arr, UINT16 len)
{   
    for(UINT16 i = 0; i < len - 1; i++) 
    {
       for(UINT16 j = 0; j < len - 1 - i; j++) 
       {
            // 相邻元素两两对比
           if(arr[j] > arr[j+1]) 
           {    
               // 元素交换
               UINT16 temp = arr[j+1];       
               arr[j+1] = arr[j];
               arr[j] = temp;
           }
       }
    }

}

INT32 L1_MATH_ABS(INT32 v)
{
    return (v < 0) ? -v : v;
}

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

// Note: Do not copy Color's reserved member
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


// Draw 8 X 16 point
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
        pBuffer[i] = 0x00;
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


EFI_STATUS L2_GRAPHICS_ChineseCharDraw2(UINT8 *pBuffer,
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

CHAR8 *L2_STRING_Maker4 (CONST CHAR8 *Format, VA_LIST VaList)
{
    for (UINT32 i = 0; i < 0x100; i++)
        AsciiBuffer[i] = 0;

    // Note this api do not supported ("%f", float)
    AsciiVSPrint (AsciiBuffer, sizeof (AsciiBuffer), Format, VaList);
    return AsciiBuffer;
}    


VOID L2_STRING_Maker (UINT16 x, UINT16 y,
  IN  CONST CHAR8   *Format,
  IN  VA_LIST       VaList
  )
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    UINT32 i = 0;
    
    for (i = 0; i < 0x100; i++)
        AsciiBuffer[i] = 0;
        
    Color.Blue = 0xFF;
    Color.Red = 0xFF;
    Color.Green = 0xFF;

    ASSERT (Format != NULL);

    // Note this api do not supported ("%f", float)
    AsciiVSPrint (AsciiBuffer, sizeof (AsciiBuffer), Format, VaList);
    /*
    if (StatusErrorCount % 61 == 0)
    {
        for (int j = 0; j < ScreenHeight - 25; j++)
        {
            for (int i = ScreenWidth / 2; i < ScreenWidth; i++)
            {
                pDeskBuffer[(j * ScreenWidth + i) * 4]     = 0x84;
                pDeskBuffer[(j * ScreenWidth + i) * 4 + 1] = 0x84;
                pDeskBuffer[(j * ScreenWidth + i) * 4 + 2] = 0x00;
            }
        }       
    }
    */
    /*
    if (DisplayCount % 52 == 0)
    {
        for (int j = 2; j < 54 * 16; j++)
        {
            for (int i = 0; i < ScreenWidth / 4; i++)
            {
                pDeskBuffer[(j * ScreenWidth + i) * 4]     = 0x84;
                pDeskBuffer[(j * ScreenWidth + i) * 4 + 1] = 0x84;
                pDeskBuffer[(j * ScreenWidth + i) * 4 + 2] = 0x00;
            }
        }       
    }
    */
    for (i = 0; i < sizeof(AsciiBuffer) /sizeof(CHAR8); i++)
        L2_GRAPHICS_AsciiCharDraw(pDeskBuffer, x + i * 8, y, AsciiBuffer[i], Color);

}


EFI_STATUS L2_GRAPHICS_AsciiCharDraw2(UINT8 *pBufferDest,
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
        pBuffer[i] = 0x00;
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

    L2_GRAPHICS_Copy(pBufferDest, pBuffer, MyComputerWidth, ScreenHeight, 8, 16, x0, y0);
    
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



VOID L2_STRING_Maker2 (UINT16 x, UINT16 y, WINDOW_LAYER_ITEM layer,
  IN  CONST CHAR8   *Format,
  IN  VA_LIST       VaList
  )
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    UINT32 i = 0;
    
    for (i = 0; i < 0x100; i++)
        AsciiBuffer[i] = 0;
        
    Color.Blue = 0xFF;
    Color.Red = 0xFF;
    Color.Green = 0xFF;
    Color.Reserved = layer.LayerID;

    ASSERT (Format != NULL);

    AsciiVSPrint (AsciiBuffer, sizeof (AsciiBuffer), Format, VaList);
            
    for (i = 0; i < sizeof(AsciiBuffer) /sizeof(CHAR8); i++)
        L2_GRAPHICS_AsciiCharDraw2(layer.pBuffer, x + i * 8, y, AsciiBuffer[i], Color);

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

/* Display a string */
CHAR8 *L2_STRING_Make5 (CONST CHAR8  *Format, ...)
{
    for (UINT32 i = 0; i < 0x100; i++)
        AsciiBuffer[i] = 0;
        
    VA_LIST         VaList;
    VA_START (VaList, Format);
    // Note this api do not supported ("%f", float)
    AsciiVSPrint (AsciiBuffer, sizeof (AsciiBuffer), Format, VaList);
    VA_END (VaList);    

    return AsciiBuffer;
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


EFI_STATUS L1_STRING_Compare(UINT8 *p1, UINT8 *p2, UINT16 length)
{
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: StrCmpSelf\n", __LINE__);
    
    for (int i = 0; i < length; i++)
    {
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%02X %02X ", p1[i], p2[i]);
        if (p1[i] != p2[i])
            return -1;
    }
    return EFI_SUCCESS;
}

//delete blanks of file name and file extension name
void L1_FILE_NameGet2(UINT8 deviceID, UINT8 *FileName)
{    
    UINT8 s[12] = {0};
    for (UINT8 i = 0; i < 11; i++)
    {       
        s[i] = pItems[deviceID].FileName[i];
    }
    UINT8 j = 0;
    for (UINT8 i = 0; i < 11; )
    {
        if (s[i] != 32)
            FileName[j++] = s[i++];
        else
            i++;
    }
}

void L1_FILE_NameGet(UINT8 ItemID, UINT8 *FileName)
 {    
    int count = 0;
    while (pItems[ItemID].FileName[count] != 0)
    {
        FileName[count] = pItems[ItemID].FileName[count];
        count++;
    }
    
    if (pItems[ItemID].ExtensionName[0] != 0)
    {
        FileName[count] = ' ';
        count++;
    }
    int count2 = 0;
    while (pItems[ItemID].ExtensionName[count2] != 0)
    {
        FileName[count] = pItems[ItemID].ExtensionName[count2];
        count++;
        count2++;
    }

 }

EFI_STATUS L1_FILE_RootPathAnalysis(UINT8 *p)
{
    memcpy(&pItems, p, DISK_BUFFER_SIZE);
    UINT16 valid_count = 0;
    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: .: %d\n", __LINE__, ReadFileName[7]);

    //display filing file and subpath. 
    for (int i = 0; i < 30; i++)
        if (pItems[i].FileName[0] != 0xE5 && (pItems[i].Attribute[0] == 0x20 
            || pItems[i].Attribute[0] == 0x10))
       {
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d FileName:%2c%2c%2c%2c%2c%2c%2c%2c ExtensionName:%2c%2c%2c StartCluster:%02X%02X%02X%02X FileLength: %02X%02X%02X%02X Attribute: %02X    ", __LINE__,
                                            pItems[i].FileName[0], pItems[i].FileName[1], pItems[i].FileName[2], pItems[i].FileName[3], pItems[i].FileName[4], pItems[i].FileName[5], pItems[i].FileName[6], pItems[i].FileName[7],
                                            pItems[i].ExtensionName[0], pItems[i].ExtensionName[1],pItems[i].ExtensionName[2],
                                            pItems[i].StartClusterHigh2B[0], pItems[i].StartClusterHigh2B[1],
                                            pItems[i].StartClusterLow2B[0], pItems[i].StartClusterLow2B[1],
                                            pItems[i].FileLength[0], pItems[i].FileLength[1], pItems[i].FileLength[2], pItems[i].FileLength[3],
                                            pItems[i].Attribute[0]);
            
            valid_count++;
            
            UINT8 FileName[13] = {0};
            UINT8 FileName2[13] = {0};
            L1_FILE_NameGet(i, FileName);
            L1_FILE_NameGet2(i, FileName2);
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FileName: %a\n", __LINE__, FileName);
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FileName2: %a\n", __LINE__, FileName2);
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: ReadFileName: %a\n", __LINE__, ReadFileName);

            //for (int j = 0; j < 5; j++)
            //  L2_DEBUG_Print1(j * 3 * 8, 16 * 40 + valid_count * 16, "%02X ", pItems[i].FileName[j]);
            if (L1_STRING_Compare(FileName2, ReadFileName, ReadFileNameLength) == EFI_SUCCESS)
            {
                L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d FileName:%2c%2c%2c%2c%2c%2c%2c%2c ExtensionName:%2c%2c%2c StartCluster:%02X%02X%02X%02X FileLength: %02X%02X%02X%02X Attribute: %02X    ",  __LINE__,
                                pItems[i].FileName[0], pItems[i].FileName[1], pItems[i].FileName[2], pItems[i].FileName[3], pItems[i].FileName[4], pItems[i].FileName[5], pItems[i].FileName[6], pItems[i].FileName[7],
                                pItems[i].ExtensionName[0], pItems[i].ExtensionName[1],pItems[i].ExtensionName[2],
                                pItems[i].StartClusterHigh2B[0], pItems[i].StartClusterHigh2B[1],
                                pItems[i].StartClusterLow2B[0], pItems[i].StartClusterLow2B[1],
                                pItems[i].FileLength[0], pItems[i].FileLength[1], pItems[i].FileLength[2], pItems[i].FileLength[3],
                                pItems[i].Attribute[0]);
                
              FileBlockStart = (UINT32)pItems[i].StartClusterHigh2B[0] * 16 * 16 * 16 * 16 + (UINT32)pItems[i].StartClusterHigh2B[1] * 16 * 16 * 16 * 16 * 16 * 16 + pItems[i].StartClusterLow2B[0] + (UINT32)pItems[i].StartClusterLow2B[1] * 16 * 16;
              FileLength = pItems[i].FileLength[0] + (UINT32)pItems[i].FileLength[1] * 16 * 16 + (UINT32)pItems[i].FileLength[2] * 16 * 16 * 16 * 16 + (UINT32)pItems[i].FileLength[3] * 16 * 16 * 16 * 16 * 16 * 16;

              L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FileBlockStart: %d FileLength: %ld\n", __LINE__, FileBlockStart, FileLength);
            }
        }

}

EFI_STATUS L1_FILE_RootPathAnalysis1(UINT8 *p)
{
    memcpy(&pItems, p, DISK_BUFFER_SIZE);
    UINT16 valid_count = 0;

    //display filing file and subpath. 
    for (int i = 0; i < 30; i++)
        //if (pItems[i].FileName[0] != 0xE5 && (pItems[i].Attribute[0] == 0x20 
        //    || pItems[i].Attribute[0] == 0x10))
       {
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d FileName:%2c%2c%2c%2c%2c%2c%2c%2c ExtensionName:%2c%2c%2c StartCluster:%02X%02X%02X%02X FileLength: %02X%02X%02X%02X Attribute: %02X    ",  __LINE__,
                                            pItems[i].FileName[0], pItems[i].FileName[1], pItems[i].FileName[2], pItems[i].FileName[3], pItems[i].FileName[4], pItems[i].FileName[5], pItems[i].FileName[6], pItems[i].FileName[7],
                                            pItems[i].ExtensionName[0], pItems[i].ExtensionName[1],pItems[i].ExtensionName[2],
                                            pItems[i].StartClusterHigh2B[0], pItems[i].StartClusterHigh2B[1],
                                            pItems[i].StartClusterLow2B[0], pItems[i].StartClusterLow2B[1],
                                            pItems[i].FileLength[0], pItems[i].FileLength[1], pItems[i].FileLength[2], pItems[i].FileLength[3],
                                            pItems[i].Attribute[0]);
            
            valid_count++;
        }
}

UINTN L1_STRING_Length(char *String)
{
    UINTN  Length;
    for (Length = 0; *String != '\0'; String++, Length++) ;
//   //DEBUG ((EFI_D_INFO, "%d Length: %d\n", __LINE__, Length));
    return Length;
}

// Analysis attribut A0 of $Root
EFI_STATUS  L2_FILE_NTFS_DollarRootA0DatarunAnalysis(UINT8 *p)
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: string length: %d\n", __LINE__,  L1_STRING_Length(p));
    UINT16 i = 0;
    UINT16 length = L1_STRING_Length(p);
    UINT8 occupyCluster = 0;
    UINT16 offset = 0;

    UINT16 Index = 0;
    while(i < length)
    {
        int  offsetLength  = p[i] >> 4;
        int  occupyClusterLength = p[i] & 0x0f;
         L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: occupyClusterLength: %d offsetLength: %d\n", __LINE__,  occupyClusterLength, offsetLength);
        
        i++;
        if (occupyClusterLength == 1)
            A0Indexes[Index].OccupyCluster = p[i];
        i++;
        //INFO_SELF(" i: %d\n", i);
        if (offsetLength == 1)
            offset = p[i];
        else if (offsetLength == 3)
        {
            UINT8 size[3];
            size[0] = p[i];
            size[1] = p[i + 1];
            size[2] = p[i + 2];
            offset = L1_NETWORK_3BytesToUINT32(size);
        }
        A0Indexes[Index].Offset = offset;
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d offset:%d \n", __LINE__, offset);
        i += offsetLength;
        Index++;
        //INFO_SELF(" i: %d\n", i);
    }

}

MEMORY_INFORMATION MemoryInformation = {0};


UINT8 L1_BIT_Set(UINT8 *pMapper, UINT64 StartPageID, UINT64 Size)
{
    UINT8 AddOneFlag = (Size % 8 == 0) ? 0 : 1;
    UINT32 ByteCount = Size / 8 + AddOneFlag;
}

VOID L1_MEMORY_Memset(void *s, UINT8 c, UINT32 n)
{
  UINT8 *d;

  d = s;

  while (n-- != 0) 
  {
    *d++ = c;
  }

  return s;
}

// memory: 1G->3G
#define ALL_PAGE_COUNT 0x80000
#define PHYSICAL_ADDRESS_START 0x40000000
#define ALLOCATE_UNIT_SIZE (8 * 512)
UINT64 FreeNumberOfPages = ALL_PAGE_COUNT;
UINT64 SystemAllPagesAllocated = 0;

UINT8 *pMapper = (UINT8 *)(PHYSICAL_ADDRESS_START);

typedef struct
{
    UINT8 ApplicationName[50];
    UINT32 AllocatedSize;
    UINT16 MemoryType;
}MEMORY_ALLOCATED_ITEMS;

typedef struct
{
    MEMORY_ALLOCATED_ITEMS items[100];
    UINT32 AllocatedSliceCount;
    UINT32 AllocatedSizeSum;
    UINT32 AllocatedPageSum;
}MEMORY_ALLOCATED_CURRENT;

MEMORY_ALLOCATED_CURRENT MemoryAllocatedCurrent;

void *L1_MEMORY_Copy(UINT8 *dest, const UINT8 *src, UINT8 count)
{
    UINT8 *d;
    const UINT8 *s;

    d = dest;
    s = src;
    while (count--)
        *d++ = *s++;        
    
    return dest;
}

void L2_MEMORY_CountInitial()
{
    MemoryAllocatedCurrent.AllocatedSliceCount = 0;
    MemoryAllocatedCurrent.AllocatedSizeSum = 0;
    MemoryAllocatedCurrent.AllocatedPageSum = 0;

    L1_MEMORY_SetValue(MemoryAllocatedCurrent, 0, sizeof(MemoryAllocatedCurrent));
}

UINT8 *L2_MEMORY_MapperInitial()
{

    for (UINT64 temp = 0; temp < ALL_PAGE_COUNT; temp++)
    {
        pMapper[temp] = 0; // no use
    }
}

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


// Find $Root file from all MFT(may be 15 file,)
// pBuffer store all MFT
EFI_STATUS  L2_FILE_NTFS_MFTDollarRootFileAnalysis(UINT8 *pBuffer)
{
    UINT8 *p = NULL;
    UINT8 pItem[200] = {0};
    
    p = (UINT8 *)AllocateZeroPool(DISK_BUFFER_SIZE * 2);
    if (p == NULL)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: p == NULL \n", __LINE__);
        return EFI_SUCCESS;
    }

    // Root file buffer copy
//  memcpy(p, pBuffer[512 * 2 * 5], DISK_BUFFER_SIZE * 2);

    for (int i = 0; i < DISK_BUFFER_SIZE * 2; i++)
        p[i] = pBuffer[i];

    //for (int i = 0; i < 7; i++)
    //{
        for (int j = 0; j < 512; j++)
        {
           //%02X: 8 * 3, 
          //L2_DEBUG_Print1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", p[j] & 0xff);
        }
    //}
    
    // File header length
    UINT16 AttributeOffset = L1_NETWORK_2BytesToUINT16(((FILE_HEADER *)p)->AttributeOffset);
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: AttributeOffset:%X \n", __LINE__, AttributeOffset);

    // location of a0 attribute may be in front of 10 
    // ofcourse mybe some bugs...
    for (int i = 0; i < 10; i++)
    {
        UINT8 size[4];

        // get Attribute size
        for (int i = 0; i < 4; i++)
            size[i] = p[AttributeOffset + 4 + i];
            
        //INFO_SELF("%02X%02X%02X%02X\n", size[0], size[1], size[2], size[3]);
        UINT16 AttributeSize = L1_NETWORK_4BytesToUINT32(size);

        // Copy attribute to buffer
        for (int i = 0; i < AttributeSize; i++)
            pItem[i] = p[AttributeOffset + i];

        // after buffer copied, we can get information in item
        UINT16  NameSize = ((CommonAttributeHeader *)pItem)->NameSize;
        
        UINT16  NameOffset = L1_NETWORK_2BytesToUINT16(((CommonAttributeHeader *)pItem)->NameOffset);
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Type[0]: %02X AttributeSize: %02X NameSize: %02X NameOffset: %02X\n", __LINE__, 
                                                            ((CommonAttributeHeader *)pItem)->Type[0],
                                                            AttributeSize,
                                                            NameSize,
                                                            NameOffset);   
                                                            
         //A0 attribute is very important for us to analysis root path items(file or folder)
         // ofcourse the other parameter of attribut can analysis, if you want
         if (0xA0 == ((CommonAttributeHeader *)pItem)->Type[0])
         {
            // every name char use two bytes.
            UINT16 DataRunsSize = AttributeSize - NameOffset - NameSize * 2;
            if (DataRunsSize > 100 || DataRunsSize < 0)
            {
                L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: DataRunsSize illegal.", __LINE__);
                return ;
            }
            UINT8 DataRuns[20] = {0};
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: A0 attribute has been found: ", __LINE__);
            for (int i = NameOffset; i < NameOffset + NameSize * 2; i++)
                L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%02X ", pItem[i] & 0xff);

            int j = 0;

            //get data runs
            for (int i = NameOffset + NameSize * 2; i < AttributeSize; i++)
            {           
                DataRuns[j] = pItem[i] & 0xff;
                L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%02X ", DataRuns[j] & 0xff);   
                j++;
            }
            L2_FILE_NTFS_DollarRootA0DatarunAnalysis(DataRuns);
            break;
         }
         
        AttributeOffset += AttributeSize;
    }
}

EFI_STATUS  L2_FILE_NTFS_MFTIndexItemsAnalysis(UINT8 *pBuffer)
{
    UINT8 *p = NULL;
    
    p = (UINT8 *)AllocateZeroPool(DISK_BUFFER_SIZE * 8);

    if (NULL == p)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d NULL == p\n", __LINE__);
        return EFI_SUCCESS;
    }
    
    //memcpy(p, pBuffer[512 * 2], DISK_BUFFER_SIZE);
    for (UINT16 i = 0; i < 512 * 8; i++)
        p[i] = pBuffer[i];

    //IndexEntryOffset:索引项的偏移 相对于当前位置
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d IndexEntryOffset: %llu IndexEntrySize: %llu\n", __LINE__, 
                                                                     L1_NETWORK_4BytesToUINT32(((INDEX_HEADER *)p)->IndexEntryOffset),
                                                                     L1_NETWORK_4BytesToUINT32(((INDEX_HEADER *)p)->IndexEntrySize));

    // 相对于当前位置 need to add size before this Byte.
    UINT8 length = L1_NETWORK_4BytesToUINT32(((INDEX_HEADER *)p)->IndexEntryOffset) + 24;
    UINT8 pItem[200] = {0};
    UINT16 index = length;

    for (UINT8 i = 0; ; i++)
    {    
         if (index >= L1_NETWORK_4BytesToUINT32(((INDEX_HEADER *)p)->IndexEntrySize))
            break;
            
         //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%s index: %d\n", __LINE__,  index);  
         UINT16 length2 = pBuffer[index + 8] + pBuffer[index + 9] * 16;
         
        for (int i = 0; i < length2; i++)
            pItem[i] = pBuffer[index + i];
            
         UINT8 FileNameSize =    ((INDEX_ITEM *)pItem)->FileNameSize;
         UINT8 FileContentRelativeSector =   L1_NETWORK_6BytesToUINT64(((INDEX_ITEM *)pItem)->MFTReferNumber);
         //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d attribut length2: %d FileNameSize: %d\n", __LINE__, 
        //                                                           length2,
        //                                                           FileNameSize);    
         UINT8 attributeName[20];                                                                    
         for (int i = 0; i < FileNameSize; i++)
         {
            attributeName[i] = pItem[82 + 2 * i];
         }
         L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: attributeName: %a, FileContentRelativeSector: %llu\n", __LINE__, attributeName, FileContentRelativeSector);
         //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%s attributeName: %a\n", __LINE__,  attributeName);  
         index += length2;
    }
}

int L1_STRING_IsSpace (int c)
{
  //
  // <space> ::= [ ]
  //
  return ((c) == ' ') || ((c) == '\t') || ((c) == '\r') || ((c) == '\n') || ((c) == '\v')  || ((c) == '\f');
}

int L1_STRING_IsAllNumber (int c)
{
  //
  // <alnum> ::= [0-9] | [a-z] | [A-Z]
  //
  return ((('0' <= (c)) && ((c) <= '9')) ||
          (('a' <= (c)) && ((c) <= 'z')) ||
          (('A' <= (c)) && ((c) <= 'Z')));
}


int
L1_STRING_ToUpper(
  IN  int c
  )
{
  if ( (c >= 'a') && (c <= 'z') ) {
    c = c - ('a' - 'A');
  }
  return c;
}

int
L1_MATH_DigitToInteger( int c)
{
  if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))) {  /* If c is one of [A-Za-z]... */
    c = L1_STRING_ToUpper(c) - 7;   // Adjust so 'A' is ('9' + 1)
  }
  return c - '0';   // Value returned is between 0 and 35, inclusive.
}


long long L1_MATH_Multi(long x, long y)
{
    return x * y;
}

// string transform into long long
long long
L1_MATH_StringToLongLong(const char * nptr, char ** endptr, int base)
{
  const char *pEnd;
  long long   Result = 0;
  long long   Previous;
  int         temp;
  BOOLEAN     Negative = FALSE;

  pEnd = nptr;
    
  base = 16; 
  ++nptr;
  ++nptr;
  //DEBUG ((EFI_D_INFO, "line:%d device->StartSectorNumber: %a base: %d \n", __LINE__, nptr, base));

  while( L1_STRING_IsAllNumber(*nptr) && ((temp = L1_MATH_DigitToInteger(*nptr)) < base)) 
  {
    //DEBUG ((EFI_D_INFO, "line:%d temp: %d, Result: %d \n", __LINE__, temp, Result));
    Previous = Result;
    Result = L1_MATH_Multi (Result, base) + (long long int)temp;
    if( Result <= Previous) 
    {   // Detect Overflow
      if(Negative) 
      {
        //DEBUG ((EFI_D_INFO, "line:%d Negative!!!!\n", __LINE__, temp, Result));
        Result = LLONG_MIN;
      }
      else 
      {
        Result = LLONG_MAX;
      }
      Negative = FALSE;
      break;
    }
    pEnd = ++nptr;
  }

  //DEBUG ((EFI_D_INFO, "line:%d device->StartSectorNumber: %d \n", __LINE__, Result));
  if(Negative) 
  {
    Result = -Result;
  }

  // Save pointer to final sequence
  if(endptr != NULL) 
  {
    *endptr = (char *)pEnd;
  }

  //DEBUG ((EFI_D_INFO, "line:%d temp: %d, Result: %d \n", __LINE__, temp, Result));
  return Result;
}


void L2_STORE_TextDevicePathAnalysis(CHAR16 *p, DEVICE_PARAMETER *device, UINTN count1)
{
    int length = StrLen(p);
    int i = 0;
    //L2_DEBUG_Print1(0, 11 * 16, "line: %d string: %s, length: %d\n", __LINE__, p, length);
    for (i = 0; i < length - 3; i++)
    { 
        //USB
        if (p[i] == 'U' && p[i + 1] == 'S' && p[i + 2] == 'B')
        {
            device->DeviceType = 1;
            break;
        }
            
        //Sata
        if (p[i] == 'S' && p[i + 1] == 'a' && p[i + 2] == 't' && p[i + 3] == 'a')
        {
            device->DeviceType = 2;
            break;
        }
        
        //NVMe
        if (p[i] == 'N' && p[i + 1] == 'V' && p[i + 2] == 'M' && p[i + 3] == 'e')
        {
            device->DeviceType = 2;
            break;
        }
    }

    //printf("line:%d i: %d\n", __LINE__, i);
    
    //HD
    while (i++)
        if (p[i] == 'H' && p[i + 1] == 'D')
            break;
            
    //printf("line:%d i: %d\n", __LINE__, i);
    if (i >= length)
        return;
                
    //printf("line:%d i: %d\n", __LINE__, i);
    device->PartitionID = p[i + 3] - '0';
    
    i+=3;
    
    while (i++)
    {
        //MBR
        if (p[i] == 'M' && p[i + 1] == 'B' && p[i + 2] == 'R')
        {
            device->PartitionType = 1;
            break;
        }
            
        //GPT
        if (p[i] == 'G' && p[i + 1] == 'P' && p[i + 2] == 'T')
        {
            device->PartitionType = 2;    
            break;
        }
    }
        
    i+=3;
    
    //printf("line:%d i: %d\n", __LINE__, i);
    int commalocation[2];
    int count = 0;
    for (int j = i + 1; p[j] != ')'; j++)
    {
        if (p[j] == ',')
        {
           commalocation [count++] = j;
           //printf("line:%d, j: %d, data: %c%c\n", __LINE__, j, p[j], p[j+1]);
        }
    }
    
    char temp[20];
    int j = 0;
    //start
    for (i = commalocation[0] + 1; i < commalocation[1]; i++)
    {
        temp[j++] = p[i];
        //printf("%c", p[i]);
    }
    temp[j] = '\0';
    //strtol(const char * nptr,char * * endptr,int base)
    //puts(temp);
    //DEBUG ((EFI_D_INFO, "line:%d device->StartSectorNumber: %a \n", __LINE__, temp));
    //printf("line:%d start: %d\n", __LINE__, strtol(temp));
    device->StartSectorNumber = L1_MATH_StringToLongLong(temp, NULL, 10);
    //putchar('\n');
    
    j = 0;
    //length
    for (i = commalocation[1] + 1; i < length - 1; i++)
    {
        temp[j++] = p[i];
        //printf("%c", p[i]);
    }
    temp[j] = '\0';
    //puts(temp);
    //printf("line:%d end: %d\n", __LINE__, strtol(temp));
    device->SectorCount = L1_MATH_StringToLongLong(temp, NULL, 10);;
    //DEBUG ((EFI_D_INFO, "line:%d device->SectorCount: %a \n", __LINE__, temp));
    //putchar('\n');
    
    //L2_DEBUG_Print1(0, 11 * 16, "line: %d string: %s, length: %d\n",       __LINE__, p, strlen(p));

    // second display
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: i: %d Start: %d Count: %d  DeviceType: %d PartitionType: %d PartitionID: %d\n",        
                                __LINE__, 
                                count1,
                                device->StartSectorNumber,
                                device->SectorCount,
                                device->DeviceType,
                                device->PartitionType,
                                device->PartitionID);
                                
    //DEBUG ((EFI_D_INFO, "line:%d device->DeviceType: %d \n", __LINE__, device->DeviceType));
    //DEBUG ((EFI_D_INFO, "line:%d device->PartitionType: %d \n", __LINE__, device->PartitionType));
    //DEBUG ((EFI_D_INFO, "line:%d device->PartitionID: %d \n", __LINE__, device->PartitionID));
    //DEBUG ((EFI_D_INFO, "line:%d device->StartSectorNumber: %d \n", __LINE__, device->StartSectorNumber));
    //DEBUG ((EFI_D_INFO, "line:%d device->SectorCount: %d \n", __LINE__, device->SectorCount));
    //putchar('\n');
}

//Note: ReadSize will be multi with 512
EFI_STATUS L1_STORE_READ(UINT8 deviceID, UINT64 StartSectorNumber, UINT16 ReadSize, UINT8 *pBuffer)
{
    if (StartSectorNumber > device[deviceID].SectorCount)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: deviceID: %d StartSectorNumber: %ld ReadSize: %d\n", __LINE__, deviceID, StartSectorNumber, ReadSize);
        return -1;
    }

    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: deviceID: %d StartSectorNumber: %ld ReadSize: %d\n", __LINE__, deviceID, StartSectorNumber, ReadSize);
    EFI_STATUS Status;
    UINTN NumHandles;
    EFI_BLOCK_IO_PROTOCOL *BlockIo;
    EFI_DISK_IO_PROTOCOL *DiskIo;
    EFI_HANDLE *ControllerHandle = NULL;
    
    Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiDiskIoProtocolGuid, NULL, &NumHandles, &ControllerHandle);
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);      
        return Status;
    }

    Status = gBS->HandleProtocol(ControllerHandle[deviceID], &gEfiBlockIoProtocolGuid, (VOID * *) &BlockIo );   
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
        return Status;
    }
    
    Status = gBS->HandleProtocol( ControllerHandle[deviceID], &gEfiDiskIoProtocolGuid, (VOID * *) &DiskIo );     
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
        return Status;
    }

    Status = DiskIo->ReadDisk(DiskIo, BlockIo->Media->MediaId, DISK_BUFFER_SIZE * (StartSectorNumber), DISK_BUFFER_SIZE * ReadSize, pBuffer);
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X sector_count:%ld\n", __LINE__, Status, sector_count);
        return Status;
    }
    
    for (int j = 0; j < 250; j++)
    {
        //L2_DEBUG_Print1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", pBuffer[j] & 0xff);
    }
    //INFO_SELF("\n");
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
    return EFI_SUCCESS;
}


//Note: ReadSize will be multi with 512
EFI_STATUS L1_STORE_Write(UINT8 deviceID, UINT64 StartSectorNumber, UINT16 WriteSize, UINT8 *pBuffer)
{
    if (StartSectorNumber > device[deviceID].SectorCount)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: deviceID: %d StartSectorNumber: %ld ReadSize: %d\n", __LINE__, deviceID, StartSectorNumber, WriteSize);
        return -1;
    }

    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: deviceID: %d StartSectorNumber: %ld ReadSize: %d\n", __LINE__, deviceID, StartSectorNumber, ReadSize);
    EFI_STATUS Status;
    UINTN NumHandles;
    EFI_BLOCK_IO_PROTOCOL *BlockIo;
    EFI_DISK_IO_PROTOCOL *DiskIo;
    EFI_HANDLE *ControllerHandle = NULL;
    
    Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiDiskIoProtocolGuid, NULL, &NumHandles, &ControllerHandle);
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);      
        return Status;
    }

    Status = gBS->HandleProtocol(ControllerHandle[deviceID], &gEfiBlockIoProtocolGuid, (VOID * *) &BlockIo );   
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
        return Status;
    }
    
    Status = gBS->HandleProtocol( ControllerHandle[deviceID], &gEfiDiskIoProtocolGuid, (VOID * *) &DiskIo );     
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
        return Status;
    }

    Status = DiskIo->WriteDisk(DiskIo, BlockIo->Media->MediaId, DISK_BUFFER_SIZE * (StartSectorNumber), DISK_BUFFER_SIZE * WriteSize, pBuffer);
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X sector_count:%ld\n", __LINE__, Status, sector_count);
        return Status;
    }
    
    for (int j = 0; j < 250; j++)
    {
        //L2_DEBUG_Print1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", pBuffer[j] & 0xff);
    }
    //INFO_SELF("\n");
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
    return EFI_SUCCESS;
}


// NTFS Main File Table items analysis
// MFT_Item_ID: 0 $MFT
/*             1 $MFTMirr
                 2 $LogFile
                 3 $Volume
                 4 $AttrDef
               5 $ROOT
               etc 
*/
EFI_STATUS L2_FILE_NTFS_MFT_Item_Read(UINT16 DeviceID, UINT16 MFT_Item_ID)
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d DeviceID: %d\n", __LINE__, DeviceID);
    //printf( "RootPathAnalysis\n" );
    //DEBUG ((EFI_D_INFO, "PartitionUSBRead!!\r\n"));
    EFI_STATUS Status ;
    EFI_HANDLE *ControllerHandle = NULL;

    //sector_count is MFT start sector, 5 * 2 means $ROOT sector...
    //every MFT Item use 2 sector .
    Status = L1_STORE_READ(DeviceID, sector_count + MFT_Item_ID * 2, 2, BufferMFT); 
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
        return Status;
    }
    
     for (int j = 0; j < 250; j++)
     {
        //L2_DEBUG_Print1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", BufferMFT[j] & 0xff);
     }  

    //Analysis MFT of NTFS File System..
    //L2_FILE_NTFS_MFTDollarRootFileAnalysis(BufferMFT);
    //MFTDollarRootAnalysisBuffer(BufferMFTDollarRoot); 

    // data area start from 1824, HZK16 file start from     FileBlockStart  block, so need to convert into sector by multi 8, block start number is 2   
    // next state is to read FAT table
    // sector_count = MBRSwitched.ReservedSelector;
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: sector_count:%ld FileLength: %d MBRSwitched.ReservedSelector:%ld\n",  __LINE__, sector_count, FileLength, MBRSwitched.ReservedSelector);
      
    return EFI_SUCCESS;
}


EFI_STATUS L1_FILE_FAT32_DataSectorAnalysis(UINT8 *p, MasterBootRecordSwitched *pMBRSwitched)
{
    MasterBootRecord *pMBR;
    
    pMBR = (MasterBootRecord *)AllocateZeroPool(DISK_BUFFER_SIZE);
    memcpy(pMBR, p, DISK_BUFFER_SIZE);

    // 大端字节序：低位字节在高地址，高位字节低地址上。这是人类读写数值的方法。
    // 小端字节序：与上面相反。低位字节在低地址，高位字节在高地址。
/*
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d ReservedSelector:%02X%02X SectorsPerFat:%02X%02X%02X%02X BootPathStartCluster:%02X%02X%02X%02X NumFATS: %X", 
                                        __LINE__,
                                        pMBR->ReservedSelector[0], pMBR->ReservedSelector[1], 
                                        pMBR->SectorsPerFat[0], pMBR->SectorsPerFat[1], pMBR->SectorsPerFat[2], pMBR->SectorsPerFat[3],
                                        pMBR->BootPathStartCluster[0], pMBR->BootPathStartCluster[1], pMBR->BootPathStartCluster[2], pMBR->BootPathStartCluster[3],
                                        pMBR->NumFATS[0]);

    */
    L2_FILE_Transfer(pMBR, pMBRSwitched);

    FreePool(pMBR);
}


// analysis a partition 
EFI_STATUS L2_FILE_FAT32_DataSectorHandle(UINT16 DeviceID)
{
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d DeviceID: %d\n", __LINE__, DeviceID);
    //printf( "RootPathAnalysis\n" );
    EFI_STATUS Status ;
    UINT8 Buffer1[DISK_BUFFER_SIZE];
    
    Status = L1_STORE_READ(DeviceID, sector_count, 1, Buffer1); 
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
        return Status;
    }
    
    //When get root path data sector start number, we can get content of root path.
    L1_FILE_FAT32_DataSectorAnalysis(Buffer1, &MBRSwitched);    

    // data area start from 1824, HZK16 file start from     FileBlockStart  block, so need to convert into sector by multi 8, block start number is 2   
    // next state is to read FAT table
    sector_count = MBRSwitched.ReservedSelector;
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: sector_count:%ld FileLength: %d MBRSwitched.ReservedSelector:%ld\n",  __LINE__, sector_count, FileLength, MBRSwitched.ReservedSelector);

    return EFI_SUCCESS;
}
EFI_STATUS L2_FILE_NTFS_RootPathItemsRead(UINT8 i)
{
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d DeviceType: %d, SectorCount: %lld\n", __LINE__, i);
    EFI_STATUS Status ;
    UINT8 BufferBlock[DISK_BLOCK_BUFFER_SIZE];

    UINT8 k = 0;
    UINT16 lastOffset = 0;
    /*
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Offset:%ld OccupyCluster:%ld\n",  __LINE__, 
                                                                     (A0Indexes[k].Offset + lastOffset) * 8, 
                                                                     A0Indexes[k].OccupyCluster * 8);
    */
    // cluster need to multi with 8 then it is sector.
   Status = L1_STORE_READ(i, (A0Indexes[k].Offset + lastOffset) * 8 , A0Indexes[k].OccupyCluster * 8, BufferBlock);
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
        return Status;
    }

    L2_FILE_NTFS_MFTIndexItemsAnalysis(BufferBlock);    
    
    lastOffset = A0Indexes[k].Offset;
    
    return EFI_SUCCESS;
}


VOID L1_FILE_NTFS_DollerRootTransfer(DOLLAR_BOOT *pSource, DollarBootSwitched *pDest)
{
    pDest->BitsOfSector = L1_NETWORK_2BytesToUINT16(pSource->BitsOfSector);
    pDest->SectorOfCluster = (UINT16)pSource->SectorOfCluster;
    pDest->AllSectorCount = L1_NETWORK_8BytesToUINT64(pSource->AllSectorCount) + 1;
    pDest->MFT_StartCluster = L1_NETWORK_8BytesToUINT64(pSource->MFT_StartCluster);
    pDest->MFT_MirrStartCluster = L1_NETWORK_8BytesToUINT64(pSource->MFT_MirrStartCluster);
    
    /*
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d BitsOfSector:%ld SectorOfCluster:%d AllSectorCount: %llu MFT_StartCluster:%llu MFT_MirrStartCluster:%llu", __LINE__,
                                                pDest->BitsOfSector,
                                                pDest->SectorOfCluster,
                                                pDest->AllSectorCount,
                                                pDest->MFT_StartCluster,
                                                pDest->MFT_MirrStartCluster);
    */
}


EFI_STATUS L2_FILE_NTFS_FirstSelectorAnalysis(UINT8 *p, DollarBootSwitched *pNTFSBootSwitched)
{
    DOLLAR_BOOT *pDollarBoot;
    
    pDollarBoot = (DOLLAR_BOOT *)AllocateZeroPool(DISK_BUFFER_SIZE);
    memcpy(pDollarBoot, p, DISK_BUFFER_SIZE);

    // 大端字节序：低位字节在高地址，高位字节低地址上。这是人类读写数值的方法。
    // 小端字节序：与上面相反。低位字节在低地址，高位字节在高地址。
    /*
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d OEM:%c%c%c%c%c%c%c%c, BitsOfSector:%02X%02X SectorOfCluster:%02X ReservedSelector:%02X%02X Description: %X, size: %d", 
                                                                      __LINE__,
                                                                      pDollarBoot->OEM[0],
                                                                      pDollarBoot->OEM[1],
                                                                      pDollarBoot->OEM[2],
                                                                      pDollarBoot->OEM[3],
                                                                      pDollarBoot->OEM[4],
                                                                      pDollarBoot->OEM[5],
                                                                      pDollarBoot->OEM[6],
                                                                      pDollarBoot->OEM[7],
                                                                  pDollarBoot->BitsOfSector[0], 
                                                                  pDollarBoot->BitsOfSector[1], 
                                                                  pDollarBoot->SectorOfCluster, 
                                                                  pDollarBoot->ReservedSelector[0],
                                                                  pDollarBoot->ReservedSelector[1], 
                                                                  pDollarBoot->Description,
                                                                  sizeof(pDollarBoot->OEM) / sizeof(pDollarBoot->OEM[0]));
                                                                  
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d AllSectorCount:%02X%02X%02X%02X%02X%02X%02X%02X, MFT_StartCluster:%02X%02X%02X%02X%02X%02X%02X%02X MFT_MirrStartCluster:%02X%02X%02X%02X%02X%02X%02X%02X ClusterPerMFT:%02X%02X%02X%02X ClusterPerIndex: %02X%02X%02X%02X", 
                                                                      __LINE__,                                                                   
                                                                  pDollarBoot->AllSectorCount[0], 
                                                                  pDollarBoot->AllSectorCount[1], 
                                                                  pDollarBoot->AllSectorCount[2], 
                                                                  pDollarBoot->AllSectorCount[3], 
                                                                  pDollarBoot->AllSectorCount[4], 
                                                                  pDollarBoot->AllSectorCount[5], 
                                                                  pDollarBoot->AllSectorCount[6], 
                                                                  pDollarBoot->AllSectorCount[7], 
                                                                  pDollarBoot->MFT_StartCluster[0],
                                                                  pDollarBoot->MFT_StartCluster[1],
                                                                  pDollarBoot->MFT_StartCluster[2],
                                                                  pDollarBoot->MFT_StartCluster[3],
                                                                  pDollarBoot->MFT_StartCluster[4],
                                                                  pDollarBoot->MFT_StartCluster[5],
                                                                  pDollarBoot->MFT_StartCluster[6],
                                                                  pDollarBoot->MFT_StartCluster[7], 
                                                                  pDollarBoot->MFT_MirrStartCluster[0],
                                                                  pDollarBoot->MFT_MirrStartCluster[1],
                                                                  pDollarBoot->MFT_MirrStartCluster[2],
                                                                  pDollarBoot->MFT_MirrStartCluster[3],
                                                                  pDollarBoot->MFT_MirrStartCluster[4],
                                                                  pDollarBoot->MFT_MirrStartCluster[5],
                                                                  pDollarBoot->MFT_MirrStartCluster[6],
                                                                  pDollarBoot->MFT_MirrStartCluster[7], 
                                                                  pDollarBoot->ClusterPerMFT[0],
                                                                  pDollarBoot->ClusterPerMFT[1],
                                                                  pDollarBoot->ClusterPerMFT[2],
                                                                  pDollarBoot->ClusterPerMFT[3],
                                                                  pDollarBoot->ClusterPerIndex[0], 
                                                                  pDollarBoot->ClusterPerIndex[1], 
                                                                  pDollarBoot->ClusterPerIndex[2], 
                                                                  pDollarBoot->ClusterPerIndex[3]);
    */
    L1_FILE_NTFS_DollerRootTransfer(pDollarBoot, pNTFSBootSwitched);

    FreePool(pDollarBoot);

}

// all partitions analysis
EFI_STATUS L2_FILE_PartitionTypeAnalysis(UINT16 DeviceID)
{    
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d DeviceID: %d\n", __LINE__, DeviceID);
    EFI_STATUS Status;
    UINT8 Buffer1[DISK_BUFFER_SIZE] = {0};

    sector_count = 0;

    Status = L1_STORE_READ(DeviceID, sector_count, 1, Buffer1 );    
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
        return Status;
    }

    // FAT32 file system
    if (Buffer1[0x52] == 'F' && Buffer1[0x53] == 'A' && Buffer1[0x54] == 'T' && Buffer1[0x55] == '3' && Buffer1[0x56] == '2')   
    {                   
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FAT32\n",  __LINE__);
        // analysis data area of patition
        L1_FILE_FAT32_DataSectorAnalysis(Buffer1, &MBRSwitched); 

        // data sector number start include: reserved selector, fat sectors(usually is 2: fat1 and fat2), and file system boot path start cluster(usually is 2, data block start number is 2)
        sector_count = MBRSwitched.ReservedSelector + MBRSwitched.SectorsPerFat * MBRSwitched.NumFATS + (MBRSwitched.BootPathStartCluster - 2) * 8;
        BlockSize = MBRSwitched.SectorOfCluster * 512;
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: sector_count:%ld BlockSize: %d\n",  __LINE__, sector_count, BlockSize);
        return FILE_SYSTEM_FAT32;
    }
    // NTFS
    else if (Buffer1[3] == 'N' && Buffer1[4] == 'T' && Buffer1[5] == 'F' && Buffer1[6] == 'S')
    {
        L2_FILE_NTFS_FirstSelectorAnalysis(Buffer1, &NTFSBootSwitched);
        sector_count = NTFSBootSwitched.MFT_StartCluster * 8;
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: NTFS sector_count:%llu\n",  __LINE__, sector_count);
        return FILE_SYSTEM_NTFS;
    }
    // the other file system can add at this place
    else
    {
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: \n",  __LINE__);
        return FILE_SYSTEM_OTHER;
    }                   

    return EFI_SUCCESS;

}

// all partitions analysis
EFI_STATUS L2_STORE_PartitionAnalysis()
{    
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d PartitionAnalysisFSM\n", __LINE__);
    EFI_STATUS Status ;
    UINTN i;
    EFI_HANDLE *ControllerHandle = NULL;
    EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;
    
    Status = gBS->LocateProtocol (&gEfiDevicePathToTextProtocolGuid, NULL, (VOID **) &DevPathToText);
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
    if (EFI_ERROR(Status))
    {
         //DEBUG ((EFI_D_INFO, "LocateProtocol1 error: %x\n", Status));
         L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
        return Status;
    }
   
    Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiDiskIoProtocolGuid, NULL, &PartitionCount, &ControllerHandle);
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);      
        return EFI_SUCCESS;
    }

    for (i = 0; i < PartitionCount; i++)
    {
        //L2_DEBUG_Print1(350, 16 * 6, "%d: %x\n", __LINE__, Status);
        EFI_DEVICE_PATH_PROTOCOL *DiskDevicePath;
        Status = gBS->OpenProtocol(ControllerHandle[i],
                                   &gEfiDevicePathProtocolGuid,
                                   (VOID **)&DiskDevicePath,
                                   gImageHandle, 
                                   NULL,
                                   EFI_OPEN_PROTOCOL_GET_PROTOCOL);
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
        if (EFI_ERROR(Status))
        {
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
            return Status;
        }

        CHAR16 *TextDevicePath = DevPathToText->ConvertDevicePathToText(DiskDevicePath, TRUE, TRUE);
         L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X Partition: %s\n", __LINE__, Status, TextDevicePath);
         L2_STORE_TextDevicePathAnalysis(TextDevicePath, &device[i], i);
                    
        if (TextDevicePath) gBS->FreePool(TextDevicePath);       
    }
    return EFI_SUCCESS;
}

L3_GRAPHICS_ItemPrint(UINT8 *pDestBuffer, UINT8 *pSourceBuffer, UINT16 pDestWidth, UINT16 pDestHeight, 
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
DEVICE_PARAMETER device2[10] = {0};
VOID L2_STORE_PartitionItemsPrint(UINT16 Index)
{
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: \n",  __LINE__);
    
    // this code may be have some problems, because my USB file system is FAT32, my Disk file system is NTFS.
    // others use this code must be careful...
    UINT8 FileSystemType = L2_FILE_PartitionTypeAnalysis(Index);

    if (FileSystemType == FILE_SYSTEM_FAT32)
    {
        L2_FILE_FAT32_DataSectorHandle(Index);
        L2_STORE_FolderItemsPrint();
    }
    else if (FileSystemType == FILE_SYSTEM_NTFS)
    {
        // get MFT $ROOT item. 
        L2_FILE_NTFS_MFT_Item_Read(Index, 5);

        // get data runs
        L2_FILE_NTFS_MFTDollarRootFileAnalysis(BufferMFT);      

        // use data run get root path item index
        L2_FILE_NTFS_RootPathItemsRead(Index);
    }
    else if (FileSystemType == FILE_SYSTEM_OTHER)
    {
        return;
    }
}


EFI_STATUS L3_PARTITION_RootPathAccess()
{	
	EFI_STATUS Status;
	UINT8 Buffer1[512];
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
	
    Color.Red = 0xff;
    Color.Green= 0x00;
    Color.Blue= 0x00;

	UINT16 i = 0;
	
    for (UINT16 i = 0; i < PartitionCount; i++)
    {   
        MyComputerPositionX = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartX;
        MyComputerPositionY = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartY;
        
        if (iMouseX >= MyComputerPositionX + 50 && iMouseX <= MyComputerPositionX + 50 + 16 * 6
            && iMouseY >= MyComputerPositionY + i * 16 + 16 * 2 && iMouseY <= MyComputerPositionY + i * 16 + 16 * 3)
        {   
            if (PreviousItem == i)
            {
                break;
            }
            
            L2_GRAPHICS_RectangleDraw(pMouseSelectedBuffer, 0,  0, 31, 15, 1,  Color, 32);
            L2_STORE_PartitionItemsPrint(i);
            PreviousItem = i;
            L2_GRAPHICS_Copy(pDeskDisplayBuffer, pMouseSelectedBuffer, ScreenWidth, ScreenHeight, 32, 16, MyComputerPositionX + 50, MyComputerPositionY  + i * (16 + 2) + 16 * 2);   
        }
    }
	/*
	Status = L1_STORE_READ(i, sector_count, 1, Buffer1); 
	if ( EFI_SUCCESS == Status )
	{
		//L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X\n", __LINE__, Status);

		//When get root path data sector start number, we can get content of root path.
		L1_FILE_RootPathAnalysis(Buffer1);  

		// data area start from 1824, HZK16 file start from     FileBlockStart  block, so need to convert into sector by multi 8, block start number is 2   
		// next state is to read FAT table
		sector_count = MBRSwitched.ReservedSelector;
		//L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: sector_count:%ld FileLength: %d MBRSwitched.ReservedSelector:%ld\n",  __LINE__, sector_count, FileLength, MBRSwitched.ReservedSelector);
	}           
	*/
}



EFI_STATUS L3_PARTITION_SubPathAccess()
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L3_PARTITION_SubPathAccess\n", __LINE__);
}


// Print Folder item in graphics layer 

EFI_STATUS L3_PARTITION_FileAccess(UINT16 DeviceID)
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L3_PARTITION_FileAccess\n", __LINE__);
	UINT8 Buffer1[512];
	
	EFI_STATUS Status = L1_STORE_READ(DeviceID, sector_count, 1, Buffer1 );  
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
        return Status;
    }
}

EFI_STATUS L3_PARTITION_AccessFinish()
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L3_PARTITION_AccessFinish\n", __LINE__);
}

EFI_STATUS L3_PARTITION_ParentPathAccess()
{
}


// need a stack to save current detailed path
PARTITION_ITEM_ACCESS_STATE_TRANSFORM PartitionItemAccessStateTransformTable[] =
{
	//Current state             Trigger Event          //Next state              //Handle function
    // init state
    {INIT_ACCESS_STATE,         ROOT_PATH_ACCESS_EVENT, ROOT_PATH_STATE,        L3_PARTITION_RootPathAccess},

    // root path
    {ROOT_PATH_STATE,           FILE_ACCESS_EVENT,   FILE_ITEM_ACCESS_STATE,    L3_PARTITION_FileAccess},
    {ROOT_PATH_STATE,           FOLDER_ACCESS_EVENT, FOLDER_ITEM_ACCESS_STATE,  L3_PARTITION_SubPathAccess},
    {ROOT_PATH_STATE,           PARENT_ACCESS_EVENT, ROOT_PATH_STATE,           L3_PARTITION_RootPathAccess},
    {ROOT_PATH_STATE,           CLOSE_ACCESS_EVENT,  INIT_ACCESS_STATE,         L3_PARTITION_AccessFinish},
    
    // some folder
    {FOLDER_ITEM_ACCESS_STATE,  FILE_ACCESS_EVENT,   FILE_ITEM_ACCESS_STATE,    L3_PARTITION_FileAccess}, // open a file and next state is current folder
    {FOLDER_ITEM_ACCESS_STATE,  FOLDER_ACCESS_EVENT, FOLDER_ITEM_ACCESS_STATE,  L3_PARTITION_SubPathAccess}, // a folder.
    {FOLDER_ITEM_ACCESS_STATE,  PARENT_ACCESS_EVENT, FOLDER_ITEM_ACCESS_STATE,  L3_PARTITION_ParentPathAccess}, // access parent path
    {FOLDER_ITEM_ACCESS_STATE,  CLOSE_ACCESS_EVENT,  INIT_ACCESS_STATE,         L3_PARTITION_AccessFinish},

    // some file
    {FILE_ITEM_ACCESS_STATE,    FILE_ACCESS_EVENT,   INIT_ACCESS_STATE,    		L3_PARTITION_FileAccess},
    {FILE_ITEM_ACCESS_STATE,    FOLDER_ACCESS_EVENT, INIT_ACCESS_STATE,  		L3_PARTITION_SubPathAccess},
    {FILE_ITEM_ACCESS_STATE,    PARENT_ACCESS_EVENT, FOLDER_ITEM_ACCESS_STATE,  L3_PARTITION_ParentPathAccess},
    {FILE_ITEM_ACCESS_STATE,    CLOSE_ACCESS_EVENT,  INIT_ACCESS_STATE,         L3_PARTITION_AccessFinish}
};


L2_STORE_FolderItemsPrint()
{
    UINT16 valid_count = 0;
    UINT16 HeightNew = SYSTEM_ICON_HEIGHT / 8;
    UINT16 WidthNew = SYSTEM_ICON_WIDTH / 8;
    UINT16 x;
    UINT16 y;
    
    for (UINT16 i = 0; i < 32; i++)
    {       
        if (pItems[i].FileName[0] == 0)
            break;
            
        char name[12] = {0};
        x = 130;

        y = valid_count * (HeightNew + 16 * 2) + 200;

        for (UINT8 i = 0; i < 12; i++)
            name[i] = '\0';
        
        char ItemType[10] = "OTHER";
        L1_FILE_NameGet(i, name);
                
        if (pItems[i].Attribute[0] == 0x10) //Folder
        {
            ;
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
            
            L3_GRAPHICS_ItemPrint(pMyComputerBuffer, pSystemIconTextBuffer, MyComputerWidth, MyComputerHeight, WidthNew, HeightNew, x, y, "222", 2, GRAPHICS_LAYER_MY_COMPUTER_WINDOW);
            valid_count++;
        }
    }       

}


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
    /*
    if (DisplaySystemLogWindowFlag)
        L2_GRAPHICS_Copy(pDeskDisplayBuffer, pSystemLogWindowBuffer, ScreenWidth, ScreenHeight, SystemLogWindowWidth, SystemLogWindowHeight, SystemLogWindowPositionX, SystemLogWindowPositionY);
    
    if (DisplayMemoryInformationWindowFlag)
        L2_GRAPHICS_Copy(pDeskDisplayBuffer, pMemoryInformationBuffer, ScreenWidth, ScreenHeight, MemoryInformationWindowWidth, MemoryInformationWindowHeight, MemoryInformationWindowPositionX, MemoryInformationWindowPositionY);

    if (DisplaySystemSettingWindowFlag)
        L2_GRAPHICS_Copy(pDeskDisplayBuffer, pSystemSettingWindowBuffer, ScreenWidth, ScreenHeight, SystemSettingWindowWidth, SystemSettingWindowHeight, SystemSettingWindowPositionX, SystemSettingWindowPositionY);

    if (DisplayStartMenuFlag)
        L2_GRAPHICS_Copy(pDeskDisplayBuffer, pStartMenuBuffer, ScreenWidth, ScreenHeight, StartMenuWidth, StartMenuHeight, StartMenuPositionX, StartMenuPositionY);

    if (DisplayMyComputerFlag)
        L2_GRAPHICS_Copy(pDeskDisplayBuffer, pMyComputerBuffer, ScreenWidth, ScreenHeight, MyComputerWidth, MyComputerHeight, MyComputerPositionX, MyComputerPositionY);
    */
}

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
       GraphicsOutput->Blt(GraphicsOutput, 
                    (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) pDeskDisplayBuffer,
                    EfiBltBufferToVideo,
                    0, 0, 
                    0, 0, 
                    ScreenWidth, ScreenHeight, 0);
                    
        return;
    }

    L2_GRAPHICS_CopyBufferFromWindowsToDesk();
        
    if (MouseClickFlag == 1 && pDeskDisplayBuffer[(iMouseY * ScreenWidth + iMouseX) * 4 + 3] == GRAPHICS_LAYER_MY_COMPUTER_WINDOW)
    {
        MyComputerPositionX += x_move * 3;
        MyComputerPositionY += y_move * 3;
        
    //  L2_GRAPHICS_Copy(pDeskDisplayBuffer, pMyComputerBuffer, ScreenWidth, ScreenHeight, MyComputerWidth, MyComputerHeight, MyComputerPositionX, MyComputerPositionX);
    }
    x_move = 0;
    y_move = 0;
    // display graphics layer id mouse over, for mouse click event.
    //L2_DEBUG_Print1(DISPLAY_X, DISPLAY_Y, "%d: Graphics Layer id: %d ", __LINE__, pDeskDisplayBuffer[(iMouseY * ScreenWidth + iMouseX) * 4 + 3]);
    
    int i, j;

    L2_MOUSE_Move();
    
    L2_GRAPHICS_Copy(pDeskDisplayBuffer, pMouseBuffer, ScreenWidth, ScreenHeight, 16, 16, iMouseX, iMouseY);
    //L2_DEBUG_Print1(DISPLAY_X, DISPLAY_Y, "%d: GraphicsLayerCompute\n", __LINE__);

    GraphicsOutput->Blt(GraphicsOutput, 
                        (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) pDeskDisplayBuffer,
                        EfiBltBufferToVideo,
                        0, 0, 
                        0, 0, 
                        ScreenWidth, ScreenHeight, 0);
}

typedef enum 
{
    START_MENU_CLICKED = 0,
    MY_COMPUTER_MENU_CLICKED,
    SETTING_MENU_CLICKED
}BUTTON_CLICKED_ID;

typedef enum 
{
    START_MENU_INIT_CLICKED_EVENT = 0,
    START_MENU_CLICKED_EVENT,
    MY_COMPUTER_CLICKED_EVENT,
    SETTING_CLICKED_EVENT,
    MEMORY_INFORMATION_CLICKED_EVENT,
    MY_COMPUTER_CLOSE_CLICKED_EVENT,
    SYSTEM_LOG_CLOSE_CLICKED_EVENT,
    MEMORY_INFORMATION_CLOSE_CLICKED_EVENT,
    SYSTEM_SETTING_CLOSE_CLICKED_EVENT,
    SYSTEM_LOG_CLICKED_EVENT,
    SYSTEM_QUIT_CLICKED_EVENT,
    WALLPAPER_SETTING_CLICKED_EVENT,
    WALLPAPER_RESET_CLICKED_EVENT,
    MAX_CLICKED_EVENT
}MOUSE_CLICK_EVENT;

typedef enum 
{
    CLICK_INIT_STATE = 0,
    MENU_CLICKED_STATE,
    MY_COMPUTER_CLICKED_STATE,
    MEMORY_INFORMATION_CLICKED_STATE,
    SYSTEM_SETTING_CLICKED_STATE,
    SYSTEM_LOG_STATE,
    SYSTEM_QUIT_STATE
}START_MENU_STATE;

typedef enum 
{
    START_MENU_BUTTON_MY_COMPUTER = 0,
    START_MENU_BUTTON_SYSTEM_SETTING,
    START_MENU_BUTTON_MEMORY_INFORMATION,
    START_MENU_BUTTON_SYSTEM_LOG,
    START_MENU_BUTTON_SYSTEM_QUIT,
    START_MENU_BUTTON_MAX
}START_MENU_BUTTON_SEQUENCE;


typedef struct
{
    START_MENU_STATE                    CurrentState;
    MOUSE_CLICK_EVENT            		event;
    START_MENU_STATE                    NextState;
    EFI_STATUS                          (*pFunc)(); 
}START_MENU_STATE_TRANSFORM;

MOUSE_CLICK_EVENT L2_GRAPHICS_DeskLayerClickEventGet()
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_GRAPHICS_DeskLayerClickEventGet\n", __LINE__);

    //start button
    if (iMouseX >= 0 && iMouseX <= 16 + 16 * 2
        && iMouseY >= ScreenHeight - 21 && iMouseY <= ScreenHeight)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: START_MENU_CLICKED_EVENT\n", __LINE__);
        return START_MENU_CLICKED_EVENT;
    }

	return MAX_CLICKED_EVENT;
}

MOUSE_CLICK_EVENT L2_GRAPHICS_StartMenuLayerClickEventGet()
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_GRAPHICS_StartMenuLayerClickEventGet\n", __LINE__);

    StartMenuPositionX = WindowLayers.item[GRAPHICS_LAYER_START_MENU].StartX;
    StartMenuPositionY = WindowLayers.item[GRAPHICS_LAYER_START_MENU].StartY;

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
	
    SystemSettingWindowPositionX = WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].StartX;
    SystemSettingWindowPositionY = WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].StartY;

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
		
    MyComputerPositionX = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartX;
    MyComputerPositionY = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartY;

    // Hide My computer window
    if (iMouseX >= MyComputerPositionX + MyComputerWidth - 20 && iMouseX <=  MyComputerPositionX + MyComputerWidth - 4 
            && iMouseY >= MyComputerPositionY + 0 && iMouseY <= MyComputerPositionY + 16)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MY_COMPUTER_CLOSE_CLICKED_EVENT\n", __LINE__);
        return MY_COMPUTER_CLOSE_CLICKED_EVENT;
    }

	return MAX_CLICKED_EVENT;
}

MOUSE_CLICK_EVENT L2_GRAPHICS_SystemLogLayerClickEventGet()
{	
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_GRAPHICS_SystemLogLayerClickEventGet\n", __LINE__);
    SystemLogWindowPositionX = WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].StartX;
    SystemLogWindowPositionY = WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].StartY;

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

    MemoryInformationWindowPositionX = WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].StartX;
    MemoryInformationWindowPositionY = WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].StartY;
    
    // Hide Memory Information window
    if (iMouseX >= MemoryInformationWindowPositionX + MemoryInformationWindowWidth - 20 && iMouseX <=  MemoryInformationWindowPositionX + MemoryInformationWindowWidth - 4 
            && iMouseY >= MemoryInformationWindowPositionY+ 0 && iMouseY <= MemoryInformationWindowPositionY + 16)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MEMORY_INFORMATION_CLOSE_CLICKED_EVENT\n", __LINE__);
        return MEMORY_INFORMATION_CLOSE_CLICKED_EVENT;
    }
			
	return MAX_CLICKED_EVENT;
}

typedef struct
{
    GRAPHICS_LAYER_ID           GraphicsLayerID;
    MOUSE_CLICK_EVENT           (*pFunction)(); 
}GRAPHICS_LAYER_EVENT_GET;

//Get click event from different graphics layer.
GRAPHICS_LAYER_EVENT_GET GraphicsLayerEventGet[] =
{
    {GRAPHICS_LAYER_DESK,       					 L2_GRAPHICS_DeskLayerClickEventGet},
    {GRAPHICS_LAYER_START_MENU,            			 L2_GRAPHICS_StartMenuLayerClickEventGet},
    {GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW,           L2_GRAPHICS_SystemSettingLayerClickEventGet},
    {GRAPHICS_LAYER_MY_COMPUTER_WINDOW,            	 L2_GRAPHICS_MyComputerLayerClickEventGet},
    {GRAPHICS_LAYER_SYSTEM_LOG_WINDOW,            	 L2_GRAPHICS_SystemLogLayerClickEventGet},
    {GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW,       L2_GRAPHICS_MemoryInformationLayerClickEventGet},
};

UINT16 LayerID = -1;

START_MENU_STATE StartMenuNextState = CLICK_INIT_STATE;

UINT16 L2_MOUSE_ClickEventGet()
{   
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: iMouseX: %d iMouseY: %d MouseClickFlag: %d\n", __LINE__, iMouseX, iMouseY, MouseClickFlag);

    //start button
    if (iMouseX >= 0 && iMouseX <= 16 + 16 * 2
        && iMouseY >= ScreenHeight - 21 && iMouseY <= ScreenHeight)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: START_MENU_CLICKED_EVENT\n", __LINE__);
        return START_MENU_CLICKED_EVENT;
    }

    StartMenuPositionX = WindowLayers.item[GRAPHICS_LAYER_START_MENU].StartX;
    StartMenuPositionY = WindowLayers.item[GRAPHICS_LAYER_START_MENU].StartY;

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
    
    SystemSettingWindowPositionX = WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].StartX;
    SystemSettingWindowPositionY = WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].StartY;

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

    MyComputerPositionX = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartX;
    MyComputerPositionY = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartY;

    // Hide My computer window
    if (iMouseX >= MyComputerPositionX + MyComputerWidth - 20 && iMouseX <=  MyComputerPositionX + MyComputerWidth - 4 
            && iMouseY >= MyComputerPositionY + 0 && iMouseY <= MyComputerPositionY + 16)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MY_COMPUTER_CLOSE_CLICKED_EVENT\n", __LINE__);
        return MY_COMPUTER_CLOSE_CLICKED_EVENT;
    }

    SystemLogWindowPositionX = WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].StartX;
    SystemLogWindowPositionY = WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].StartY;

    // Hide System Log window
    if (iMouseX >= SystemLogWindowPositionX + SystemLogWindowWidth - 20 && iMouseX <=  SystemLogWindowPositionX + SystemLogWindowWidth - 4 
            && iMouseY >= SystemLogWindowPositionY+ 0 && iMouseY <= SystemLogWindowPositionY + 16)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: SYSTEM_LOG_CLOSE_CLICKED_EVENT\n", __LINE__);
        return SYSTEM_LOG_CLOSE_CLICKED_EVENT;
    }
    
    MemoryInformationWindowPositionX = WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].StartX;
    MemoryInformationWindowPositionY = WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].StartY;
    
    // Hide Memory Information window
    if (iMouseX >= MemoryInformationWindowPositionX + MemoryInformationWindowWidth - 20 && iMouseX <=  MemoryInformationWindowPositionX + MemoryInformationWindowWidth - 4 
            && iMouseY >= MemoryInformationWindowPositionY+ 0 && iMouseY <= MemoryInformationWindowPositionY + 16)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MEMORY_INFORMATION_CLOSE_CLICKED_EVENT\n", __LINE__);
        return MEMORY_INFORMATION_CLOSE_CLICKED_EVENT;
    }

    SystemSettingWindowPositionX = WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].StartX;
    SystemSettingWindowPositionY = WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].StartY;

    // Hide Memory Information window
    if (iMouseX >= SystemSettingWindowPositionX + SystemSettingWindowWidth - 20 && iMouseX <=  SystemSettingWindowPositionX + SystemSettingWindowWidth - 4 
            && iMouseY >= SystemSettingWindowPositionY+ 0 && iMouseY <= SystemSettingWindowPositionY + 16)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: SYSTEM_SETTING_CLOSE_CLICKED_EVENT\n", __LINE__);
        return SYSTEM_SETTING_CLOSE_CLICKED_EVENT;
    }
    
    //DisplayMyComputerFlag = 0;
    DisplaySystemSettingWindowFlag = 0;
    
    if (TRUE  == WindowLayers.item[GRAPHICS_LAYER_START_MENU].DisplayFlag)
    {
        WindowLayers.ActiveWindowCount--;
        WindowLayers.item[GRAPHICS_LAYER_START_MENU].DisplayFlag = FALSE;
    }
    
    StartMenuNextState = CLICK_INIT_STATE;

    return START_MENU_INIT_CLICKED_EVENT;

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


VOID L2_MOUSE_SystemQuitClicked()
{   
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_SystemQuitClicked\n", __LINE__);
    SystemQuitFlag = TRUE;  
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
    L1_GRAPHICS_UpdateWindowLayer(GRAPHICS_LAYER_SYSTEM_LOG_WINDOW);
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

    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer,  16, ScreenHeight - 21,     (18 - 1) * 94 + 43 - 1, Color, ScreenWidth);
    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer,  16 * 2, ScreenHeight - 21, (21 - 1) * 94 + 05 - 1, Color, ScreenWidth);      
}

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
EFI_STATUS L2_GRAPHICS_ButtonDraw2(UINT16 StartX, UINT16 StartY, UINT16 Width, UINT16 Height)
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    
    // Button
    // white
    Color.Red   = 0xFF;
    Color.Green = 0xFF;
    Color.Blue  = 0xFF;
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

// bmp format
UINT8 SystemIcon[SYSTEM_ICON_MAX][SYSTEM_ICON_WIDTH * SYSTEM_ICON_HEIGHT * 3 + 0x36];



VOID L2_MOUSE_MyComputerClicked()
{       
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_MyComputerClicked\n", __LINE__);
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    Color.Red = 0xff;
    Color.Green= 0x00;
    Color.Blue= 0x00;

    //DisplayMyComputerFlag = 1;
    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].DisplayFlag = TRUE;
    
    L1_GRAPHICS_UpdateWindowLayer(GRAPHICS_LAYER_MY_COMPUTER_WINDOW);
    
    for (UINT16 i = 0; i < PartitionCount; i++)
    {   
        MyComputerPositionX = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartX;
        MyComputerPositionY = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartY;
        if (iMouseX >= MyComputerPositionX + 50 && iMouseX <= MyComputerPositionX + 50 + 16 * 6
            && iMouseY >= MyComputerPositionY + i * 16 + 16 * 2 && iMouseY <= MyComputerPositionY + i * 16 + 16 * 3)
        {   
            if (PreviousItem == i)
            {
                break;
            }
            
            L2_GRAPHICS_RectangleDraw(pMouseSelectedBuffer, 0,  0, 31, 15, 1,  Color, 32);

            // need to save result into cache, for next time to read, reduce disk operation
            L2_STORE_PartitionItemsPrint(i);
            PreviousItem = i;
            L2_GRAPHICS_Copy(pDeskDisplayBuffer, pMouseSelectedBuffer, ScreenWidth, ScreenHeight, 32, 16, MyComputerPositionX + 50, MyComputerPositionY  + i * (16 + 2) + 16 * 2);   
        }
    }
}


VOID L2_MOUSE_MENU_Clicked()
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_MENU_Clicked\n", __LINE__);
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    Color.Red   = 0xff;
    Color.Green = 0x00;
    Color.Blue   = 0x00;
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: iMouseX: %d iMouseY: %d \n",  __LINE__, iMouseX, iMouseY);
    L2_GRAPHICS_RectangleDraw(pMouseSelectedBuffer, 0,  0, 31, 15, 1,  Color, 32);
    //MenuButtonClickResponse();
    Color.Red   = 0xFF;
    Color.Green = 0xFF;
    Color.Blue  = 0xFF;
    //L1_MEMORY_RectangleFill(pDeskBuffer, 3,     ScreenHeight - 21, 13,     ScreenHeight - 11, 1, Color);
    L2_GRAPHICS_Copy(pDeskDisplayBuffer, pMouseSelectedBuffer, ScreenWidth, ScreenHeight, 32, 16, 3, ScreenHeight - 21);
        
    //L2_GRAPHICS_Copy(pDeskDisplayBuffer, pStartMenuBuffer, ScreenWidth, ScreenHeight, StartMenuWidth, StartMenuHeight, StartMenuPositionX, StartMenuPositionY);
    //DisplayStartMenuFlag = 1;

    if (FALSE == WindowLayers.item[GRAPHICS_LAYER_START_MENU].DisplayFlag)
    {
        WindowLayers.ActiveWindowCount++;
        WindowLayers.item[GRAPHICS_LAYER_START_MENU].DisplayFlag = TRUE;
        
    }
    L1_GRAPHICS_UpdateWindowLayer(GRAPHICS_LAYER_START_MENU);

}


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


VOID L2_MOUSE_MoveOver()
{    
	L3_PARTITION_RootPathAccess();
	
    if (MouseClickFlag == 1)
    {
        UINT16 layer = pDeskDisplayBuffer[(iMouseY * ScreenWidth + iMouseX) * 4 + 3];
        if (0 != layer)
        {
            WindowLayers.item[layer].StartX += x_move * 3;
            WindowLayers.item[layer].StartY += y_move * 3;
            L1_GRAPHICS_UpdateWindowLayer(layer);
        }
    }
    
    x_move = 0;
    y_move = 0;

}

VOID L2_MOUSE_MenuButtonClick()
{    
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    Color.Red = 0xff;
    
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 32; j++)
        {   
            pMouseSelectedBuffer[(i * 32 + j) * 4]     = pDeskDisplayBuffer[((MyComputerPositionX + 50 + i) * ScreenWidth +  MyComputerPositionY  + i * (16 + 2) + 16 * 2 + j) * 4];
            pMouseSelectedBuffer[(i * 32 + j) * 4 + 1] = pDeskDisplayBuffer[((MyComputerPositionX + 50 + i) * ScreenWidth +  MyComputerPositionY  + i * (16 + 2) + 16 * 2 + j) * 4 + 1];
            pMouseSelectedBuffer[(i * 32 + j) * 4 + 2] = pDeskDisplayBuffer[((MyComputerPositionX + 50 + i) * ScreenWidth +  MyComputerPositionY  + i * (16 + 2) + 16 * 2 + j) * 4 + 2];            
        }
    }
     //L1_MEMORY_RectangleFill(UINT8 * pBuffer,IN UINTN x0,UINTN y0,UINTN x1,UINTN y1,IN UINTN BorderWidth,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color)
    
    //L1_MEMORY_RectangleFill(pMouseSelectedBuffer, 0,  0, 32, 16, 1,  Color);

    // Draw a red rectangle when mouse move over left down (like menu button)
    L2_GRAPHICS_RectangleDraw(pMouseSelectedBuffer, 0,  0, 31, 15, 1,  Color, 32);

    //L2_DEBUG_Print1(DISPLAY_X, DISPLAY_Y, "%d: GraphicsLayerCompute\n", __LINE__);

    if (MouseClickFlag == 1)
    {
        char *pClickMenuBuffer = (UINT8 *)AllocateZeroPool(16 * 16 * 4);
        if (NULL != pClickMenuBuffer)
        {
            //DEBUG ((EFI_D_INFO, "ScreenInit AllocatePool pDeskDisplayBuffer NULL\n"));

            for(int i = 0; i < 16; i++)
                for(int j = 0; j < 16; j++)
                {
                    pClickMenuBuffer[(i * 16 + j) * 4] = 0x33;
                    pClickMenuBuffer[(i * 16 + j) * 4 + 1] = 0x33;
                    pClickMenuBuffer[(i * 16 + j) * 4 + 2] = 0x33;
                }
            L2_GRAPHICS_Copy(pDeskDisplayBuffer, pClickMenuBuffer, ScreenWidth, ScreenHeight, 16, 16, 0, ScreenHeight - 22 - 16);
        }
    }
    
    L2_GRAPHICS_Copy(pDeskDisplayBuffer, pMouseSelectedBuffer, ScreenWidth, ScreenHeight, 32, 16, 15, ScreenHeight - 22); 
}

UINT16 GraphicsLayerIDCount = 0;
VOID L2_MOUSE_Move()
{   
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: GraphicsLayerMouseMove\n",  __LINE__);
    
    // display graphics layer id mouse over, for mouse click event.
    //L2_DEBUG_Print1(DISPLAY_X, DISPLAY_Y, "%d: Graphics Layer id: %d ", __LINE__, pDeskDisplayBuffer[(iMouseY * ScreenWidth + iMouseX) * 4 + 3]);
    L2_DEBUG_Print1(0, ScreenHeight - 30 -  7 * 16, "%d: iMouseX: %d iMouseY: %d Graphics Layer id: %d GraphicsLayerIDCount: %u", __LINE__, iMouseX, iMouseY, pDeskDisplayBuffer[(iMouseY * ScreenWidth + iMouseX) * 4 + 3], GraphicsLayerIDCount++);
    L2_MOUSE_MoveOver();
    
    L2_MOUSE_Click();
}


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


//http://quwei.911cha.com/
EFI_STATUS L2_GRAPHICS_DrawChineseCharIntoBuffer(UINT8 *pBuffer,
        IN UINTN x0, UINTN y0,UINT8 offset,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color , UINT16 AreaWidth)
{
    INT8 i;
    if (NULL == pBuffer)
    {
        //DEBUG ((EFI_D_INFO, "NULL == pBuffer"));
        return EFI_SUCCESS;
    }
    
    for(i = 0; i < 32; i += 2)
    {
        L2_GRAPHICS_ChineseHalfDra2(pBuffer, sChinese[offset][i],     x0,     y0 + i / 2, 1, Color, AreaWidth);             
        L2_GRAPHICS_ChineseHalfDra2(pBuffer, sChinese[offset][i + 1], x0 + 8, y0 + i / 2, 1, Color, AreaWidth);     
    }
    
    return EFI_SUCCESS;
}

EFI_STATUS L2_GRAPHICS_ChineseCharDraw(UINT8 *pBuffer,
        IN UINTN x0, UINTN y0,UINT8 *c, UINT8 count,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL FontColor, UINT16 AreaWidth)
{
    INT16 i, j;    
    
    if (NULL == pBuffer)
    {
        //DEBUG ((EFI_D_INFO, "NULL == pBuffer"));
        return EFI_SUCCESS;
    }

    for (i = 0; i < 16 * 16 * 4; i++)
    {
        pBuffer[i] = 0x00;
    }

    for (j = 0; j < count; j++)
    {
        for(i = 0; i < 32; i += 2)
        {
            L2_GRAPHICS_ChineseHalfDraw(pBuffer, sChinese[j][i],     x0,     y0 + i / 2, 1, FontColor, AreaWidth);                
            L2_GRAPHICS_ChineseHalfDraw(pBuffer, sChinese[j][i + 1], x0 + 8, y0 + i / 2, 1, FontColor, AreaWidth);        
        }
        x0 += 16;
    }
    
    return EFI_SUCCESS;
}

VOID L2_NETWORK_Init()
{}


VOID L2_NETWORK_DriverSend()
{}

VOID L2_NETWORK_DriverReceive()
{}

VOID L2_FILE_Transfer(MasterBootRecord *pSource, MasterBootRecordSwitched *pDest)
{
    pDest->ReservedSelector = pSource->ReservedSelector[0] + pSource->ReservedSelector[1] * 16 * 16;
    pDest->SectorsPerFat    = (UINT16)pSource->SectorsPerFat[0] + (UINT16)(pSource->SectorsPerFat[1]) * 16 * 16 + pSource->SectorsPerFat[2] * 16 * 16 * 16 * 16 + pSource->SectorsPerFat[3] * 16 * 16 * 16 * 16 * 16 * 16;
    pDest->BootPathStartCluster = (UINT16)pSource->BootPathStartCluster[0] + pSource->BootPathStartCluster[1] * 16 * 16 + pSource->BootPathStartCluster[2] * 16 * 16 * 16 * 16, pSource->BootPathStartCluster[3] * 16 * 16 * 16 * 16 * 16 * 16;
    pDest->NumFATS      = pSource->NumFATS[0];
    pDest->SectorOfCluster = pSource->SectorOfCluster[0];

    //Todo: the other parameters can compute like above too
    //Current only get parameters we need to use
    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "ReservedSelector:%d SectorsPerFat:%d BootPathStartCluster: %d NumFATS:%d SectorOfCluster:%d", 
                                                pDest->ReservedSelector,
                                                pDest->SectorsPerFat,
                                                pDest->BootPathStartCluster,
                                                pDest->NumFATS,
                                                pDest->SectorOfCluster);
}

// all partitions analysis
EFI_STATUS L2_STORE_PartitionAnalysisFSM()
{    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d PartitionAnalysisFSM\n", __LINE__);
    EFI_STATUS Status ;
    UINT8 Buffer1[DISK_BUFFER_SIZE];
    
    for (int i = 0; i < PartitionCount; i++)
    {
        if (device[i].DeviceType == 1 && device[i].SectorCount == STORE_EFI_PATH_PARTITION_SECTOR_COUNT)
        {
             Status = L1_STORE_READ(i, sector_count, 1, Buffer1); 
             if ( EFI_SUCCESS == Status )
            {
                L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
                  
                  // analysis data area of patition
                  L1_FILE_FAT32_DataSectorAnalysis(Buffer1, &MBRSwitched); 

                  // data sector number start include: reserved selector, fat sectors(usually is 2: fat1 and fat2), and file system boot path start cluster(usually is 2, data block start number is 2)
                  sector_count = MBRSwitched.ReservedSelector + MBRSwitched.SectorsPerFat * MBRSwitched.NumFATS + (MBRSwitched.BootPathStartCluster - 2) * 8;
                  BlockSize = MBRSwitched.SectorOfCluster * 512;
                  L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: sector_count:%ld BlockSize: %d\n",  __LINE__, sector_count, BlockSize);
             }           
            break;
         }       
    }

    return EFI_SUCCESS;
}

// analysis a partition 
EFI_STATUS L2_STORE_RootPathAnalysisFSM()
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d RootPathAnalysisFSM\n", __LINE__);
    EFI_STATUS Status ;
    UINT8 Buffer1[DISK_BUFFER_SIZE];
    
    for (int i = 0; i < PartitionCount; i++)
    {
        if (device[i].DeviceType == 1 && device[i].SectorCount == STORE_EFI_PATH_PARTITION_SECTOR_COUNT)
        {
            Status = L1_STORE_READ(i, sector_count, 1, Buffer1); 
            if ( EFI_SUCCESS == Status )
            {
                 //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X\n", __LINE__, Status);
                  
                  //When get root path data sector start number, we can get content of root path.
                    L1_FILE_RootPathAnalysis(Buffer1);  

                    // data area start from 1824, HZK16 file start from     FileBlockStart  block, so need to convert into sector by multi 8, block start number is 2   
                    // next state is to read FAT table
                    sector_count = MBRSwitched.ReservedSelector;
                    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: sector_count:%ld FileLength: %d MBRSwitched.ReservedSelector:%ld\n",  __LINE__, sector_count, FileLength, MBRSwitched.ReservedSelector);
             }           
            break;
         }       
    }

    return EFI_SUCCESS;
}

// 
EFI_STATUS L2_STORE_GetFatTableFSM()
{    
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d GetFatTableFSM\n", __LINE__);
    EFI_STATUS Status ;
    if (NULL != FAT32_Table)
    {
        // start sector of file
        sector_count = MBRSwitched.ReservedSelector + MBRSwitched.SectorsPerFat * MBRSwitched.NumFATS + MBRSwitched.BootPathStartCluster - 2 + (FileBlockStart - 2) * 8;
        
        // for FAT32_Table get next block number
        PreviousBlockNumber = FileBlockStart;

        return EFI_SUCCESS;
    }
    
    for (int i = 0; i < PartitionCount; i++)
    {
        if (device[i].DeviceType == 1 && device[i].SectorCount == STORE_EFI_PATH_PARTITION_SECTOR_COUNT)
        {
             //start block need to recompute depends on file block start number 
             //FileBlockStart;
             L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d MBRSwitched.SectorsPerFat: %d\n", __LINE__, MBRSwitched.SectorsPerFat);
            
             // 512 = 16 * 32 = 4 item * 32
            FAT32_Table = (UINT8 *)AllocateZeroPool(DISK_BUFFER_SIZE * MBRSwitched.SectorsPerFat);
             if (NULL == FAT32_Table)
             {
                 L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: NULL == FAT32_Table\n", __LINE__);                             
             }             
             
            Status = L1_STORE_READ(i, sector_count,  MBRSwitched.SectorsPerFat, FAT32_Table); 
            if ( EFI_SUCCESS == Status )
            {
                  //CopyMem(FAT32_Table, Buffer1, DISK_BUFFER_SIZE * MBRSwitched.SectorsPerFat);
                  for (int j = 0; j < 250; j++)
                  {
                        //L2_DEBUG_Print1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", Buffer1[j] & 0xff);
                  }

                  // start sector of file
                  sector_count = MBRSwitched.ReservedSelector + MBRSwitched.SectorsPerFat * MBRSwitched.NumFATS + MBRSwitched.BootPathStartCluster - 2 + (FileBlockStart - 2) * 8;

                  // for FAT32_Table get next block number
                  PreviousBlockNumber = FileBlockStart;
                  //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: sector_count:%ld FileLength: %d PreviousBlockNumber: %d\n",  __LINE__, sector_count, FileLength, PreviousBlockNumber);
             }           
            break;
         }       
    }

    return EFI_SUCCESS;
}

UINT32 L2_FILE_GetNextBlockNumber()
{
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: PreviousBlockNumber: %d\n",  __LINE__, PreviousBlockNumber);

    if (PreviousBlockNumber == 0)
    {
        return 0x0fffffff;
    }
    
    if (FAT32_Table[PreviousBlockNumber * 4] == 0xff 
        && FAT32_Table[PreviousBlockNumber * 4 + 1] == 0xff 
        && FAT32_Table[PreviousBlockNumber * 4 + 2] == 0xff 
        && FAT32_Table[PreviousBlockNumber * 4 + 3] == 0x0f)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: PreviousBlockNumber: %d, PreviousBlockNumber: %llX\n",  __LINE__, PreviousBlockNumber, 0x0fffffff);
        return 0x0fffffff;
    }
    
    return FAT32_Table[PreviousBlockNumber  * 4] + (UINT32)FAT32_Table[PreviousBlockNumber * 4 + 1] * 16 * 16 + (UINT32)FAT32_Table[PreviousBlockNumber * 4 + 2] * 16 * 16 * 16 * 16 + (UINT32)FAT32_Table[PreviousBlockNumber * 4 + 3] * 16 * 16 * 16 * 16 * 16 * 16;  
}

//InternalMemSetMem
void L1_MEMORY_SetValue(UINT8 *pBuffer, UINT32 Length, UINT8 Value)
{
    //
  // Declare the local variables that actually move the data elements as
  // volatile to prevent the optimizer from replacing this function with
  // the intrinsic memset()
  //
  volatile UINT8                    *Pointer8;
  volatile UINT32                   *Pointer32;
  volatile UINT64                   *Pointer64;
  UINT32                            Value32;
  UINT64                            Value64;

  if ((((UINTN)pBuffer & 0x7) == 0) && (Length >= 8)) 
  {
    // Generate the 64bit value
    Value32 = (Value << 24) | (Value << 16) | (Value << 8) | Value;
    Value64 = LShiftU64 (Value32, 32) | Value32;

    Pointer64 = (UINT64*)pBuffer;
    while (Length >= 8) 
    {
      *(Pointer64++) = Value64;
      Length -= 8;
    }

    // Finish with bytes if needed
    Pointer8 = (UINT8*)Pointer64;
  } 
  else if ((((UINTN)pBuffer & 0x3) == 0) && (Length >= 4)) 
  {
    // Generate the 32bit value
    Value32 = (Value << 24) | (Value << 16) | (Value << 8) | Value;

    Pointer32 = (UINT32*)pBuffer;
    while (Length >= 4) 
    {
      *(Pointer32++) = Value32;
      Length -= 4;
    }

    // Finish with bytes if needed
    Pointer8 = (UINT8*)Pointer32;
  } 
  else 
  {
    Pointer8 = (UINT8*)pBuffer;
  }
  
  while (Length-- > 0) 
  {
    *(Pointer8++) = Value;
  }
  return pBuffer;
}


//https://blog.csdn.net/goodwillyang/article/details/45559925

EFI_STATUS L2_STORE_ReadFileFSM()
{    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d ReadFileFSM: PartitionCount: %d FileLength: %d sector_count: %llu\n", __LINE__, PartitionCount, FileLength, sector_count);
    ////DEBUG ((EFI_D_INFO, "PartitionUSBRead!!\r\n"));
    EFI_STATUS Status ;
    UINT8 BufferBlock[DISK_BLOCK_BUFFER_SIZE];
            
    for (int i = 0; i < PartitionCount; i++)
    {
        if (device[i].DeviceType == 1 && device[i].SectorCount == STORE_EFI_PATH_PARTITION_SECTOR_COUNT)
        {
            // read from USB by block(512 * 8)
            // Read file content from FAT32(USB), minimum unit is block

            UINT8 AddOneFlag = (FileLength % (512 * 8)) == 0 ? 0 : 1;
            
            for (UINT16 k = 0; k < FileLength / (512 * 8) + AddOneFlag; k++)
            {
                Status = L1_STORE_READ(i, sector_count, 8, BufferBlock); 
                if (EFI_ERROR(Status))
                {
                    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
                    return Status;
                }           

                /*
                BufferBlock[0] = 0xff;
                BufferBlock[1] = 0xff;

                Status = L1_STORE_Write(i, sector_count, 8, BufferBlock); 
                if (EFI_ERROR(Status))
                {
                    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
                    return Status;
                }     
                */           
                //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: HZK16FileReadCount: %d DISK_BLOCK_BUFFER_SIZE: %d\n", __LINE__, HZK16FileReadCount, DISK_BLOCK_BUFFER_SIZE);

                  //Copy buffer to ChineseBuffer
                  if (pReadFileDestBuffer != NULL)
                  {
                        for (UINT16 j = 0; j < DISK_BLOCK_BUFFER_SIZE; j++)
                            pReadFileDestBuffer[FileReadCount * DISK_BLOCK_BUFFER_SIZE + j] = BufferBlock[j];
                 }
                  
                  for (int j = 0; j < 250; j++)
                  {
                        //L2_DEBUG_Print1(DISK_READ_BUFFER_X + (j % 32) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 32), "%02X ", BufferBlock[j] & 0xff);
                  }
                  
                  FileReadCount++;
                  UINT32 NextBlockNumber = L2_FILE_GetNextBlockNumber();
                  sector_count = MBRSwitched.ReservedSelector + MBRSwitched.SectorsPerFat * MBRSwitched.NumFATS + MBRSwitched.BootPathStartCluster - 2 + (NextBlockNumber - 2) * 8;
                  PreviousBlockNumber = NextBlockNumber;
                  //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: NextBlockNumber: %llu\n",  __LINE__, NextBlockNumber);
             }
             
            break;
        }

    }

    return EFI_SUCCESS;
}

STATE_TRANS StatusTransitionTable[] =
{
    { INIT_STATE,                READ_PATITION_EVENT,   GET_PARTITION_INFO_STATE, L2_STORE_PartitionAnalysisFSM},
    { GET_PARTITION_INFO_STATE,  READ_ROOT_PATH_EVENT,  GET_ROOT_PATH_INFO_STATE, L2_STORE_RootPathAnalysisFSM},
    { GET_ROOT_PATH_INFO_STATE,  READ_FAT_TABLE_EVENT,  GET_FAT_TABLE_STATE,      L2_STORE_GetFatTableFSM},
    { GET_FAT_TABLE_STATE,       READ_FILE_EVENT,       READ_FILE_STATE,          L2_STORE_ReadFileFSM},
    { READ_FILE_STATE,           READ_FILE_EVENT,       READ_FILE_STATE,          L2_STORE_ReadFileFSM },
};

int L2_STORE_FileRead(EVENT event)
{
    EFI_STATUS Status;
    
    if (FileReadCount > FileLength / (512 * 8))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
        return;
    }
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FileReadFSM current event: %d, NextState: %d table event:%d table NextState: %d\n", 
                              __LINE__, 
                            event, 
                            NextState,
                            StatusTransitionTable[NextState].event,
                            StatusTransitionTable[NextState].NextState);
    
    if ( event == StatusTransitionTable[NextState].event )
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
        StatusTransitionTable[NextState].pFunc();
        NextState = StatusTransitionTable[NextState].NextState;
    }
    else  
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
        NextState = INIT_STATE;
    }

    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
    StatusErrorCount++;
}

VOID EFIAPI L2_TIMER_Slice0(
    IN EFI_EVENT Event,
    IN VOID           *Context
    )
{
    //Print(L"%lu\n", *((UINT32 *)Context));
    if (TimerSliceCount % 2 == 0)
       gBS->SignalEvent (MultiTaskTriggerGroup0Event);
    
    TimerSliceCount++;
    return;
}


VOID EFIAPI L2_TIMER_Slice(
    IN EFI_EVENT Event,
    IN VOID           *Context
    )
{
    L2_DEBUG_Print1(0, ScreenHeight - 30 - 5 * 16, "%d: TimeSlice %x %lu \n", __LINE__, Context, *((UINT32 *)Context));
    L2_DEBUG_Print1(0, ScreenHeight - 30 - 6 * 16, "%d: TimerSliceCount: %lu \n", __LINE__, TimerSliceCount);
    //Print(L"%lu\n", *((UINT32 *)Context));
    if (TimerSliceCount % 10 == 0)
       gBS->SignalEvent (MultiTaskTriggerGroup1Event);
       
    if (TimerSliceCount % 50 == 0)
       gBS->SignalEvent (MultiTaskTriggerGroup2Event);
    
    ////DEBUG ((EFI_D_INFO, "System time slice Loop ...\n"));
    //gBS->SignalEvent (MultiTaskTriggerEvent);

    TimerSliceCount++;
    return;
}

EFI_STATUS L2_MOUSE_Init()
{
    EFI_STATUS                         Status;
    EFI_HANDLE                         *PointerHandleBuffer = NULL;
    UINTN                              i = 0;
    UINTN                              HandleCount = 0;
    
    //get the handles which supports
    Status = gBS->LocateHandleBuffer(
                                    ByProtocol,
                                    &gEfiSimplePointerProtocolGuid,
                                    NULL,
                                    &HandleCount,
                                    &PointerHandleBuffer
                                    );
        
    if (EFI_ERROR(Status))  return Status;      //unsupport
    
    for (i = 0; i < HandleCount; i++)
    {
        Status = gBS->HandleProtocol(PointerHandleBuffer[i],
                                    &gEfiSimplePointerProtocolGuid,
                                    (VOID**)&gMouse);
            
        if (EFI_ERROR(Status))  
            continue;
        
        else
        {
            return EFI_SUCCESS;
        }
    }   
    
    return EFI_SUCCESS;

}



EFI_STATUS L2_MEMORY_MapInitial()
{
    MEMORY_INFORMATION MemoryInformationTemp;

    MemoryInformation.CurrentAllocatedCount = 0;

    INFO_SELF(L"%X \r\n", L2_MEMORY_MapInitial);  

    L1_MEMORY_SetValue(&MemoryInformation.AllocatedInformation, sizeof(MemoryInformation.AllocatedInformation), 0);

    // Sort by pages
    for (UINT16 i = 0; i < MemoryInformation.MemorySliceCount; i++)
    {
        for (UINT16 j = 0; j < MemoryInformation.MemorySliceCount - i; j++)
        {       
            if ( MemoryInformation.MemoryContinuous[j].NumberOfPages > MemoryInformation.MemoryContinuous[j + 1].NumberOfPages)
            {
                MEMORY_CONTINUOUS temp;
                temp.NumberOfPages = MemoryInformation.MemoryContinuous[j + 1].NumberOfPages;
                temp.PhysicalStart = MemoryInformation.MemoryContinuous[j + 1].PhysicalStart;
                
                MemoryInformation.MemoryContinuous[j + 1].NumberOfPages = MemoryInformation.MemoryContinuous[j].NumberOfPages;
                MemoryInformation.MemoryContinuous[j + 1].PhysicalStart = MemoryInformation.MemoryContinuous[j].PhysicalStart;
                
                MemoryInformation.MemoryContinuous[j].NumberOfPages = temp.NumberOfPages;
                MemoryInformation.MemoryContinuous[j].PhysicalStart = temp.PhysicalStart;
            }
        }
    }

    // Initial 
    for (UINT16 i = 0; i < MemoryInformation.MemorySliceCount; i++)
    {
        UINT8 AddOne = (MemoryInformation.MemoryContinuous[i].NumberOfPages % 8 != 0) ? 1 : 0;

        // Allocate memory page use flag. for memory allocate or free.
        // Maybe use bit map, can save more memory
        MemoryInformation.MemoryContinuous[i].pMapper = AllocateZeroPool(MemoryInformation.MemoryContinuous[i].NumberOfPages);  
        if (NULL == MemoryInformation.MemoryContinuous[i].pMapper)
        {
            //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: AllocateZeroPool failed \n", __LINE__);
        }
        UINT64 PhysicalStart = MemoryInformation.MemoryContinuous[i].PhysicalStart;
        UINT64 NumberOfPages = MemoryInformation.MemoryContinuous[i].NumberOfPages;
        MemoryInformation.MemoryContinuous[i].FreeNumberOfPages = NumberOfPages;
        
        // check sort above by pages whether success.
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: i: %d Start: %X Pages: %d End: %X \n", __LINE__, i, 
                                        PhysicalStart, 
                                        NumberOfPages,
                                        PhysicalStart +NumberOfPages * 4 * 1024);
                                            
    }

    // want to use Link to save Memory information
    // 1. Sort by avaliable pages number
    // 2. After allocate new memory,  the link can resort
    // 3. After free allocated memory, the link can resort
    // 4. when allocate memory, can allocate almost match with request
    // 5. maybe more and more fragment, after allocates and frees.
}

#define BitSet(x,y) x |= (0x01 << y)
#define BitSlr(x,y) x &= ~(0x01 << y)
#define BitReverse(x,y) x ^= (0x01 << y)
#define BitGet(x,y)  ((x) >> (y) & 0x01)
#define ByteSet(x)    x |= 0xff
#define ByteClear(x)  x |= 0x00


EFI_STATUS L2_MEMORY_Free(UINT32 *p)
{   
    INFO_SELF(L"p: %X \r\n", p); 
    
    UINT64 PhysicalBlockStart;
    UINT64 PageStart;
    UINT64 Pages;
    UINT64 BlockID;
    UINT8 *pMapper;

    for (UINT64 i = 0; i < MemoryInformation.CurrentAllocatedCount; i++)
    {
        PhysicalBlockStart = MemoryInformation.AllocatedInformation[i][ALLOCATED_INFORMATION_DOMAIN_PHYSICAL_BLOCK_START] ; // start location
        PageStart          = MemoryInformation.AllocatedInformation[i][ALLOCATED_INFORMATION_DOMAIN_PAGE_START] ; // memory block size
        Pages              = MemoryInformation.AllocatedInformation[i][ALLOCATED_INFORMATION_DOMAIN_PAGE_COUNT] ; 
        BlockID            = MemoryInformation.AllocatedInformation[i][ALLOCATED_INFORMATION_DOMAIN_PHYSICAL_BLOCK_ID] ; 

        INFO_SELF(L"PhysicalBlockStart: %X, PageStart:%llu, Pages: %llu, BlockID: %llu \r\n", PhysicalBlockStart, PageStart, Pages, BlockID); 

        if (p == PhysicalBlockStart + PageStart * 8 * 512)
        {           
            INFO_SELF(L"PhysicalBlockStart: %X, PageStart:%llu, Pages: %llu, BlockID: %llu \r\n", PhysicalBlockStart, PageStart, Pages, BlockID); 
            pMapper = MemoryInformation.MemoryContinuous[BlockID].pMapper;
            for (UINT64 j = 0; j < Pages; j++)
                pMapper[PageStart + j] = 0; 

            p = NULL;
            
            break;
        }
    }

}

float L2_MEMORY_UseRecords(UINT32 *p)
{  }

float L2_MEMORY_Remainings()
{  }

float L2_MEMORY_GETs()
{  
    EFI_STATUS                           Status;
    UINT8                                TmpMemoryMap[1];
    UINTN                                MapKey;
    UINTN                                DescriptorSize;
    UINT32                               DescriptorVersion;
    UINTN                                MemoryMapSize;
    EFI_MEMORY_DESCRIPTOR                *MemoryMap;
    UINTN MemoryClassifySize[6] = {0};

    L1_MEMORY_SetValue(&MemoryInformation, sizeof(MemoryInformation), 0);
    
    //
    // Get System MemoryMapSize
    //
    MemoryMapSize = sizeof (TmpMemoryMap);
    Status = gBS->GetMemoryMap (&MemoryMapSize,
                                (EFI_MEMORY_DESCRIPTOR *)TmpMemoryMap,
                                &MapKey,
                                &DescriptorSize,
                                &DescriptorVersion);
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);            
    //DEBUG ((EFI_D_INFO, "%d:  Status:%X \n", __LINE__, Status));
    ASSERT (Status == EFI_BUFFER_TOO_SMALL);
    //
    // Enlarge space here, because we will allocate pool now.
    //
    MemoryMapSize += EFI_PAGE_SIZE;
    Status = gBS->AllocatePool (EfiLoaderData,
                                MemoryMapSize,
                                (VOID **) &MemoryMap);
    ASSERT_EFI_ERROR (Status);
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
        
    //DEBUG ((EFI_D_INFO, "%d:  Status:%X \n", __LINE__, Status));
    //
    // Get System MemoryMap
    //
    Status = gBS->GetMemoryMap (&MemoryMapSize,
                                MemoryMap,
                                &MapKey,
                                &DescriptorSize,
                                &DescriptorVersion);
    ASSERT_EFI_ERROR (Status);
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
        
    //DEBUG ((EFI_D_INFO, "%d:  Status:%X \n", __LINE__, Status));
    
    EFI_PHYSICAL_ADDRESS lastPhysicalEnd = 0;
    UINTN ContinuousMemoryPages = 0;
    UINTN ContinuousMemoryStart = 0;
    UINTN ContinuousVirtualMemoryStart = 0;
    UINTN MemoryAllSize = 0;
    UINTN E820Type = 0;

    //
    for (UINT16 Index = 0; Index < (MemoryMapSize / DescriptorSize); Index++) 
    //for (UINT16 Index = 0; Index < (MemoryMapSize / DescriptorSize) - 1; Index++)     
    //for (UINT16 Index = 0; Index < 20; Index++) 
    {
        E820Type = 0;
      //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Index:%X \n", __LINE__, Index);
        
      ////DEBUG ((EFI_D_INFO, "%d:  Status:%X \n", __LINE__, Status));
    
      if (MemoryMap->NumberOfPages == 0) 
      {
          continue;
      }
    
      switch(MemoryMap->Type) 
      {
          case EfiReservedMemoryType:
          case EfiRuntimeServicesCode:
          case EfiRuntimeServicesData:
          case EfiMemoryMappedIO:
          case EfiMemoryMappedIOPortSpace:
          case EfiPalCode:
                E820Type = E820_RESERVED;
                MemoryClassifySize[0] += MemoryMap->NumberOfPages;
                break;
                
          /// Memory in which errors have been detected.
          case EfiUnusableMemory:
                E820Type = E820_UNUSABLE;
                MemoryClassifySize[1] += MemoryMap->NumberOfPages;
                break;

           //// Memory that holds the ACPI tables.
          case EfiACPIReclaimMemory:
                E820Type = E820_ACPI;
                MemoryClassifySize[2] += MemoryMap->NumberOfPages;
                break;
        
          case EfiLoaderCode:
          case EfiLoaderData:
          case EfiBootServicesCode:
          case EfiBootServicesData:
          case EfiConventionalMemory:
                E820Type = E820_RAM; // Random access memory
                MemoryClassifySize[3] += MemoryMap->NumberOfPages;

                
                /*L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Index: %d: Start: %X Pages:%X End: %X Type: %d \n", __LINE__, 
                                                                            Index,
                                                                            MemoryMap->PhysicalStart, 
                                                                            MemoryMap->NumberOfPages,
                                                                            MemoryMap->PhysicalStart + MemoryMap->NumberOfPages * 4 * 1024,
                                                                            MemoryMap->Type);*/
                                                                            
                if (lastPhysicalEnd != MemoryMap->PhysicalStart)
               {
                    /* INFO_SELF(L"Start: %X Pages:%X End: %X\n", MemoryMap->PhysicalStart,
                                                                                ContinuousMemoryPages,
                                                                                ContinuousMemoryStart + ContinuousMemoryPages * 4 * 1024);
                                                                                
                     //DEBUG ((EFI_D_INFO, "%d: E820Type:%X Start:%X Virtual Start:%X Number:%X\n", __LINE__, 
                                                                                ContinuousMemoryStart,
                                                                                ContinuousVirtualMemoryStart,
                                                                                ContinuousMemoryPages,
                                                                                ContinuousMemoryStart + ContinuousMemoryPages * 4 * 1024);
        
                    */

                    MemoryInformation.MemoryContinuous[MemoryInformation.MemorySliceCount].PhysicalStart = ContinuousMemoryStart;
                    MemoryInformation.MemoryContinuous[MemoryInformation.MemorySliceCount].NumberOfPages = ContinuousMemoryPages;
                    MemoryInformation.MemorySliceCount++;
                    
                    /*
                    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Start: %X  Virtual Start:%X: Pages:%X End: %X\n", __LINE__, 
                                                                                MemoryMap->PhysicalStart,
                                                                                ContinuousVirtualMemoryStart,
                                                                                ContinuousMemoryPages,
                                                                                ContinuousMemoryStart + ContinuousMemoryPages * 4 * 1024);
                    */
                    MemoryAllSize += ContinuousMemoryPages;
                    ContinuousMemoryPages = 0;    
                    ContinuousMemoryStart = MemoryMap->PhysicalStart;
                    ContinuousVirtualMemoryStart = MemoryMap->VirtualStart;
                }
                 ContinuousMemoryPages += MemoryMap->NumberOfPages;
               lastPhysicalEnd = MemoryMap->PhysicalStart + MemoryMap->NumberOfPages * 4 * 1024;
                break;
        
          case EfiACPIMemoryNVS:       // // Address space reserved for use by the firmware.
                MemoryClassifySize[4] += MemoryMap->NumberOfPages;
                E820Type = E820_NVS;
                break;
        
          default:
                MemoryClassifySize[5] += MemoryMap->NumberOfPages;
                /*DEBUG ((DEBUG_ERROR,
                  "Invalid EFI memory descriptor type (0x%x)!\n",
                  MemoryMap->Type));
                  */  
                   //DEBUG ((EFI_D_INFO, "%d:  Invalid EFI memory descriptor type (0x%x)!\n", __LINE__, MemoryMap->Type));
                continue;         
      }

      /*//DEBUG ((EFI_D_INFO, "%d: E820Type:%X Start:%X Virtual Start:%X Number:%X\n", __LINE__, 
                                                                            E820Type, 
                                                                            MemoryMap->PhysicalStart, 
                                                                            MemoryMap->VirtualStart, 
                                                                            MemoryMap->NumberOfPages));
        */  //
        // Get next item
        //
        MemoryMap = (EFI_MEMORY_DESCRIPTOR *)((UINTN)MemoryMap + DescriptorSize);
    }
    
    MemoryAllSize += ContinuousMemoryPages;

    MemoryInformation.MemoryContinuous[MemoryInformation.MemorySliceCount].PhysicalStart = ContinuousMemoryStart;
    MemoryInformation.MemoryContinuous[MemoryInformation.MemorySliceCount].NumberOfPages = ContinuousMemoryPages;
    MemoryInformation.MemorySliceCount++;
      
    /*INFO_SELF(L"Start: %X Pages:%X End: %X\n", MemoryMap->PhysicalStart,
                                                              ContinuousMemoryPages,
                                                              ContinuousMemoryStart + ContinuousMemoryPages * 4 * 1024);
                                                                  
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Start: %X: Pages:%X End: %X\n", __LINE__, 
                                                                ContinuousMemoryStart, 
                                                                ContinuousMemoryPages,
                                                                ContinuousMemoryStart + ContinuousMemoryPages * 4 * 1024);
    
    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d:  (MemoryMapSize / DescriptorSize):%X MemoryClassifySize[0]:%d %d %d %d %d %d MemoryAllSize: %d\n", __LINE__,
                                                                                (MemoryMapSize / DescriptorSize),
                                                                        MemoryClassifySize[0],
                                                                        MemoryClassifySize[1],
                                                                        MemoryClassifySize[2],
                                                                        MemoryClassifySize[3],
                                                                        MemoryClassifySize[4],
                                                                        MemoryClassifySize[5],
                                                                        MemoryAllSize
                                                                        );*/
    return  MemoryClassifySize[3];                                                                    
}

float MemorySize = 0;

float L2_MEMORY_Initial()
{
    MemorySize = (float)L2_MEMORY_GETs();
    L2_MEMORY_MapInitial();
}

char *L1_STRING_FloatToString(float val, int precision, char *buf)
{
    char *cur, *end;
    float temp = 0;
    int count = 0;

    if (val == 0)
    {
        buf[count++] = '0';
        buf[count++] = '\0';
        return;
    }

    if (val > 0)
    {
    buf[count++] = (int)val + '0'; 
    }

    buf[count++] = '.'; 

    temp = val - (int)val;
    //printf("%d: %f\n", __LINE__, temp);

    while(precision--) 
    {
        temp = temp * 10;
        buf[count++] = (int)temp + '0';
        temp = temp - (int)temp;
    }
    return buf;
}



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

    L2_GRAPHICS_ChineseCharDraw2(pBuffer, Width - 3 * 16 - 3, 6, (12 - 1) * 94 + 58 - 1, Color, Width);
    L2_GRAPHICS_ChineseCharDraw2(pBuffer, Width - 2 * 16 - 3, 6, (01 - 1) * 94 + 85 - 1, Color, Width);
    L2_GRAPHICS_ChineseCharDraw2(pBuffer, Width - 1 * 16 - 3, 6, (14 - 1) * 94 + 21 - 1, Color, Width);

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
    L2_GRAPHICS_ChineseCharDraw2(pMyComputerBuffer, x, y, (46 - 1) * 94 + 50 - 1, Color, MyComputerWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pMyComputerBuffer, x, y, (21 - 1) * 94 + 36 - 1, Color, MyComputerWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pMyComputerBuffer, x, y, (21 - 1) * 94 + 71 - 1, Color, MyComputerWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pMyComputerBuffer, x, y, (36 - 1) * 94 + 52 - 1, Color, MyComputerWidth);

    
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
        x = 50;
        y = i * 18 + 16 * 2;        

        if (device[i].DeviceType == 2)
        {
            //
            L2_GRAPHICS_ChineseCharDraw2(pBuffer, x, y,          (20 - 1 ) * 94 + 37 - 1, Color, MyComputerWidth); 
            x += 16;
        }
        else
        {
            // U pan
            L2_GRAPHICS_ChineseCharDraw2(pBuffer, x, y,          (51 - 1 ) * 94 + 37 - 1, Color, MyComputerWidth); 
            x += 16;
        }   
        //pan
        L2_GRAPHICS_ChineseCharDraw2(pBuffer, x, y,          (37 - 1 ) * 94 + 44 - 1, Color, MyComputerWidth); 
        x += 16;

        //  2354 分 3988 区
        L2_GRAPHICS_ChineseCharDraw2(pBuffer, x, y,          (23 - 1 ) * 94 + 54 - 1, Color, MyComputerWidth);    
        x += 16;
        
        L2_GRAPHICS_ChineseCharDraw2(pBuffer, x, y,          (39 - 1 ) * 94 + 88 - 1, Color, MyComputerWidth);   
        x += 16;

        // 5027
        // 一
        // 2294
        // 二
        // 4093
        // 三
        if (device[i].PartitionID == 1)
        {
            L2_GRAPHICS_ChineseCharDraw2(pBuffer, x, y,          (50 - 1 ) * 94 + 27 - 1, Color, MyComputerWidth);   
            x += 16;
        }
        else if (device[i].PartitionID == 2)
        {
            L2_GRAPHICS_ChineseCharDraw2(pBuffer, x, y,          (22 - 1 ) * 94 + 94 - 1, Color, MyComputerWidth);   
            x += 16;
        }
        else if (device[i].PartitionID == 3)
        {
            L2_GRAPHICS_ChineseCharDraw2(pBuffer, x, y,          (40 - 1 ) * 94 + 93 - 1, Color, MyComputerWidth);   
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
        L2_GRAPHICS_ChineseCharDraw2(pBuffer, x, y,          (20 - 1 ) * 94 + 83 - 1, Color, MyComputerWidth);  
        x += 16;
        
        L2_GRAPHICS_ChineseCharDraw2(pBuffer, x, y,          (48 - 1 ) * 94 + 1 - 1, Color, MyComputerWidth);  
        x += 16;

        L2_DEBUG_Print3(x, y, WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW], "%d%a", size, sizePostfix);
        x += 16;

        x += 64;

        char type[10] = "OTHER";
        UINT8 FileSystemType = L2_FILE_PartitionTypeAnalysis(i);
        if (FILE_SYSTEM_FAT32 == FileSystemType)
        {
            type[0] = 'F';
            type[1] = 'A';
            type[2] = 'T';
            type[3] = '3';
            type[4] = '2';
            type[5] = '\0';
        }
        else if(FILE_SYSTEM_NTFS == FileSystemType) 
        {   
            type[0] = 'N';
            type[1] = 'T';
            type[2] = 'F';
            type[3] = 'S';
            type[4] = '\0';
        }

        L2_DEBUG_Print3(x, y, WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW], "%a", type);
        
    }
    
    y += 16;
    y += 16;
    
    x = 50;
    //3658
    //内
    //2070
    //存
    L2_GRAPHICS_ChineseCharDraw2(pBuffer, x, y,          (36 - 1 ) * 94 + 58 - 1, Color, MyComputerWidth);  
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pBuffer, x, y,          (20 - 1 ) * 94 + 70 - 1, Color, MyComputerWidth);  
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
    
}

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

    MemoryInformationWindowPositionX = StartX;
    MemoryInformationWindowPositionY = StartY;

    
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
    L2_GRAPHICS_ChineseCharDraw2(pBuffer, TitleX, TitleY, (36 - 1) * 94 + 58 - 1, Color, Width); 

    TitleX += 16;
    L2_GRAPHICS_ChineseCharDraw2(pBuffer, TitleX, TitleY, (20 - 1) * 94 + 70 - 1, Color, Width);
    
    TitleX += 16;
    L2_GRAPHICS_ChineseCharDraw2(pBuffer, TitleX, TitleY, (47 - 1) * 94 + 74 - 1, Color, Width);
    
    TitleX += 16;
    L2_GRAPHICS_ChineseCharDraw2(pBuffer, TitleX, TitleY, (47 - 1) * 94 + 24 - 1, Color, Width);

    L2_GRAPHICS_ChineseCharDraw2(pBuffer, MyComputerWidth - 3 * 16 - 3, 6, (12 - 1) * 94 + 58 - 1, Color, Width);
    L2_GRAPHICS_ChineseCharDraw2(pBuffer, MyComputerWidth - 2 * 16 - 3, 6, (01 - 1) * 94 + 85 - 1, Color, Width);
    L2_GRAPHICS_ChineseCharDraw2(pBuffer, MyComputerWidth - 1 * 16 - 3, 6, (14 - 1) * 94 + 21 - 1, Color, Width);

    // The Left of Window
    for (i = 23; i < Height; i++)
    {
        for (j = 0; j < Width / 3; j++)
        {
            pBuffer[(i * Width + j) * 4] = 214;
            pBuffer[(i * Width + j) * 4 + 1] = 211;
            pBuffer[(i * Width + j) * 4 + 2] = 204;
            pBuffer[(i * Width + j) * 4 + 3] = GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW;
        }
    }

    // The right of Window
    for (i = 23; i < Height; i++)
    {
        for (j = Width / 3 - 1; j < Width; j++)
        {
            pBuffer[(i * Width + j) * 4] = 214;
            pBuffer[(i * Width + j) * 4 + 1] = 211;
            pBuffer[(i * Width + j) * 4 + 2] = 104;
            pBuffer[(i * Width + j) * 4 + 3] = GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW;
        }
    }
    
    int x = 0, y = 0;
    
    y += 16;
    y += 16;
    
    x = 50;
    //3658
    //内
    //2070
    //存
    L2_GRAPHICS_ChineseCharDraw2(pBuffer, x, y,          (36 - 1 ) * 94 + 58 - 1, Color, Width);  
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pBuffer, x, y,          (20 - 1 ) * 94 + 70 - 1, Color, Width);  
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


VOID L3_APPLICATION_SystemLogWindow(UINT16 StartX, UINT16 StartY)
{
    UINT8 *pParent;
    UINT16 Type;
    CHAR8 *pWindowTitle;
    UINT16 i = 0;
    UINT16 j = 0;
    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: SystemLogWindow: %d \n", __LINE__, SystemLogWindowWidth);
    L3_WINDOW_Create(pSystemLogWindowBuffer, pParent, SystemLogWindowWidth, SystemLogWindowHeight, GRAPHICS_LAYER_SYSTEM_LOG_WINDOW, pWindowTitle);

    UINT8 *pBuffer = pSystemLogWindowBuffer;
    UINT16 Width = SystemLogWindowWidth;
    UINT16 Height = SystemLogWindowHeight;

    SystemLogWindowPositionX = StartX;
    SystemLogWindowPositionY= StartY;

    
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].StartX = StartX;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].StartY = StartY;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].WindowWidth = SystemLogWindowWidth;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].WindowHeight= SystemLogWindowHeight;
    
    
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    
    Color.Blue  = 0xff;
    Color.Red   = 0xff;
    Color.Green = 0xff;
    Color.Reserved = GRAPHICS_LAYER_SYSTEM_LOG_WINDOW;
    
    //汉字	区位码	汉字	区位码	汉字	区位码	汉字	区位码
    //系	4721	统	4519	日	4053	志	5430
    UINT16 TitleX = 3;
    UINT16 TitleY = 6;
    // wo de dian nao
    L2_GRAPHICS_ChineseCharDraw2(pBuffer, TitleX, TitleY, (47 - 1) * 94 + 21 - 1, Color, Width); 

    TitleX += 16;
    L2_GRAPHICS_ChineseCharDraw2(pBuffer, TitleX, TitleY, (45 - 1) * 94 + 19 - 1, Color, Width);
    
    TitleX += 16;
    L2_GRAPHICS_ChineseCharDraw2(pBuffer, TitleX, TitleY, (40 - 1) * 94 + 53 - 1, Color, Width);
    
    TitleX += 16;
    L2_GRAPHICS_ChineseCharDraw2(pBuffer, TitleX, TitleY, (54 - 1) * 94 + 30 - 1, Color, Width);

    L2_GRAPHICS_ChineseCharDraw2(pBuffer, Width - 3 * 16 - 3, 6, (12 - 1) * 94 + 58 - 1, Color, Width);
    L2_GRAPHICS_ChineseCharDraw2(pBuffer, Width - 2 * 16 - 3, 6, (01 - 1) * 94 + 85 - 1, Color, Width);
    L2_GRAPHICS_ChineseCharDraw2(pBuffer, Width - 1 * 16 - 3, 6, (14 - 1) * 94 + 21 - 1, Color, Width);

    // The Left of Window
    for (i = 23; i < Height; i++)
    {
        for (j = 0; j < Width / 3; j++)
        {
            pBuffer[(i * Width + j) * 4] = 214;
            pBuffer[(i * Width + j) * 4 + 1] = 211;
            pBuffer[(i * Width + j) * 4 + 2] = 204;
            pBuffer[(i * Width + j) * 4 + 3] = GRAPHICS_LAYER_SYSTEM_LOG_WINDOW;
        }
    }

    // The right of Window
    for (i = 23; i < Height; i++)
    {
        for (j = Width / 3 - 1; j < Width; j++)
        {
            pBuffer[(i * Width + j) * 4] = 214;
            pBuffer[(i * Width + j) * 4 + 1] = 211;
            pBuffer[(i * Width + j) * 4 + 2] = 104;
            pBuffer[(i * Width + j) * 4 + 3] = GRAPHICS_LAYER_SYSTEM_LOG_WINDOW;
        }
    }
    
    int x = 0, y = 0;
    
    y += 16;
    y += 16;
    
    x = 50;
    //3658
    //内
    //2070
    //存
    L2_GRAPHICS_ChineseCharDraw2(pBuffer, x, y,          (36 - 1 ) * 94 + 58 - 1, Color, Width);  
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pBuffer, x, y,          (20 - 1 ) * 94 + 70 - 1, Color, Width);  
    x += 16;
    
    return EFI_SUCCESS;
}


EFI_STATUS L3_APPLICATION_ReadFile(UINT8 *FileName, UINT8 NameLength, UINT8 *pBuffer)
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FileName: %a \n", __LINE__, FileName);
    if (pBuffer == NULL)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d:  \n", __LINE__);
        return -1;
    }
    
    L1_MEMORY_Copy(ReadFileName, FileName, NameLength);
    pReadFileDestBuffer = pBuffer;
    ReadFileNameLength = NameLength;

    FileReadCount = 0;
    sector_count = 0;
    PreviousBlockNumber = 0;
    FileBlockStart = 0;
    NextState = INIT_STATE;
    READ_FILE_FSM_Event = 0;

    for (int i = 0; i < 5; i++)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: i: %d \n", __LINE__, i);
        //DEBUG ((EFI_D_INFO, "%d HandleEnterPressed FSM_Event: %d\n", __LINE__, READ_FILE_FSM_Event));
        L2_STORE_FileRead(READ_FILE_FSM_Event++);

        if (READ_FILE_EVENT <= READ_FILE_FSM_Event)
            READ_FILE_FSM_Event = READ_FILE_EVENT;
    }

    
}

UINT8 readflag = 0;


VOID L2_KEYBOARD_KeyPressed()
{
    //DEBUG ((EFI_D_INFO, "%d HandleEnterPressed\n", __LINE__));
    
        
    EFI_STATUS Status;
    //PartitionUSBReadSynchronous();
    //PartitionUSBReadAsynchronous();

    // char buffer[SYSTEM_ICON_WIDTH * SYSTEM_ICON_HEIGHT * 3 + 0x36] = {0};
    
    //DEBUG ((EFI_D_INFO, "%d HandleEnterPressed\n", __LINE__));
    return EFI_SUCCESS;
}

STATIC
VOID
EFIAPI
L2_KEYBOARD_Event (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
    keyboard_count++;
    UINT16 scanCode = 0;
    UINT16 uniChar = 0;
    UINT32 shiftState;
    EFI_STATUS Status;

    EFI_KEY_TOGGLE_STATE toggleState;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;

    Color.Blue = 0xFF;
    Color.Red = 0xFF;
    Color.Green = 0xFF;

    EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *SimpleEx;
    EFI_KEY_DATA                      KeyData;
    EFI_HANDLE                        *Handles;
    UINTN                             HandleCount;
    UINTN                             HandleIndex;
    UINTN                             Index;
        
    Status = gBS->LocateHandleBuffer ( ByProtocol,
                                        &gEfiSimpleTextInputExProtocolGuid,
                                        NULL,
                                        &HandleCount,
                                        &Handles
                                        );    
    if(EFI_ERROR (Status))
        return ;
    //L2_DEBUG_Print1(DISPLAY_X, DISPLAY_Y, "%d:HandleKeyboardEvent \n", __LINE__);
    for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) 
    {
        Status = gBS->HandleProtocol (Handles[HandleIndex], &gEfiSimpleTextInputExProtocolGuid, (VOID **) &SimpleEx);
        if (EFI_ERROR(Status))  
        {    
            continue;
        }
        
        Status = gBS->CheckEvent(SimpleEx->WaitForKeyEx);
        if (Status == EFI_NOT_READY)
            continue;
          
        gBS->WaitForEvent(1, &(SimpleEx->WaitForKeyEx), &Index);

        Status = SimpleEx->ReadKeyStrokeEx(SimpleEx, &KeyData);
        if(EFI_ERROR(Status))
        {
            continue;
        }
        
        scanCode    = KeyData.Key.ScanCode;
        uniChar     = KeyData.Key.UnicodeChar;
        shiftState  = KeyData.KeyState.KeyShiftState;
        toggleState  = KeyData.KeyState.KeyToggleState;
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: keyboard input uniChar: %d", __LINE__, uniChar);
       
        pKeyboardInputBuffer[keyboard_input_count++] = uniChar;

        display_sector_number = uniChar - '0';

        if (display_sector_number > 10)
        {
            display_sector_number = 10;
        }

        if (display_sector_number < 0)
        {
            display_sector_number = 0;
        }

        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: uniChar: %d display_sector_number: %d \n", __LINE__, uniChar, display_sector_number);


        // Enter pressed
        if (0x0D == uniChar)
        {
            keyboard_input_count = 0;
            memset(pKeyboardInputBuffer, '\0', KEYBOARD_BUFFER_LENGTH);
            //L2_DEBUG_Print1(DISPLAY_KEYBOARD_X, DISPLAY_KEYBOARD_Y, "%a keyboard_input_count: %04d enter pressed", pKeyboardInputBuffer, keyboard_input_count);

            L2_KEYBOARD_KeyPressed();
        }
        else
        {
            L2_DEBUG_Print1(DISPLAY_KEYBOARD_X, DISPLAY_KEYBOARD_Y, "%a keyboard_input_count: %04d ", pKeyboardInputBuffer, keyboard_input_count);
        }
       
    }  
    
     //DrawAsciiCharUseBuffer(pDeskBuffer, DISPLAY_KEYBOARD_X, DISPLAY_KEYBOARD_Y, uniChar, Color);
     
    L2_GRAPHICS_LayerCompute(iMouseX, iMouseY, 0);
}

 // iMouseX: left top
 // iMouseY: left top
 VOID L2_GRAPHICS_RightClickMenu(UINT16 iMouseX, UINT16 iMouseY)
 {
     INT16 i;    
     UINT16 width = 100;
     UINT16 height = 300;
     EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
          
     for (i = 0; i < width * height; i++)
     {
         pMouseClickBuffer[i * 4] = 160;
         pMouseClickBuffer[i * 4 + 1] = 160;
         pMouseClickBuffer[i * 4 + 2] = 160;
     }

     Color.Blue = 0xFF;
     Color.Red  = 0xFF;
     Color.Green= 0xFF;
     
     //DrawChineseCharUseBuffer(pBuffer, 10, 10, sChinese[0], 5, Color, width);
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

STATIC
VOID
EFIAPI
L2_SYSTEM_Start (IN EFI_EVENT Event, IN VOID *Context)
{

    //如果不加下面这几行，则是直接显示内存信息，看起来有点像雪花
    /*
    for (int j = 0; j < ScreenHeight; j++)
    {
        for (int i = 0; i < ScreenWidth; i++)
        {
            pDeskBuffer[(j * ScreenWidth + i) * 4]     = TimerSliceCount % 256;
            pDeskBuffer[(j * ScreenWidth + i) * 4 + 1] = TimerSliceCount % 256;
            pDeskBuffer[(j * ScreenWidth + i) * 4 + 2] = TimerSliceCount % 256;
        }
    }       
    
    GraphicsOutput->Blt(
                GraphicsOutput, 
                (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) pDeskBuffer,
                EfiBltBufferToVideo,
                0, 0, 
                0, 0, 
                ScreenWidth, ScreenHeight, 0);   
    */

}


STATIC
VOID
EFIAPI
L2_MAIN_Start (IN EFI_EVENT Event, IN VOID *Context)
{

    //如果不加下面这几行，则是直接显示内存信息，看起来有点像雪花
    
    for (int j = 0; j < ScreenHeight; j++)
    {
        for (int i = 0; i < ScreenWidth; i++)
        {
            pDeskBuffer[(j * ScreenWidth + i) * 4]     = TimerSliceCount % 256;
            pDeskBuffer[(j * ScreenWidth + i) * 4 + 1] = TimerSliceCount % 256;
            pDeskBuffer[(j * ScreenWidth + i) * 4 + 2] = TimerSliceCount % 256;
        }
    }       
    /**/
    GraphicsOutput->Blt(
                GraphicsOutput, 
                (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) pDeskBuffer,
                EfiBltBufferToVideo,
                0, 0, 
                0, 0, 
                ScreenWidth, ScreenHeight, 0);   

}


// for mouse move & click
STATIC
VOID
EFIAPI
L2_MOUSE_Event (IN EFI_EVENT Event, IN VOID *Context)
{
    mouse_count++;
    //L2_DEBUG_Print1(DISPLAY_X, DISPLAY_Y, "%d: HandleMouseEvent\n", __LINE__);
    EFI_STATUS Status;
    UINTN Index;
    EFI_SIMPLE_POINTER_STATE State;

    Status = gBS->CheckEvent(gMouse->WaitForInput); 
    if (Status == EFI_NOT_READY)
    {
        //return ;
    }
    
    Status = gMouse->GetState(gMouse, &State);
    if (Status == EFI_DEVICE_ERROR)
    {
        return ;
    }
    if (0 == State.RelativeMovementX && 0 == State.RelativeMovementY && 0 == State.LeftButton && 0 == State.RightButton)
    {
        return;
    }
    
    //X
    if (State.RelativeMovementX < 0)
    {
        int temp = State.RelativeMovementX >> 16;
        x_move = (0xFFFF - temp) & 0xff;  
        x_move = - x_move;
    }
    else if(State.RelativeMovementX > 0)
    {
        x_move = (State.RelativeMovementX >> 16) & 0xff;
    }

    //Y
    if (State.RelativeMovementY < 0)
    {       
        int temp = State.RelativeMovementY >> 16;
        y_move = (0xFFFF - temp) & 0xff;  
        y_move = - y_move;
    }
    else if(State.RelativeMovementY > 0)
    {
        y_move = (State.RelativeMovementY >> 16) & 0xff;
    }

    ////DEBUG ((EFI_D_INFO, "X: %X, Y: %X ", x_move, y_move));
    L2_DEBUG_Print1(0, ScreenHeight - 30 -  8 * 16, "%d: X: %04d, Y: %04d move X: %X move Y: %X", __LINE__, iMouseX, iMouseY, x_move, y_move );
    
    //L2_DEBUG_Print1(0, ScreenHeight - 30 -  8 * 16, "%d: X move: %d Y move: %d left: %d right: %d", __LINE__, State.RelativeMovementX, State.RelativeMovementY, State.LeftButton, State.RightButton);
    iMouseX = iMouseX + x_move * 3;
    iMouseY = iMouseY + y_move * 3; 
    

    if (iMouseX < 0)
        iMouseX = 0;
        
    if (iMouseX > ScreenWidth)
        iMouseX = ScreenWidth;

    if (iMouseY < 0)
        iMouseY = 0;

    if (iMouseY > ScreenHeight)
        iMouseY = ScreenHeight;

    //Button
    if (State.LeftButton == 0x01)
    {
        //DEBUG ((EFI_D_INFO, "Left button clicked\n"));
        
        L2_GRAPHICS_RightClickMenu(iMouseX, iMouseY);
        
        MouseClickFlag = (MouseClickFlag == MOUSE_NO_CLICKED) ? MOUSE_LEFT_CLICKED : MOUSE_NO_CLICKED;
        
        //DrawAsciiCharUseBuffer(GraphicsOutput, 20 + process2_i * 8 + 16, 60, 'E', Color);  
    }
    
    if (State.RightButton == 0x01)
    {
        //DEBUG ((EFI_D_INFO, "Right button clicked\n"));
        //DrawAsciiCharUseBuffer(GraphicsOutput, 20 + process2_i * 8 + 16, 60, 'R', Color);

        L2_GRAPHICS_RightClickMenu(iMouseX, iMouseY);
        
        MouseClickFlag = 2;
    }
    //L2_DEBUG_Print1(DISPLAY_X, DISPLAY_Y, "%d: HandleMouseEvent\n", __LINE__);
    ////DEBUG ((EFI_D_INFO, "\n"));
    L2_MOUSE_Move();
    
    gBS->WaitForEvent( 1, &gMouse->WaitForInput, &Index );
}

UINT8 L1_TIMER_DayOfWeek(int y, int m, int d)
{
    UINT8 T;
    
    if ( m < 3 )
    {
        m += 12;
        y -= 1;
    }
    
    return ( (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400 + 1) % 7 + 7) % 7;
    /*
    if ( T == 0 )
    {
        printf( "Sunday\n" );
        L1_MEMORY_Copy();
    }
    
    if ( T == 1 )
    {
        printf( "Monday\n" );
        L1_MEMORY_Copy();
    }
        
    if ( T == 2 )
    {
        printf( "Tuesday\n" );
        L1_MEMORY_Copy();
    }
        
    if ( T == 3 )
    {
        printf( "Wednesday\n" );
        L1_MEMORY_Copy();
    }
        
    if ( T == 4 )
    {
        printf( "Thursday\n" );
        L1_MEMORY_Copy();
    }
        
    if ( T == 5 )
    {
        printf( "Friday\n" );
        L1_MEMORY_Copy();
    }
        
    if ( T == 6 )
    {
        printf( "Saturday\n" );
        L1_MEMORY_Copy();
    }*/
        
}

UINT16 date_time_count_increase_flag = 0;


// display system date & time
STATIC
VOID
EFIAPI
L2_TIMER_Print (
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
	
    gRT->GetTime(&EFITime, NULL);
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    UINT16 x, y;
    
    Color.Blue = 0x00;
    Color.Red = 0xFF;
    Color.Green = 0x00;

    x = DISPLAY_DESK_DATE_TIME_X;
    y = DISPLAY_DESK_DATE_TIME_Y;

    //Display system date time weekday.
    L2_DEBUG_Print1(x, y, "%04d-%02d-%02d %02d:%02d:%02d ", 
                  EFITime.Year, EFITime.Month, EFITime.Day, EFITime.Hour, EFITime.Minute, EFITime.Second);
    //  星   4839    期   3858
    x += 21 * 8 + 3;
    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer, x, y,  (48 - 1) * 94 + 39 - 1, Color, ScreenWidth); 
    
    x += 16;
    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer, x, y,  (38 - 1) * 94 + 58 - 1, Color, ScreenWidth);

    x += 16;

    UINT8 DayOfWeek = L1_TIMER_DayOfWeek(EFITime.Year, EFITime.Month, EFITime.Day);
    if (0 == DayOfWeek)
    {
        L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer, x, y, (48 - 1 ) * 94 + 39 - 1, Color, ScreenWidth);    
    }
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

    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer, x, y, (AreaCode - 1 ) * 94 + BitCode - 1, Color, ScreenWidth);
    
   L2_DEBUG_Print1(DISPLAY_DESK_HEIGHT_WEIGHT_X, DISPLAY_DESK_HEIGHT_WEIGHT_Y, "%d: ScreenWidth:%d, ScreenHeight:%d\n", __LINE__, ScreenWidth, ScreenHeight);
   /*
   GraphicsOutput->Blt(GraphicsOutput, 
                        (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) pDateTimeBuffer,
                        EfiBltBufferToVideo,
                        0, 0, 
                        0, 16 * 8, 
                        8 * 50, 16, 0);*/
}

EFI_STATUS L2_COMMON_SingleProcessInit ()
{
    UINT16 i;

    // task group for mouse keyboard
    EFI_GUID gMultiProcessGroup1Guid  = { 0x0579257E, 0x1843, 0x45FB, { 0x83, 0x9D, 0x6B, 0x79, 0x09, 0x38, 0x29, 0xA9 } };
        
    EFI_EVENT_NOTIFY       TaskProcessesGroup1[] = {L2_SYSTEM_Start, L2_MAIN_Start};

    for (i = 0; i < sizeof(TaskProcessesGroup1) / sizeof(EFI_EVENT_NOTIFY); i++)
    {
        gBS->CreateEventEx(
                          EVT_NOTIFY_SIGNAL,
                          TPL_NOTIFY,
                          TaskProcessesGroup1[i],
                          NULL,
                          &gMultiProcessGroup1Guid,
                          &MultiTaskTriggerGroup0Event
                          );
    }    

    return EFI_SUCCESS;
}


EFI_STATUS L2_COMMON_MultiProcessInit ()
{
    UINT16 i;

    // task group for mouse keyboard
    EFI_GUID gMultiProcessGroup1Guid  = { 0x0579257E, 0x1843, 0x45FB, { 0x83, 0x9D, 0x6B, 0x79, 0x09, 0x38, 0x29, 0xA9 } };
    
    // task group for display date time
    EFI_GUID gMultiProcessGroup2Guid  = { 0x0579257E, 0x1843, 0x45FB, { 0x83, 0x9D, 0x6B, 0x79, 0x09, 0x38, 0x29, 0xAA } };
    
    //L2_GRAPHICS_ChineseCharDraw2(pMouseBuffer, 0, 0, 11 * 94 + 42, Color, 16);
    
    //DrawChineseCharIntoBuffer(pMouseBuffer, 0, 0, 0, Color, 16);
    
    EFI_EVENT_NOTIFY       TaskProcessesGroup1[] = {L2_KEYBOARD_Event, L2_MOUSE_Event, L2_SYSTEM_Start};

    EFI_EVENT_NOTIFY       TaskProcessesGroup2[] = {L2_TIMER_Print};

    for (i = 0; i < sizeof(TaskProcessesGroup1) / sizeof(EFI_EVENT_NOTIFY); i++)
    {
        gBS->CreateEventEx(
                          EVT_NOTIFY_SIGNAL,
                          TPL_NOTIFY,
                          TaskProcessesGroup1[i],
                          NULL,
                          &gMultiProcessGroup1Guid,
                          &MultiTaskTriggerGroup1Event
                          );
    }    

    for (i = 0; i < sizeof(TaskProcessesGroup2) / sizeof(EFI_EVENT_NOTIFY); i++)
    {
        gBS->CreateEventEx(
                          EVT_NOTIFY_SIGNAL,
                          TPL_NOTIFY,
                          TaskProcessesGroup2[i],
                          NULL,
                          &gMultiProcessGroup2Guid,
                          &MultiTaskTriggerGroup2Event
                          );
    }    

    return EFI_SUCCESS;
}
// https://blog.csdn.net/longsonssss/article/details/80221513


EFI_STATUS L2_TIMER_IntervalInit0()
{
    EFI_STATUS  Status;
    EFI_HANDLE  TimerOne    = NULL;
    static const UINTN TimeInterval = 20000;
    
    UINT32 *TimerCount;

    TimerCount = (UINT32 *)AllocateZeroPool(4);
    if (NULL == TimerCount)
    {
        //DEBUG(( EFI_D_INFO, "%d, NULL == TimerCount \r\n", __LINE__));
        return;
    }

    Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL | EVT_TIMER,
                            TPL_CALLBACK,
                            L2_TIMER_Slice0,
                            (VOID *)TimerCount,
                            &TimerOne);

    if ( EFI_ERROR( Status ) )
    {
        //DEBUG(( EFI_D_INFO, "Create Event Error! \r\n" ));
        return(1);
    }

    Status = gBS->SetTimer(TimerOne,
                          TimerPeriodic,
                          MultU64x32( TimeInterval, 1)); // will multi 100, ns

    if ( EFI_ERROR( Status ) )
    {
        //DEBUG(( EFI_D_INFO, "Set Timer Error! \r\n" ));
        return(2);
    }

    while (1)
    {
        *TimerCount = *TimerCount + 1;
        //L2_DEBUG_Print1(DISPLAY_X, DISPLAY_Y, "%d: SystemTimeIntervalInit while\n", __LINE__);
        //if (*TimerCount % 1000000 == 0)
           //L2_DEBUG_Print1(0, 4 * 16, "%d: while (1) p:%x %lu \n", __LINE__, TimerCount, *TimerCount);
    }
    
    gBS->SetTimer( TimerOne, TimerCancel, 0 );
    gBS->CloseEvent( TimerOne );    

    return EFI_SUCCESS;
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
    
    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer,  x, y, (52 - 1) * 94 + 57 - 1, Color, ScreenWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer,  x, y, (28 - 1) * 94 + 91 - 1, Color, ScreenWidth);
    x += 16;

    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer,  x, y, (3 - 1) * 94 + 12 - 1, Color, ScreenWidth);
    x += 16;

    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer,  x, y, (27 - 1) * 94 + 22 - 1, Color, ScreenWidth);
    x += 16;

    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer,  x, y, (51 - 1) * 94 + 13 - 1, Color, ScreenWidth);
    x += 16;

    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer,  x, y, (47 - 1) * 94 + 34 - 1, Color, ScreenWidth);
    x += 16;

    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer,  x, y, (20 - 1) * 94 + 46 - 1, Color, ScreenWidth);
    x += 16;

    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer,  x, y, (27 - 1) * 94 + 56 - 1, Color, ScreenWidth);
    x += 16;

    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer,  x, y, (32 - 1) * 94 + 20 - 1, Color, ScreenWidth);
    x += 16;

    GraphicsOutput->Blt(
                GraphicsOutput, 
                (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) pDeskBuffer,
                EfiBltBufferToVideo,
                0, 0, 
                0, 0, 
                ScreenWidth, ScreenHeight, 0);   

}


EFI_STATUS L2_TIMER_IntervalInit()
{
    EFI_STATUS  Status;
    EFI_HANDLE  TimerOne    = NULL;
    static const UINTN TimeInterval = 20000;
    
    UINT32 *TimerCount;

    // initial with 0
    TimerCount = (UINT32 *)AllocateZeroPool(4);
    if (NULL == TimerCount)
    {
        //DEBUG(( EFI_D_INFO, "%d, NULL == TimerCount \r\n", __LINE__));
        return;
    }

    Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL | EVT_TIMER,
                            TPL_CALLBACK,
                            L2_TIMER_Slice,
                            (VOID *)TimerCount,
                            &TimerOne);

    if ( EFI_ERROR( Status ) )
    {
        //DEBUG(( EFI_D_INFO, "Create Event Error! \r\n" ));
        return(1);
    }

    Status = gBS->SetTimer(TimerOne,
                          TimerPeriodic,
                          MultU64x32( TimeInterval, 1)); // will multi 100, ns

    if ( EFI_ERROR( Status ) )
    {
        //DEBUG(( EFI_D_INFO, "Set Timer Error! \r\n" ));
        return(2);
    }
    UINT32 QuitTimerCount = 0;
    UINT32 SystemQuitCount = 0;
    while (1)
    {
        *TimerCount = *TimerCount + 1;
        //L2_DEBUG_Print1(DISPLAY_X, DISPLAY_Y, "%d: SystemTimeIntervalInit while\n", __LINE__);
        //if (*TimerCount % 1000000 == 0)
        L2_DEBUG_Print1(0, ScreenHeight - 30 - 4 * 16, "%d: L2_TIMER_IntervalInit p:%x %lu %llu\n", __LINE__, TimerCount, *TimerCount, QuitTimerCount);

        if (TRUE == SystemQuitFlag)
        {   
            if (QuitTimerCount == 0)
            {   
                QuitTimerCount = *TimerCount;
                L2_GRAPHICS_SayGoodBye();
            }
        }

        SystemQuitCount = *TimerCount - QuitTimerCount;

        if ((TRUE == SystemQuitFlag) && ( SystemQuitCount % 100 == 0))
        {
            UINT16 count = SystemQuitCount / 100;
            
            L2_DEBUG_Print1(0, ScreenHeight - 30 -  5 * 16, "%d: L2_TIMER_IntervalInit p:%x %lu \n", __LINE__, TimerCount, *TimerCount);
            
            for (UINT16 j = ScreenHeight / 4 ; j < ScreenHeight / 3; j++)
            {
                for (UINT16 i = count * 100; i < (count + 1) * 100; i++)
                {
                    pDeskBuffer[(j * ScreenWidth + i) * 4]     = 0xff;
                    pDeskBuffer[(j * ScreenWidth + i) * 4 + 1] = 0xff;
                    pDeskBuffer[(j * ScreenWidth + i) * 4 + 2] = 0;
                }
            }
            GraphicsOutput->Blt(GraphicsOutput, 
                                (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) pDeskBuffer,
                                EfiBltBufferToVideo,
                                0, 0, 
                                0, 0, 
                                ScreenWidth, ScreenHeight, 0);   
        }

        if ((TRUE == SystemQuitFlag) && ( SystemQuitCount >= ScreenWidth))
        {
            L2_DEBUG_Print1(0, ScreenHeight - 30 - 6 * 16, "%d: L2_TIMER_IntervalInit p:%x %lu \n", __LINE__, TimerCount, *TimerCount);
            
            gBS->SetTimer( TimerOne, TimerCancel, 0 );
            gBS->CloseEvent( TimerOne );  
            gRT->ResetSystem (EfiResetShutdown, EFI_SUCCESS, 0, NULL);  
        }
                   
    }
    
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
    L2_GRAPHICS_ChineseCharDraw2(pStartMenuBuffer, x , y,     (46 - 1 ) * 94 + 50 - 1, Color, StartMenuWidth);    
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pStartMenuBuffer, x , y,     (21 - 1) * 94 + 36 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pStartMenuBuffer, x , y,     (21 - 1) * 94 + 71 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pStartMenuBuffer, x , y,     (36 - 1) * 94 + 52 - 1, Color, StartMenuWidth);   

    //系统设置
    x = 3;
    y += 16;
    L2_GRAPHICS_ChineseCharDraw2(pStartMenuBuffer, x , y,     (47 - 1 ) * 94 + 21 - 1, Color, StartMenuWidth);    
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pStartMenuBuffer, x , y,     (45 - 1) * 94 + 19 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pStartMenuBuffer, x , y,     (41 - 1) * 94 + 72 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pStartMenuBuffer, x , y,     (54 - 1) * 94 + 35 - 1, Color, StartMenuWidth);   
    

    //内存查看
    //汉字    区位码 汉字  区位码 汉字  区位码 汉字  区位码
    //内 3658    存       2070    查       1873    看       3120
    x = 3;
    y += 16;
    L2_GRAPHICS_ChineseCharDraw2(pStartMenuBuffer, x , y,     (36 - 1 ) * 94 + 58 - 1, Color, StartMenuWidth);    
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pStartMenuBuffer, x , y,     (20 - 1) * 94 + 70 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pStartMenuBuffer, x , y,     (18 - 1) * 94 + 73 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pStartMenuBuffer, x , y,     (31 - 1) * 94 + 20 - 1, Color, StartMenuWidth);   
    
    //系统日志
    //汉字	区位码	汉字	区位码	汉字	区位码	汉字	区位码
    //系	4721	统	4519	日	4053	志	5430
    x = 3;
    y += 16;
    L2_GRAPHICS_ChineseCharDraw2(pStartMenuBuffer, x , y,     (47 - 1 ) * 94 + 21 - 1, Color, StartMenuWidth);    
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pStartMenuBuffer, x , y,     (45 - 1) * 94 + 19 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pStartMenuBuffer, x , y,     (40 - 1) * 94 + 53 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pStartMenuBuffer, x , y,     (54 - 1) * 94 + 30 - 1, Color, StartMenuWidth);   
    
    //系统退出
    //退 4543    出   1986
    x = 3;
    y += 16;
    L2_GRAPHICS_ChineseCharDraw2(pStartMenuBuffer, x , y,     (47 - 1 ) * 94 + 21 - 1, Color, StartMenuWidth);    
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pStartMenuBuffer, x , y,     (45 - 1) * 94 + 19 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pStartMenuBuffer, x , y,     (45 - 1) * 94 + 43 - 1, Color, StartMenuWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pStartMenuBuffer, x , y,     (19 - 1) * 94 + 86 - 1, Color, StartMenuWidth);   
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
    L2_GRAPHICS_ChineseCharDraw2(pSystemSettingWindowBuffer, x , y,   (17 - 1) * 94 + 19 - 1, Color, SystemSettingWindowWidth);   
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pSystemSettingWindowBuffer, x , y,   (30 - 1) * 94 + 16 - 1, Color, SystemSettingWindowWidth);
    x += 16;
        
    L2_GRAPHICS_ChineseCharDraw2(pSystemSettingWindowBuffer, x , y,   (41 - 1) * 94 + 72 - 1, Color, SystemSettingWindowWidth); 
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pSystemSettingWindowBuffer, x , y,   (54 - 1) * 94 + 35 - 1, Color, SystemSettingWindowWidth);
    x += 16;

    //背景还原
    //还 2725    原   5213
    x = 3;
    y += 16;
    L2_GRAPHICS_ChineseCharDraw2(pSystemSettingWindowBuffer, x , y,   (17 - 1) * 94 + 19 - 1, Color, SystemSettingWindowWidth);   
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pSystemSettingWindowBuffer, x , y,   (30 - 1) * 94 + 16 - 1, Color, SystemSettingWindowWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pSystemSettingWindowBuffer, x , y,   (27 - 1) * 94 + 25 - 1, Color, SystemSettingWindowWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pSystemSettingWindowBuffer, x , y,   (52 - 1) * 94 + 13 - 1, Color, SystemSettingWindowWidth);
    

}

double L1_GRAPHICS_Bilinear(double a, double blue, int uv, int u1v, int uv1, int u1v1)
{
    return (double) (uv*(1-a)*(1-blue)+u1v*a*(1-blue)+uv1*blue*(1-a)+u1v1*a*blue);
}

void L1_GRAPHICS_ZoomImage(UINT8* pDest, int DestWidth, int DestHeight, UINT8* pSource, int SourceWidth, int SourceHeight )
{
    UINT8    *pDestTemp;
    UINT8    *pSourceTemp;
    UINT16    i, j;
    pDestTemp    = pDest;
    pSourceTemp  = pSource;


    for ( i = 0; i < DestHeight; i++ )
    {
        for ( j = 0; j < DestWidth; j++ )
        {
            int    HeightRatio, WidthRatio;
            double    u, v, red, green, blue;
            v    = (i * SourceHeight) / (double) (DestHeight);
            u    = (j * SourceWidth) / (double) (DestWidth);
            HeightRatio    = (int) (v);
            WidthRatio    = (int) (u);

            blue = L1_GRAPHICS_Bilinear( u - WidthRatio, v - HeightRatio,
                      pSourceTemp[HeightRatio * SourceWidth * 3 + WidthRatio * 3],
                      pSourceTemp[HeightRatio * SourceWidth * 3 + (WidthRatio + 1) * 3],
                      pSourceTemp[(HeightRatio + 1) * SourceWidth * 3 + WidthRatio * 3],
                      pSourceTemp[(HeightRatio + 1) * SourceWidth * 3 + (WidthRatio + 1) * 3] );
                      
            green = L1_GRAPHICS_Bilinear( u - WidthRatio, v - HeightRatio,
                      pSourceTemp[HeightRatio * SourceWidth * 3 + WidthRatio * 3 + 1],
                      pSourceTemp[HeightRatio * SourceWidth * 3 + (WidthRatio + 1) * 3 + 1],
                      pSourceTemp[(HeightRatio + 1) * SourceWidth * 3 + WidthRatio * 3 + 1],
                      pSourceTemp[(HeightRatio + 1) * SourceWidth * 3 + (WidthRatio + 1) * 3 + 1] );
                      
            red = L1_GRAPHICS_Bilinear( u - WidthRatio, v - HeightRatio,
                      pSourceTemp[HeightRatio * SourceWidth * 3 + WidthRatio * 3 + 2],
                      pSourceTemp[HeightRatio * SourceWidth * 3 + (WidthRatio + 1) * 3 + 2],
                      pSourceTemp[(HeightRatio + 1) * SourceWidth * 3 + WidthRatio * 3 + 2],
                      pSourceTemp[(HeightRatio + 1) * SourceWidth * 3 + (WidthRatio + 1) * 3 + 2] );

            pDestTemp[i * DestWidth * 3 + j * 3 + 2]    = red;
            pDestTemp[i * DestWidth * 3 + j * 3 + 1]    = green;
            pDestTemp[i * DestWidth * 3 + j * 3 + 0]    = blue;
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
    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer, x1, y1, (46 - 1) * 94 + 50 - 1, Color, ScreenWidth);
    x1 += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer, x1, y1, (21 - 1) * 94 + 36 - 1, Color, ScreenWidth);
    x1 += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer, x1, y1, (21 - 1) * 94 + 71 - 1, Color, ScreenWidth);
    x1 += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer, x1, y1, (36 - 1) * 94 + 52 - 1, Color, ScreenWidth);

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
    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer, x1, y1, (46 - 1) * 94 + 50 - 1, Color, ScreenWidth);
    x1 += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer, x1, y1, (21 - 1) * 94 + 36 - 1, Color, ScreenWidth);
    x1 += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer, x1, y1, (21 - 1) * 94 + 71 - 1, Color, ScreenWidth);
    x1 += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer, x1, y1, (36 - 1) * 94 + 52 - 1, Color, ScreenWidth);

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
    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer, x1, y1, (46 - 1) * 94 + 50 - 1, Color, ScreenWidth);
    x1 += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer, x1, y1, (21 - 1) * 94 + 36 - 1, Color, ScreenWidth);
    x1 += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer, x1, y1, (21 - 1) * 94 + 71 - 1, Color, ScreenWidth);
    x1 += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer, x1, y1, (36 - 1) * 94 + 52 - 1, Color, ScreenWidth);

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
            L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer,  (16 + 2) * j, i * (16 + 2) + 322, (i - 1) * 94 + j - 1, Color, ScreenWidth);
        }
    }*/


    for (int j = 0; j < 150; j++)
    {
        //L2_DEBUG_Print1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", sChineseChar[j] & 0xff);
    }

    // menu chinese
    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer,  16, ScreenHeight - 21,     (18 - 1) * 94 + 43 - 1, Color, ScreenWidth);
    L2_GRAPHICS_ChineseCharDraw2(pDeskBuffer,  16 * 2, ScreenHeight - 21, (21 - 1) * 94 + 05 - 1, Color, ScreenWidth);
}



VOID L2_MOUSE_WallpaperResetClicked()
{   
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_WallpaperResetClicked\n", __LINE__);
    
    L2_GRAPHICS_DeskInit();
}


START_MENU_STATE_TRANSFORM StartMenuStateTransformTable[] =
{
    {CLICK_INIT_STATE,              START_MENU_CLICKED_EVENT,           MENU_CLICKED_STATE,                 L2_MOUSE_MENU_Clicked},
    {MENU_CLICKED_STATE,            MY_COMPUTER_CLICKED_EVENT,          MY_COMPUTER_CLICKED_STATE,          L2_MOUSE_MyComputerClicked},
    {MENU_CLICKED_STATE,            SETTING_CLICKED_EVENT,              SYSTEM_SETTING_CLICKED_STATE,       L2_MOUSE_SystemSettingClicked},
    {MENU_CLICKED_STATE,            MEMORY_INFORMATION_CLICKED_EVENT,   MEMORY_INFORMATION_CLICKED_STATE,   L2_MOUSE_MemoryInformationClicked},
    {MENU_CLICKED_STATE,            SYSTEM_LOG_CLICKED_EVENT,           SYSTEM_LOG_STATE,                   L2_MOUSE_SystemLogClicked},
    {MENU_CLICKED_STATE,            SYSTEM_QUIT_CLICKED_EVENT,          SYSTEM_QUIT_STATE,                  L2_MOUSE_SystemQuitClicked},
    {SYSTEM_SETTING_CLICKED_STATE,  WALLPAPER_SETTING_CLICKED_EVENT,    CLICK_INIT_STATE,                   L2_MOUSE_WallpaperSettingClicked},
    {SYSTEM_SETTING_CLICKED_STATE,  WALLPAPER_RESET_CLICKED_EVENT,      CLICK_INIT_STATE,                   L2_MOUSE_WallpaperResetClicked},
    {MY_COMPUTER_CLICKED_STATE,     MY_COMPUTER_CLOSE_CLICKED_EVENT,    CLICK_INIT_STATE,                   L2_MOUSE_MyComputerCloseClicked},
};


MOUSE_CLICK_EVENT    StartMenuClickEvent = START_MENU_INIT_CLICKED_EVENT;

typedef struct
{
    // Current only surpport 12 level path depth.
    PATH_DETAIL PathStack[12];
    UINT8 *pFATTable;
    MasterBootRecordSwitched stMBRSwitched;
}PARTITION_PATH_DETAIL;
PARTITION_ITEM_ACCESS_STATE PartitionItemAccessNextState = INIT_ACCESS_STATE;
PARTITION_ITEM_ACCESS_EVENT PartitionItemAccessEvent = START_MENU_INIT_CLICKED_EVENT;


VOID L3_PARTITION_PathAccess()
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L3_PARTITION_Access\n", __LINE__);
	
    PARTITION_ITEM_ACCESS_STATE     CurrentState;
	UINT16 RootFlag = TRUE;

    for (UINT16 i = 0; i < sizeof(PartitionItemAccessStateTransformTable) / sizeof(PartitionItemAccessStateTransformTable[0]); i++ )
    {
        if (PartitionItemAccessStateTransformTable[i].CurrentState == PartitionItemAccessNextState 
            && PartitionItemAccessEvent == PartitionItemAccessStateTransformTable[i].event )
        {
        	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L3_PARTITION_Access For\n", __LINE__);
            PartitionItemAccessNextState = PartitionItemAccessStateTransformTable[i].NextState;

            // need to check the return value after function runs..... 
            PartitionItemAccessStateTransformTable[i].pFunc();
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, 
                                                        WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], 
                                                        "%d: L3_PARTITION_Access For: %d L3_PARTITION_Access For: %d\n", 
                                                        __LINE__, 
                                                        PartitionItemAccessEvent, 
                                                        PartitionItemAccessNextState);
            break;
        }   
    }
    
}

int L1_STACK_Push(char* a,int top,char elem)
{
    a[++top]=elem;
    return top;
}


int L1_STACK_Pop(char * a,int top)
{
    if (top==-1) 
    {
        //printf("空栈");
        return -1;
    }
    //printf("弹栈元素：%c\n",a[top]);
    top--;
    return top;
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

    StartMenuClickEvent = L2_MOUSE_ClickEventGet();
    
    if (START_MENU_INIT_CLICKED_EVENT == StartMenuClickEvent)
        return;
    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: StartMenuClickEvent: %d \n", __LINE__, StartMenuClickEvent);

    MouseClickFlag = MOUSE_NO_CLICKED;
    EFI_STATUS Status;

    if (StartMenuClickEvent == MY_COMPUTER_CLOSE_CLICKED_EVENT)
    {
        //DisplayMyComputerFlag = 0;
        if (TRUE  == WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].DisplayFlag)
        {
            WindowLayers.ActiveWindowCount--;
            WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].DisplayFlag = FALSE;
        }
        return;
    }

    if (StartMenuClickEvent == SYSTEM_LOG_CLOSE_CLICKED_EVENT)
    {
        //DisplaySystemLogWindowFlag = 0;
        
        if (TRUE  == WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].DisplayFlag)
        {
            WindowLayers.ActiveWindowCount--;
            WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].DisplayFlag = FALSE;
        }
    
        return;
    }

    if (StartMenuClickEvent == SYSTEM_SETTING_CLOSE_CLICKED_EVENT)
    {
        //DisplaySystemSettingWindowFlag = 0;
        if (TRUE  == WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].DisplayFlag)
        {
            WindowLayers.ActiveWindowCount--;
            WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].DisplayFlag = FALSE;
        }
        return;
    }

    if (StartMenuClickEvent == MEMORY_INFORMATION_CLOSE_CLICKED_EVENT)
    {
        //DisplayMemoryInformationWindowFlag = 0;
        if (TRUE  == WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].DisplayFlag)
        {
            WindowLayers.ActiveWindowCount--;
            WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].DisplayFlag = FALSE;
        }
        return;
    }

    if (StartMenuClickEvent == MY_COMPUTER_CLICKED_EVENT || StartMenuClickEvent == MEMORY_INFORMATION_CLICKED_EVENT  || StartMenuClickEvent == SYSTEM_LOG_CLICKED_EVENT
        || StartMenuClickEvent == WALLPAPER_RESET_CLICKED_EVENT  || StartMenuClickEvent == WALLPAPER_SETTING_CLICKED_EVENT)
    {
        //DisplayStartMenuFlag = 0;
        //WindowLayers.item[2].DisplayFlag = 0;
        if (TRUE  == WindowLayers.item[GRAPHICS_LAYER_START_MENU].DisplayFlag)
        {
            WindowLayers.ActiveWindowCount--;
            WindowLayers.item[GRAPHICS_LAYER_START_MENU].DisplayFlag = FALSE;
        }
    }

   
    for (int i = 0; i <  sizeof(StartMenuStateTransformTable)/sizeof(StartMenuStateTransformTable[0]); i++ )
    {
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: StartMenuStateTransformTable[i].CurrentState: %d\n", __LINE__, StartMenuStateTransformTable[i].CurrentState);
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: i: %d\n", __LINE__, i);
        if (StartMenuStateTransformTable[i].CurrentState == StartMenuNextState 
            && StartMenuClickEvent == StartMenuStateTransformTable[i].event )
        {
            StartMenuNextState = StartMenuStateTransformTable[i].NextState;

            // need to check the return value after function runs..... 
            StartMenuStateTransformTable[i].pFunc();
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: StartMenuClickEvent: %d StartMenuNextState: %d\n", __LINE__, StartMenuClickEvent, StartMenuNextState);
            break;
        }   
    }
    
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
    L2_GRAPHICS_ChineseCharDraw2(pMouseBuffer, 0, 0, 11 * 94 + 42, MouseColor, 16);
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
    
    StartMenuPositionX = 0;
    StartMenuPositionY = ScreenHeight - StartMenuHeight - 25;

    SystemSettingWindowPositionX = StartMenuWidth;
    SystemSettingWindowPositionY = StartMenuPositionY + 16;

    INFO_SELF(L"\r\n");

    return EFI_SUCCESS;
}

EFI_STATUS L2_GRAPHICS_ChineseCharInit()
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d:  \n", __LINE__);

    L3_APPLICATION_ReadFile("HZK16", 5, sChineseChar);
    /*
    for (int j = 0; j < 150; j++)
    {
        L2_DEBUG_Print1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", sChineseChar[j] & 0xff);
    }

    GraphicsOutput->Blt(
                GraphicsOutput, 
                (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) pDeskBuffer,
                EfiBltBufferToVideo,
                0, 0, 
                0, 0, 
                ScreenWidth, ScreenHeight, 0);   

    */
    
}

char *p1;   

void L2_COMMON_ParameterInit()
{
    WindowLayers.LayerCount = 0;

    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].DisplayFlag = TRUE;
    L1_MEMORY_Copy(WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].Name, "System Log Window", sizeof("System Log Window"));
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].pBuffer = pSystemLogWindowBuffer;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].StartX = SystemLogWindowPositionX;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].StartY = SystemLogWindowPositionY;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].WindowWidth = SystemLogWindowWidth;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].WindowHeight= SystemLogWindowHeight;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW].LayerID = GRAPHICS_LAYER_SYSTEM_LOG_WINDOW;

    WindowLayers.LayerCount++;
    
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].DisplayFlag = FALSE;
    L1_MEMORY_Copy(WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].Name, "System Setting Window", sizeof("System Setting Window"));
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].pBuffer = pSystemSettingWindowBuffer;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].StartX = SystemSettingWindowPositionX;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].StartY = SystemSettingWindowPositionY;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].WindowWidth = SystemSettingWindowWidth;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].WindowHeight= SystemSettingWindowHeight;
    WindowLayers.item[GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW].LayerID = GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW;
    
    WindowLayers.LayerCount++;

    WindowLayers.item[GRAPHICS_LAYER_START_MENU].DisplayFlag = FALSE;
    L1_MEMORY_Copy(WindowLayers.item[GRAPHICS_LAYER_START_MENU].Name, "Start Menu", sizeof("Start Menu"));
    WindowLayers.item[GRAPHICS_LAYER_START_MENU].pBuffer = pStartMenuBuffer;
    WindowLayers.item[GRAPHICS_LAYER_START_MENU].StartX = StartMenuPositionX;
    WindowLayers.item[GRAPHICS_LAYER_START_MENU].StartY = StartMenuPositionY;
    WindowLayers.item[GRAPHICS_LAYER_START_MENU].WindowWidth = StartMenuWidth;
    WindowLayers.item[GRAPHICS_LAYER_START_MENU].WindowHeight= StartMenuHeight;
    WindowLayers.item[GRAPHICS_LAYER_START_MENU].LayerID = GRAPHICS_LAYER_START_MENU;
    
    WindowLayers.LayerCount++;

    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].DisplayFlag = FALSE;
    L1_MEMORY_Copy(WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].Name, "My Computer", sizeof("My Computer"));
    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].pBuffer = pMyComputerBuffer;
    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartX = MyComputerPositionX;
    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartY = MyComputerPositionY;
    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].WindowWidth = MyComputerWidth;
    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].WindowHeight= MyComputerHeight;
    WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].LayerID = GRAPHICS_LAYER_MY_COMPUTER_WINDOW;
    
    WindowLayers.LayerCount++;

    WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].DisplayFlag = FALSE;
    L1_MEMORY_Copy(WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].Name, "Memory Information", sizeof("Memory Information"));
    WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].pBuffer = pMemoryInformationBuffer;
    WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].StartX = MemoryInformationWindowPositionX;
    WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].StartY = MemoryInformationWindowPositionY;
    WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].WindowWidth = MemoryInformationWindowWidth;
    WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].WindowHeight= MemoryInformationWindowHeight;
    WindowLayers.item[GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW].LayerID = GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW;

    WindowLayers.LayerCount++;
	
    WindowLayers.item[GRAPHICS_LAYER_DESK].DisplayFlag = FALSE;
    L1_MEMORY_Copy(WindowLayers.item[GRAPHICS_LAYER_DESK].Name, "Memory Information", sizeof("Memory Information"));
    WindowLayers.item[GRAPHICS_LAYER_DESK].pBuffer = pMemoryInformationBuffer;
    WindowLayers.item[GRAPHICS_LAYER_DESK].StartX = MemoryInformationWindowPositionX;
    WindowLayers.item[GRAPHICS_LAYER_DESK].StartY = MemoryInformationWindowPositionY;
    WindowLayers.item[GRAPHICS_LAYER_DESK].WindowWidth = MemoryInformationWindowWidth;
    WindowLayers.item[GRAPHICS_LAYER_DESK].WindowHeight= MemoryInformationWindowHeight;
    WindowLayers.item[GRAPHICS_LAYER_DESK].LayerID = GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW;

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


EFI_STATUS
EFIAPI
Main (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_STATUS  Status;    

    //For test AllocatePool allocated memory memory phyics address.
    char *pBuffer = (UINT8 *)AllocatePool(4); 
    if (NULL == pBuffer)
    {
        //DEBUG ((EFI_D_INFO, "ScreenInit AllocatePool pDeskBuffer NULL\n"));
        return -1;
    }
    
    INFO_SELF(L"Main: 0x%X Status: 0x%X sASCII: 0x%X p1: 0x%X pBuffer: 0x%X \r\n", Main, &Status, sASCII, p1, pBuffer);  

    Status = gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (VOID **) &GraphicsOutput);    
    //INFO_SELF(L"\r\n");    
    if (EFI_ERROR (Status)) 
    {
         INFO_SELF(L"%X\n", Status);
        return EFI_UNSUPPORTED;
    }
    
    ScreenWidth  = GraphicsOutput->Mode->Info->HorizontalResolution;
    ScreenHeight = GraphicsOutput->Mode->Info->VerticalResolution;
    
    //INFO_SELF(L"%X \r\n", ScreenWidth);  

    //L2_MEMORY_Initial();

    L2_COMMON_MemoryAllocate();

    L2_COMMON_ParameterInit();

    //For locate bug
    //return;

    //如果不加下面这几行，则是直接显示内存信息，看起来有点像雪花
    
    for (int j = 0; j < ScreenHeight; j++)
    {
        for (int i = 0; i < ScreenWidth; i++)
        {
            pDeskBuffer[(j * ScreenWidth + i) * 4]     = 0xff;
            pDeskBuffer[(j * ScreenWidth + i) * 4 + 1] = 0x00;
            pDeskBuffer[(j * ScreenWidth + i) * 4 + 2] = 0x00;
        }
    }       
    /**/
    GraphicsOutput->Blt(
                GraphicsOutput, 
                (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) pDeskBuffer,
                EfiBltBufferToVideo,
                0, 0, 
                0, 0, 
                ScreenWidth, ScreenHeight, 0);   

    //本来想在这做一个起动画面，试了下，未成功，放弃了                  
    //L2_COMMON_SingleProcessInit();

    //L2_TIMER_IntervalInit0();

    L2_MOUSE_Init();
        
    // get partitions use api
    L2_STORE_PartitionAnalysis();
    
    L2_COMMON_MultiProcessInit();
    
    //L2_GRAPHICS_ChineseCharInit();
    
    //return;
    L2_GRAPHICS_ScreenInit();
    
    L2_GRAPHICS_StartMenuInit();

    L2_GRAPHICS_SystemSettingInit();
    
    L3_APPLICATION_MyComputerWindow(100, 300);
        
    L3_APPLICATION_SystemLogWindow(ScreenWidth / 2, 10);
    
    L3_APPLICATION_MemoryInformationWindow(600, 100);
    
    L2_TIMER_IntervalInit();    
    
    //GraphicsLayerCompute(iMouseX, iMouseY, 0);
    
    //L2_DEBUG_Print1(100, 100, "%d %d\n", __LINE__, Status);
    
    return EFI_SUCCESS;
}


/*

一些参考资料
区位码:
http://witmax.cn/gb2312.html

汉字的区位码查询:
http://quwei.911cha.com/

中文显示：
https://blog.csdn.net/zenwanxin/article/details/8349124?utm_medium=distribute.pc_relevant.none-task-blog-2~default~baidujs_baidulandingword~default-0.control&spm=1001.2101.3001.4242

英文显示：
https://blog.csdn.net/czg13548930186/article/details/79861914

区位码查询：http://quwei.911cha.com/

InitializeMemory
InternalMemSetMem
*/
