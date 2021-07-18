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
**/

#include "FrontPage.h"
#include "FrontPageCustomizedUi.h"

#include <Protocol/SimplePointer.h>
#include <Protocol/AbsolutePointer.h>
#include <Protocol/Shell.h>
#include <Protocol/ShellParameters.h>
#include <Library/ShellLib.h>

#include <Library/UefiLib.h> 
#include <Library/ShellCEntryLib.h> 
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/SimplePointer.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/DebugLib.h>

#include <Guid/GlobalVariable.h>
#include <Guid/ShellLibHiiGuid.h>

#include <Protocol/Shell.h>
#include <Protocol/ShellParameters.h>
#include <Protocol/DevicePath.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/UnicodeCollation.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/ShellCommandLib.h>
#include <Library/ShellLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/HiiLib.h>
#include <Library/SortLib.h>
#include <Guid/GlobalVariable.h>
#include <Guid/ConsoleInDevice.h>
#include <Guid/ConsoleOutDevice.h>
#include <Guid/ShellLibHiiGuid.h>

#include <IndustryStandard/Pci.h>

#include <Pi/PiFirmwareVolume.h>
#include <Pi/PiFirmwareFile.h>

#include <Library/FileHandleLib.h>
#include <Pi/PiFirmwareFile.h>
#include <Protocol/FirmwareVolume2.h>

#include <Protocol/Shell.h>
#include <Protocol/ShellParameters.h>
#include <Protocol/DevicePath.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/UnicodeCollation.h>
#include <Protocol/DriverDiagnostics2.h>
#include <Protocol/DriverDiagnostics.h>
#include <Protocol/PlatformDriverOverride.h>
#include <Protocol/BusSpecificDriverOverride.h>
#include <Protocol/PlatformToDriverConfiguration.h>
#include <Protocol/DriverSupportedEfiVersion.h>
#include <Protocol/DriverFamilyOverride.h>
#include <Protocol/DriverHealth.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/ShellCommandLib.h>
#include <Library/ShellLib.h>
#include <Library/SortLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/HiiLib.h>
#include <Library/FileHandleLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PrintLib.h>
#include <Library/HandleParsingLib.h>
#include <Library/PeCoffGetEntryPointLib.h>
#include <Library/HandleParsingLib.h>
#include <Guid/TtyTerm.h>
#include <Guid/MdeModuleHii.h>
#include <Guid/FileSystemVolumeLabelInfo.h>
#include <Guid/GlobalVariable.h>
#include <Guid/HiiBootMaintenanceFormset.h>

#include <Protocol/LoadFile.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/SerialIo.h>
#include <Protocol/DevicePathToText.h>
#include <Protocol/FormBrowserEx2.h>

#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/HiiLib.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/UefiBootManagerLib.h>
#include <Library/FileExplorerLib.h>

static UINTN ScreenWidth, ScreenHeight;  

UINT8 *pMyComputerBuffer = NULL;
UINT16 MyComputerWidth = 700;
UINT16 MyComputerHeight = 400;

const UINT8 *sChineseChar = NULL;

EFI_GRAPHICS_OUTPUT_PROTOCOL       *GraphicsOutput;
EFI_SIMPLE_POINTER_PROTOCOL        *gMouse;

extern EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *gSimpleFileSystem; 

UINT8 *DeskBuffer = NULL;
UINT8 *pDeskDisplayBuffer = NULL;

UINT8 *MouseBuffer;
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

void CopyColorIntoBuffer3(UINT8 *pBuffer, EFI_GRAPHICS_OUTPUT_BLT_PIXEL color, UINT16 x0, UINT16 y0, UINT8 fontWidth)
{
    pBuffer[y0 * fontWidth * 4 + x0 * 4]     = color.Blue;
    pBuffer[y0 * fontWidth * 4 + x0 * 4 + 1] = color.Green;
    pBuffer[y0 * fontWidth * 4 + x0 * 4 + 2] = color.Red;
}

INT32 Math_ABS(INT32 v)
{
	return (v < 0) ? -v : v;
}

