#pragma once

#include <Library/BaseLib.h>
#include <Protocol/SimplePointer.h>
#include <Protocol/GraphicsOutput.h>
			
#include <Graphics/L2_GRAPHICS.h>

#include <Graphics/L1_GRAPHICS.h>

#define INFO_SELF(...)   \
            do {   \
                 Print(L"%d ",__LINE__);  \
                 Print(__VA_ARGS__); \
            }while(0);

typedef enum
{
    SYSTEM_ICON_MYCOMPUTER = 0,
    SYSTEM_ICON_SETTING,
    SYSTEM_ICON_RECYCLE,
    SYSTEM_ICON_FOLDER,
    SYSTEM_ICON_TEXT,
    SYSTEM_ICON_MAX 
}SYSTEM_ICON_320_400_BMP;

//save path and block information.
typedef struct
{
    UINT8  Path[50];
    UINT64 BlockNumber;
}PATH_DETAIL;

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

extern FAT32_ROOTPATH_SHORT_FILE_ITEM pItems[32];
extern INT8 SystemQuitFlag;
extern UINT16 FolderItemValidIndexArray[10];
extern float MemorySize;

//last Line
#define DISPLAY_DESK_DATE_TIME_X (ScreenWidth - 22 * 8 - 16 * 3) 
#define DISPLAY_DESK_DATE_TIME_Y (ScreenHeight - 21)


#define DISK_BUFFER_SIZE (512)

#define SYSTEM_ICON_WIDTH 400
#define SYSTEM_ICON_HEIGHT 320


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
    UINT16 DeviceType; // 0 Disk, 1: USB, 2: Sata;
    UINT16 PartitionType; // 0 MBR, 1 GPT;
    UINT16 PartitionID; // a physics device consist of Several parts like c: d: e:
    UINT16 PartitionGUID; // like FA458FD2-4FF7-44D8-B542-BA560A5990B3
    UINT16 DeviceSquenceID; //0025384961B47ECD
    char Signare[50]; // MBR:0x077410A0
    long long StartSectorNumber; //0x194000
    long long SectorCount; //0xC93060

    //Partition parameter
    MasterBootRecordSwitched stMBRSwitched; //FAT32

	//Items in folder include some folders and files
	FAT32_ROOTPATH_SHORT_FILE_ITEM pItems[32]; //FAT32

    // Path Stack
    PATH_DETAIL PathStack[12];

}DEVICE_PARAMETER;

extern UINT64 sector_count;

extern UINT8 *FAT32_Table;
extern UINT16 FileReadCount;

extern EFI_SIMPLE_POINTER_PROTOCOL        *gMouse;
extern UINT16 mouse_count;
extern UINT8 MouseClickFlag;

extern CHAR8 x_move;
extern CHAR8 y_move;

extern UINT16 iMouseX;
extern UINT16 iMouseY;
extern unsigned char AsciiBuffer[0x100];
extern WINDOW_LAYERS WindowLayers;
extern EFI_GRAPHICS_OUTPUT_BLT_PIXEL MouseColor;

extern UINT8 *pDeskBuffer;
extern UINT8 *pDeskDisplayBuffer;
extern UINT8 *pDeskWallpaperBuffer;
extern UINT8 *sChineseChar;
extern UINT8 *pSystemIconBuffer[SYSTEM_ICON_MAX];

extern UINT8 *pSystemIconFolderBuffer;
extern UINT8 *pSystemIconTextBuffer;
extern UINT8 *pSystemIconTempBuffer2;
extern UINT8 *pMouseClickBuffer;
extern UINT8 *pMemoryInformationBuffer;
extern UINT8 *pDateTimeBuffer;
extern UINT8 *pMouseBuffer;
extern UINT8 *pMouseSelectedBuffer;
extern UINT8 *pStartMenuBuffer ;
extern UINT8 *pMyComputerBuffer;
extern UINT8 *pSystemIconMyComputerBuffer;
extern UINT8 *pSystemIconMySettingBuffer;
extern UINT8 *pSystemIconRecycleBuffer;
extern UINT8 *pSystemLogWindowBuffer;
extern UINT8 *pSystemSettingWindowBuffer;
extern UINT16 LogStatusErrorCount;
extern UINT16 SystemLogWindowHeight;

