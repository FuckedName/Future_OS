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


//https://blog.csdn.net/goodwillyang/article/details/45559925
UINT32 BlockSize = 0;
//注意：FAT32分区的卷标存放在第2个簇前几个字符
//      NTFS分区的卷标存放在MFT表项$VOLUME表里
UINT8 *FAT32_Table = NULL;
UINT32 FileBlockStart = 0;
UINT32 FileLength = 0;
UINT16 FileReadCount = 0;
EFI_GRAPHICS_OUTPUT_PROTOCOL       *GraphicsOutput = NULL;
UINT16 LogStatusErrorCount = 0;
UINT16 MemoryInformationWindowHeight = 16 * 40;
UINT16 MemoryInformationWindowWidth = 16 * 30;
UINT16 MouseClickWindowWidth = 300;
UINT16 MouseClickWindowHeight = 400;
EFI_GRAPHICS_OUTPUT_BLT_PIXEL MouseColor;
UINT16 MyComputerWidth = 16 * 40;
UINT16 MyComputerHeight = 16 * 50;
STATE   NextState = INIT_STATE;
UINTN PartitionCount = 0;
UINT8 *pDateTimeBuffer = NULL; //Mouse layer: 3
UINT8 *pDeskBuffer = NULL; //only Desk layer include wallpaper and button : 1
UINT8 *pDeskDisplayBuffer = NULL; //desk display after multi graphicses layers compute
UINT8 *pDeskWallpaperBuffer = NULL;
UINT8 *pMemoryInformationBuffer = NULL; // MyComputer layer: 2
UINT8 *pMouseBuffer = NULL; //Mouse layer: 4
UINT8 *pMouseClickBuffer = NULL; // for mouse click 
UINT8 *pMouseSelectedBuffer = NULL;  // after mouse selected
UINT8 *pMyComputerBuffer = NULL; // MyComputer layer: 2
UINT8 *pReadFileDestBuffer = NULL;
UINT32 PreviousBlockNumber = 0;
UINT8 *pStartMenuBuffer = NULL;
UINT8 *pSystemIconBuffer[SYSTEM_ICON_MAX]; //desk display after multi graphicses layers compute
UINT8 *pSystemIconFolderBuffer = NULL; //after zoom in or zoom out
UINT8 *pSystemIconMySettingBuffer = NULL; //after zoom in or zoom out
UINT8 *pSystemIconRecycleBuffer = NULL; //after zoom in or zoom out
UINT8 *pSystemIconTempBuffer2 = NULL;
UINT8 *pSystemIconTextBuffer = NULL; //after zoom in or zoom out
UINT8 *pSystemLogWindowBuffer = NULL; // MyComputer layer: 2
UINT8 *pSystemSettingWindowBuffer = NULL;
UINT8 ReadFileName[20];
UINT8 ReadFileNameLength = 0;
UINT8 *sChineseChar = NULL;
UINTN ScreenWidth, ScreenHeight;
UINT64 sector_count = 0;
UINT16 StartMenuWidth = 16 * 10;
UINT16 StartMenuHeight = 16 * 20;
UINT16 StatusErrorCount = 0;
UINT16 SystemLogWindowWidth = 16 * 30;
UINT16 SystemLogWindowHeight = 16 * 40;
UINT16 SystemSettingWindowWidth = 16 * 10;
UINT16 SystemSettingWindowHeight = 16 * 10;
UINT16 parameter_count = 0;

DEVICE_PARAMETER device[10] = {0};
MasterBootRecordSwitched MBRSwitched;
DollarBootSwitched NTFSBootSwitched;

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