EFI_STATUS DrawLineIntoBuffer(UINT8 *pBuffer,
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

EFI_STATUS DrawAsciiCharUseBuffer(IN EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput,
        IN UINTN x0, UINTN y0, UINT8 c,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL BorderColor)
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
		    CopyColorIntoBuffer2(pBuffer, BorderColor, 0, i); 
		
		if ((d & 0x40) != 0) 
            CopyColorIntoBuffer2(pBuffer, BorderColor, 1, i);
		
		if ((d & 0x20) != 0) 
		    CopyColorIntoBuffer2(pBuffer, BorderColor, 2, i);
		
		if ((d & 0x10) != 0) 
		    CopyColorIntoBuffer2(pBuffer, BorderColor, 3, i);
		
		if ((d & 0x08) != 0) 
		    CopyColorIntoBuffer2(pBuffer, BorderColor, 4, i);
		
		if ((d & 0x04) != 0) 
		    CopyColorIntoBuffer2(pBuffer, BorderColor, 5, i);
		
		if ((d & 0x02) != 0) 
		    CopyColorIntoBuffer2(pBuffer, BorderColor, 6, i);
		
		if ((d & 0x01) != 0) 
		    CopyColorIntoBuffer2(pBuffer, BorderColor, 7, i);
		
	}
	
    GraphicsOutput->Blt(GraphicsOutput, 
                        (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) pBuffer,
                        EfiBltBufferToVideo,
                        0, 0, 
                        x0, y0, 
                        8, 16, 
                        0);   
                
    return EFI_SUCCESS;
}

VOID StringMaker (UINT16 x, UINT16 y,
  IN  CONST CHAR8   *Format,
  IN  VA_LIST       VaList
  )
{
    CHAR8    AsciiBuffer[0x100];
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
	UINT32 i = 0;

	Color.Blue = 0xFF;
	Color.Red = 0xFF;
	Color.Green = 0xFF;

    ASSERT (Format != NULL);

    AsciiVSPrint (AsciiBuffer, sizeof (AsciiBuffer), Format, VaList);

    for (i = 0; i < sizeof(AsciiBuffer) /sizeof(CHAR8); i++)
        DrawAsciiCharUseBuffer(GraphicsOutput, x + i * 8, y, AsciiBuffer[i], Color);

}

