/** @file
  FrontPage routines to handle the callbacks and browser calls

Copyright (c) 2004 - 2017, Intel Corporation. All rights reserved.<BR>
(C) Copyright 2018 Hewlett Packard Enterprise Development LP<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent
ToDo:

1. ReadFileFromPath;
2. Fix trigger event with mouse and keyboard
3. NetWork connect baidu.com
4. File System Simple Management
5. progcess
6. My Computer window(disk partition) 
7. Setting window(select file, delete file, modify file)
8. Memory Simple Management
9. Multi Windows, button click event.
10. Application.
11. How to Automated Testing?

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

unsigned int date_time_count = 0;

//Line 0
#define DISPLAY_DESK_HEIGHT_WEIGHT_X (date_time_count % 30) 
#define DISPLAY_DESK_HEIGHT_WEIGHT_Y (0)

//Line 1
#define DISPLAY_MOUSE_X (0) 
#define DISPLAY_MOUSE_Y (16 * 1)

#define KEYBOARD_BUFFER_LENGTH (30) 

int keyboard_input_count = 0;
char pKeyboardInputBuffer[KEYBOARD_BUFFER_LENGTH] = {0};

//Line 2
#define DISPLAY_KEYBOARD_X (0) 
#define DISPLAY_KEYBOARD_Y (16 * 2)

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
#define DISK_READ_BUFFER_Y (16 * 56)

// Line 22

#define DISK_READ_BUFFER_X (0) 
#define DISK_READ_BUFFER_Y (16 * 56)


//last Line
#define DISPLAY_DESK_DATE_TIME_X (ScreenWidth - 20 * 8) 
#define DISPLAY_DESK_DATE_TIME_Y (ScreenHeight - 21)


#define BYTES	512
#define EXBYTE	4



UINT16 StatusErrorCount = 0;
// For exception returned status 
#define DISPLAY_ERROR_STATUS_X (ScreenWidth * 3 / 4) 
#define DISPLAY_ERROR_STATUS_Y (16 * (StatusErrorCount++ % 15) )

#define DEBUG_STATUS_2(x, y, Express)  DebugPrint1(x, y,IN CONST CHAR8 * Format,...)

#define DEBUG_STATUS(x, y, Express)	 DEBUG_STATUS_2(x, y, "message(%s, %s(), %d): ", __FILE__, __FUNCTION__, __LINE__); 

EFI_GRAPHICS_OUTPUT_PROTOCOL       *GraphicsOutput;
EFI_SIMPLE_POINTER_PROTOCOL        *gMouse;

extern EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *gSimpleFileSystem; 

UINT8 *pMyComputerBuffer = NULL;

EFI_HANDLE *handle;

UINT8 *pDeskBuffer = NULL;
UINT8 *pDeskDisplayBuffer = NULL; //desk display after multi graphicses layers compute
UINT8 *pMouseSelectedBuffer = NULL;  // after mouse selected
UINT8 *pMouseClickBuffer = NULL; // for mouse click 
UINT8 *pMouseBuffer = NULL;

typedef struct
{
	unsigned int DeviceType; // 0 Disk, 1 USB;
	unsigned int PartitionType; // 0 MBR, 1 GPT;
	unsigned int PartitionID; // a physics device consist of Several parts like c: d: e:
	unsigned int PartitionGUID; // like FA458FD2-4FF7-44D8-B542-BA560A5990B3
	unsigned int DeviceSquenceID; //0025384961B47ECD
	char Signare[50]; // MBR:0x077410A0
	long long StartSectorNumber; //0x194000
	long long SectorCount; //0xC93060
}DEVICE_PARAMETER;


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


const UINT8 *sChineseChar = NULL;
static UINTN ScreenWidth, ScreenHeight;  
UINT16 MyComputerWidth = 100;
UINT16 MyComputerHeight = 100;
UINT16 MouseClickWindowWidth = 300;
UINT16 MouseClickWindowHeight = 400;


DEVICE_PARAMETER device[10] = {0};



int iMouseX;
int iMouseY;

EFI_GRAPHICS_OUTPUT_BLT_PIXEL MouseColor;

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
	
	UINT16 CurrentState; // maximize; minimize; other
	CHAR8 pTitle[50];
	BUTTON *pButtons;
} WINDOW;


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
    {0x00,0x00,0x10,0x38,0x6C,0xC6,0xC6,0xFE,0xC6,0xC6,0xC6,0xC6,0x00,0x00,0x00,0x00},       //0x41'A'
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

void CopyColorIntoBuffer(UINT8 *pBuffer, EFI_GRAPHICS_OUTPUT_BLT_PIXEL color, UINT16 x0, UINT16 y0, UINT16 AreaWidth)
{
    pBuffer[y0 * AreaWidth * 4 + x0 * 4]     = color.Blue;
    pBuffer[y0 * AreaWidth * 4 + x0 * 4 + 1] = color.Green;
    pBuffer[y0 * AreaWidth * 4 + x0 * 4 + 2] = color.Red;

}

void CopyColorIntoBuffer2(UINT8 *pBuffer, EFI_GRAPHICS_OUTPUT_BLT_PIXEL color, UINT16 x0, UINT16 y0)
{
    pBuffer[y0 * 8 * 4 + x0 * 4]     = color.Blue;
    pBuffer[y0 * 8 * 4 + x0 * 4 + 1] = color.Green;
    pBuffer[y0 * 8 * 4 + x0 * 4 + 2] = color.Red;
}

void CopyColorIntoBuffer3(UINT8 *pBuffer, EFI_GRAPHICS_OUTPUT_BLT_PIXEL color, UINT16 x0, UINT16 y0, UINT8 AreaWidth)
{
    pBuffer[y0 * AreaWidth * 4 + x0 * 4]     = color.Blue;
    pBuffer[y0 * AreaWidth * 4 + x0 * 4 + 1] = color.Green;
    pBuffer[y0 * AreaWidth * 4 + x0 * 4 + 2] = color.Red;
}

INT32 Math_ABS(INT32 v)
{
	return (v < 0) ? -v : v;
}

EFI_STATUS LineDrawIntoBuffer(UINT8 *pBuffer,
        IN UINTN x0, UINTN y0, UINTN x1, UINTN y1, 
        IN UINTN BorderWidth,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL BorderColor, UINT16 AreaWidth)
{

    INT32 dx  = Math_ABS((int)(x1 - x0));
    INT32 sx  = x0 < x1 ? 1 : -1;
    INT32 dy  = Math_ABS((int)(y1-y0)), sy = y0 < y1 ? 1 : -1;
    INT32 err = ( dx > dy ? dx : -dy) / 2, e2;
    
    for(;;)
    {    
        CopyColorIntoBuffer(pBuffer, BorderColor, x0, y0, AreaWidth);
        
        if (x0==x1 && y0==y1) break;
    
        e2 = err;
    
        if (e2 > -dx) { err -= dy; x0 += sx; }
        if (e2 <  dy) { err += dx; y0 += sy; }
    }
    return EFI_SUCCESS;
}



// draw rectangle borders
void RectangleDrawIntoBuffer(UINT8 *pBuffer,
        IN UINTN x0, UINTN y0, UINTN x1, UINTN y1, 
        IN UINTN BorderWidth,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color, UINT16 AreaWidth)
{    
    if (NULL == pBuffer)
	{
		DEBUG ((EFI_D_INFO, "NULL == pBuffer"));
		return ;
	}
	
    LineDrawIntoBuffer(pBuffer, x0, y0, x0, y1, 1, Color, AreaWidth);
    LineDrawIntoBuffer(pBuffer, x0, y0, x1, y0, 1, Color, AreaWidth);
    LineDrawIntoBuffer(pBuffer, x0, y1, x1, y1, 1, Color, AreaWidth);
    LineDrawIntoBuffer(pBuffer, x1, y0, x1, y1, 1, Color, AreaWidth);

}


VOID GraphicsCopy(UINT8 *pDest, UINT8 *pSource, 
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
VOID GraphicsLayerCompute(int iMouseX, int iMouseY)
{
	GraphicsCopy(pDeskDisplayBuffer, pDeskBuffer, ScreenWidth, ScreenHeight, ScreenWidth, ScreenHeight, 0, 0);
	GraphicsCopy(pDeskDisplayBuffer, pMyComputerBuffer, ScreenWidth, ScreenHeight, MyComputerWidth, MyComputerHeight, DISPLAY_ERROR_STATUS_X, 900);

	int i, j;


	//16, ScreenHeight - 21, For event trigger

    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;  

	if (NULL == pMouseSelectedBuffer)
	{
		DEBUG ((EFI_D_INFO, "NULL == pMouseSelectedBuffer"));
		return;
	}
    
    //DEBUG ((EFI_D_INFO, "Line: %d\n", __LINE__));

    for (i = 0; i < 16; i++)
        for (j = 0; j < 16; j++)
        {       
                /*pMouseBuffer[(i * 16 + j) * 4]     = pDeskDisplayBuffer[((iMouseY + i) * ScreenWidth +  iMouseX + j) * 4];
                pMouseBuffer[(i * 16 + j) * 4 + 1] = pDeskDisplayBuffer[((iMouseY + i) * ScreenWidth +  iMouseX + j) * 4 + 1];
                pMouseBuffer[(i * 16 + j) * 4 + 2] = pDeskDisplayBuffer[((iMouseY + i) * ScreenWidth +  iMouseX + j) * 4 + 2];            
					*/
                pMouseBuffer[(i * 16 + j) * 4]     = 0xff;
                pMouseBuffer[(i * 16 + j) * 4 + 1] = 0xff;
                pMouseBuffer[(i * 16 + j) * 4 + 2] = 0;             
        }

	/*
    DrawChineseCharIntoBuffer2(pDeskBuffer,  16, ScreenHeight - 21,     (18 - 1) * 94 + 43 - 1, Color, ScreenWidth);
    DrawChineseCharIntoBuffer2(pDeskBuffer,  16 * 2, ScreenHeight - 21, (21 - 1) * 94 + 05 - 1, Color, ScreenWidth);

	*/
	
    Color.Red   = 0xff;
    Color.Green = 0x00;
    Color.Blue	  = 0x00;
    
    if (iMouseX >= 16 && iMouseX <= 16 + 16 * 2
        && iMouseY >= ScreenHeight - 21 && iMouseY <= ScreenHeight)
    {	
		for (i = 0; i < 16; i++)
		{
			for (j = 0; j < 32; j++)
			{	
				pMouseSelectedBuffer[(i * 32 + j) * 4]     = pDeskDisplayBuffer[((ScreenHeight - 21 + i) * ScreenWidth +  16 + j) * 4];
				pMouseSelectedBuffer[(i * 32 + j) * 4 + 1] = pDeskDisplayBuffer[((ScreenHeight - 21 + i) * ScreenWidth +  16 + j) * 4 + 1];
				pMouseSelectedBuffer[(i * 32 + j) * 4 + 2] = pDeskDisplayBuffer[((ScreenHeight - 21 + i) * ScreenWidth +  16 + j) * 4 + 2];			
			}
		}
		 //RectangleFillIntoBuffer(UINT8 * pBuffer,IN UINTN x0,UINTN y0,UINTN x1,UINTN y1,IN UINTN BorderWidth,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color)
		
        //RectangleFillIntoBuffer(pMouseSelectedBuffer, 0,  0, 32, 16, 1,  Color);
        
        RectangleDrawIntoBuffer(pMouseSelectedBuffer, 0,  0, 31, 15, 1,  Color, 32);
        
        GraphicsCopy(pDeskDisplayBuffer, pMouseSelectedBuffer, ScreenWidth, ScreenHeight, 32, 16, 15, ScreenHeight - 22);
    }

    // init mouse buffer for use in use
	//DrawChineseCharIntoBuffer2(pMouseBuffer, 0, 0, 11 * 94 + 42, MouseColor, 16);

	
	GraphicsCopy(pDeskDisplayBuffer, pMouseBuffer, ScreenWidth, ScreenHeight, 16, 16, iMouseX, iMouseY);

   GraphicsOutput->Blt(GraphicsOutput, 
			            (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) pDeskDisplayBuffer,
			            EfiBltBufferToVideo,
			            0, 0, 
			            0, 0, 
			            ScreenWidth, ScreenHeight, 0);   
}