extern UINTN PartitionCount; 
   

extern MasterBootRecordSwitched MBRSwitched;;                                   
extern UINT32 BlockSize;                                     
extern UINTN PartitionCount;                                    
extern UINT32 FileBlockStart;                               
extern UINT32 PreviousBlockNumber;                              
extern  UINT32 FileLength;                           
extern  UINT8 *pReadFileDestBuffer;                          
extern UINT32 PreviousBlockNumber;   
extern UINT16 PartitionItemID;
extern UINT16 FolderItemID;


 // init -> partition analysised -> root path analysised -> read fat table -> start read file -> reading a file -> read finished
 typedef enum 
 {
	 INIT_STATE = 0,
	 GET_PARTITION_INFO_STATE,
	 GET_ROOT_PATH_INFO_STATE,
	 GET_FAT_TABLE_STATE,
	 READ_FILE_STATE,
 }STATE;


typedef enum 
{
    START_MENU_BUTTON_MY_COMPUTER = 0,
    START_MENU_BUTTON_SYSTEM_SETTING,
    START_MENU_BUTTON_MEMORY_INFORMATION,
    START_MENU_BUTTON_SYSTEM_LOG,
    START_MENU_BUTTON_SYSTEM_QUIT,
    START_MENU_BUTTON_MAX
}START_MENU_BUTTON_SEQUENCE;

          
typedef enum 
{
    READ_PATITION_EVENT = 0,
    READ_ROOT_PATH_EVENT,
    READ_FAT_TABLE_EVENT,
    READ_FILE_EVENT,
}EVENT;

typedef struct
{
    STATE          CurrentState;
    EVENT          event;
    STATE          NextState;
    EFI_STATUS    (*pFunc)(); 
}STATE_TRANS;


	                      
extern UINT8 ReadFileName[20];                                
extern UINT8 *pReadFileDestBuffer;                          
extern UINT8 ReadFileNameLength;                           
extern UINT32 PreviousBlockNumber;                          
extern UINT32 FileBlockStart;                               
extern STATE   NextState;                                    
extern int READ_FILE_FSM_Event;     

extern UINT16 StatusErrorCount; 


#define STORE_EFI_PATH_PARTITION_SECTOR_COUNT 1691648  
#define DISK_BLOCK_BUFFER_SIZE (512 * 8)

#define ALLOCATE_UNIT_SIZE (8 * 512)


#define DISPLAY_LOG_ERROR_STATUS_X (4) 
#define DISPLAY_LOG_ERROR_STATUS_Y (16 * (LogStatusErrorCount++ % (SystemLogWindowHeight / 16 + 2)) )

extern UINTN ScreenWidth, ScreenHeight;

extern UINT16 MouseClickWindowWidth;
extern UINT16 MouseClickWindowHeight;
extern UINT8 *pMapper;
extern EFI_GRAPHICS_OUTPUT_PROTOCOL       *GraphicsOutput;

// memory: 1G->3G
#define ALL_PAGE_COUNT 0x80000
#define PHYSICAL_ADDRESS_START 0x40000000

extern UINT64 FreeNumberOfPages;
extern UINT64 SystemAllPagesAllocated;
extern DEVICE_PARAMETER device[10];


#define MOUSE_NO_CLICKED 0
#define MOUSE_LEFT_CLICKED 1
#define MOUSE_RIGHT_CLICKED 2


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

extern MEMORY_ALLOCATED_CURRENT MemoryAllocatedCurrent;

typedef enum
{
    MEMORY_TYPE_GRAPHICS,
    MEMORY_TYPE_APPLICATION,
    MEMORY_TYPE_TEST
}MEMORY_TYPE;

extern UINT16 MemoryInformationWindowWidth;
extern UINT16 MemoryInformationWindowHeight;

extern UINT16 StartMenuWidth;
extern UINT16 StartMenuHeight;

extern UINT16 MyComputerWidth;
extern UINT16 MyComputerHeight;
extern UINT16 SystemLogWindowWidth;

extern UINT16 SystemSettingWindowWidth;
extern UINT16 SystemSettingWindowHeight;