/* Display a string */
VOID EFIAPI DebugPrint1 (UINT16 x, UINT16 y, IN  CONST CHAR8  *Format, ...)
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

	if (offset < 1)
	{
		DEBUG ((EFI_D_INFO, "offset < 1"));
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
    INT16 i,j;    
    int k = y0;
    //UINT8 pBuffer[16 * 16 * 4];

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


EFI_STATUS OpenShellProtocol( EFI_SHELL_PROTOCOL **gEfiShellProtocol )
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


EFI_STATUS PrintNode(EFI_DEVICE_PATH_PROTOCOL *Node)
{
    Print(L"(%d, %d)/", Node->Type, Node->SubType);
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


EFI_STATUS PartitionRead()
{
    DEBUG ((EFI_D_INFO, "PartitionRead!!\r\n"));
    EFI_STATUS Status ;
    EFI_SHELL_PROTOCOL  *gEfiShellProtocol;
    UINTN NumHandles, i;
    EFI_HANDLE *ControllerHandle = NULL;
    EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;

    Status = OpenShellProtocol(&gEfiShellProtocol);    
    if (EFI_ERROR(Status))
    {
    	 DEBUG ((EFI_D_INFO, "OpenShellProtocol error: %x\n", Status));
        return Status;
    }
    
    Status = gBS->LocateProtocol (&gEfiDevicePathToTextProtocolGuid, NULL, (VOID **) &DevPathToText);
    if (EFI_ERROR(Status))
    {
    	 DEBUG ((EFI_D_INFO, "LocateProtocol1 error: %x\n", Status));
        return Status;
    }
   
    Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiDiskIoProtocolGuid, NULL, &NumHandles, &ControllerHandle);
    if (EFI_ERROR(Status))
    {
        DEBUG ((EFI_D_INFO, "LocateProtocol1 error: %x\n", Status));
        return Status;
    }

    DEBUG ((EFI_D_INFO, "Before for\n", Status));
    
    for (i = 0; i < NumHandles; i++)
    {
        EFI_DEVICE_PATH_PROTOCOL *DiskDevicePath;
        Status = gBS->OpenProtocol(ControllerHandle[i],
                                   &gEfiDevicePathProtocolGuid,
                                   (VOID **)&DiskDevicePath,
                                   gImageHandle, 
                                   NULL,
                                   EFI_OPEN_PROTOCOL_GET_PROTOCOL);
        if (EFI_ERROR(Status))
        {
            DEBUG ((EFI_D_INFO, "Status = gBS->OpenProtocol error index %d: %x\n", i, Status));
            return Status;
        }

        CHAR16 *TextDevicePath = 0;
        TextDevicePath = DevPathToText->ConvertDevicePathToText(DiskDevicePath, TRUE, TRUE);
        DEBUG ((EFI_D_INFO, "%s\n", TextDevicePath));

        if (TextDevicePath) gBS->FreePool(TextDevicePath);

        WalkthroughDevicePath(DiskDevicePath, PrintNode);
        
        DEBUG ((EFI_D_INFO, "\n\n"));
    }

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
/*
	UINT64 position = 0;
    Status = gEfiShellProtocol->SetFilePosition(FileHandle, &position);    
    if (EFI_ERROR(Status))
    {
        DEBUG ((EFI_D_INFO, "SetFilePosition Failed: %x!\n ", Status));
        
        return (-1);
    }
  */      
    Status = gEfiShellProtocol->ReadFile(FileHandle, &size, pBuffer);    
    if (EFI_ERROR(Status))
    {
        DEBUG ((EFI_D_INFO, "ReadFile Failed: %x!\n ", Status));
        
        return (-1);
    }
	UINT64 Position;
    gEfiShellProtocol->GetFilePosition(FileHandle, &Position);

    DEBUG ((EFI_D_INFO, "Read File: %s successfully, Postion: %d!\n ", FileName, Position));

    return EFI_SUCCESS;

}


EFI_STATUS WindowCreateUseBuffer(UINT8 *pBuffer, UINT8 *pParent, UINT16 Width, UINT16 Height, UINT16 Type, CHAR8 *pWindowTitle)
{
	UINT16 i, j;

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


VOID ChineseCharArrayInit()
{
	UINT32 size = 32 * 96 * 87;
    //UINT8 *sChineseChar;
    
	sChineseChar = (UINT8 *)AllocateZeroPool(size);
	if (NULL == sChineseChar)
    {
        DEBUG ((EFI_D_INFO, "ChineseCharArrayInit AllocateZeroPool Failed: %x!\n "));
        
        return (-1);
    }
    
	if (FileReadSelf2(L"HZK16", size, sChineseChar) == -1)
	{
		FreePool(sChineseChar);
		DEBUG ((EFI_D_INFO, "Read HZK16 failed\n "));
		return EFI_SUCCESS;
	}

	DEBUG ((EFI_D_INFO, "Before color: \n "));
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
	    	DrawChineseCharIntoBuffer2(pMyComputerBuffer, x + i * 16, y, j * 94 + i, Color, MyComputerWidth);
	    	//DEBUG ((EFI_D_INFO, "ChineseCharArrayInit: %x \n ", sChineseChar[1504 * 32 + i]));
	    }
		x = 10;
		y += 16;
	 }
	
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
        return Status;
    
    for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) 
    {
		Status = gBS->HandleProtocol (Handles[HandleIndex], &gEfiSimpleTextInputExProtocolGuid, (VOID **) &SimpleEx);

		if (EFI_ERROR(Status))  
		    continue;
		else
		{
			EFI_STATUS r = gBS->CheckEvent(SimpleEx->WaitForKeyEx);
			if (Status == EFI_NOT_READY)
			    continue;
			  
			//DEBUG ((EFI_D_INFO, "gBS->CheckEvent: %x ...\n", EFI_ERROR(r)));
			gBS->WaitForEvent(1, &(SimpleEx->WaitForKeyEx), &Index);

			Status = SimpleEx->ReadKeyStrokeEx(SimpleEx, &KeyData);
			if(!EFI_ERROR(Status))
			{    
			    scanCode    = KeyData.Key.ScanCode;
			    uniChar     = KeyData.Key.UnicodeChar;
			    shiftState  = KeyData.KeyState.KeyShiftState;
			    toggleState  = KeyData.KeyState.KeyToggleState;
			}
		}    
    }  
	
	 DrawAsciiCharUseBuffer(GraphicsOutput, 20, 40, uniChar, Color);
}

 // iMouseX: left top
 // iMouseY: left top
 VOID HandleMouseRightClick(int iMouseX, int iMouseY)
 {
     INT16 i;    
     UINT8 *pBuffer = NULL;
     UINT16 width = 100;
     UINT16 height = 300;
     EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
          
     pBuffer = (UINT8 *)AllocatePool(width * height * 4); 
     if (pBuffer == NULL)
     {
        return;
     }
 
     for (i = 0; i < width * height; i++)
     {
         pBuffer[i * 4] = 160;
         pBuffer[i * 4 + 1] = 160;
         pBuffer[i * 4 + 2] = 160;
     }

     Color.Blue = 0xFF;
     Color.Red  = 0xFF;
     Color.Green= 0xFF;

     DrawChineseCharUseBuffer(pBuffer, 10, 10, sChinese[0], 5, Color, width);
            
     GraphicsOutput->Blt(GraphicsOutput, 
                         (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) pBuffer,
                         EfiBltBufferToVideo,
                         0, 0, 
                         iMouseX, iMouseY, 
                         width, height, 0);  
     FreePool(pBuffer);
     return ;
 
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
	GraphicsCopy(pDeskDisplayBuffer, DeskBuffer, ScreenWidth, ScreenHeight, ScreenWidth, ScreenHeight, 0, 0);
	GraphicsCopy(pDeskDisplayBuffer, pMyComputerBuffer, ScreenWidth, ScreenHeight, MyComputerWidth, MyComputerHeight, 100, 100);

	int i, j;
	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < 16; j++)
		{	
			MouseBuffer[(i * 16 + j) * 4]     = pDeskDisplayBuffer[((iMouseY + i) * ScreenWidth +  iMouseX + j) * 4];
			MouseBuffer[(i * 16 + j) * 4 + 1] = pDeskDisplayBuffer[((iMouseY + i) * ScreenWidth +  iMouseX + j) * 4 + 1];
			MouseBuffer[(i * 16 + j) * 4 + 2] = pDeskDisplayBuffer[((iMouseY + i) * ScreenWidth +  iMouseX + j) * 4 + 2];			
		}
	}

	DrawChineseCharIntoBuffer2(MouseBuffer, 0, 0, 11 * 94 + 42, MouseColor, 16);
	
	GraphicsCopy(pDeskDisplayBuffer, MouseBuffer, ScreenWidth, ScreenHeight, 16, 16, iMouseX, iMouseY);

    GraphicsOutput->Blt(GraphicsOutput, 
			            (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) pDeskDisplayBuffer,
			            EfiBltBufferToVideo,
			            0, 0, 
			            0, 0, 
			            ScreenWidth, ScreenHeight, 0);   