EFI_STATUS DrawAsciiCharIntoBuffer(UINT8 *pBuffer,
        IN UINTN x0, UINTN y0,UINT8 c,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL BorderColor, UINT16 AreaWidth)
{
    INT8 i;
    UINT8 d;
    
	for(i = 0; i < 16; i++)
	{
		d = sASCII[c][i];
		
		if ((d & 0x80) != 0) 
		    CopyColorIntoBuffer(pBuffer, BorderColor, x0 + 0, y0 + i, AreaWidth); 
		
		if ((d & 0x40) != 0) 
            CopyColorIntoBuffer(pBuffer, BorderColor, x0 + 1, y0 + i, AreaWidth);
		
		if ((d & 0x20) != 0) 
		    CopyColorIntoBuffer(pBuffer, BorderColor, x0 + 2, y0 + i, AreaWidth);
		
		if ((d & 0x10) != 0) 
		    CopyColorIntoBuffer(pBuffer, BorderColor, x0 + 3, y0 + i, AreaWidth);
		
		if ((d & 0x08) != 0) 
		    CopyColorIntoBuffer(pBuffer, BorderColor, x0 + 4, y0 + i, AreaWidth);
		
		if ((d & 0x04) != 0) 
		    CopyColorIntoBuffer(pBuffer, BorderColor, x0 + 5, y0 + i, AreaWidth);
		
		if ((d & 0x02) != 0) 
		    CopyColorIntoBuffer(pBuffer, BorderColor, x0 + 6, y0 + i, AreaWidth);
		
		if ((d & 0x01) != 0) 
		    CopyColorIntoBuffer(pBuffer, BorderColor, x0 + 7, y0 + i, AreaWidth);
		
	}
	
    return EFI_SUCCESS;
}

EFI_STATUS DrawAsciiCharUseBuffer(UINT8 *pBufferDest,
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
		    CopyColorIntoBuffer2(pBuffer, Color, 0, i); 
		
		if ((d & 0x40) != 0) 
            CopyColorIntoBuffer2(pBuffer, Color, 1, i);
		
		if ((d & 0x20) != 0) 
		    CopyColorIntoBuffer2(pBuffer, Color, 2, i);
		
		if ((d & 0x10) != 0) 
		    CopyColorIntoBuffer2(pBuffer, Color, 3, i);
		
		if ((d & 0x08) != 0) 
		    CopyColorIntoBuffer2(pBuffer, Color, 4, i);
		
		if ((d & 0x04) != 0) 
		    CopyColorIntoBuffer2(pBuffer, Color, 5, i);
		
		if ((d & 0x02) != 0) 
		    CopyColorIntoBuffer2(pBuffer, Color, 6, i);
		
		if ((d & 0x01) != 0) 
		    CopyColorIntoBuffer2(pBuffer, Color, 7, i);
		
	}

	GraphicsCopy(pBufferDest, pBuffer, ScreenWidth, ScreenHeight, 8, 16, x0, y0);
	
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

UINT16 DebugPrintX = 0;
UINT16 DebugPrintY = 0; 

CHAR8    AsciiBuffer[0x100] = {0};

VOID StringMakerWithLine (UINT16 x, UINT16 y, UINT16 line,
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

    AsciiVSPrint (AsciiBuffer, sizeof (AsciiBuffer), Format, VaList);

    for (i = 0; i < sizeof(AsciiBuffer) /sizeof(CHAR8); i++)
        DrawAsciiCharUseBuffer(pDeskBuffer, x + i * 8, y, AsciiBuffer[i], Color);

}


VOID StringMaker (UINT16 x, UINT16 y,
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

    AsciiVSPrint (AsciiBuffer, sizeof (AsciiBuffer), Format, VaList);

    for (i = 0; i < sizeof(AsciiBuffer) /sizeof(CHAR8); i++)
        DrawAsciiCharUseBuffer(pDeskBuffer, x + i * 8, y, AsciiBuffer[i], Color);

}

/* Display a string */
VOID EFIAPI DebugPrint1 (UINT16 x, UINT16 y,  IN  CONST CHAR8  *Format, ...)
{
	VA_LIST         VaList;
	VA_START (VaList, Format);
	StringMaker(x, y, Format, VaList);
	VA_END (VaList);
}

// Draw 8 X 16 point
EFI_STATUS Draw8_16IntoBuffer(UINT8 *pBuffer,UINT8 d,
        IN UINTN x0, UINTN y0,
        UINT8 width,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color, UINT16 AreaWidth)
{
    //DebugPrint1(10, 10, "%X %X %X %X", x0, y0, AreaWidth);
    if (NULL == pBuffer)
    {
    	DEBUG ((EFI_D_INFO, "pBuffer is NULL\n"));
    	return EFI_SUCCESS;
    }
        
    if ((d & 0x80) != 0) 
        CopyColorIntoBuffer(pBuffer, Color, x0 + 0, y0, AreaWidth ); 
    
    if ((d & 0x40) != 0) 
        CopyColorIntoBuffer(pBuffer, Color, x0 + 1, y0, AreaWidth );
    
    if ((d & 0x20) != 0) 
        CopyColorIntoBuffer(pBuffer, Color, x0 + 2, y0, AreaWidth );
    
    if ((d & 0x10) != 0) 
        CopyColorIntoBuffer(pBuffer, Color, x0 + 3, y0, AreaWidth );
    
    if ((d & 0x08) != 0) 
        CopyColorIntoBuffer(pBuffer, Color, x0 + 4, y0, AreaWidth );
    
    if ((d & 0x04) != 0) 
        CopyColorIntoBuffer(pBuffer, Color, x0 + 5, y0, AreaWidth );
    
    if ((d & 0x02) != 0) 
        CopyColorIntoBuffer(pBuffer, Color, x0 + 6, y0, AreaWidth );
    
    if ((d & 0x01) != 0) 
        CopyColorIntoBuffer(pBuffer, Color, x0 + 7, y0, AreaWidth );



    return EFI_SUCCESS;
}

// Draw 8 X 16 point
EFI_STATUS Draw8_16IntoBufferWithWidth(UINT8 *pBuffer,UINT8 d,
        IN UINTN x0, UINTN y0,
        UINT8 width,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color, UINT8 fontWidth)
{
    if ((d & 0x80) != 0) 
        CopyColorIntoBuffer3(pBuffer, Color, x0 + 0, y0, fontWidth); 
    
    if ((d & 0x40) != 0) 
        CopyColorIntoBuffer3(pBuffer, Color, x0 + 1, y0, fontWidth );
    
    if ((d & 0x20) != 0) 
        CopyColorIntoBuffer3(pBuffer, Color, x0 + 2, y0, fontWidth );
    
    if ((d & 0x10) != 0) 
        CopyColorIntoBuffer3(pBuffer, Color, x0 + 3, y0, fontWidth );
    
    if ((d & 0x08) != 0) 
        CopyColorIntoBuffer3(pBuffer, Color, x0 + 4, y0, fontWidth );
    
    if ((d & 0x04) != 0) 
        CopyColorIntoBuffer3(pBuffer, Color, x0 + 5, y0, fontWidth );
    
    if ((d & 0x02) != 0) 
        CopyColorIntoBuffer3(pBuffer, Color, x0 + 6, y0, fontWidth );
    
    if ((d & 0x01) != 0) 
        CopyColorIntoBuffer3(pBuffer, Color, x0 + 7, y0, fontWidth );


    return EFI_SUCCESS;
}


//http://quwei.911cha.com/
EFI_STATUS DrawChineseCharIntoBuffer(UINT8 *pBuffer,
        IN UINTN x0, UINTN y0,UINT8 offset,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color , UINT16 AreaWidth)
{
    INT8 i;
    if (NULL == pBuffer)
	{
		DEBUG ((EFI_D_INFO, "NULL == pBuffer"));
		return EFI_SUCCESS;
	}
    
	for(i = 0; i < 32; i += 2)
	{
        Draw8_16IntoBuffer(pBuffer, sChinese[offset][i],     x0,     y0 + i / 2, 1, Color, AreaWidth);		        
		Draw8_16IntoBuffer(pBuffer, sChinese[offset][i + 1], x0 + 8, y0 + i / 2, 1, Color, AreaWidth);		
	}
	
    return EFI_SUCCESS;
}

EFI_STATUS DrawChineseCharIntoBuffer2(UINT8 *pBuffer,
        IN UINTN x0, UINTN y0, UINT32 offset,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color , UINT16 AreaWidth)
{
    INT8 i;
    //DebugPrint1(10, 10, "%X %X %X %X", x0, y0, offset, AreaWidth);
    //DEBUG ((EFI_D_INFO, "%X %X %X %X", x0, y0, offset, AreaWidth));

	if (NULL == pBuffer)
	{
		DEBUG ((EFI_D_INFO, "NULL == pBuffer"));
		return EFI_SUCCESS;
	}

	if (offset < 1)
	{
		DEBUG ((EFI_D_INFO, "offset < 1 \n"));
		return EFI_SUCCESS;
	}
    
	for(i = 0; i < 32; i += 2)
	{
        Draw8_16IntoBuffer(pBuffer, sChineseChar[offset * 32 + i ],     x0,     y0 + i / 2, 1, Color, AreaWidth);		        
		 Draw8_16IntoBuffer(pBuffer, sChineseChar[offset * 32 + i + 1],  x0 + 8, y0 + i / 2, 1, Color, AreaWidth);		
	}
	
    //DEBUG ((EFI_D_INFO, "\n"));
	
    return EFI_SUCCESS;
}


EFI_STATUS DrawChineseCharUseBuffer(UINT8 *pBuffer,
        IN UINTN x0, UINTN y0,UINT8 *c, UINT8 count,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL FontColor, UINT16 AreaWidth)
{
    INT16 i, j;    
    
    if (NULL == pBuffer)
	{
		DEBUG ((EFI_D_INFO, "NULL == pBuffer"));
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
            Draw8_16IntoBufferWithWidth(pBuffer, sChinese[j][i],     x0,     y0 + i / 2, 1, FontColor, AreaWidth);                
            Draw8_16IntoBufferWithWidth(pBuffer, sChinese[j][i + 1], x0 + 8, y0 + i / 2, 1, FontColor, AreaWidth);        
        }
        x0 += 16;
    }
	
    return EFI_SUCCESS;
}

// fill into rectangle
void RectangleFillIntoBuffer(UINT8 *pBuffer,
        IN UINTN x0, UINTN y0, UINTN x1, UINTN y1, 
        IN UINTN BorderWidth,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color)
{    
    if (NULL == pBuffer)
	{
		DEBUG ((EFI_D_INFO, "NULL == pBuffer"));
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
        }
    }

}


