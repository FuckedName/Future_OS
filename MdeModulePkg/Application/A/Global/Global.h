
/*************************************************
    .
    File name:      	*.*
    Author��	        	������
    ID��					00001
    Date:          		202107
    Description:    	����ϵͳ��һЩȫ�ֱ���������ߺܶ�ȫ�ֱ�������Ҫ�ɵ��ġ�
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




#pragma once
#include <Library/UefiLib.h>

#include <Library/BaseLib.h>
#include <Protocol/SimplePointer.h>
#include <Protocol/GraphicsOutput.h>
#include <Graphics/L2_GRAPHICS.h>
			
#include <Libraries/DataStructure/L1_LIBRARY_DataStructure.h>
#include <Graphics/Window/L2_GRAPHICS_Window.h>

#include <Graphics/L1_GRAPHICS.h>
#include <Graphics/L2_GRAPHICS.h>
#include <Protocol/Shell.h>

extern UINT8 EFI_FILE_STORE_PATH_PARTITION_NAME[50];

#define KEYBOARD_BUFFER_LENGTH (512) 
#define COMMAND_LINE_PREFIX_BUFFER_LENGTH (512) 


#define PARAMETER_COUNT 10
#define PARAMETER_LENGTH 30


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
    SYSTEM_ICON_SHUTDOWN,
    SYSTEM_ICON_MAX 
}SYSTEM_ICON_320_400_BMP;

typedef enum
{
    DESK_ICON_ITEMS_MYCOMPUTER = 0,
    DESK_ICON_ITEMS_SETTING,
    DESK_ICON_ITEMS_RECYCLE,
    DESK_ICON_ITEMS_SHUTDOWN,
    DESK_ICON_ITEMS_MAX 
}DESK_ICON_ITEMS;


typedef enum
{
    MOUSE_RIGHT_BUTTON_CLICK_MENU_OPEN = 0,
    MOUSE_RIGHT_BUTTON_CLICK_MENU_DELETE,
    MOUSE_RIGHT_BUTTON_CLICK_MENU_ADD,
    MOUSE_RIGHT_BUTTON_CLICK_MENU_MODIFY,
    MOUSE_RIGHT_BUTTON_CLICK_MENU_MAX 
}MOUSE_RIGHT_BUTTON_CLICK_MENU;


// Line 22
#define DISK_READ_BUFFER_X (0) 
#define DISK_READ_BUFFER_Y (6 * 56)


#define PARTITION_NAME_LENGTH (50)

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
    UINT8 Attribute[1];  // if 0x0FH then Long path structor, 0x20�ļ���0x10�ļ���
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

    //����ָ��ǰ�ļ�����Ŀ¼��Ӧ���������棬��Ϊ��ʱ��һ������ָ��ܶ�Ŀ¼���ļ���
    //�����������Ҫɾ���ļ���ʱ����Ҫ������ļ����ڵ�Ŀ¼�Ҫ��ȡ��������д�����
    //��֪���ܲ����Ż��£���ǰ��¼��Ϊ1025����ʾ�ǵ�3�������ȵȣ���Ϊ��ȡ��д�룬��Ҫ������Ϊ��λ��
    //UINT8 *pSectorBuffer;
}FAT32_ROOTPATH_SHORT_FILE_ITEM;


#define DISK_BUFFER_SIZE (512)

extern UINT8 Buffer1[DISK_BUFFER_SIZE];
extern BOOLEAN TestFlag;


typedef struct 
{
    UINT16 BitsOfSector;
    UINT16 SectorOfCluster; 
    UINT64 AllSectorCount;
    UINT64 MFT_StartCluster;
    UINT64 MFT_MirrStartCluster;
}DollarBootSwitched;

// NTFS first sector
// first sector of partition
typedef struct
{
    UINT8 JMP[3] ; // 0x00 3 ��תָ�������ͷһ������
    UINT8 OEM[8] ; // 0x03 8 OEM���Ƴ���ֵ��MSDOS5.0, NTFS.

    // 0x0B
    UINT8 BitsOfSector[2];        //  0x0200����������С��512B
    UINT8 SectorOfCluster;            //  0x08����  ÿ����������4KB
    UINT8 ReservedSelector[2];            // ��������������������
    UINT8 NoUse01[5];            //
    
    // 0x15
    UINT8 Description;            //  0xF8     
    
    // ���̽��� -- Ӳ��
    UINT8 NoUse02[2];            //
    
    // 0x18
    UINT8 SectorPerTrack[2];     // ��0x003F ��ÿ�������� 63
    UINT8 Headers[2];           //�� 0x00FF  ��ͷ��
    UINT8 SectorsHide[4];          // ��0x3F������������
    UINT8 NoUse03[8];           //
    
    // 0x28
    UINT8 AllSectorCount[8];        // ����������, ��λ��ǰ, ��λ�ں�
    
    // 0x30
    UINT8 MFT_StartCluster[8];      // MFT ��ʼ��
    UINT8 MFT_MirrStartCluster[8];  // MTF ���� MFTMirr λ��
    
    //0x40
    UINT8 ClusterPerMFT[4];    // ÿ��¼���� 0xF6
    UINT8 ClusterPerIndex[4];    // ÿ��������
    
    //0x48
    UINT8 SerialNumber[8];    // �����к�
    UINT8 CheckSum[8];    // У���
    UINT8 EndFlag[2];    // 0x1FE 2 ������������0x55 0xAA�� ������־��MBR�Ľ�����־��DBR��EBR�Ľ�����־��ͬ��
}DOLLAR_BOOT;


// To save folder or file after read from file system.
typedef struct
{
	UINT8 Name[100]; 
	UINT8 Type; // COMMON_STORAGE_ITEM_FOLDER; COMMON_STORAGE_ITEM_FILE; 
	UINT16 Size;
	UINT16 CreateTime;
	UINT16 LatestModifiedTime;
	UINT16 ItemCount;
	UINT64 FileContentRelativeSector; // NTFS relative sector of item 0x26 = 38 (six of header)
									  // 38 * 2 + 6,291,456 = 6291532
}COMMON_STORAGE_ITEM;


typedef enum
{
	COMMON_STORAGE_ITEM_FILE = 0,
	COMMON_STORAGE_ITEM_FOLDER,
	COMMON_STORAGE_ITEM_MAX
}COMMON_STORAGE_ITEM_TYPE;


extern QUEUE Queue;

extern FAT32_ROOTPATH_SHORT_FILE_ITEM pItems[32];
extern INT8 SystemQuitFlag;
extern UINT16 FolderItemValidIndexArray[10];
extern float MemorySize;

//last Line
#define DISPLAY_DESK_DATE_TIME_X (ScreenWidth - 22 * 8 - 16 * 3) 
#define DISPLAY_DESK_DATE_TIME_Y (ScreenHeight - 21)

extern DollarBootSwitched NTFSBootSwitched;
extern COMMON_STORAGE_ITEM pCommonStorageItems[100];


typedef struct
{
    UINT16           FontSize;
    EFI_STATUS 	(*pDrawFunction)(UINT8 *pBuffer, UINTN x0, UINTN y0, UINT16 AreaCode,	UINT16 BitCode, EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color , UINT16 AreaWidth);
}CHINESE_FONT_DRAW_FUNCTION_GET;

extern CHINESE_FONT_DRAW_FUNCTION_GET ChineseDrawFunctionGet[17];


#define SYSTEM_ICON_WIDTH 400
#define SYSTEM_ICON_HEIGHT 320


typedef struct 
{
    UINT16 ReservedSelector;
    UINT32 SectorsPerFat;   
    UINT32 BootPathStartCluster;
    UINT8 FATCount;
    UINT8 SectorOfCluster;
}MasterBootRecordSwitched;    

typedef struct
{
    UINT8 JMP[3] ; // 0x00 3 ��תָ�������ͷһ������
    UINT8 OEM[8] ; // 0x03 8 OEM���Ƴ���ֵ��MSDOS5.0.
    UINT8 BitsOfSector[2] ; // 0x0b 2 ÿ���������ֽ�����ȡֵֻ�������¼��֣�512��1024��2048����4096����Ϊ512��ȡ����õļ�����
    UINT8 SectorOfCluster[1] ; // 0x0d 1 ÿ���������� ��ֵ������2�������η���ͬʱ��Ҫ��֤ÿ�ص��ֽ������ܳ���32K
    UINT8 ReservedSelector[2] ; // 0x0e 2 ��������������������������������Ϊ0��FAT12/FAT16����Ϊ1��FAT32�ĵ���ֵȡΪ32
    UINT8 FATCount[1] ; // 0x10 1 �ļ��������Ŀ�� NumFATS���κ�FAT��ʽ������Ϊ2
    UINT8 RootPathRecords[2] ; // 0x11 2 ����Ŀ¼��Ŀ����, 0 for fat32, 512 for fat16
    UINT8 AllSectors[2] ; // 0x13 2 ���������������0����ʹ��ƫ��0x20����4�ֽ�ֵ��0 for fat32
    UINT8 Description[1] ; // 0x15 1 �������� 0xF8 ���桢ÿ��80�ŵ���ÿ�ŵ�9����
    UINT8 xxx1[2] ; // 0x16 2 ÿ���ļ�������������FAT16��,0 for fat32
    UINT8 xxx2[2] ; // 0x18 2 ÿ�ŵ�������, 0x003f
    UINT8 xxx3[2] ; // 0x1a 2 ��ͷ����0xff
    UINT8 xxx4[4] ; // 0x1c 4 ��������, ��MBR�е�ַ0x1C6��ʼ��4���ֽ���ֵ���
    UINT8 SectorCounts[4] ; // 0x20 4 �����������������65535ʹ�ô˵�ַ��С��65536�μ�ƫ��0x13����FAT32����������Ƿ�0��
    UINT8 SectorsPerFat[4] ; // Sectors count each FAT use
    UINT8 Fat32Flag[2] ; // 0x28 2 Flags (FAT32����)
    UINT8 FatVersion[2] ; // 0x2a 2 �汾�� (FAT32����)
    UINT8 BootPathStartCluster[4] ; // 0x2c 4 ��Ŀ¼��ʼ�� (FAT32)��һ��Ϊ2
    UINT8 ClusterName[11] ; // 0x2b 11 ��꣨��FAT32��
    UINT8 BootStrap[2] ; // 0x30 2 FSInfo ���� (FAT32) bootstrap
    UINT8 BootSectorBackup[2] ; // 0x32 2 ������������ (FAT32)�����Ϊ0����ʾ�ڱ�������������¼�ı�������ռ����������ͨ��Ϊ6ͬʱ������ʹ��6�����������ֵ
    UINT8 Reserved[2] ; // 0x34 2 ����δʹ�� (FAT32) ������0���
    UINT8 FileSystemType[8] ; // 0x36 8 FAT�ļ�ϵͳ���ͣ���FAT��FAT12��FAT16����"FAT"����PBR,�������MBR
    UINT8 SelfBootCode[2] ; // 0x3e 2 ����ϵͳ����������
    UINT8 DeviceNumber[1] ; // 0x40 1 BIOS�豸���� (FAT32)
    UINT8 NoUse[1] ; // 0x41 1 δʹ�� (FAT32)
    UINT8 Flag[1] ; // 0x42 1 ��� (FAT32)
    UINT8 SequeenNumber[4] ; // 0x43 4 ����� (FAT32)
    UINT8 juanbiao[11] ; // 0x47 11 ��꣨FAT32��
    UINT8 TypeOfFileSystem[8] ; // 0x52 8 FAT�ļ�ϵͳ���ͣ�FAT32��
    UINT8 BootAssembleCode[338]; // code
    UINT8 Partition1[16] ; // 0x1be 64 partitions table, DOS_PART_TBL_OFFSET
    UINT8 Partition2[16] ; // 0X1BE ~0X1CD 16 talbe entry for Partition 1
    UINT8 Partition3[16] ; // 0X1CE ~0X1DD 16 talbe entry for Partition 2
    UINT8 Partition4[16] ; // 0X1DE ~0X1ED 16 talbe entry for Partition 3
    UINT8 EndFlag[2] ; // 0x1FE 2 ������������0x55 0xAA�� ������־��MBR�Ľ�����־��DBR��EBR�Ľ�����־��ͬ��
}MasterBootRecord;


typedef enum
{
	FILE_SYSTEM_MIN = 0,
	FILE_SYSTEM_FAT32,
	FILE_SYSTEM_NTFS,
	FILE_SYSTEM_MAX
}FILE_SYSTEM_TYPE;

typedef struct
{	
	UINT16 FileSystemType; //FILE_SYSTEM_FAT32 FILE_SYSTEM_NTFS
    UINT16 DeviceType; // 0 Disk, 1: USB, 2: Sata;
    UINT16 PartitionType; // 0 MBR, 1 GPT;
    UINT16 PartitionID; // a physics device consist of Several parts like c: d: e:
    UINT64 StartSectorNumber; //Very important
    UINT64 SectorCount; //0xC93060
    UINT8  PartitionName[PARTITION_NAME_LENGTH];

    UINT64 FAT_TableStartSectorNumber;

	UINT8 *pFAT_TableBuffer;

    //Partition parameter
    MasterBootRecordSwitched stMBRSwitched; //FAT32

	//Items in folder include some folders and files
	FAT32_ROOTPATH_SHORT_FILE_ITEM pItems[32]; //FAT32

    // Path Stack
    PATH_DETAIL PathStack[12];
}DEVICE_PARAMETER;

extern UINT64 sector_count;

extern UINT64 FileReadCount;
extern EFI_GRAPHICS_OUTPUT_BLT_PIXEL WhiteColor;
extern EFI_GRAPHICS_OUTPUT_BLT_PIXEL BlackColor;

extern EFI_SIMPLE_POINTER_PROTOCOL        *gMouse;
extern UINT16 mouse_count;
extern UINT8 MouseClickFlag;

extern CHAR8 x_move;
extern CHAR8 y_move;

extern INT16 iMouseX;
extern INT16 iMouseY;
extern unsigned char AsciiBuffer[0x100];
extern WINDOW_LAYERS WindowLayers;
extern EFI_GRAPHICS_OUTPUT_BLT_PIXEL MouseColor;

extern UINT8 *pDeskBuffer;
extern UINT8 *pDeskDisplayBuffer;
extern UINT8 *pDeskWallpaperBuffer;
extern UINT8 *pDeskWallpaperZoomedBuffer;
extern UINT8 *pDeskWallpaperTempBuffer;

extern UINT8 *sChineseChar;
extern UINT8 *sChineseChar12;
extern UINT8 *pSystemIconBuffer[SYSTEM_ICON_MAX];
extern char  pKeyboardInputBuffer[KEYBOARD_BUFFER_LENGTH];


extern UINT8 *pSystemIconFolderBuffer;
extern UINT8 *pSystemIconTextBuffer;
extern UINT8 *pSystemIconTempBuffer2;
extern UINT8 *pMouseRightButtonClickWindowBuffer;
extern UINT8 *pMemoryInformationBuffer;
extern UINT8 *pDateTimeBuffer;
extern UINT8 *pMouseBuffer;
extern UINT8 *pMouseSelectedBuffer;
extern UINT8 *pStartMenuBuffer ;
extern UINT8 *pTerminalWindowBuffer;

extern UINT8 *pMyComputerBuffer;
extern UINT8 *pMyComputerNewBuffer;
extern UINT8 *pSystemIconMyComputerBuffer;
extern UINT8 *pSystemIconMySettingBuffer;
extern UINT8 *pSystemIconRecycleBuffer;
extern UINT8 *pSystemLogWindowBuffer;
extern UINT8 *pSystemSettingWindowBuffer;
extern UINT16 SystemLogWindowHeight;
extern char pCommandLinePrefixBuffer[COMMAND_LINE_PREFIX_BUFFER_LENGTH];
extern UINT16 TerminalCurrentLineCount;
                      
extern UINT32 BlockSize;                                     
extern UINTN PartitionCount;                                    
extern UINT64 FileBlockStart;                               
extern UINT64 PreviousBlockNumber;                              
extern  UINT64 FileLength;                           
extern  UINT8 *pReadFileDestBuffer;    
extern UINT16 PartitionItemID;
extern UINT16 FolderItemID;
 
typedef struct
{
	UINT16 PartitionStartX;
	UINT16 PartitionStartY;
	UINT16 PartitionWidth;
	UINT16 PartitionHeight;
	UINT16 ItemStartX;
	UINT16 ItemStartY;
	UINT16 ItemWidth;
	UINT16 ItemHeight;
}MY_COMPUTER_WINDOW_STATE;

extern MY_COMPUTER_WINDOW_STATE MyComputerWindowState;


// init -> partition analysised -> root path analysised -> read fat table -> start read file -> reading a file -> read finished
typedef enum 
{
	 READ_FILE_INIT_STATE = 0, //��ʼ״̬
	 READ_FILE_GET_PARTITION_INFO_STATE, //��ȡ������Ϣ
	 READ_FILE_GET_ROOT_PATH_INFO_STATE, //��ȡ��Ŀ¼��Ϣ
	 READ_FILE_GET_FAT_TABLE_STATE,  //��ȡFAT��������Ϣ
	 READ_FILE_GET_DATA_STATE, //��ȡ������Ϣ
}READ_FILE_STATE;


typedef enum 
{
    START_MENU_BUTTON_MY_COMPUTER = 0,
    START_MENU_BUTTON_SYSTEM_SETTING,
    START_MENU_BUTTON_MEMORY_INFORMATION,
    START_MENU_BUTTON_SYSTEM_LOG,
    START_MENU_BUTTON_TERMINAL,
    START_MENU_BUTTON_SYSTEM_QUIT,
    START_MENU_BUTTON_MAX
}START_MENU_BUTTON_SEQUENCE;

          
typedef enum 
{
    READ_PATITION_INFO_EVENT = 0,
    READ_ROOT_PATH_EVENT,
    READ_FAT_TABLE_EVENT,
    READ_FILE_EVENT,
}EVENT;

typedef struct
{
    UINT16          CurrentState;
    UINT16          event;
    UINT16          NextState;
    EFI_STATUS    (*pAction)(); 
}STATE_TRANSFORM;


typedef struct
{
    UINT16          CurrentState;
    UINT16          event;
    UINT16          NextState;
    EFI_STATUS    (*pAction)(VOID *pData); 
}STATE_TRANSFORM_NEW;

	                      
extern UINT8 ReadFileName[20];                              
extern UINT8 ReadFileNameLength;                             
extern READ_FILE_STATE   NextState;                                    
extern int READ_FILE_FSM_Event;     
extern CHAR8 *ReceiveBuffer;
extern CHAR8 *SendBuffer;
extern UINT32 SendBufferLength;
extern UINT32 ReceiveBufferLength;
extern UINT32 TimerSliceCount;



extern UINT16 StatusErrorCount; 

#define DISK_BLOCK_BUFFER_SIZE (512 * 8)

#define ALLOCATE_UNIT_SIZE (8 * 512)


extern UINT16 LogStatusErrorCount;

#define DISPLAY_LOG_ERROR_STATUS_X (4) 
#define DISPLAY_LOG_ERROR_STATUS_Y (16 * (LogStatusErrorCount++ % (SystemLogWindowHeight / 16) + 2) )

extern UINTN ScreenWidth, ScreenHeight;

extern UINT16 MouseRightButtonClickWindowWidth;
extern UINT16 MouseRightButtonClickWindowHeight;
extern UINT8 *pMapper;

// memory: 1G->3G
#define ALL_PAGE_COUNT 0x80000
#define PHYSICAL_ADDRESS_START 0x20000000


//����洢�豸һ����50������
#define PARTITION_COUNT 50

extern UINT64 FreeNumberOfPages;
extern UINT64 SystemAllPagesAllocated;
extern DEVICE_PARAMETER device[PARTITION_COUNT];
extern EFI_HANDLE  TimerOne;
extern EFI_HANDLE  SystemHandle;
extern EFI_SHELL_PROTOCOL    *EfiShellProtocol;

typedef struct
{
    UINT16 StartX;
    UINT16 StartY;
    UINT16 EndX;
    UINT16 EndY;
    UINT16 GraphicsLayerID;
}MOUSE_MOVEOVER_OBJECT;

extern MOUSE_MOVEOVER_OBJECT MouseMoveoverObject;
extern BOOLEAN ApplicationRunFinished;
extern BOOLEAN bMouseMoveoverObject;


typedef enum
{
    APPLICATION_CALL_ID_INIT = 0,
    APPLICATION_CALL_ID_SHUTDOWN,
    APPLICATION_CALL_ID_PRINT_STRING,
    APPLICATION_CALL_ID_MAX
}APPLICATION_CALL_ID;

typedef struct
{
    APPLICATION_CALL_ID ID;
    EFI_STATUS          RanStatus;
    UINT8 pApplicationCallInput[20];
    UINT8 (*pCallbackFunction)(EFI_STATUS ret);
}APPLICATION_CALL_DATA;

// Application call
extern APPLICATION_CALL_DATA *pApplicationCallData;

typedef enum
{
    MOUSE_EVENT_TYPE_NO_CLICKED = 0,
    MOUSE_EVENT_TYPE_LEFT_CLICKED,
    MOUSE_EVENT_TYPE_RIGHT_CLICKED,
    MOUSE_EVENT_TYPE_MAX
}MOUSE_EVENT_TYPE;


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
    MEMORY_TYPE_PARTITION,
    MEMORY_TYPE_NETWORK,
    MEMORY_TYPE_TEST
}MEMORY_TYPE;

extern UINT16 MemoryInformationWindowWidth;
extern UINT16 MemoryInformationWindowHeight;

extern UINT16 StartMenuWidth;
extern UINT16 StartMenuHeight;

extern UINT16 MyComputerWidth;
extern UINT16 MyComputerHeight;
extern UINT16 SystemLogWindowWidth;
extern UINT16 TerminalWindowWidth;
extern UINT16 TerminalWindowHeight;

extern UINT16 SystemSettingWindowWidth;
extern UINT16 SystemSettingWindowHeight;
extern VOID (*pFunction)();
extern UINT8 *pApplication;




