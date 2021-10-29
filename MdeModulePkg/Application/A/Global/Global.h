#pragma once

#include <Library/BaseLib.h>
#include <Protocol/SimplePointer.h>
#include <Protocol/GraphicsOutput.h>

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


#define ALLOCATE_UNIT_SIZE (8 * 512)


#define DISPLAY_LOG_ERROR_STATUS_X (4) 
#define DISPLAY_LOG_ERROR_STATUS_Y (16 * (LogStatusErrorCount++ % (SystemLogWindowHeight / 16 + 2)) )

extern UINTN ScreenWidth, ScreenHeight;

extern UINT16 MouseClickWindowWidth;
extern UINT16 MouseClickWindowHeight;
extern UINT8 *pMapper;

// memory: 1G->3G
#define ALL_PAGE_COUNT 0x80000
#define PHYSICAL_ADDRESS_START 0x40000000

extern UINT64 FreeNumberOfPages;
extern UINT64 SystemAllPagesAllocated;


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