EFI_EVENT MultiTaskTriggerEvent;


EFI_STATUS OpenShellProtocolSelf( EFI_SHELL_PROTOCOL **gEfiShellProtocol )
{
    EFI_STATUS        Status;
    
    Status = gBS->OpenProtocol(gImageHandle,
				              &gEfiShellProtocolGuid,
				              (VOID **)gEfiShellProtocol,
				              gImageHandle,
				              NULL,
				              EFI_OPEN_PROTOCOL_GET_PROTOCOL);
				              
    if (EFI_ERROR(Status)) 
    {
        Status = gBS->LocateProtocol(&gEfiShellProtocolGuid,
					                NULL,
					                (VOID **)gEfiShellProtocol);
        if (EFI_ERROR(Status)) 
        {
            gEfiShellProtocol = NULL;
        }
    }
    return Status;
}

int Nodei = 0;
EFI_STATUS PrintNode(EFI_DEVICE_PATH_PROTOCOL *Node)
{    
    //DebugPrint1(350 + 700, 16 * (Nodei + 10), "%d:Nodei:%d (%d, %d)\n", __LINE__, Nodei, Node->Type, Node->SubType);
    //Nodei++;
    return 0;
}

EFI_DEVICE_PATH_PROTOCOL *WalkthroughDevicePath(EFI_DEVICE_PATH_PROTOCOL *DevPath,
                                                EFI_STATUS (* CallBack)(EFI_DEVICE_PATH_PROTOCOL*))
{
    EFI_DEVICE_PATH_PROTOCOL *pDevPath = DevPath;

    while(! IsDevicePathEnd(pDevPath))
    {
        CallBack(pDevPath);
        pDevPath = NextDevicePathNode(pDevPath);
    }

    return pDevPath;
}

EFI_STATUS ShellServiceRead()
{
    //DEBUG ((EFI_D_INFO, "ShellServiceRead error: %x\n", Status));
    
	//DebugPrint1(0, 16 * 6, "%d, ShellServiceRead\n", __LINE__);
    
	EFI_STATUS Status ;
    EFI_SHELL_PROTOCOL  *gEfiShellProtocol;
    Status = OpenShellProtocolSelf(&gEfiShellProtocol);    
    if (EFI_ERROR(Status))
    {
    	 DEBUG ((EFI_D_INFO, "OpenShellProtocol error: %x\n", Status));
        return Status;
    }
    return EFI_SUCCESS;
}

UINTN strlen1(char *String)
{
    UINTN  Length;
    for (Length = 0; *String != '\0'; String++, Length++) ;
//	 DEBUG ((EFI_D_INFO, "%d Length: %d\n", __LINE__, Length));
    return Length;
}

int isspaceSelf (int c)
{
  //
  // <space> ::= [ ]
  //
  return ((c) == ' ') || ((c) == '\t') || ((c) == '\r') || ((c) == '\n') || ((c) == '\v')  || ((c) == '\f');
}

int isalnumSelf (int c)
{
  //
  // <alnum> ::= [0-9] | [a-z] | [A-Z]
  //
  return ((('0' <= (c)) && ((c) <= '9')) ||
          (('a' <= (c)) && ((c) <= 'z')) ||
          (('A' <= (c)) && ((c) <= 'Z')));
}

#define LLONG_MIN  (((INT64) -9223372036854775807LL) - 1)
#define LLONG_MAX   ((INT64)0x7FFFFFFFFFFFFFFFULL)

int
toupperSelf(
  IN  int c
  )
{
  if ( (c >= 'a') && (c <= 'z') ) {
    c = c - ('a' - 'A');
  }
  return c;
}



int
Digit2ValSelf( int c)
{
  if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))) {  /* If c is one of [A-Za-z]... */
    c = toupperSelf(c) - 7;   // Adjust so 'A' is ('9' + 1)
  }
  return c - '0';   // Value returned is between 0 and 35, inclusive.
}

long long Multi(long x, long y)
{
	return x * y;
}

