#define Note ""

#include <Library/UefiRuntimeServicesTableLib.h>

#include <Libraries/Math/L1_LIBRARY_Math.h>
#include <Libraries/Network/L1_LIBRARY_Network.h>
#include <Libraries/String/L2_LIBRARY_String.h>
#include <Libraries/String/L1_LIBRARY_String.h>
#include <Libraries/Memory/L1_LIBRARY_Memory.h>
#include <Libraries/DataStructure/L1_LIBRARY_DataStructure.h>
#include <Partitions/FAT32/L2_PARTITION_FAT32.h>
#include <Graphics/L3_GRAPHICS.h>
#include <Devices/Mouse/L1_DEVICE_Mouse.h>
#include <Devices/Keyboard/L2_DEVICE_Keyboard.h>
#include <Devices/Store/L2_DEVICE_Store.h>


#include <Memory/L2_MEMORY.h>
#include <Global/Global.h>
#include <Partitions/L2_PARTITION.h>

UINT16 parameter_count = 0;

            
/*
#define INFO_SELF(...)   \
            do {   \
                  Print(L"%d %a %a: ",__LINE__, __FILE__, __FUNCTION__);  \
                 Print(__VA_ARGS__); \
            }while(0);
*/


//Line 1
#define DISPLAY_MOUSE_X (0) 
#define DISPLAY_MOUSE_Y (ScreenHeight - 16 * 2  - 30)


//Line 3
#define TEXT_DEVICE_PATH_X (0) 
#define TEXT_DEVICE_PATH_Y (16 * 4)

//Line 18
#define DISK_READ_X (0) 
#define DISK_READ_Y (16 * 19)

//Line 18
#define DISK_READ_X (0) 
#define DISK_READ_Y (16 * 19)


#define DISK_MBR_X (0) 
#define DISK_MBR_Y (16 * 62)

#define DISK_MFT_BUFFER_SIZE (512 * 2 * 15)
#define EXBYTE 4


#define LLONG_MIN  (((INT64) -9223372036854775807LL) - 1)
#define LLONG_MAX   ((INT64)0x7FFFFFFFFFFFFFFFULL)

// For exception returned status 
#define DISPLAY_X (0) 
#define DISPLAY_Y (16 * (2 + DisplayCount++ % 52) )

// For exception returned status 
#define DISPLAY_ERROR_STATUS_X (ScreenWidth * 2 / 4) 
#define DISPLAY_ERROR_STATUS_Y (16 * (StatusErrorCount++ % (ScreenHeight / 16 - 3)) )
#define MOUSE_NO_CLICKED 0
#define MOUSE_LEFT_CLICKED 1
#define MOUSE_RIGHT_CLICKED 2

#define SYSTEM_QUIT_FLAG FALSE

extern EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *gSimpleFileSystem; 
UINTN ScreenWidth, ScreenHeight;

UINT16 LogStatusErrorCount = 0;

UINT16 StatusErrorCount = 0;
UINT16 DisplayCount = 0;
UINTN PartitionCount = 0;

UINT16 DebugPrintX = 0;
UINT16 DebugPrintY = 0; 

INT8 DisplayRootItemsFlag = 0;
INT8 DisplayMyComputerFlag = 0;
INT8 DisplaySystemLogWindowFlag = 0;
INT8 DisplaySystemSettingWindowFlag = 0;
INT8 DisplayMemoryInformationWindowFlag = 0;
INT8 DisplayStartMenuFlag = 0;

EFI_GRAPHICS_OUTPUT_PROTOCOL       *GraphicsOutput = NULL;

EFI_HANDLE *handle;

UINT8 *pDeskBuffer = NULL; //only Desk layer include wallpaper and button : 1
UINT8 *pMyComputerBuffer = NULL; // MyComputer layer: 2
UINT8 *pSystemLogWindowBuffer = NULL; // MyComputer layer: 2
UINT8 *pMemoryInformationBuffer = NULL; // MyComputer layer: 2
UINT8 *pDeskDisplayBuffer = NULL; //desk display after multi graphicses layers compute
UINT8 *pSystemIconBuffer[SYSTEM_ICON_MAX]; //desk display after multi graphicses layers compute
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

int Nodei = 0;

UINT8 *FAT32_Table = NULL;


UINT8 *pStartMenuBuffer = NULL;
UINT8 *pSystemSettingWindowBuffer = NULL;

UINT8 *sChineseChar = NULL;
//UINT8 sChineseChar[267616];


UINT16 MyComputerWidth = 16 * 40;
UINT16 MyComputerHeight = 16 * 50;

UINT16 SystemLogWindowWidth = 16 * 30;
UINT16 SystemLogWindowHeight = 16 * 40;

UINT16 MemoryInformationWindowWidth = 16 * 30;
UINT16 MemoryInformationWindowHeight = 16 * 40;

UINT16 MouseClickWindowWidth = 300;
UINT16 MouseClickWindowHeight = 400;

UINT16 SystemSettingWindowWidth = 16 * 10;
UINT16 SystemSettingWindowHeight = 16 * 10;

UINT16 StartMenuWidth = 16 * 10;
UINT16 StartMenuHeight = 16 * 20;

UINT64 sector_count = 0;
UINT32 FileBlockStart = 0;
UINT32 BlockSize = 0;
UINT32 FileLength = 0;
UINT32 PreviousBlockNumber = 0;


EFI_GRAPHICS_OUTPUT_BLT_PIXEL MouseColor;




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


//注意：FAT32分区的卷标存放在第2个簇前几个字符
//      NTFS分区的卷标存放在MFT表项$VOLUME表里
DEVICE_PARAMETER device[10] = {0};

MasterBootRecordSwitched MBRSwitched;
DollarBootSwitched NTFSBootSwitched;

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

STATE   NextState = INIT_STATE;

int READ_FILE_FSM_Event = READ_PATITION_EVENT;

//https://blog.csdn.net/goodwillyang/article/details/45559925

#define BitSet(x,y) x |= (0x01 << y)
#define BitSlr(x,y) x &= ~(0x01 << y)
#define BitReverse(x,y) x ^= (0x01 << y)
#define BitGet(x,y)  ((x) >> (y) & 0x01)
#define ByteSet(x)    x |= 0xff
#define ByteClear(x)  x |= 0x00

UINT8 readflag = 0;





char *p1;   
extern const UINT8 sASCII[][16];

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
    
    L2_COMMON_MemoryAllocate();

    L2_GRAPHICS_ParameterInit();

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
	
    GraphicsOutput->Blt(
                GraphicsOutput, 
                (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) pDeskBuffer,
                EfiBltBufferToVideo,
                0, 0, 
                0, 0, 
                ScreenWidth, ScreenHeight, 0);   

    L2_MOUSE_Init();
        
    // get partitions use api
    L2_STORE_PartitionAnalysis();
    
    L2_COMMON_MultiProcessInit();
	
    L2_GRAPHICS_ScreenInit();
    
    L2_GRAPHICS_StartMenuInit();

    L2_GRAPHICS_SystemSettingInit();

	L3_APPLICATION_WindowsInitial();
    
    L2_TIMER_IntervalInit();    
        
    return EFI_SUCCESS;
}