/*
    GraphicsOutput->Blt(GraphicsOutput, 
			            (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) pMyComputerBuffer,
			            EfiBltBufferToVideo,
			            0, 0,
			            100, 0,
			            MyComputerWidth, MyComputerHeight, 
			            0);
	           
    GraphicsOutput->Blt(GraphicsOutput, 
			            (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) MouseBuffer,
			            EfiBltBufferToVideo,
			            0, 0, 
			            iMouseX, iMouseY, 
			            16, 16, 0);  */ 
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
    //EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;

	//Color.Blue = 0xFF;
	//Color.Red = 0xFF;
	//Color.Green = 0xFF;
	
    static int iMouseX = 500 / 2;
    static int iMouseY = 600 / 2;

    EFI_STATUS Status;
	UINTN Index;
	EFI_SIMPLE_POINTER_STATE State;

	EFI_STATUS r = gBS->CheckEvent(gMouse->WaitForInput);
	//DEBUG ((EFI_D_INFO, "BS->CheckEvent(gMouse: %X\n", r));
	if (Status == EFI_NOT_READY)
		return;
		
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
    
    // cover old mouse cursor
	//DrawAsciiCharUseBuffer(GraphicsOutput, iMouseX, iMouseY, 0, Color);

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
    //DebugPrint1(30, 70, "X: %X, Y: %X ", x_move, y_move );
    
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

	gRT->GetTime(&et, NULL);

	DebugPrint1(ScreenWidth - 20 * 8, ScreenHeight - 21,"%04d-%02d-%02d %02d:%02d:%02d", et.Year, et.Month, et.Day, et.Hour, et.Minute, et.Second);
}