long long
strtollSelf(const char * nptr, char ** endptr, int base)
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
  DEBUG ((EFI_D_INFO, "line:%d device->StartSectorNumber: %a base: %d \n", __LINE__, nptr, base));

  while( isalnumSelf(*nptr) && ((temp = Digit2ValSelf(*nptr)) < base)) 
  {
    DEBUG ((EFI_D_INFO, "line:%d temp: %d, Result: %d \n", __LINE__, temp, Result));
    Previous = Result;
    Result = Multi (Result, base) + (long long int)temp;
    if( Result <= Previous) 
    {   // Detect Overflow
      if(Negative) 
      {
	    DEBUG ((EFI_D_INFO, "line:%d Negative!!!!\n", __LINE__, temp, Result));
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

  DEBUG ((EFI_D_INFO, "line:%d device->StartSectorNumber: %d \n", __LINE__, Result));
  if(Negative) 
  {
    Result = -Result;
  }

  // Save pointer to final sequence
  if(endptr != NULL) 
  {
    *endptr = (char *)pEnd;
  }

  DEBUG ((EFI_D_INFO, "line:%d temp: %d, Result: %d \n", __LINE__, temp, Result));
  return Result;
}


void TextDevicePathAnalysis(char *p, DEVICE_PARAMETER *device)
{
    int length = strlen1(p);
    int i = 0;
    //DebugPrint1(0, 11 * 16, "line: %d string: %s, length: %d\n", __LINE__, p, length);
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
    DEBUG ((EFI_D_INFO, "line:%d device->StartSectorNumber: %a \n", __LINE__, temp));
    //printf("line:%d start: %d\n", __LINE__, strtol(temp));
    device->StartSectorNumber = strtollSelf(temp, NULL, 10);
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
    device->SectorCount = strtollSelf(temp, NULL, 10);;
    DEBUG ((EFI_D_INFO, "line:%d device->SectorCount: %a \n", __LINE__, temp));
    //putchar('\n');
    
    //DebugPrint1(0, 11 * 16, "line: %d string: %s, length: %d\n",       __LINE__, p, strlen(p));
    DebugPrint1(TEXT_DEVICE_PATH_X, (TEXT_DEVICE_PATH_Y) + 16 * 1, "line:%d device->DeviceType: %d \n",        __LINE__, device->DeviceType);
    DebugPrint1(TEXT_DEVICE_PATH_X, (TEXT_DEVICE_PATH_Y) + 16 * 2, "line:%d device->PartitionType: %d \n",     __LINE__, device->PartitionType);
    DebugPrint1(TEXT_DEVICE_PATH_X, (TEXT_DEVICE_PATH_Y) + 16 * 3, "line:%d device->PartitionID: %d \n",       __LINE__, device->PartitionID);
    DebugPrint1(TEXT_DEVICE_PATH_X, (TEXT_DEVICE_PATH_Y) + 16 * 4, "line:%d device->StartSectorNumber: %d \n",  __LINE__, device->StartSectorNumber);
    DebugPrint1(TEXT_DEVICE_PATH_X, (TEXT_DEVICE_PATH_Y) + 16 * 5, "line:%d device->SectorCount: %d \n",        __LINE__, device->SectorCount);

    DEBUG ((EFI_D_INFO, "line:%d device->DeviceType: %d \n", __LINE__, device->DeviceType));
    DEBUG ((EFI_D_INFO, "line:%d device->PartitionType: %d \n", __LINE__, device->PartitionType));
    DEBUG ((EFI_D_INFO, "line:%d device->PartitionID: %d \n", __LINE__, device->PartitionID));
    DEBUG ((EFI_D_INFO, "line:%d device->StartSectorNumber: %d \n", __LINE__, device->StartSectorNumber));
    DEBUG ((EFI_D_INFO, "line:%d device->SectorCount: %d \n", __LINE__, device->SectorCount));
    //putchar('\n');
}

int PartitionCount = 0;
void TextDevicePathAnalysisCHAR16(CHAR16 *p, DEVICE_PARAMETER *device, UINTN count1)
{
    int length = StrLen(p);
    int i = 0;
    //DebugPrint1(0, 11 * 16, "line: %d string: %s, length: %d\n", __LINE__, p, length);
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
    DEBUG ((EFI_D_INFO, "line:%d device->StartSectorNumber: %a \n", __LINE__, temp));
    //printf("line:%d start: %d\n", __LINE__, strtol(temp));
    device->StartSectorNumber = strtollSelf(temp, NULL, 10);
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
    device->SectorCount = strtollSelf(temp, NULL, 10);;
    DEBUG ((EFI_D_INFO, "line:%d device->SectorCount: %a \n", __LINE__, temp));
    //putchar('\n');
    
    //DebugPrint1(0, 11 * 16, "line: %d string: %s, length: %d\n",       __LINE__, p, strlen(p));

    // second display
    DebugPrint1(TEXT_DEVICE_PATH_X, TEXT_DEVICE_PATH_Y + 16 * (4 * count1 + 1), "%d: Start: %d Count: %d  DeviceType: %d PartitionType: %d PartitionID: %d\n",        
    							__LINE__, 
    							device->StartSectorNumber,
    							device->SectorCount,
    							device->DeviceType,
    							device->PartitionType,
    							device->PartitionID);
    							
    DEBUG ((EFI_D_INFO, "line:%d device->DeviceType: %d \n", __LINE__, device->DeviceType));
    DEBUG ((EFI_D_INFO, "line:%d device->PartitionType: %d \n", __LINE__, device->PartitionType));
    DEBUG ((EFI_D_INFO, "line:%d device->PartitionID: %d \n", __LINE__, device->PartitionID));
    DEBUG ((EFI_D_INFO, "line:%d device->StartSectorNumber: %d \n", __LINE__, device->StartSectorNumber));
    DEBUG ((EFI_D_INFO, "line:%d device->SectorCount: %d \n", __LINE__, device->SectorCount));
    //putchar('\n');
}


EFI_STATUS PartitionRead()
{
    //DebugPrint1(350, 0, "%d: 0 PartitionRead \n", __LINE__);
    DEBUG ((EFI_D_INFO, "PartitionRead!!\r\n"));
    EFI_STATUS Status ;
    UINTN NumHandles, i;
    EFI_HANDLE *ControllerHandle = NULL;
    EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;
    EFI_BLOCK_IO_PROTOCOL           *BlockIo;
    UINT8 Buffer1[BYTES];
    EFI_DISK_IO_PROTOCOL            *DiskIo;
    
    Status = gBS->LocateProtocol (&gEfiDevicePathToTextProtocolGuid, NULL, (VOID **) &DevPathToText);
    if (EFI_ERROR(Status))
    {
    	 DEBUG ((EFI_D_INFO, "LocateProtocol1 error: %x\n", Status));    	 
    	 DebugPrint1(350, 16 * 2, "%d: 2 : %x \n", __LINE__, Status);
        return Status;
    }
   
    Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiDiskIoProtocolGuid, NULL, &NumHandles, &ControllerHandle);
    if (EFI_ERROR(Status))
    {
        //DebugPrint1(350, 16 * 3, "%d : %x\n", __LINE__, Status);
        DEBUG ((EFI_D_INFO, "LocateProtocol1 error: %x\n", Status));   

		 CHAR8 sTest[20] = "123456789";
		 //DebugPrint1(350, 16 * 4, "%d : %a\n", __LINE__, sTest);
		
        CHAR16  pPatitionsParameterBuffer[20][200] = 
        {
           L"PciRoot(0x0)/Pci(0x14,0x0)/USB(0x1,0x0)",
		    L"PciRoot(0x0)/Pci(0x14,0x0)/USB(0x1,0x0)/HD(1,MBR,0x077410A0,0x194000,0xC93060)",
		    L"PciRoot(0x0)/Pci(0x14,0x0)/USB(0x1,0x0)/HD(2,MBR,0x077410A0,0xE27800,0xDF85F)",
		    L"PciRoot(0x0)/Pci(0x17,0x0)/Sata(0x1800,0x8000,0x1)/HD(1,GPT,85CA0C62-D24C-4D6C-A8ED-B469EE665B5C,0x800,0x82000)",
		    L"PciRoot(0x0)/Pci(0x17,0x0)/Sata(0x1800,0x8000,0x1)/HD(2,GPT,432E1506-D24C-4D6C-A8ED-B469EE665B5C,0x82800,0x8000)",
		    L"PciRoot(0x0)/Pci(0x17,0x0)/Sata(0x1800,0x8000,0x1)/HD(3,GPT,69262129-D24C-4D6C-A8ED-B469EE665B5C,0x8A800,0x1F263000)",
		    L"PciRoot(0x0)/Pci(0x17,0x0)/Sata(0x1800,0x8000,0x1)/HD(4,GPT,9E518177-D24C-4D6C-A8ED-B469EE665B5C,0x1F2ED800,0x112800)",
		    L"PciRoot(0x0)/Pci(0x17,0x0)/Sata(0x1800,0x8000,0x1)/HD(5,GPT,FE80C43C-D24C-4D6C-A8ED-B469EE665B5C,0x1F400000,0x1C5E00CF)"
		 };
	    
        DEBUG ((EFI_D_INFO, "%d: %a\n", __LINE__, sTest));

        //DEVICE_PARAMETER  device;
		for (i = 0; i < 6; i++)
	    {
        	DEBUG ((EFI_D_INFO, "%d: %a\n", __LINE__, pPatitionsParameterBuffer[i]));
        	TextDevicePathAnalysisCHAR16(pPatitionsParameterBuffer[i], &device[i], i);
	    	
    	    DebugPrint1(TEXT_DEVICE_PATH_X, TEXT_DEVICE_PATH_Y + 16 * 2 * i, "%d: %s\n", __LINE__, pPatitionsParameterBuffer[i]);
	    }

		for (i = 0; i < 6; i++)
		{
		    /*
		    DebugPrint1(0, (22 + i + 7) * 16, "line:%d StartSectorNumber: %d SectorCount: %d  device->DeviceType: %d PartitionType: %d PartitionID: %d\n",        
    							__LINE__, 
    							device[i].StartSectorNumber,
    							device[i].SectorCount,
    							device[i].DeviceType,
    							device[i].PartitionType,
    							device[i].PartitionID);
    							*/
		}
	    
        return Status;
    }

    DEBUG ((EFI_D_INFO, "Before for\n", Status));
    //DebugPrint1(350, 16 * 5, "%d: %x\n", __LINE__, Status);

    for (i = 0; i < NumHandles; i++)
    {
    	//DebugPrint1(350, 16 * 6, "%d: %x\n", __LINE__, Status);
        EFI_DEVICE_PATH_PROTOCOL *DiskDevicePath;
        Status = gBS->OpenProtocol(ControllerHandle[i],
                                   &gEfiDevicePathProtocolGuid,
                                   (VOID **)&DiskDevicePath,
                                   gImageHandle, 
                                   NULL,
                                   EFI_OPEN_PROTOCOL_GET_PROTOCOL);
        if (EFI_ERROR(Status))
        {
    		 DebugPrint1(350, 16 * 7, "%d: %x\n", __LINE__, Status);
            DEBUG ((EFI_D_INFO, "Status = gBS->OpenProtocol error index %d: %x\n", i, Status));
            return Status;
        }

        CHAR16 *TextDevicePath = 0;
        TextDevicePath = DevPathToText->ConvertDevicePathToText(DiskDevicePath, TRUE, TRUE);

        // first display
    	 DebugPrint1(TEXT_DEVICE_PATH_X, TEXT_DEVICE_PATH_Y + 16 * 4 * i, "%d: %s\n", __LINE__, TextDevicePath);
        //DEBUG ((EFI_D_INFO, "%s\n", TextDevicePath));

		 TextDevicePathAnalysisCHAR16(TextDevicePath, &device[i], i);
    	            
        if (TextDevicePath) gBS->FreePool(TextDevicePath);
        
        Status = gBS->HandleProtocol(ControllerHandle[i],
                                    &gEfiBlockIoProtocolGuid,
                                    (VOID * *) &BlockIo );                                                
        DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
        
        if ( EFI_SUCCESS == Status )
        {			        	 
            Status = gBS->HandleProtocol( ControllerHandle[i],
                                          &gEfiDiskIoProtocolGuid,
                                          (VOID * *) &DiskIo ); 
            DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
            
            if ( Status == EFI_SUCCESS )
            {
                Status = DiskIo->ReadDisk( DiskIo, BlockIo->Media->MediaId, 0, BYTES, Buffer1 );
                
                DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X BlockIo->Media->MediaId: %d\n", __LINE__, Status, BlockIo->Media->MediaId);

                if ( EFI_SUCCESS == Status )
					  for (int j = 0; j < 50; j++)
					  	   DebugPrint1(DISK_READ_BUFFER_X + j * 8 * 3, TEXT_DEVICE_PATH_Y + 16 * (4 * i + 2) , "%02X ", Buffer1[j] & 0xff);
            }       
        }
    }
	
    return EFI_SUCCESS;
}

Network()
{}

//DiskIo: Block mode, operation can handle by byte and any position 
//DiskIo2: UnBlocks, operation can handle by byte and any position
//BlockIo: Block mode, operation only by sector and sector start
//BlockIo2: UnBlocks mode, operation only by sector and sector start
//同步(Synchronous)和异步(Asynchronous) - 菠萝小妹。 - ...
EFI_STATUS PartitionUSBReadPassThrough()
{
    DebugPrint1(350, 0, "%d: 0 PartitionUSBRead \n", __LINE__);
    DEBUG ((EFI_D_INFO, "PartitionUSBRead!!\r\n"));
    EFI_STATUS Status ;
    UINTN NumHandles, i;
    EFI_HANDLE *ControllerHandle = NULL;
    EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;
    EFI_BLOCK_IO_PROTOCOL           *BlockIo;
    UINT8 Buffer1[BYTES];
    EFI_DISK_IO_PROTOCOL            *DiskIo;
/*    EFI_ATA_PASS_THRU_PROTOCOL        *AtaPassThru;

    Status = gBS->HandleProtocol ( handle,
				                    &gEfiAtaPassThruProtocolGuid,
				                    (VOID **) AtaPassThru
				                    );
    if (EFI_ERROR (Status)) 
    {
        DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
        break;
    }    
*/
}

unsigned int sector_count = 0;

//DiskIo: Block mode, operation can handle by byte and any position 
//DiskIo2: UnBlocks, operation can handle by byte and any position
//BlockIo: Block mode, operation only by sector and sector start
//BlockIo2: UnBlocks mode, operation only by sector and sector start
//同步(Synchronous)和异步(Asynchronous) - 菠萝小妹。 - ...
EFI_STATUS PartitionUSBReadSynchronous()
{
    DebugPrint1(350, 0, "%d: 0 PartitionUSBRead \n", __LINE__);
    DEBUG ((EFI_D_INFO, "PartitionUSBRead!!\r\n"));
    EFI_STATUS Status ;
    UINTN NumHandles, i;
    EFI_HANDLE *ControllerHandle = NULL;
    EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;
    EFI_BLOCK_IO_PROTOCOL           *BlockIo;
    UINT8 Buffer1[BYTES];
    EFI_DISK_IO_PROTOCOL            *DiskIo;
    
    Status = gBS->LocateProtocol (&gEfiDevicePathToTextProtocolGuid, NULL, (VOID **) &DevPathToText);
    if (EFI_ERROR(Status))
    {
    	 DEBUG ((EFI_D_INFO, "LocateProtocol1 error: %x\n", Status));    	 
    	 DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
        return Status;
    }
   
    Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiDiskIoProtocolGuid, NULL, &NumHandles, &ControllerHandle);
    if (EFI_ERROR(Status))
    {
        DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
	    
        return EFI_SUCCESS;
    }

    DEBUG ((EFI_D_INFO, "Before for\n", Status));
    //DebugPrint1(350, 16 * 5, "%d: %x\n", __LINE__, Status);

    for (i = 0; i < NumHandles; i++)
    {
    	//DebugPrint1(350, 16 * 6, "%d: %x\n", __LINE__, Status);
        EFI_DEVICE_PATH_PROTOCOL *DiskDevicePath;
        Status = gBS->OpenProtocol(ControllerHandle[i],
                                   &gEfiDevicePathProtocolGuid,
                                   (VOID **)&DiskDevicePath,
                                   gImageHandle, 
                                   NULL,
                                   EFI_OPEN_PROTOCOL_GET_PROTOCOL);
        if (EFI_ERROR(Status))
        {
            DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
            return Status;
        }

        CHAR16 *TextDevicePath = DevPathToText->ConvertDevicePathToText(DiskDevicePath, TRUE, TRUE);

        // first display
    	 DebugPrint1(DISK_READ_BUFFER_X, DISK_READ_BUFFER_Y + 16 * i, "%d: %s\n", __LINE__, TextDevicePath);
        //DEBUG ((EFI_D_INFO, "%s\n", TextDevicePath));

		 TextDevicePathAnalysisCHAR16(TextDevicePath, &device[i], i);
    	            
        if (TextDevicePath) gBS->FreePool(TextDevicePath);

		 // the USB we save our *.efi file and relative resource files..
		 if (device[i].DeviceType == 1 && device[i].SectorCount == 915551)
		 {
		    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		 	Status = gBS->HandleProtocol(ControllerHandle[i],
                                    &gEfiBlockIoProtocolGuid,
                                    (VOID * *) &BlockIo );                                                
		    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		    
		    if ( EFI_SUCCESS == Status )
		    {			        	 
		        Status = gBS->HandleProtocol( ControllerHandle[i],
		                                      &gEfiDiskIoProtocolGuid,
		                                      (VOID * *) &DiskIo ); 
		        DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		        
		        if ( Status == EFI_SUCCESS )
		        {
		        	 // Read from disk....

		        	 //for (int k = 0; k < 10; k++)
			        //{
			            /*
			                 IN EFI_DISK_IO_PROTOCOL         *This,
							  IN UINT32                       MediaId,
							  IN UINT64                       Offset,
							  IN UINTN                        BufferSize,
							  OUT VOID                        *Buffer
			            */
			        	 Status = DiskIo->ReadDisk( DiskIo, BlockIo->Media->MediaId, BYTES * sector_count++, BYTES, Buffer1 );
			            
			            DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X BlockIo->Media->MediaId: %d\n", __LINE__, Status, BlockIo->Media->MediaId);

			            if ( EFI_SUCCESS == Status )
			            {
							  for (int j = 0; j < 250; j++)
							  {
							  		DebugPrint1(DISK_READ_BUFFER_X + (j % 50) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 50), "%02X ", Buffer1[j] & 0xff);
							  }
					     }
					 //}
		        }       
		    }

		    return EFI_SUCCESS;
		 }  
    }
    return EFI_SUCCESS;
}

