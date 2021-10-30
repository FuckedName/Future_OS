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
#include <Devices/Store/L2_DEVICE_Store.h>
#include <Memory/L1_MEMORY.h>
#include <Global/Global.h>
#include <Partitions/L2_PARTITION.h>

UINT16 date_time_count = 0;
UINT16 keyboard_count = 0;
UINT16 parameter_count = 0;

int keyboard_input_count = 0;

#define KEYBOARD_BUFFER_LENGTH (30) 
char pKeyboardInputBuffer[KEYBOARD_BUFFER_LENGTH] = {0};
            
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

EFI_EVENT MultiTaskTriggerGroup0Event;
EFI_EVENT MultiTaskTriggerGroup1Event;
EFI_EVENT MultiTaskTriggerGroup2Event;

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

UINT32 TimerSliceCount = 0;

UINT16 display_sector_number = 0;

typedef enum
{
    ALLOCATED_INFORMATION_DOMAIN_PHYSICAL_BLOCK_START = 0,
    ALLOCATED_INFORMATION_DOMAIN_PAGE_START,
    ALLOCATED_INFORMATION_DOMAIN_PAGE_COUNT,
    ALLOCATED_INFORMATION_DOMAIN_PHYSICAL_BLOCK_ID,
    ALLOCATED_INFORMATION_DOMAIN_MAX
}MEMORY_ALLOCATED_INFORMATION_DOMAIN;


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
    UINT8 count = 0;
    while (pItems[ItemID].FileName[count] != 0)
    {
    	if (count >= 8)
		{
			break;
    	}
        FileName[count] = pItems[ItemID].FileName[count];
        count++;
    }
	
    if (pItems[ItemID].ExtensionName[0] != 0)
    {
        FileName[count] = ' ';
        count++;
    }

	UINT8 count2 = 0;
    while (pItems[ItemID].ExtensionName[count2] != 0)
    {
    	if (count2 >= 3)
		{
			break;
    	}
    
        FileName[count] = pItems[ItemID].ExtensionName[count2];
        count++;
        count2++;
    }
	
	FileName[12] = 0;
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


MEMORY_INFORMATION MemoryInformation = {0};

void L2_MEMORY_CountInitial()
{
    MemoryAllocatedCurrent.AllocatedSliceCount = 0;
    MemoryAllocatedCurrent.AllocatedSizeSum = 0;
    MemoryAllocatedCurrent.AllocatedPageSum = 0;

    L1_MEMORY_SetValue((UINT8 *)&MemoryAllocatedCurrent, 0, sizeof(MemoryAllocatedCurrent));
}

UINT8 *L2_MEMORY_MapperInitial()
{

    for (UINT64 temp = 0; temp < ALL_PAGE_COUNT; temp++)
    {
        pMapper[temp] = 0; // no use
    }
}


extern UINT8 BufferMFT[DISK_BUFFER_SIZE * 2];





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

VOID L3_GRAPHICS_StartMenuClicked()
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L3_GRAPHICS_StartMenuClicked\n", __LINE__);
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

	//Update state
    StartMenuNextState = MENU_CLICKED_STATE;

    if (FALSE == WindowLayers.item[GRAPHICS_LAYER_START_MENU].DisplayFlag)
    {
        WindowLayers.ActiveWindowCount++;
        WindowLayers.item[GRAPHICS_LAYER_START_MENU].DisplayFlag = TRUE;
        
    }
    L1_GRAPHICS_UpdateWindowLayer(GRAPHICS_LAYER_START_MENU);

}

BOOLEAN L1_GRAPHICS_InsideRectangle(UINT16 StartX, UINT16 EndX, UINT16 StartY, UINT16 EndY)
{
	return (iMouseX >= StartX && iMouseX <= EndX && iMouseY >= StartY && iMouseY <= EndY);
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

VOID L2_PARTITION_FileContentPrint(UINT8 *Buffer)
{	
	for (int j = 0; j < 250; j++)
    {
        L2_DEBUG_Print1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", Buffer[j] & 0xff);
    }
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

//https://blog.csdn.net/goodwillyang/article/details/45559925

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


float L2_MEMORY_Initial()
{
    MemorySize = (float)L2_MEMORY_GETs();
    L2_MEMORY_MapInitial();
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

STATIC
VOID
EFIAPI
L2_SYSTEM_Start (IN EFI_EVENT Event, IN VOID *Context)
{

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
    if (State.LeftButton == MOUSE_LEFT_CLICKED)
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
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x, y,  (48 - 1) * 94 + 39 - 1, Color, ScreenWidth); 
    
    x += 16;
    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x, y,  (38 - 1) * 94 + 58 - 1, Color, ScreenWidth);

    x += 16;

    UINT8 DayOfWeek = L1_MATH_DayOfWeek(EFITime.Year, EFITime.Month, EFITime.Day);
    if (0 == DayOfWeek)
    {
        L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x, y, (48 - 1 ) * 94 + 39 - 1, Color, ScreenWidth);    
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

    L2_GRAPHICS_ChineseCharDraw(pDeskBuffer, x, y, (AreaCode - 1 ) * 94 + BitCode - 1, Color, ScreenWidth);
    
   L2_DEBUG_Print1(DISPLAY_DESK_HEIGHT_WEIGHT_X, DISPLAY_DESK_HEIGHT_WEIGHT_Y, "%d: ScreenWidth:%d, ScreenHeight:%d\n", __LINE__, ScreenWidth, ScreenHeight);
   /*
   GraphicsOutput->Blt(GraphicsOutput, 
                        (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) pDateTimeBuffer,
                        EfiBltBufferToVideo,
                        0, 0, 
                        0, 16 * 8, 
                        8 * 50, 16, 0);*/
}

EFI_STATUS L2_COMMON_MultiProcessInit ()
{
    UINT16 i;

    // task group for mouse keyboard
    EFI_GUID gMultiProcessGroup1Guid  = { 0x0579257E, 0x1843, 0x45FB, { 0x83, 0x9D, 0x6B, 0x79, 0x09, 0x38, 0x29, 0xA9 } };
    
    // task group for display date time
    EFI_GUID gMultiProcessGroup2Guid  = { 0x0579257E, 0x1843, 0x45FB, { 0x83, 0x9D, 0x6B, 0x79, 0x09, 0x38, 0x29, 0xAA } };
    
    //L2_GRAPHICS_ChineseCharDraw(pMouseBuffer, 0, 0, 11 * 94 + 42, Color, 16);
    
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