EFI_STATUS MultiProcessInit ()
{
    UINT16 i;
	EFI_GUID gMultiProcessGuid  = { 0x0579257E, 0x1843, 0x45FB, { 0x83, 0x9D, 0x6B, 0x79, 0x09, 0x38, 0x29, 0xA9 } };
    MouseBuffer = (UINT8 *)AllocateZeroPool(16 * 16 * 4);
	
	MouseColor.Blue  = 0xff;
    MouseColor.Red   = 0xff;
    MouseColor.Green = 0xff;

	//DrawChineseCharIntoBuffer2(MouseBuffer, 0, 0, 11 * 94 + 42, Color, 16);
	
    //DrawChineseCharIntoBuffer(MouseBuffer, 0, 0, 0, Color, 16);
    
    EFI_EVENT_NOTIFY       TaskProcesses[] = {DisplaySystemDateTime, HandleKeyboardEvent, HandleMouseEvent};

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
  
    DeskBuffer = (UINT8 *)AllocatePool(ScreenWidth * ScreenHeight * 4); 
    if (NULL == DeskBuffer)
    	DEBUG ((EFI_D_INFO, "ScreenInit AllocatePool DeskBuffer NULL\n"));

    
    pDeskDisplayBuffer = (UINT8 *)AllocatePool(ScreenWidth * ScreenHeight * 4); 
    if (NULL == pDeskDisplayBuffer)
    	DEBUG ((EFI_D_INFO, "ScreenInit AllocatePool pDeskDisplayBuffer NULL\n"));
              
    Color.Red   = 0x00;
    Color.Green = 0x84;
    Color.Blue	= 0x84;
    RectangleFillIntoBuffer(DeskBuffer, 0,     0,      x -  1, y - 29, 1, Color);
    
    Color.Red   = 0xC6;
    Color.Green = 0xC6;
    Color.Blue	= 0xC6;
    RectangleFillIntoBuffer(DeskBuffer, 0,     y - 28, x -  1, y - 28, 1, Color);

    Color.Red   = 0xFF;
    Color.Green = 0xFF;
    Color.Blue	= 0xFF;
    RectangleFillIntoBuffer(DeskBuffer, 0,     y - 27, x -  1, y - 27, 1, Color);
    
    Color.Red   = 0xC6;
    Color.Green = 0xC6;
    Color.Blue	= 0xC6;
    RectangleFillIntoBuffer(DeskBuffer, 0,     y - 26, x -  1, y -  1, 1, Color);


  	
    Color.Red   = 0xFF;
    Color.Green = 0xFF;
    Color.Blue	= 0xFF;
    RectangleFillIntoBuffer(DeskBuffer, 3,     y - 24, 59,     y - 24, 1, Color);
    RectangleFillIntoBuffer(DeskBuffer, 2,     y - 24, 59,     y - 4, 1, Color);

    Color.Red   = 0x84;
    Color.Green = 0x84;
    Color.Blue	= 0x84;
    RectangleFillIntoBuffer(DeskBuffer, 3,     y -  4, 59,     y -  4, 1, Color);
    RectangleFillIntoBuffer(DeskBuffer, 59,     y - 23, 59,     y -  5, 1, Color);

    
    Color.Red   = 0x00;
    Color.Green = 0x00;
    Color.Blue	= 0x00;
    RectangleFillIntoBuffer(DeskBuffer, 2,     y -  3, 59,     y -  3, 1, Color);
    RectangleFillIntoBuffer(DeskBuffer, 60,    y - 24, 60,     y -  3, 1, Color);

    Color.Red   = 0x84;
    Color.Green = 0x84;
    Color.Blue	= 0x84;
    RectangleFillIntoBuffer(DeskBuffer, x - 47, y - 24, x -  4, y - 24, 1, Color);
    RectangleFillIntoBuffer(DeskBuffer, x - 47, y - 23, x - 47, y -  4, 1, Color);
    
    Color.Red   = 0xFF;
    Color.Green = 0xFF;
    Color.Blue	= 0xFF;
    RectangleFillIntoBuffer(DeskBuffer, x - 47,    y - 3, x - 4,     y - 3, 1, Color);
    RectangleFillIntoBuffer(DeskBuffer, x - 3,     y - 24, x - 3,     y - 3, 1, Color);
        
    Color.Red   = 0xFF;
    Color.Green = 0xFF;
    Color.Blue	= 0xFF;
    RectangleFillIntoBuffer(DeskBuffer, 0, 100, 100, 200, 1, Color);
    
    Color.Red   = 0x00;
    Color.Green = 0x00;
    Color.Blue	= 0xFF;
    RectangleFillIntoBuffer(DeskBuffer, 0, 200, 100, 400, 1, Color);

    
    Color.Red   = 0x00;
    Color.Green = 0xFF;
    Color.Blue	= 0x00;
    RectangleFillIntoBuffer(DeskBuffer, 0, 300, 100, 400, 1, Color);
    
    
    Color.Red   = 0xFF;
    Color.Green = 0x00;
    Color.Blue	= 0x00;
    RectangleFillIntoBuffer(DeskBuffer, 0, 400, 100, 500, 1, Color);
    
    Color.Red  = 0xFF;
    Color.Green = 0x00;
    Color.Blue	= 0xFF;

    DrawLineIntoBuffer(DeskBuffer, 0, 0, 100, 100, 2, Color, ScreenWidth);
    DrawLineIntoBuffer(DeskBuffer, 100, 0, 0, 100, 1, Color, ScreenWidth);

    Color.Red  = 0xFF;
    Color.Green = 0xFF;
    Color.Blue	= 0xFF;

    // Display "wo"    
    DrawChineseCharIntoBuffer(DeskBuffer, 20, 20 + 16, 0, Color, ScreenWidth);
    
    MyComputerWindow(100, 100);
	ChineseCharArrayInit();
	
    Color.Red   = 0x00;
    Color.Green = 0x00;
    Color.Blue	 = 0x00;
    DrawChineseCharIntoBuffer2(DeskBuffer,  16, ScreenHeight - 21,     (18 - 1) * 94 + 43 - 1, Color, ScreenWidth);
    DrawChineseCharIntoBuffer2(DeskBuffer,  16 * 2, ScreenHeight - 21, (21 - 1) * 94 + 05 - 1, Color, ScreenWidth);

/**/
    //Display ASCII Char
    //count = 60;
    //for (i = 40; i < 65 + 60; i++)
    //    DrawAsciiCharIntoBuffer(DeskBuffer, 20 + (i - 40) * 8, 20, i, Color);
	
    /*GraphicsOutput->Blt(
                GraphicsOutput, 
                (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) DeskBuffer,
                EfiBltBufferToVideo,
                0, 0, 
                0, 0, 
                ScreenWidth, ScreenHeight, 0);   
*/
	// Desk graphics layer, buffer can not free!!
    //FreePool(DeskBuffer);

    
    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
Main (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_HII_HANDLE                     HiiHandle;
    EFI_STATUS                         Status;
    
    //
    // Install customized fonts needed by Front Page
    //
    HiiHandle = ExportFonts ();
    ASSERT (HiiHandle != NULL);


    Status = gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (VOID **) &GraphicsOutput);        
    if (EFI_ERROR (Status)) 
    	{
        return EFI_UNSUPPORTED;
    }

    ScreenWidth  = GraphicsOutput->Mode->Info->HorizontalResolution;
    ScreenHeight = GraphicsOutput->Mode->Info->VerticalResolution;

    DEBUG(( EFI_D_INFO, "ScreenWidth:%d, ScreenHeight:%d\n\n", ScreenWidth, ScreenHeight));
    DebugPrint1(ScreenWidth - 20 * 8, ScreenHeight - 21, "ScreenWidth:%d, ScreenHeight:%d\n\n", ScreenWidth, ScreenHeight);



    PartitionRead();
    
    ScreenInit(GraphicsOutput);
        
	Status = MouseInit();    
	if (EFI_ERROR (Status)) 
    {
        return EFI_UNSUPPORTED;
    }
    
    MultiProcessInit();
    SystemTimeIntervalInit();
	
    return EFI_SUCCESS;
}