EFI_STATUS PartitionUSBReadAsynchronous()
{
    DebugPrint1(350, 0, "%d: 0 PartitionUSBReadAsynchronous \n", __LINE__);
    DEBUG ((EFI_D_INFO, "PartitionUSBReadAsynchronous!!\r\n"));
    EFI_STATUS Status ;
    UINTN HandlesCount, i;
    EFI_HANDLE *Handles = NULL;
    EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;
    EFI_BLOCK_IO2_PROTOCOL           *BlockIo2;
    EFI_BLOCK_IO2_PROTOCOL           BlockIo22;
    UINT8 Buffer1[BYTES];
    EFI_DISK_IO2_PROTOCOL            *DiskIo2;

    EFI_DEVICE_PATH_PROTOCOL* dp;

	/*
    Status = gBS->InstallMultipleProtocolInterfaces (
												        handle,
												        &gEfiBlockIo2ProtocolGuid,
												        &BlockIo22,
												        NULL
												        );
    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
    */    
    Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiBlockIo2ProtocolGuid, NULL, &HandlesCount, &Handles);
    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X HandlesCount:%d \n", __LINE__, Status, HandlesCount);
    for (int index = 0; index < (int)HandlesCount; index++)
    {
        Status = gBS->HandleProtocol(Handles[index], &gEfiBlockIo2ProtocolGuid, BlockIo2);
        DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X index:%d \n", __LINE__, Status, index);
            
        dp = DevicePathFromHandle(Handles[index]);

        EFI_LBA  LBA = 0;
        
       BlockIo2->ReadBlocksEx(BlockIo2, BlockIo2->Media->MediaId, LBA, &disk_handle_task.DiskIo2Token, BYTES, Buffer1);
        
        //gBS->SignalEvent (disk_handle_task.FileIoToken->Event);
        DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X BlockIo->Media->MediaId: %d\n", __LINE__, Status, BlockIo2->Media->MediaId);

        if ( EFI_SUCCESS == Status )
        {
              for (int j = 0; j < 250; j++)
              {
                    DebugPrint1(DISK_READ_BUFFER_X + (j % 50) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 50) , "%02X ", Buffer1[j] & 0xff);
              }
         }


        //while (!IsDevicePathEnd(NextDevicePathNode(dp)))
        //    dp = NextDevicePathNode(dp);

        //AsciiPrint("-HP BlockIO2 Status: %d; handle: %d; DevicePath: %s\r\n", Status, bio2HandleBuffer[index], ConvertDevicePathToText(dp, TRUE, TRUE));
        //AsciiPrint("--MediaPresent: %d; RemovableMedia: %d; LogicalPartition: %d\r\n", BlockIo2->Media->MediaPresent, BlockIo2->Media->RemovableMedia, bio2->Media->LogicalPartition);
    }















/*




    
    
    Status = gBS->LocateProtocol (&gEfiDevicePathToTextProtocolGuid, NULL, (VOID **) &DevPathToText);
    if (EFI_ERROR(Status))
    {
    	 DEBUG ((EFI_D_INFO, "LocateProtocol1 error: %x\n", Status));    	 
    	 DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
        return Status;
    }
   
    Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiBlockIoProtocolGuid, NULL, &HandlesCount, &Handles);
    if (EFI_ERROR(Status))
    {
        DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
	    
        return EFI_SUCCESS;
    }

    DEBUG ((EFI_D_INFO, "Before for\n", Status));
    //DebugPrint1(350, 16 * 5, "%d: %x\n", __LINE__, Status);
    EFI_DEVICE_PATH_PROTOCOL *DiskDevicePath;

    for (i = 0; i < HandlesCount; i++)
    {
    	//DebugPrint1(350, 16 * 6, "%d: %x\n", __LINE__, Status);
        Status = gBS->OpenProtocol(Handles[i],
                                   &gEfiDevicePathProtocolGuid,
                                   (VOID **)&DiskDevicePath,
                                   gImageHandle, 
                                   NULL,
                                   EFI_OPEN_PROTOCOL_GET_PROTOCOL);
        if (EFI_ERROR(Status))
        {
            DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
            return Status;
        }

        CHAR16 *TextDevicePath = DevPathToText->ConvertDevicePathToText(DiskDevicePath, TRUE, TRUE);

        // first display
    	 DebugPrint1(DISK_READ_BUFFER_X, DISK_READ_BUFFER_Y + 16 * i, "%d: %s\n", __LINE__, TextDevicePath);
        //DEBUG ((EFI_D_INFO, "%s\n", TextDevicePath));

		 TextDevicePathAnalysisCHAR16(TextDevicePath, &device[i], i);
    	            
        if (TextDevicePath) gBS->FreePool(TextDevicePath);

		 // the USB we save our *.efi file and relative resource files..
		 if (device[i].DeviceType == 1 && device[i].SectorCount == 915551)
		 {
		    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		 	Status = gBS->HandleProtocol(Handles[i],
                                    &gEfiBlockIo2ProtocolGuid,
                                    (VOID * *) &BlockIo2 );      
			
		    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		    
		    if ( EFI_SUCCESS == Status )
		    {			        	 
		        //Status = gBS->HandleProtocol( ControllerHandle[i],
		                                      &gEfiDiskIo2ProtocolGuid,
		                                      (VOID * *) &DiskIo2 ); 
		        DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		        //
		        if ( Status == EFI_SUCCESS )
		        {
		        	 // Read from disk....
		        	 
                     typedef
                     EFI_STATUS
                     (EFIAPI *EFI_DISK_READ_EX) (
                       IN EFI_DISK_IO2_PROTOCOL        *This,
                       IN UINT32                       MediaId,
                       IN UINT64                       Offset,
                       IN OUT EFI_DISK_IO2_TOKEN       *Token,
                       IN UINTN                        BufferSize,
                       OUT VOID                        *Buffer
                       );

		        	 
		            Status = DiskIo2->ReadDiskEx( DiskIo2, BlockIo2->Media->MediaId, &disk_handle_task.DiskIo2Token, 0, BYTES, Buffer1 );

					
					typedef
					EFI_STATUS
					(EFIAPI *EFI_BLOCK_READ_EX) (
					  IN     EFI_BLOCK_IO2_PROTOCOL *This,
					  IN     UINT32                 MediaId,
					  IN     EFI_LBA                LBA,
					  IN OUT EFI_BLOCK_IO2_TOKEN    *Token,
					  IN     UINTN                  BufferSize,
					     OUT VOID                  *Buffer
					  );
					

					EFI_LBA  LBA = 0;

		           BlockIo2->ReadBlocksEx(BlockIo2, BlockIo2->Media->MediaId, LBA, &disk_handle_task.DiskIo2Token, BYTES, Buffer1);
		            
			        //gBS->SignalEvent (disk_handle_task.FileIoToken->Event);
		            DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X BlockIo->Media->MediaId: %d\n", __LINE__, Status, BlockIo2->Media->MediaId);

		            if ( EFI_SUCCESS == Status )
		            {
						  for (int j = 0; j < 250; j++)
						  {
						  		DebugPrint1(DISK_READ_BUFFER_X + (j % 50) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 50) , "%02X ", Buffer1[j] & 0xff);
						  }
				     }
		        }       
		    }

		    return EFI_SUCCESS;
		 }  
    }*/
    return EFI_SUCCESS;
}

VOID EFIAPI TimeSlice(
	IN EFI_EVENT Event,
	IN VOID           *Context
	)
{
    
    //DEBUG ((EFI_D_INFO, "System time slice Loop ...\n"));
    gBS->SignalEvent (MultiTaskTriggerEvent);

	return;
}

EFI_STATUS MouseInit()
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
		
	if (EFI_ERROR(Status))	return Status;		//unsupport
	
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



EFI_STATUS FileReadSelf(CHAR16 *FileName, UINT32 size, UINT8 *pBuffer)
{    
    //CHAR16  FileName[128] = L"test2.txt";
    EFI_STATUS Status ;
    SHELL_FILE_HANDLE FileHandle;
    EFI_SHELL_PROTOCOL  *gEfiShellProtocol;

	DEBUG ((EFI_D_INFO, "FileReadSelf Handle File: %s start:\n ", FileName));
	
    Status = OpenShellProtocol(&gEfiShellProtocol);
    
    Status = gEfiShellProtocol->OpenFileByName(FileName, &FileHandle, EFI_FILE_MODE_READ); 
    if (EFI_ERROR(Status))
    {
        DEBUG ((EFI_D_INFO, "OpenFileByName: %s Failed!\n ", FileName));
        
        return (-1);
    }
        
    Status = gEfiShellProtocol->ReadFile(FileHandle, &size, pBuffer);    
    if (EFI_ERROR(Status))
    {
        DEBUG ((EFI_D_INFO, "ReadFile Failed: %x!\n ", Status));
        
        return (-1);
    }

    DEBUG ((EFI_D_INFO, "Read File: %s successfully!\n ", FileName));

    return EFI_SUCCESS;

}

EFI_STATUS FileReadSelf2(CHAR16 *FileName, UINT32 size, UINT8 *pBuffer)
{    
	EFI_STATUS		Status;
	UINTN					NumHandles, i;
	EFI_HANDLE				*ControllerHandle = NULL;
	EFI_DEVICE_PATH_TO_TEXT_PROTOCOL	*DevPathToText;
	EFI_DISK_IO_PROTOCOL			*DiskIo;
	EFI_BLOCK_IO_PROTOCOL			*BlockIo;


	DEBUG ((EFI_D_INFO, "%d FileReadSelf2 START\n ", __LINE__));
	Status = gBS->LocateProtocol( &gEfiDevicePathToTextProtocolGuid, NULL, (VOID * *) &DevPathToText );
	Print( L"%d :%X\n", __LINE__ ,Status );
	if ( EFI_ERROR( Status ) )
	{
		Print( L"%d, LocateProtocol gEfiDevicePathToTextProtocolGuid error: %x\n", __LINE__, Status );
		return(Status);
	}

	Status = gBS->LocateHandleBuffer( ByProtocol, &gEfiDiskIoProtocolGuid, NULL, &NumHandles, &ControllerHandle );
	Print( L"%d :%X %d\n", __LINE__ , Status, NumHandles );
	if ( EFI_ERROR( Status ))
	{
		Print( L"%d, LocateHandleBuffer gEfiDiskIoProtocolGuid error: %x\n", __LINE__, Status );
		return(Status);
	}

	Print( L"%d, NumHandles: %d\n", __LINE__, NumHandles );

	Print( L"Before for\n", Status );
	for ( i = 0; i < NumHandles; i++ )
	{
		Status = gBS->HandleProtocol(ControllerHandle[i],
										&gEfiBlockIoProtocolGuid,
										(VOID * *) &BlockIo );
       Print( L"%d :%X\n", __LINE__ ,Status );

		if ( EFI_SUCCESS == Status )
		{
			Status = gBS->HandleProtocol( ControllerHandle[i],
									      &gEfiDiskIoProtocolGuid,
									      (VOID * *) &DiskIo );

	       Print( L"%d :%X\n", __LINE__ ,Status );
			if ( Status == EFI_SUCCESS )
			{
				Print( L"%d ==read something==%d\n", __LINE__, BlockIo->Media->MediaId);


				/*
                (EFIAPI *EFI_DISK_READ)(
                  IN EFI_DISK_IO_PROTOCOL         *This,
                  IN UINT32                       MediaId,
                  IN UINT64                       Offset,
                  IN UINTN                        BufferSize,
                  OUT VOID                        *Buffer
                  );

				*/
				Status = DiskIo->ReadDisk( DiskIo, BlockIo->Media->MediaId, 0x1C5166000, 267616, pBuffer);
				Print( L"%d :%X\n", __LINE__ ,Status );

				if ( EFI_SUCCESS == Status )
				{
					//Buffer[10] = '\0';
					Print( L"%d EFI_SUCCESS == Status\n", __LINE__ );

					//DecToCharBuffer(Buffer, BYTES, Bufferout);
			       
			       //ShowHex(Buffer, BYTES);
				}

				return -1;
			}		
		}
		else 
		{
			Print( L"!!failed to read disk!!\n" );
		}

	}

    return EFI_SUCCESS;

}


EFI_STATUS WindowCreateUseBuffer(UINT8 *pBuffer, UINT8 *pParent, UINT16 Width, UINT16 Height, UINT16 Type, CHAR8 *pWindowTitle)
{
	UINT16 i, j;

    if (NULL == pBuffer)
	{
		DEBUG ((EFI_D_INFO, "NULL == pBuffer"));
		return EFI_SUCCESS;
	}	

    // The Left of Window
	for (i = 0; i < Height; i++)
	{
		for (j = 0; j < Width / 3; j++)
		{
			pBuffer[(i * Width + j) * 4] = 230;
			pBuffer[(i * Width + j) * 4 + 1] = 130;
			pBuffer[(i * Width + j) * 4 + 2] = 30;
		}
	}

	// The right of Window
	for (i = 0; i < Height; i++)
	{
		for (j = Width / 3 - 1; j < Width; j++)
		{
			pBuffer[(i * Width + j) * 4] = 0xff;
			pBuffer[(i * Width + j) * 4 + 1] = 0xff;
			pBuffer[(i * Width + j) * 4 + 2] = 0xff;
		}
	}

    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    
    Color.Blue = 145;
    Color.Green= 145;
    Color.Red= 145;
    
    //DrawChineseCharIntoBuffer2(pBuffer, 3, 6,         46 * 94 + 50, Color, 16);
    
    //DrawChineseCharIntoBuffer2(pBuffer, 3 + 16, 6,     21 * 94 + 36, Color, Width);
    //DrawChineseCharIntoBuffer2(pBuffer, 3 + 16 * 2, 6, 21 * 94 + 71, Color, Width);
    //DrawChineseCharIntoBuffer2(pBuffer, 3 + 16 * 3, 6, 36 * 94 + 52, Color, Width);
    /*

    Color.Blue = 145;
    Color.Green= 145;
    Color.Red= 145;
	DrawLineIntoBuffer(pBuffer, 0, 0, 100, 100, 1, Color, Width);
    DrawLineIntoBuffer(pBuffer, 100, 0, 0, 100, 1, Color, Width);
    DrawLineIntoBuffer(pBuffer, 100, 0, 0, 100, 1, Color, Width);


    // Display *.txt from disk
    UINT16 size = 27;
    UINT8 *pBuffer2;
    
	pBuffer2 = (UINT8 *)AllocateZeroPool(size);
	if (NULL == pBuffer2)
    {
        DEBUG ((EFI_D_INFO, "AllocateZeroPool Failed: %x!\n "));
        
        return (-1);
    }
    
	EFI_STATUS Status = FileReadSelf(L"test2.txt", size, pBuffer2);
	if (Status == -1)
	{
		FreePool(pBuffer2);
		DEBUG ((EFI_D_INFO, "Read test2.txt failed: \n "));
		return EFI_SUCCESS;
	}
	
    DEBUG ((EFI_D_INFO, "Before color: \n "));
    Color.Blue  = 0x00;
    Color.Red   = 0x00;
    Color.Green = 0x00;
    
    for (i = 0; i < 10; i++)
    	DrawAsciiCharIntoBuffer(pMyComputerBuffer, 10 + i * 8, 20, pBuffer2[i], Color, MyComputerWidth);
	*/

	return EFI_SUCCESS;
}



VOID MyComputerWindow(UINT16 StartX, UINT16 StartY)
{
	UINT8 *pParent;
	UINT16 Type;
	CHAR8 * pWindowTitle;


	pMyComputerBuffer = (UINT8 *)AllocateZeroPool(MyComputerWidth * MyComputerHeight * 4); 
	if (pMyComputerBuffer == NULL)
	{
		DEBUG ((EFI_D_INFO, "MyComputer , AllocateZeroPool failed... "));
		return;
	}

	WindowCreateUseBuffer(pMyComputerBuffer, pParent, MyComputerWidth, MyComputerHeight, Type, pWindowTitle);
}


EFI_STATUS ChineseCharArrayInit()
{
    // 87 line, 96 Chinese char each line, 32 Char each Chinese char
	UINT32 size = 267616 + 1;
    //UINT8 *sChineseChar;
    
	sChineseChar = (UINT8 *)AllocateZeroPool(size);
	if (NULL == sChineseChar)
    {
        DEBUG ((EFI_D_INFO, "ChineseCharArrayInit AllocateZeroPool Failed: %x!\n "));
        
        return (-1);
    }
    
	if (FileReadSelf2(L"HZK16", size, sChineseChar) == -1)
	{
		FreePool((VOID *)sChineseChar);
		sChineseChar = NULL;
		DEBUG ((EFI_D_INFO, "Read HZK16 failed\n"));
		return EFI_SUCCESS;
	}


    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    Color.Blue  = 0x00;
    Color.Red   = 0x00;
    Color.Green = 0x00;
    UINT16 x, y;
    x = 10;
    y = 20;
    for (UINT16 j = 0 ; j < 40 ; j++)
	{
		for (UINT16 i = 0 ; i < 37 ; i++)
	    {	    
	    	//DrawChineseCharIntoBuffer2(pMyComputerBuffer, x + i * 16, y, j * 94 + i, Color, MyComputerWidth);
	    	//DEBUG ((EFI_D_INFO, "ChineseCharArrayInit: %x \n ", sChineseChar[1504 * 32 + i]));
	    }
		x = 10;
		y += 16;
	 }
	return EFI_SUCCESS;
}



VOID EFIAPI DecToChar1( UINT8* CharBuff, UINT8 I )
{
	CharBuff[0]	= ( (I / 16) > 9) ? ('A' + (I / 16) - 10) : ('0' + (I / 16) );
	CharBuff[1]	= ( (I % 16) > 9) ? ('A' + (I % 16) - 10) : ('0' + (I % 16) );
}

VOID EFIAPI DecToCharBuffer1( UINT8* Buffin, UINTN len, UINT8* Buffout )
{
	UINTN i = 0;

	for ( i = 0; i < len; ++i )
	{
		DecToChar1( Buffout + i * 4, Buffin[i] );
		if ( (i + 1) % 16 == 0 )
		{
			*(Buffout + i * 4 + 2)	= '\r';
			*(Buffout + i * 4 + 3)	= '\n';
		}
		else  
		{
			*(Buffout + i * 4 + 2)	= ' ';
			*(Buffout + i * 4 + 3)	= ' ';
		}
	}
}
 


VOID EFIAPI ShowHex1( UINT8* Buffer, UINTN len )
{
	UINTN i = 0;
	for ( i = 0; i < 52; ++i )
	{
		if ( i % 26 == 0 )			
        	DebugPrint1(1350, 16 * 16, "%d\n", __LINE__);
			
		DebugPrint1(i * 4, 16 * 16, L"%02x ", Buffer[i] );
	}
}


EFI_STATUS DiskReadUseDiskIo(UINT32   MediaId,
										  UINT64   Offset,
										  UINTN    BufferSize,
										  VOID    *Buffer)
{	
    //DebugPrint1(DISK_READ_X, DISK_READ_Y, "%d: DiskReadUseDiskIo \n", __LINE__);
	EFI_STATUS		Status;
	UINTN					NumHandles, i;
	EFI_HANDLE				*ControllerHandle = NULL;
	EFI_DEVICE_PATH_TO_TEXT_PROTOCOL	*DevPathToText;
    EFI_DISK_IO_PROTOCOL            *DiskIo;
    EFI_BLOCK_IO_PROTOCOL           *BlockIo;

    UINT8 Buffer1[BYTES];
    UINT8 Bufferout[BYTES * EXBYTE];

    Status = gBS->LocateProtocol( &gEfiDevicePathToTextProtocolGuid, NULL, (VOID * *) &DevPathToText );
    //Print( L"%d :%X\n", __LINE__ ,Status );
    if ( EFI_ERROR( Status ) )
    {
        DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
        return(Status);
    }

    Status = gBS->LocateHandleBuffer( ByProtocol, &gEfiDiskIoProtocolGuid, NULL, &NumHandles, &ControllerHandle );
    //Print( L"%d :%X %d\n", __LINE__ , Status, NumHandles );
    if ( EFI_ERROR( Status ))
    {
        DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
        //Print( L"%d, LocateHandleBuffer gEfiDiskIoProtocolGuid error: %x\n", __LINE__, Status );
        return(Status);
    }

    //Print( L"%d, NumHandles: %d\n", __LINE__, NumHandles );

    //Print( L"Before for\n", Status );
    for ( i = 0; i < NumHandles; i++ )
    {   
        
        Status = gBS->HandleProtocol(ControllerHandle[i],
                                        &gEfiBlockIoProtocolGuid,
                                        (VOID * *) &BlockIo );
        //Print( L"%d :%X\n", __LINE__ ,Status );
        DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);

        if ( EFI_SUCCESS == Status )
        {			        	 
        	 
             Status = gBS->HandleProtocol( ControllerHandle[i],
                                          &gEfiDiskIoProtocolGuid,
                                          (VOID * *) &DiskIo ); 
            DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);

            //Print( L"%d :%X\n", __LINE__ ,Status );
            if ( Status == EFI_SUCCESS )
            {
            	  BlockIo->Media->MediaId;
                //Print( L"%d, %d\n", __LINE__, BlockIo->Media->MediaId);

                Status = DiskIo->ReadDisk( DiskIo, BlockIo->Media->MediaId, 0, BYTES, Buffer1 );
                //Print( L"%d :%X\n", __LINE__ ,Status );
                DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X BlockIo->Media->MediaId: %d\n", __LINE__, Status, BlockIo->Media->MediaId);

                if ( EFI_SUCCESS == Status )
                {
                    //Buffer1[10] = '\0';
                    //Print( L"%d :%s\n", __LINE__ , Buffer );

					  for (int j = 0; j < 50; j++)
					  	   DebugPrint1(DISK_READ_BUFFER_X + j * 8 * 3, DISK_READ_BUFFER_Y + i * 16, "%02X ", Buffer1[j] & 0xff);

                    //DecToCharBuffer1(Buffer1, BYTES, Bufferout);
                   
                    //ShowHex1(Bufferout, BYTES);
                }
            }       
        }
        else 
        {
        	 DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
            //Print( L"!!failed to read disk!!\n" );
        }
    }
    return EFI_SUCCESS;
}


EFIAPI HandleEnterPressed()
{
	DEBUG ((EFI_D_INFO, "%d HandleEnterPressed\n", __LINE__));
	
    PartitionUSBReadSynchronous();

    //PartitionUSBReadAsynchronous();
}

STATIC
VOID
EFIAPI
HandleKeyboardEvent (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
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
    
    for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) 
    {
		Status = gBS->HandleProtocol (Handles[HandleIndex], &gEfiSimpleTextInputExProtocolGuid, (VOID **) &SimpleEx);

		if (EFI_ERROR(Status))  
		    continue;
		else
		{
			Status = gBS->CheckEvent(SimpleEx->WaitForKeyEx);
			if (Status == EFI_NOT_READY)
			    continue;
			  
			gBS->WaitForEvent(1, &(SimpleEx->WaitForKeyEx), &Index);

			Status = SimpleEx->ReadKeyStrokeEx(SimpleEx, &KeyData);
			if(!EFI_ERROR(Status))
			{    
			    scanCode    = KeyData.Key.ScanCode;
			    uniChar     = KeyData.Key.UnicodeChar;
			    shiftState  = KeyData.KeyState.KeyShiftState;
			    toggleState  = KeyData.KeyState.KeyToggleState;
                
               pKeyboardInputBuffer[keyboard_input_count++] = uniChar;

			    // Enter pressed
		     	 if (0x0D == uniChar)
		     	 {
		        	keyboard_input_count = 0;
		        	memset(pKeyboardInputBuffer, '\0', KEYBOARD_BUFFER_LENGTH);
		     	 	DebugPrint1(DISPLAY_KEYBOARD_X, DISPLAY_KEYBOARD_Y, "%a keyboard_input_count: %04d enter pressed", pKeyboardInputBuffer, keyboard_input_count);
		     	 	HandleEnterPressed();
		     	 }
		     	 else
		     	 {
		     	 	DebugPrint1(DISPLAY_KEYBOARD_X, DISPLAY_KEYBOARD_Y, "%a keyboard_input_count: %04d ", pKeyboardInputBuffer, keyboard_input_count);
		     	 }
		        
		        if (keyboard_input_count >= KEYBOARD_BUFFER_LENGTH - 1)
		        {
		        }
		        

			}
		}    
    }  
	
	 //DrawAsciiCharUseBuffer(pDeskBuffer, DISPLAY_KEYBOARD_X, DISPLAY_KEYBOARD_Y, uniChar, Color);
	 
	 GraphicsLayerCompute(iMouseX, iMouseY);
}

 EFI_STATUS DiskReadEx()
 {   
     DebugPrint1(550, 0, "%d: DiskReadEx \n", __LINE__);
     EFI_STATUS      Status;
     UINTN                   NumHandles, i;
     EFI_HANDLE              *ControllerHandle = NULL;
     EFI_DEVICE_PATH_TO_TEXT_PROTOCOL    *DevPathToText;
     EFI_DISK_IO2_PROTOCOL            *DiskIo2;
     EFI_BLOCK_IO2_PROTOCOL           *BlockIo2;
 
     UINT8 Buffer[BYTES];
 
     UINT8 Bufferout[BYTES * EXBYTE];
 
     Status = gBS->LocateProtocol( &gEfiDevicePathToTextProtocolGuid, NULL, (VOID * *) &DevPathToText );
     Print( L"%d :%X\n", __LINE__ ,Status );
     if ( EFI_ERROR( Status ) )
     {
         Print( L"%d, LocateProtocol gEfiDevicePathToTextProtocolGuid error: %x\n", __LINE__, Status );
         return(Status);
     }
 
     Status = gBS->LocateHandleBuffer( ByProtocol, &gEfiDiskIo2ProtocolGuid, NULL, &NumHandles, &ControllerHandle );
     Print( L"%d :%X %d\n", __LINE__ , Status, NumHandles );
     if ( EFI_ERROR( Status ))
     {
         Print( L"%d, LocateHandleBuffer gEfiDiskIoProtocolGuid error: %x\n", __LINE__, Status );
         return(Status);
     }
     Print( L"Before for\n", Status );
 
     Print( L"%d, NumHandles: %d\n", __LINE__, NumHandles );
 
     for ( i = 0; i < NumHandles; i++ )
     {       
         Status = gBS->HandleProtocol(ControllerHandle[i],
                                         &gEfiBlockIo2ProtocolGuid,
                                         (VOID * *) &BlockIo2 );
              
         Print( L"%d :%X\n", __LINE__ ,Status );
 
         if ( EFI_SUCCESS == Status )
         {           
             Status = gBS->HandleProtocol( ControllerHandle[i],
                                       &gEfiDiskIo2ProtocolGuid,
                                       (VOID * *) &DiskIo2 );
             if ( Status != EFI_SUCCESS )
             {
             	   Print( L"%d :%X\n", __LINE__ ,Status );
                 return;
             }
                
             Print( L"%d :%X\n", __LINE__ ,Status );
             if ( Status == EFI_SUCCESS )
             {
                 Print( L"%d, %d\n", __LINE__, BlockIo2->Media->MediaId);
 
                 Status = DiskIo2->ReadDiskEx( DiskIo2, BlockIo2->Media->MediaId, disk_handle_task.DiskIo2Token.Event, 0, BYTES, Buffer );
				  
                 //Print( L"%d :%X\n", __LINE__ ,Status );
 
                 if ( EFI_SUCCESS == Status )
                 {
                     Buffer[10] = '\0';
                     //Print( L"%d :%s\n", __LINE__ , Buffer );
 
                     DecToCharBuffer1(Buffer, BYTES, Bufferout);
                    
                     ShowHex1(Bufferout, BYTES);
                 }
             }       
         }
         else 
         {
             Print( L"!!failed to read disk!!\n" );
         }
     }
     return EFI_SUCCESS;
 }


 // iMouseX: left top
 // iMouseY: left top
 VOID HandleMouseRightClick(int iMouseX, int iMouseY)
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





// for mouse move & click
STATIC
VOID
EFIAPI
HandleMouseEvent (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{	
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
    
    CHAR8 x_move = 0;
    CHAR8 y_move = 0;
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

    //DEBUG ((EFI_D_INFO, "X: %X, Y: %X ", x_move, y_move));
    DebugPrint1(DISPLAY_MOUSE_X, DISPLAY_MOUSE_Y, "X: %04d, Y: %04d Increment X: %X Y: %X", iMouseX, iMouseY, x_move, y_move );
    
    iMouseX = iMouseX + x_move;
    iMouseY = iMouseY + y_move;

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
        DEBUG ((EFI_D_INFO, "Left button clicked\n"));
        
	    HandleMouseRightClick(iMouseX, iMouseY);
	    //DrawAsciiCharUseBuffer(GraphicsOutput, 20 + process2_i * 8 + 16, 60, 'E', Color);  
    }
    
    if (State.RightButton == 0x01)
    {
        DEBUG ((EFI_D_INFO, "Right button clicked\n"));
	    //DrawAsciiCharUseBuffer(GraphicsOutput, 20 + process2_i * 8 + 16, 60, 'R', Color);

	    HandleMouseRightClick(iMouseX, iMouseY);
    }
    
    //DEBUG ((EFI_D_INFO, "\n"));
    GraphicsLayerCompute(iMouseX, iMouseY);
	    
	gBS->WaitForEvent( 1, &gMouse->WaitForInput, &Index );
}


VOID MemoryParameterGet()
{
	UINT32 EfiMemoryMapSize = 0;
	EFI_STATUS Status;
	UINTN                       *MemoryMapSize = NULL;
	EFI_MEMORY_DESCRIPTOR       *EfiMemoryMap = NULL;
	UINTN                       *EfiMapKey = NULL;
	UINTN                       *EfiDescriptorSize = NULL;
	UINT32                      *EfiDescriptorVersion = NULL;
	UINT64   ReservedMemoryTypePage = 0;
	UINT64   LoaderCodePage = 0;
	
	//DebugPrint1(0, 8 * 16, "%d: \n", __LINE__);

	DEBUG ((EFI_D_INFO, "%d: MemoryParameterGet\n", __LINE__));

	Status = gBS->GetMemoryMap (&MemoryMapSize,
									EfiMemoryMap,
									&EfiMapKey,
									&EfiDescriptorSize,
									&EfiDescriptorVersion);
	
	ASSERT_EFI_ERROR(Status == EFI_BUFFER_TOO_SMALL);
	
	do 
	{
		EfiMemoryMap = AllocatePool (EfiMemoryMapSize);
		if (EfiMemoryMap == NULL)
		{
			DEBUG ((EFI_D_ERROR, "ERROR!! Null Pointer returned by AllocatePool ()\n"));
			ASSERT_EFI_ERROR (EFI_OUT_OF_RESOURCES);
			return Status;
		}
		Status = gBS->GetMemoryMap (&EfiMemoryMapSize,
										EfiMemoryMap,
										&EfiMapKey,
										&EfiDescriptorSize,
										&EfiDescriptorVersion);
		if (EFI_ERROR(Status)) 
		{
			FreePool (EfiMemoryMap);
		}
	} while (Status == EFI_BUFFER_TOO_SMALL);

	DEBUG((DEBUG_ERROR | DEBUG_PAGE, "EfiMemoryMapSize=0x%x EfiDescriptorSize=0x%x EfiMemoryMap=0x%x \n", EfiMemoryMapSize, EfiDescriptorSize, (UINTN)EfiMemoryMap));

	EFI_MEMORY_DESCRIPTOR *EfiMemoryMapEnd = (EFI_MEMORY_DESCRIPTOR *)((UINT8 *)EfiMemoryMap + EfiMemoryMapSize);
	EFI_MEMORY_DESCRIPTOR *EfiEntry = EfiMemoryMap;

	DEBUG((DEBUG_ERROR | DEBUG_PAGE,"===========================%S============================== Start\n", L"CSDN MemMap"));

	while (EfiEntry < EfiMemoryMapEnd) 
	{
		if (EfiEntry->Type == EfiReservedMemoryType)
		{
			DEBUG((DEBUG_ERROR | DEBUG_PAGE, "[CSDN] EfiReservedMemoryType  %3d %16lx pn %16lx \n", EfiEntry->Type, EfiEntry->PhysicalStart, EfiEntry->NumberOfPages));
			ReservedMemoryTypePage = ReservedMemoryTypePage + EfiEntry->NumberOfPages;
		}
		else if (EfiEntry->Type == EfiLoaderCode)
		{
			DEBUG((DEBUG_ERROR | DEBUG_PAGE, "[CSDN] EfiLoaderCode  %3d %16lx pn %16lx \n", EfiEntry->Type, EfiEntry->PhysicalStart, EfiEntry->NumberOfPages));
			LoaderCodePage = LoaderCodePage + EfiEntry->NumberOfPages;
		}
	}
    /**/
   //：https://blog.csdn.net/xiaopangzi313/article/details/109928878
}


// display system date & time
STATIC
VOID
EFIAPI
SystemParameterRead (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
	ShellServiceRead();
	//MemoryParameterGet();
}

// display system date & time
STATIC
VOID
EFIAPI
DisplaySystemDateTime (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
    EFI_TIME et;
	date_time_count++;
	gRT->GetTime(&et, NULL);

	DebugPrint1(DISPLAY_DESK_DATE_TIME_X, DISPLAY_DESK_DATE_TIME_Y, "%04d-%02d-%02d %02d:%02d:%02d", 
				  et.Year, et.Month, et.Day, et.Hour, et.Minute, et.Second);
	
   DebugPrint1(DISPLAY_DESK_HEIGHT_WEIGHT_X, DISPLAY_DESK_HEIGHT_WEIGHT_Y, "%d ScreenWidth:%d, ScreenHeight:%d\n", __LINE__, ScreenWidth, ScreenHeight);
   GraphicsLayerCompute(iMouseX, iMouseY);
}

EFI_STATUS MultiProcessInit ()
{
    UINT16 i;
	EFI_GUID gMultiProcessGuid  = { 0x0579257E, 0x1843, 0x45FB, { 0x83, 0x9D, 0x6B, 0x79, 0x09, 0x38, 0x29, 0xA9 } };
    pMouseBuffer = (UINT8 *)AllocateZeroPool(16 * 16 * 4);
    if (NULL == pMouseBuffer)
		DEBUG ((EFI_D_INFO, "MultiProcessInit pMouseBuffer pDeskDisplayBuffer NULL\n"));
	
	pMouseSelectedBuffer = (UINT8 *)AllocateZeroPool(16 * 16 * 4 * 2);
    if (NULL == pDeskDisplayBuffer)
		DEBUG ((EFI_D_INFO, "ScreenInit AllocatePool pDeskDisplayBuffer NULL\n"));
		
	MouseColor.Blue  = 0xff;
    MouseColor.Red   = 0xff;
    MouseColor.Green = 0xff;

	//DrawChineseCharIntoBuffer2(pMouseBuffer, 0, 0, 11 * 94 + 42, Color, 16);
	
    //DrawChineseCharIntoBuffer(pMouseBuffer, 0, 0, 0, Color, 16);
    
    EFI_EVENT_NOTIFY       TaskProcesses[] = {DisplaySystemDateTime, HandleKeyboardEvent, HandleMouseEvent, SystemParameterRead};

    for (i = 0; i < sizeof(TaskProcesses) / sizeof(EFI_EVENT_NOTIFY); i++)
    {
        gBS->CreateEventEx(
                          EVT_NOTIFY_SIGNAL,
                          TPL_NOTIFY,
                          TaskProcesses[i],
                          NULL,
                          &gMultiProcessGuid,
                          &MultiTaskTriggerEvent
                          );
    }    

    return EFI_SUCCESS;
}
// https://blog.csdn.net/longsonssss/article/details/80221513


EFI_STATUS SystemTimeIntervalInit()
{
    EFI_STATUS	Status;
	EFI_HANDLE	TimerOne	= NULL;
	static const UINTN TimeInterval = 1000000;

	Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL | EVT_TIMER,
                       		TPL_CALLBACK,
                       		TimeSlice,
                       		NULL,
                       		&TimerOne);

	if ( EFI_ERROR( Status ) )
	{
		DEBUG(( EFI_D_INFO, "Create Event Error! \r\n" ));
		return(1);
	}

	Status = gBS->SetTimer(TimerOne,
						  TimerPeriodic,
						  MultU64x32( TimeInterval, 1));

	if ( EFI_ERROR( Status ) )
	{
		DEBUG(( EFI_D_INFO, "Set Timer Error! \r\n" ));
		return(2);
	}

	while (1)
	{		
	}
	
	gBS->SetTimer( TimerOne, TimerCancel, 0 );
	gBS->CloseEvent( TimerOne );    

	return EFI_SUCCESS;
}

EFI_STATUS ScreenInit(EFI_GRAPHICS_OUTPUT_PROTOCOL   *GraphicsOutput)
{
	//UINT32 i = 0;
	//UINT32 j = 0;
	UINT32 x = ScreenWidth;
	UINT32 y = ScreenHeight;
	//UINT8 p[100];
	
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
  
 
    Color.Red   = 0x00;
    Color.Green = 0x84;
    Color.Blue	= 0x84;
    RectangleFillIntoBuffer(pDeskBuffer, 0,     0,      x -  1, y - 29, 1, Color);
    
    Color.Red   = 0xC6;
    Color.Green = 0xC6;
    Color.Blue	= 0xC6;
    RectangleFillIntoBuffer(pDeskBuffer, 0,     y - 28, x -  1, y - 28, 1, Color);

    Color.Red   = 0xFF;
    Color.Green = 0xFF;
    Color.Blue	= 0xFF;
    RectangleFillIntoBuffer(pDeskBuffer, 0,     y - 27, x -  1, y - 27, 1, Color);
    
    Color.Red   = 0xC6;
    Color.Green = 0xC6;
    Color.Blue	= 0xC6;
    RectangleFillIntoBuffer(pDeskBuffer, 0,     y - 26, x -  1, y -  1, 1, Color);
  	
    Color.Red   = 0xFF;
    Color.Green = 0xFF;
    Color.Blue	= 0xFF;
    RectangleFillIntoBuffer(pDeskBuffer, 3,     y - 24, 59,     y - 24, 1, Color);
    RectangleFillIntoBuffer(pDeskBuffer, 2,     y - 24, 59,     y - 4, 1, Color);

    Color.Red   = 0x84;
    Color.Green = 0x84;
    Color.Blue	= 0x84;
    RectangleFillIntoBuffer(pDeskBuffer, 3,     y -  4, 59,     y -  4, 1, Color);
    RectangleFillIntoBuffer(pDeskBuffer, 59,     y - 23, 59,     y -  5, 1, Color);

    
    Color.Red   = 0x00;
    Color.Green = 0x00;
    Color.Blue	= 0x00;
    RectangleFillIntoBuffer(pDeskBuffer, 2,     y -  3, 59,     y -  3, 1, Color);
    RectangleFillIntoBuffer(pDeskBuffer, 60,    y - 24, 60,     y -  3, 1, Color);

    Color.Red   = 0x84;
    Color.Green = 0x84;
    Color.Blue	= 0x84;
    RectangleFillIntoBuffer(pDeskBuffer, x - 163, y - 24, x -  4, y - 24, 1, Color);
    RectangleFillIntoBuffer(pDeskBuffer, x - 163, y - 23, x - 47, y -  4, 1, Color);
    
    Color.Red   = 0xFF;
    Color.Green = 0xFF;
    Color.Blue	= 0xFF;
    RectangleFillIntoBuffer(pDeskBuffer, x - 163,    y - 3, x - 4,     y - 3, 1, Color);
    RectangleFillIntoBuffer(pDeskBuffer, x - 3,     y - 24, x - 3,     y - 3, 1, Color);
        /*
    Color.Red   = 0xFF;
    Color.Green = 0xFF;
    Color.Blue	= 0xFF;
    RectangleFillIntoBuffer(pDeskBuffer, 0, 100, 100, 200, 1, Color);
    
    Color.Red   = 0x00;
    Color.Green = 0x00;
    Color.Blue	= 0xFF;
    RectangleFillIntoBuffer(pDeskBuffer, 0, 200, 100, 400, 1, Color);

    
    Color.Red   = 0x00;
    Color.Green = 0xFF;
    Color.Blue	= 0x00;
    RectangleFillIntoBuffer(pDeskBuffer, 0, 300, 100, 400, 1, Color);
    
    
    Color.Red   = 0xFF;
    Color.Green = 0x00;
    Color.Blue	= 0x00;
    RectangleFillIntoBuffer(pDeskBuffer, 0, 400, 100, 500, 1, Color);
    
    Color.Red  = 0xFF;
    Color.Green = 0x00;
    Color.Blue	= 0xFF;

    LineDrawIntoBuffer(pDeskBuffer, 0, 0, 100, 100, 2, Color, ScreenWidth);
    LineDrawIntoBuffer(pDeskBuffer, 100, 0, 0, 100, 1, Color, ScreenWidth);

    Color.Red  = 0xFF;
    Color.Green = 0xFF;
    Color.Blue	= 0xFF;

    // Display "wo"    
    //DrawChineseCharIntoBuffer(pDeskBuffer, 20, 20 + 16, 0, Color, ScreenWidth);
    
    */
    MyComputerWindow(100, 100);
	//ChineseCharArrayInit();
	
    Color.Red   = 0x00;
    Color.Green = 0x00;
    Color.Blue	 = 0x00;
    //DrawChineseCharIntoBuffer2(pDeskBuffer,  16, ScreenHeight - 21,     (18 - 1) * 94 + 43 - 1, Color, ScreenWidth);
    //DrawChineseCharIntoBuffer2(pDeskBuffer,  16 * 2, ScreenHeight - 21, (21 - 1) * 94 + 05 - 1, Color, ScreenWidth);

/**/
    //Display ASCII Char
    //count = 60;
    //for (i = 40; i < 65 + 60; i++)
    //    DrawAsciiCharIntoBuffer(pDeskBuffer, 20 + (i - 40) * 8, 20, i, Color);
	
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

VOID
EFIAPI
DiskHandleComplete (
  IN  EFI_EVENT                Event,
  IN  VOID                     *Context
  )
{	
    DebugPrint1(DISK_READ_X, DISK_READ_Y, "line:%d DiskHandleComplete \n",  __LINE__);	
    gBS->SignalEvent(disk_handle_task.DiskIo2Token.Event);
}


EFI_STATUS ParametersInitial()
{
    EFI_STATUS	Status;
	pDeskBuffer = (UINT8 *)AllocatePool(ScreenWidth * ScreenHeight * 4); 
	if (NULL == pDeskBuffer)
	{
		DEBUG ((EFI_D_INFO, "ScreenInit AllocatePool pDeskBuffer NULL\n"));
		return -1;
	}

	pDeskDisplayBuffer = (UINT8 *)AllocatePool(ScreenWidth * ScreenHeight * 4); 
	if (NULL == pDeskDisplayBuffer)
	{
		DEBUG ((EFI_D_INFO, "ScreenInit AllocatePool pDeskDisplayBuffer NULL\n"));
		return -1;
	}

	pMouseClickBuffer = (UINT8 *)AllocatePool(MouseClickWindowWidth * MouseClickWindowHeight * 4); 
	if (pMouseClickBuffer == NULL)
	{
		return -1;
	}   

	iMouseX = ScreenWidth / 2;
	iMouseY = ScreenHeight / 2;

    //disk_handle_task.DiskIo2Token.Event;

	/*
	  IN  UINT32                       Type,
	  IN  EFI_TPL                      NotifyTpl,
	  IN  EFI_EVENT_NOTIFY             NotifyFunction,
	  IN  VOID                         *NotifyContext,
	  OUT EFI_EVENT                    *Event
	*/

	//refer to FatDiskIo
	Status = gBS->CreateEvent(
					          EVT_NOTIFY_SIGNAL,
					          TPL_NOTIFY,
					          DiskHandleComplete,
					          &disk_handle_task,
					          disk_handle_task.DiskIo2Token.Event);
	if (EFI_ERROR (Status)) 
    {
    	DEBUG((EFI_D_INFO, "%d Status:%X\n", __LINE__, Status));
       return Status;
    }
	return EFI_SUCCESS;
}



EFI_STATUS
EFIAPI
Main (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_STATUS                         Status;
    
    Status = gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (VOID **) &GraphicsOutput);        
    if (EFI_ERROR (Status)) 
    {
    	 DebugPrint1(100, 100, "%d %d\n", __LINE__, Status);
        return EFI_UNSUPPORTED;
    }
    handle = &ImageHandle;


    ScreenWidth  = GraphicsOutput->Mode->Info->HorizontalResolution;
    ScreenHeight = GraphicsOutput->Mode->Info->VerticalResolution;

    DEBUG(( EFI_D_INFO, "ScreenWidth:%d, ScreenHeight:%d\n", ScreenWidth, ScreenHeight));

    if (-1 == ParametersInitial())
    {
		DEBUG(( EFI_D_INFO, "-1 == AllocateMemory1()\n"));
    }
    
    
    ScreenInit(GraphicsOutput);
        
	Status = MouseInit();    
	if (EFI_ERROR (Status)) 
    {
        return EFI_UNSUPPORTED;
    }
    
    PartitionRead();    
    
    MultiProcessInit();

    SystemTimeIntervalInit();

	
	DebugPrint1(100, 100, "%d %d\n", __LINE__, Status);
	
    return EFI_SUCCESS;
}

