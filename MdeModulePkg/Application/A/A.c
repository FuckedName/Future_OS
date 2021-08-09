/** @file
  FrontPage routines to handle the callbacks and browser calls

Copyright (c) 2004 - 2017, Intel Corporation. All rights reserved.<BR>
(C) Copyright 2018 Hewlett Packard Enterprise Development LP<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent
ToDo:

1. ReadFileFromPath; OK
2. Fix trigger event with mouse and keyboard; OK
3. NetWork connect baidu.com; ==> driver not found
4. File System Simple Management; ==> current can read and write, Sub directory read.
5. progcess; ==> current mouse move, keyboard input, read file they can similar to process, but the real process is very complex, for example: 
	a.register push and pop, 
	b.progress communicate, 
	c.progress priority,
	d.PCB
	e.semaphoe
	etc.
6. My Computer window(disk partition)  ==> finish partly
7. Setting window(select file, delete file, modify file) ==> 0%
8. Memory Simple Management ==> can get memory infomation, but it looks like something wrong.
9. Multi Windows, button click event. ==> 10%
10. Application. ==>10%
11. How to Automated Testing? ==>0%
12. Graphics run slowly. ===> need to fix the bug.
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

UINT16 date_time_count = 0;
UINT16 keyboard_count = 0;
UINT16 mouse_count = 0;
UINT16 parameter_count = 0;

#define INFO(...)   \
			do {   \
				  Print(L"[INFO  ]%a %a(Line %d): ", __FILE__,__FUNCTION__,__LINE__);  \
			     Print(__VA_ARGS__); \
			}while(0);


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
#define DISK_READ_BUFFER_Y (6 * 56)

#define DISK_MBR_X (0) 
#define DISK_MBR_Y (16 * 62)


//last Line
#define DISPLAY_DESK_DATE_TIME_X (ScreenWidth - 20 * 8) 
#define DISPLAY_DESK_DATE_TIME_Y (ScreenHeight - 21)


#define DISK_BUFFER_SIZE (512)
#define DISK_BLOCK_BUFFER_SIZE (512 * 8)
#define DISK_MFT_BUFFER_SIZE (512 * 2 * 15)
#define EXBYTE	4


#define E820_RAM		1
#define E820_RESERVED		2
#define E820_ACPI		3
#define E820_NVS		4
#define E820_UNUSABLE		5

#define LLONG_MIN  (((INT64) -9223372036854775807LL) - 1)
#define LLONG_MAX   ((INT64)0x7FFFFFFFFFFFFFFFULL)


UINT16 StatusErrorCount = 0;
// For exception returned status 
#define DISPLAY_ERROR_STATUS_X (ScreenWidth * 2 / 4) 
#define DISPLAY_ERROR_STATUS_Y (16 * (StatusErrorCount++ % 61) )


UINT16 DisplayCount = 0;
// For exception returned status 
#define DISPLAY_X (0) 
#define DISPLAY_Y (16 * (2 + DisplayCount++ % 52) )

UINTN NumHandles = 0;

CHAR8 x_move = 0;
CHAR8 y_move = 0;

UINT16 DebugPrintX = 0;
UINT16 DebugPrintY = 0; 

CHAR8    AsciiBuffer[0x100] = {0};


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

typedef enum
{
	GRAPHICS_LAYER_DESK = 0,
	GRAPHICS_LAYER_MY_COMPUTER
}GRAPHICS_LAYER_ID;


struct efi_info_self {
	UINT32 efi_loader_signature;
	UINT32 efi_systab;
	UINT32 efi_memdesc_size;
	UINT32 efi_memdesc_version;
	UINT32 efi_memmap;
	UINT32 efi_memmap_size;
	UINT32 efi_systab_hi;
	UINT32 efi_memmap_hi;
};

struct e820_entry_self {
	UINT64 addr;		/* start of memory segment */
	UINT64 size;		/* size of memory segment */
	UINT32 type;		/* type of memory segment */
};


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
}DEVICE_PARAMETER;

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

struct Value0x10
{
    UINT8 fileCreateTime[8];    // 文件创建时间
    UINT8 fileChangeTime[8];    // 文件修改时间
    UINT8 MFTChangeTime[8];     // MFT修改时间
    UINT8 fileLatVisited[8];    // 文件最后访问时间
    UINT8 tranAtrribute[4];     // 文件传统属性
    UINT8 otherInfo[28];        // 版本，所有者，配额，安全等等信息(详细略)
    UINT8 updataNum[8];         // 文件更新序列号
};

//------- 这个结构只是数据内容部分，不包括属性头 NTFSAttribute ---- //------- 由属性头的属性值偏移量确定属性值的起始位置 --- 1 
struct Value0x20
{
    UINT8 type[4];           // 类型
    UINT8 recordType[2];     // 记录类型
    UINT8 nameLen[2];        // 属性名长度
    UINT8 nameOffset;        // 属性名偏移
    UINT8 startVCN[8];       // 起始 VCN
    UINT8 baseRecordNum[8];  // 基本文件记录索引号
    UINT8 attributeId[2];    // 属性 id
   //---- 属性名(Unicode) 长度取决于 nameLen 的值 ---
 };
 
 struct Value0x30
{
     UINT8 parentFileNum[8];     // 父目录的文件参考号,前6B
     UINT8 createTime[8];        // 文件创建时间
     UINT8 changeTime[8];        // 文件修改时间
     UINT8 MFTchangeTime[8];     // MFT 修改时间
     UINT8 lastVisitTime[8];      // 最后一次访问时间
     UINT8 AllocSize[8];         // 文件分配大小
     UINT8 realSize[8];          // 实际大小
     UINT8 fileFlag[4];          // 文件标志，系统 隐藏 压缩等等
     UINT8 EAflags[4];            // EA扩展属性和重解析点
     UINT8 nameLength;           // 文件名长
     UINT8 nameSpace;            // 文件命名空间
    //----- Name (Unicode编码) 长度为 2 * nameLength ----
};

typedef struct
{
     UINT8 GObjectId[16];            // 全局对象ID 给文件的唯一ID号
     UINT8 GOriginalVolumeId[16];    // 全局原始卷ID 永不改变
     UINT8 GOriginalObjectId[16];    // 全局原始对象ID 派给本MFT记录的第一个对象ID
     UINT8 GDomain[16];              // 全局域ID (未使用)
}Value0x40;


// may be have some errors
#define N 127

// todo
 struct Value0x60
 {
     //---- 通用属性头 --
      UINT8 VolumeName[N];    //(Unicode) N 最大为 127 外加一个结束符'\0'
 };

struct Value0x70
{    //----- 通用属性头 ---
     UINT8 noUsed1[8];        // 00
     UINT8 mainVersion;       // 主版本号 1--winNT, 3--Win2000/XP
     UINT8 SecVersion;        // 次版本号 当主为3, 0--win2000, 1--WinXP/7
     UINT8 flag[2];           // 标志
     UINT8 noUsed2[4];        // 00
};
/* flag:
*  0x0001  坏区标志 下次重启时chkdsk/f进行修复
*  0x0002  调整日志文件大小
*  0x0004  更新装载
*  0x0008  装载到 NT4
*  0x0010  删除进行中的USN
*  0x0020  修复对象 Ids
*  0x8000  用 chkdsk 修正卷
*/

struct indexHeader
{
     UINT8 type[4];             //  属性类型
     UINT8 checkRule[4];        // 校对规则
     UINT8 allocSize[4];        // 索引项分配大小
     UINT8 CluPerIdx;           // 每索引记录的簇数
     UINT8 noUse01[3];          // 填充

     UINT8 firstIdxOffset[4];    // 第一个索引项偏移
     UINT8 idxTotalSize[4];      // 索引项总大小
     UINT8 indxAllocSize[4];     // 索引项分配大小
     UINT8 flag;                 // 标志
     UINT8 noUse02[3];
};


// 一般小目录在90属性中有常驻目录项，目录项的结构与INDX缓存中的目录项一样
struct indexItem
{
     UINT8 MFTnum[8];               // 文件的 MFT 记录号，前6B是MFT编号,用于定位此文件记录
     UINT8 ItemLen[2];              // 索引项大小
     UINT8 IndexIdentifier[2];      // 索引标志 R
     UINT8 isNode[2];                // 1---此索引包含一个子节点，0---此为最后一项
     UINT8 noUse03[2];               // 填充
     UINT8 FMFTnum[8];               // 父目录 MFT文件参考号 0x05表示根目录
     UINT8 CreateTime[8];            //文件创建时间
     UINT8 fileModifyTime[8];        //文件修改时间
     UINT8 MFTModifyTime[8];         //文件记录最后修改时间
     UINT8 LastVisitTime[8];         //文件最后访问时间
     UINT8 FileAllocSize[8];         //文件分配大小 (B)
     UINT8 FileRealSize[8];          //文件实际大小 (B)
     UINT8 FileIdentifier[8];        //文件标志
     UINT8 FileNameLen;              //文件名长度
     UINT8 FileNameSpace;            //文件名命名空间
	 //---- 0x52 ---
    //FileName;                     // 文件名 (末尾填满 8 字节)
     UINT8 nextItemVCN[8];           // 下一项的 VCN (有子节点才有)
};

struct indxHeader    // A0H外部缓存文件结构，最大长度一般为4K
{
     UINT8 mark[4];        //  标志 "INDX"
     UINT8 usnOffset[2];   // 更新序列偏移
     UINT8 usnSize[2];     // 更新序列数组大小S
     UINT8 LSN[8];         // 日志文件序列号
     UINT8 indxVCN[8];     // 本索引缓存在分配索引中的VCN
     UINT8 itemOffset[4];  // 第一项索引的偏移(从这里开始计算)
     UINT8 itemSize[4];    // 索引项实际大小(B)
     UINT8 itemAlloc[4];   // 索引项分配大小(B)(不包括头部) 略小于4K
     UINT8 isNode;         // 是结点置1，表示有子节点
     UINT8 noUse[3];
    // UINT8 USN[2S];      // 更新序列号和数组
};



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
enum DirFirstChar
{
	DirUnUsed            = 0x00,        /*本表项没有使用*/
	DirCharE5            = 0x05,        /*首字符为0xe5*/
	DirisSubDir          = 0x2e,        /*是一个子目录 .,..为父目录*/
	DirFileisDeleted     = 0xe5        /*文件已删除*/
};


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

UINT8 *FAT32_Table;

UINT8 *sChineseChar = NULL;
//UINT8 sChineseChar[267616];

UINT8 HZK16FileReadCount = 0;
static UINTN ScreenWidth, ScreenHeight;  
UINT16 MyComputerWidth = 16 * 15;
UINT16 MyComputerHeight = 16 * 30;
UINT16 MyComputerPositionX = 700;
UINT16 MyComputerPositionY = 160;
UINT16 MouseClickWindowWidth = 300;
UINT16 MouseClickWindowHeight = 400;


UINT64 sector_count = 0;
UINT32 FileBlockStart = 0;
UINT32 BlockSize = 0;
UINT32 FileLength = 0;
UINT32 PreviousBlockNumber = 0;


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
	
	UINT16 NextState; // maximize; minimize; other
	CHAR8 pTitle[50];
	BUTTON *pButtons;
} WINDOW;


// we need a FSM（Finite State Machine）to Read content of file

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
	READ_PATITION_EVENT = 0,
	READ_ROOT_PATH_EVENT,
	READ_FAT_TABLE_EVENT,
	READ_FILE_EVENT,
}EVENT;

typedef struct
{
	STATE	       CurrentState;
	EVENT	       event;
	STATE	       NextState;
	EFI_STATUS    (*pFunc)(); 
}STATE_TRANS;





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


// 小端模式
// byte转int  
UINT64 BytesToInt8(UINT8 *bytes)
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

UINT32 BytesToInt4(UINT8 *bytes)
{
    UINT32 Result = bytes[0] & 0xFF;
    Result |= (bytes[1] << 8 & 0xFF00);
    Result |= ((bytes[2] << 16) & 0xFF0000);
    Result |= ((bytes[3] << 24) & 0xFF000000);
    return Result;
}

UINT16 BytesToInt2(UINT8 *bytes)
{
    UINT16 Result = bytes[0] & 0xFF;
    Result |= (bytes[1] << 8 & 0xFF00);
    return Result;
}


void CopyColorIntoBuffer(UINT8 *pBuffer, EFI_GRAPHICS_OUTPUT_BLT_PIXEL color, UINT16 x0, UINT16 y0, UINT16 AreaWidth)
{
    pBuffer[y0 * AreaWidth * 4 + x0 * 4]     = color.Blue;
    pBuffer[y0 * AreaWidth * 4 + x0 * 4 + 1] = color.Green;
    pBuffer[y0 * AreaWidth * 4 + x0 * 4 + 2] = color.Red;
    pBuffer[y0 * AreaWidth * 4 + x0 * 4 + 3] = color.Reserved;

}

void CopyColorIntoBuffer2(UINT8 *pBuffer, EFI_GRAPHICS_OUTPUT_BLT_PIXEL color, UINT16 x0, UINT16 y0)
{
    pBuffer[y0 * 8 * 4 + x0 * 4]     = color.Blue;
    pBuffer[y0 * 8 * 4 + x0 * 4 + 1] = color.Green;
    pBuffer[y0 * 8 * 4 + x0 * 4 + 2] = color.Red;
    pBuffer[y0 * 8 * 4 + x0 * 4 + 3] = color.Reserved;
}

void CopyColorIntoBuffer3(UINT8 *pBuffer, EFI_GRAPHICS_OUTPUT_BLT_PIXEL color, UINT16 x0, UINT16 y0, UINT8 AreaWidth)
{
    pBuffer[y0 * AreaWidth * 4 + x0 * 4]     = color.Blue;
    pBuffer[y0 * AreaWidth * 4 + x0 * 4 + 1] = color.Green;
    pBuffer[y0 * AreaWidth * 4 + x0 * 4 + 2] = color.Red;
    pBuffer[y0 * AreaWidth * 4 + x0 * 4 + 3] = color.Reserved;
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
			pDest[((StartY + i) * DestWidth + StartX + j) * 4 + 3] = pSource[(i * SourceWidth + j) * 4 + 3];
		}
	}
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
        DrawAsciiCharUseBuffer(pDeskBuffer, x + i * 8, y, AsciiBuffer[i], Color);

}


EFI_STATUS DrawAsciiCharUseBuffer2(UINT8 *pBufferDest,
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

	GraphicsCopy(pBufferDest, pBuffer, MyComputerWidth, ScreenHeight, 8, 16, x0, y0);
	
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



VOID StringMaker2 (UINT16 x, UINT16 y, UINT8 *pBuffer,
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
	Color.Reserved = GRAPHICS_LAYER_MY_COMPUTER;

    ASSERT (Format != NULL);

    AsciiVSPrint (AsciiBuffer, sizeof (AsciiBuffer), Format, VaList);
			
    for (i = 0; i < sizeof(AsciiBuffer) /sizeof(CHAR8); i++)
        DrawAsciiCharUseBuffer2(pBuffer, x + i * 8, y, AsciiBuffer[i], Color);

}


/* Display a string */
VOID EFIAPI DebugPrint1 (UINT16 x, UINT16 y,  IN  CONST CHAR8  *Format, ...)
{
	if (y > ScreenHeight - 16 || x > ScreenWidth - 8)
		return;

	VA_LIST         VaList;
	VA_START (VaList, Format);
	StringMaker(x, y, Format, VaList);
	VA_END (VaList);
}

/* Display a string */
VOID EFIAPI DebugPrint2 (UINT16 x, UINT16 y, UINT8 *pBuffer, IN  CONST CHAR8  *Format, ...)
{
	if (y > MyComputerHeight - 16 || x > MyComputerWidth - 8)
		return;

	VA_LIST         VaList;
	VA_START (VaList, Format);
	StringMaker2(x, y, pBuffer, Format, VaList);
	VA_END (VaList);
}


EFI_STATUS StrCmpSelf(UINT8 *p1, UINT8 *p2, UINT16 length)
{
	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: \n", __LINE__);
	
	for (int i = 0; i < length; i++)
	{
		DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%02X %02X ", p1[i], p2[i]);
		if (p1[i] != p2[i])
			return -1;
	}
	return EFI_SUCCESS;
}


FAT32_ROOTPATH_SHORT_FILE_ITEM pItems[32];

EFI_STATUS RootPathAnalysis(UINT8 *p)
{
	/*pItems = (FAT32_ROOTPATH_SHORT_FILE_ITEM *)AllocateZeroPool(DISK_BUFFER_SIZE);
	if (NULL == pItems)
	{
		DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: \n", __LINE__);
		return EFI_SUCCESS;
	}
	*/
	memcpy(&pItems, p, DISK_BUFFER_SIZE);
	UINT16 valid_count = 0;

	//display filing file and subpath. 
	for (int i = 0; i < 30; i++)
		if (pItems[i].FileName[0] != 0xE5 && (pItems[i].Attribute[0] == 0x20 
		    || pItems[i].Attribute[0] == 0x10))
       {
        	DebugPrint1(DISK_MBR_X, 16 * 36 + (valid_count) * 16, "FileName:%2c%2c%2c%2c%2c%2c%2c%2c ExtensionName:%2c%2c%2c StartCluster:%02X%02X%02X%02X FileLength: %02X%02X%02X%02X Attribute: %02X    ", 
                                            pItems[i].FileName[0], pItems[i].FileName[1], pItems[i].FileName[2], pItems[i].FileName[3], pItems[i].FileName[4], pItems[i].FileName[5], pItems[i].FileName[6], pItems[i].FileName[7],
                                            pItems[i].ExtensionName[0], pItems[i].ExtensionName[1],pItems[i].ExtensionName[2],
                                            pItems[i].StartClusterHigh2B[0], pItems[i].StartClusterHigh2B[1],
                                            pItems[i].StartClusterLow2B[0], pItems[i].StartClusterLow2B[1],
                                            pItems[i].FileLength[0], pItems[i].FileLength[1], pItems[i].FileLength[2], pItems[i].FileLength[3],
                                            pItems[i].Attribute[0]);
			
			valid_count++;

			//for (int j = 0; j < 5; j++)
			//	DebugPrint1(j * 3 * 8, 16 * 40 + valid_count * 16, "%02X ", pItems[i].FileName[j]);
			if (StrCmpSelf(pItems[i].FileName, "HZK16", 5) == EFI_SUCCESS)			
			{
	        	/*DebugPrint1(DISK_MBR_X, 16 * 30 + (valid_count) * 16 + 4 * 16, "FileName:%2c%2c%2c%2c%2c%2c%2c%2c ExtensionName:%2c%2c%2c StartCluster:%02X%02X%02X%02X FileLength: %02X%02X%02X%02X Attribute: %02X    ", 
                                pItems[i].FileName[0], pItems[i].FileName[1], pItems[i].FileName[2], pItems[i].FileName[3], pItems[i].FileName[4], pItems[i].FileName[5], pItems[i].FileName[6], pItems[i].FileName[7],
                                pItems[i].ExtensionName[0], pItems[i].ExtensionName[1],pItems[i].ExtensionName[2],
                                pItems[i].StartClusterHigh2B[0], pItems[i].StartClusterHigh2B[1],
                                pItems[i].StartClusterLow2B[0], pItems[i].StartClusterLow2B[1],
                                pItems[i].FileLength[0], pItems[i].FileLength[1], pItems[i].FileLength[2], pItems[i].FileLength[3],
                                pItems[i].Attribute[0]);
				*/
              FileBlockStart = (UINT32)pItems[i].StartClusterHigh2B[0] * 16 * 16 * 16 * 16 + (UINT32)pItems[i].StartClusterHigh2B[1] * 16 * 16 * 16 * 16 * 16 * 16 + pItems[i].StartClusterLow2B[0] + (UINT32)pItems[i].StartClusterLow2B[1] * 16 * 16;
              FileLength = pItems[i].FileLength[0] + (UINT32)pItems[i].FileLength[1] * 16 * 16 + (UINT32)pItems[i].FileLength[2] * 16 * 16 * 16 * 16 + (UINT32)pItems[i].FileLength[3] * 16 * 16 * 16 * 16 * 16 * 16;

              DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: FileBlockStart: %d FileLength: %ld\n", __LINE__, FileBlockStart, FileLength);
			}
		}

}

EFI_STATUS RootPathAnalysis1(UINT8 *p)
{
	memcpy(&pItems, p, DISK_BUFFER_SIZE);
	UINT16 valid_count = 0;

	//display filing file and subpath. 
	for (int i = 0; i < 30; i++)
		if (pItems[i].FileName[0] != 0xE5 && (pItems[i].Attribute[0] == 0x20 
		    || pItems[i].Attribute[0] == 0x10))
       {
        	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "FileName:%2c%2c%2c%2c%2c%2c%2c%2c ExtensionName:%2c%2c%2c StartCluster:%02X%02X%02X%02X FileLength: %02X%02X%02X%02X Attribute: %02X    ", 
                                            pItems[i].FileName[0], pItems[i].FileName[1], pItems[i].FileName[2], pItems[i].FileName[3], pItems[i].FileName[4], pItems[i].FileName[5], pItems[i].FileName[6], pItems[i].FileName[7],
                                            pItems[i].ExtensionName[0], pItems[i].ExtensionName[1],pItems[i].ExtensionName[2],
                                            pItems[i].StartClusterHigh2B[0], pItems[i].StartClusterHigh2B[1],
                                            pItems[i].StartClusterLow2B[0], pItems[i].StartClusterLow2B[1],
                                            pItems[i].FileLength[0], pItems[i].FileLength[1], pItems[i].FileLength[2], pItems[i].FileLength[3],
                                            pItems[i].Attribute[0]);
			
			valid_count++;
		}
}


MasterBootRecordSwitched MBRSwitched;
DollarBootSwitched NTFSBootSwitched;

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

int display_sector_number = 0;
typedef struct 
{
	UINT8 OccupyCluster;
	UINT64 Offset; //Please Note: The first item is start offset, and next item is relative offset....
}IndexInformation;

UINT16 BytesToInt3(UINT8 *bytes)
{
	//INFO("%x %x %x\n", bytes[0], bytes[1], bytes[2]);
	UINT16 Result = bytes[0] & 0xFF;
    Result |= (bytes[1] << 8 & 0xFF00);
    Result |= ((bytes[2] << 16) & 0xFF0000);
	return Result;
}

UINTN strlen1(char *String)
{
    UINTN  Length;
    for (Length = 0; *String != '\0'; String++, Length++) ;
//	 DEBUG ((EFI_D_INFO, "%d Length: %d\n", __LINE__, Length));
    return Length;
}


// Index of A0 attribute
IndexInformation A0Indexes[10] = {0};

// Analysis attribut A0 of $Root
EFI_STATUS  DollarRootA0DatarunAnalysis(UINT8 *p)
{
    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: string length: %d\n", __LINE__,  strlen1(p));
    UINT16 i = 0;
    UINT16 length = strlen1(p);
    UINT8 occupyCluster = 0;
    UINT16 offset = 0;

    UINT16 Index = 0;
    while(i < length)
    {
        int  offsetLength  = p[i] >> 4;
        int  occupyClusterLength = p[i] & 0x0f;
    	 DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: occupyClusterLength: %d offsetLength: %d\n", __LINE__,  occupyClusterLength, offsetLength);
        
        i++;
        if (occupyClusterLength == 1)
            A0Indexes[Index].OccupyCluster = p[i];
        i++;
        //INFO(" i: %d\n", i);
        if (offsetLength == 1)
            offset = p[i];
        else if (offsetLength == 3)
        {
            UINT8 size[3];
            size[0] = p[i];
            size[1] = p[i + 1];
            size[2] = p[i + 2];
            offset = BytesToInt3(size);
        }
        A0Indexes[Index].Offset = offset;
        DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d offset:%d \n", __LINE__, offset);
        i += offsetLength;
        Index++;
        //INFO(" i: %d\n", i);
    }

}

// Find $Root file from all MFT(may be 15 file,)
// pBuffer store all MFT
EFI_STATUS  MFTDollarRootFileAnalysisBuffer(UINT8 *pBuffer)
{
	UINT8 *p = NULL;
	UINT8 pItem[200] = {0};
	
	p = (UINT8 *)AllocateZeroPool(DISK_BUFFER_SIZE * 2);
	if (p == NULL)
	{
		DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: p == NULL \n", __LINE__);
		return EFI_SUCCESS;
	}

	// Root file buffer copy
//	memcpy(p, pBuffer[512 * 2 * 5], DISK_BUFFER_SIZE * 2);

	for (int i = 0; i < DISK_BUFFER_SIZE * 2; i++)
		p[i] = pBuffer[i];

	//for (int i = 0; i < 7; i++)
	//{
		for (int j = 0; j < 512; j++)
	    {
		   //%02X: 8 * 3, 
	      //DebugPrint1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", p[j] & 0xff);
	    }
	//}
	
	// File header length
	UINT16 AttributeOffset = BytesToInt2(((FILE_HEADER *)p)->AttributeOffset);
	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: AttributeOffset:%X \n", __LINE__, AttributeOffset);

	for (int i = 0; i < 10; i++)
	{
		UINT8 size[4];
		for (int i = 0; i < 4; i++)
			size[i] = p[AttributeOffset + 4 + i];
		//INFO("%02X%02X%02X%02X\n", size[0], size[1], size[2], size[3]);
		UINT16 AttributeSize = BytesToInt4(size);
		
		for (int i = 0; i < AttributeSize; i++)
			pItem[i] = p[AttributeOffset + i];
		
		UINT16  NameSize = ((CommonAttributeHeader *)pItem)->NameSize;		
		
		UINT16  NameOffset = BytesToInt2(((CommonAttributeHeader *)pItem)->NameOffset);
		DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Type[0]: %02X AttributeSize: %02X NameSize: %02X NameOffset: %02X\n", __LINE__, 
															((CommonAttributeHeader *)pItem)->Type[0],
															AttributeSize,
															NameSize,
															NameOffset);   
							
		 if (0xA0 == ((CommonAttributeHeader *)pItem)->Type[0])
		 {
		 	UINT16 DataRunsSize = AttributeSize - NameOffset - NameSize * 2;
		 	if (DataRunsSize > 100 || DataRunsSize < 0)
		 	{
				DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: DataRunsSize illegal.", __LINE__);
				return ;
		 	}
		 	UINT8 DataRuns[20] = {0};
		 	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: A0 attribute has been found: ", __LINE__);
		 	for (int i = NameOffset; i < NameOffset + NameSize * 2; i++)
		 		DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%02X ", pItem[i] & 0xff);

		 	int j = 0;
		 	for (int i = NameOffset + NameSize * 2; i < AttributeSize; i++)
		 	{	 		
		 		DataRuns[j] = pItem[i] & 0xff;
		 		DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%02X ", DataRuns[j] & 0xff);	
		 		j++;
		 	}
		 	DollarRootA0DatarunAnalysis(DataRuns);
		 	break;
		 }
		 
		AttributeOffset +=AttributeSize;
	}
}


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

typedef struct {
     UINT8 MFTReferNumber[8];//文件的MFT参考号
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


EFI_STATUS  MFTIndexItemsBufferAnalysis(UINT8 *pBuffer)
{
	UINT8 *p = NULL;
	
	p = (UINT8 *)AllocateZeroPool(DISK_BUFFER_SIZE * 2);

	if (NULL == p)
	{
		DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d NULL == p\n", __LINE__);
		return EFI_SUCCESS;
	}
	
	//memcpy(p, pBuffer[512 * 2], DISK_BUFFER_SIZE);
	for (UINT16 i = 0; i < 512 * 2; i++)
		p[i] = pBuffer[i];

	//IndexEntryOffset:索引项的偏移 相对于当前位置
	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d IndexEntryOffset: %llu IndexEntrySize: %llu\n", __LINE__, 
																     BytesToInt4(((INDEX_HEADER *)p)->IndexEntryOffset),
																     BytesToInt4(((INDEX_HEADER *)p)->IndexEntrySize));

	// 相对于当前位置 need to add size before this Byte.
	UINT8 length = BytesToInt4(((INDEX_HEADER *)p)->IndexEntryOffset) + 24;
    UINT8 pItem[200] = {0};
    UINT16 index = length;

	for (UINT8 i = 0; i < 4; i++)
	{        
		 DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%s index: %d\n", __LINE__,  index);  
		 UINT16 length2 = pBuffer[index + 8] + pBuffer[index + 9] * 16;
		 
        for (int i = 0; i < length2; i++)
            pItem[i] = pBuffer[index + i];
            
		 UINT8 FileNameSize =	 ((INDEX_ITEM *)pItem)->FileNameSize;
		 DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d attribut length2: %d FileNameSize: %d\n", __LINE__, 
																     length2,
																     FileNameSize);    
		 CHAR16 attributeName[20];																     
		 for (int i = 0; i < FileNameSize * 2; i++)
		 {
		 	attributeName[i] = pItem[82 + i];
			DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: attributeName: %c\n", __LINE__, attributeName[i]);
		 }
		 //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%s attributeName: %a\n", __LINE__,  attributeName);  
		 index += length2;
	}
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
    DebugPrint1(TEXT_DEVICE_PATH_X, TEXT_DEVICE_PATH_Y + 16 * (4 * count1 + 1), "%d: i: %d Start: %d Count: %d  DeviceType: %d PartitionType: %d PartitionID: %d\n",        
    							__LINE__, 
    							count1,
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


// analysis a partition 
EFI_STATUS RootPathAnalysisFSM1(UINT16 DeviceType, long long SectorCount)
{
    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d DeviceType: %d, SectorCount: %lld\n", __LINE__, DeviceType, SectorCount);
    //printf( "RootPathAnalysis\n" );
    DEBUG ((EFI_D_INFO, "PartitionUSBRead!!\r\n"));
    EFI_STATUS Status ;
    UINTN NumHandles, i;
    EFI_HANDLE *ControllerHandle = NULL;
    EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;
    EFI_BLOCK_IO_PROTOCOL           *BlockIo;
    UINT8 Buffer1[DISK_BUFFER_SIZE];
    UINT8 BufferBlock[DISK_BLOCK_BUFFER_SIZE];
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
    	DebugPrint1(350, 16 * 6, "%d: %x\n", __LINE__, Status);
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
    	 //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: %s\n", __LINE__, TextDevicePath);
        //DEBUG ((EFI_D_INFO, "%s\n", TextDevicePath));

		 TextDevicePathAnalysisCHAR16(TextDevicePath, &device[i], i);
    	            
        if (TextDevicePath) gBS->FreePool(TextDevicePath);

		 // the USB we save our *.efi file and relative resource files..
		 if (device[i].DeviceType == DeviceType && device[i].SectorCount == SectorCount)
		 {
		    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		 	Status = gBS->HandleProtocol(ControllerHandle[i], &gEfiBlockIoProtocolGuid, (VOID * *) &BlockIo );                                                
		    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		    
		    if ( EFI_SUCCESS == Status )
		    {			        	 
		        Status = gBS->HandleProtocol( ControllerHandle[i], &gEfiDiskIoProtocolGuid, (VOID * *) &DiskIo ); 
		        //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		        
		        if ( Status == EFI_SUCCESS )
		        {
					 if (device[i].SectorCount <= sector_count)
					 {
						  DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: device[i].SectorCount <= sector_count \n", __LINE__);
						  return EFI_SUCCESS;
					 }

	        	    // Read FAT32 file system partition infomation , minimum unit is sector.
	        	 	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X sector_count:%ld\n", __LINE__, Status, sector_count);
	        	 	Status = DiskIo->ReadDisk( DiskIo, BlockIo->Media->MediaId, DISK_BUFFER_SIZE * sector_count, DISK_BUFFER_SIZE, Buffer1 );
		            if ( EFI_SUCCESS == Status )
		            {
						  for (int j = 0; j < 250; j++)
						  {
						  		DebugPrint1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", Buffer1[j] & 0xff);
						  }
				     }
		        	 
				 	//When get root path data sector start number, we can get content of root path.
				 	RootPathAnalysis1(Buffer1);	

					// data area start from 1824, HZK16 file start from 	FileBlockStart	block, so need to convert into sector by multi 8, block start number is 2 	
					// next state is to read FAT table
				 	sector_count = MBRSwitched.ReservedSelector;
				 	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: sector_count:%ld FileLength: %d MBRSwitched.ReservedSelector:%ld\n",  __LINE__, sector_count, FileLength, MBRSwitched.ReservedSelector);
		        }       
		    }

		    return EFI_SUCCESS;
		 } 		 
    }
    return EFI_SUCCESS;
}

UINT8 BufferMFT[DISK_BUFFER_SIZE * 2];

// NTFS Main File Table items analysis
EFI_STATUS MFTReadFromPartition(UINT16 DeviceType, long long SectorCount)
{
    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d DeviceType: %d, SectorCount: %lld\n", __LINE__, DeviceType, SectorCount);
    //printf( "RootPathAnalysis\n" );
    DEBUG ((EFI_D_INFO, "PartitionUSBRead!!\r\n"));
    EFI_STATUS Status ;
    UINTN NumHandles, i;
    EFI_HANDLE *ControllerHandle = NULL;
    EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;
    EFI_BLOCK_IO_PROTOCOL           *BlockIo;
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

	for (i = 8; i < 9; i++)

    //for (i = 0; i < NumHandles; i++)
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
    	 //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: %s\n", __LINE__, TextDevicePath);
        //DEBUG ((EFI_D_INFO, "%s\n", TextDevicePath));

		 TextDevicePathAnalysisCHAR16(TextDevicePath, &device[i], i);
    	            
        if (TextDevicePath) gBS->FreePool(TextDevicePath);

		 // the USB we save our *.efi file and relative resource files..
		 //if (device[i].DeviceType == DeviceType && device[i].SectorCount == SectorCount)
		 //if (device[i].DeviceType == DeviceType && device[i].SectorCount == device[4].SectorCount)
		 {
		    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		 	Status = gBS->HandleProtocol(ControllerHandle[i], &gEfiBlockIoProtocolGuid, (VOID * *) &BlockIo );                                                
		    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		    
		    if ( EFI_SUCCESS == Status )
		    {			        	 
		        Status = gBS->HandleProtocol( ControllerHandle[i], &gEfiDiskIoProtocolGuid, (VOID * *) &DiskIo ); 
		        //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		        
		        if ( Status == EFI_SUCCESS )
		        {
					 if (device[i].SectorCount <= sector_count)
					 {
						  DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: device[i].SectorCount <= sector_count: %ld device[i].SectorCount: %ld\n", __LINE__, sector_count, device[i].SectorCount);
						  continue;
					 }

					//sector_count = 6858752;
	        	    // Read FAT32 file system partition infomation , minimum unit is sector.
	        	 	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X sector_count:%ld\n", __LINE__, Status, sector_count);

	        	 	// sector_count: MFT start , and plus 5 * 2: $Root table, we need the A0 attribute to analysis items of root path.
	        	 	Status = DiskIo->ReadDisk( DiskIo, BlockIo->Media->MediaId, DISK_BUFFER_SIZE * (sector_count + 5 *2), DISK_BUFFER_SIZE * 2, BufferMFT);
	        	 	if (EFI_ERROR(Status))
	        	 	{
						DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X sector_count:%ld\n", __LINE__, Status, sector_count);
						return Status;
	        	 	}
					for (int j = 0; j < 250; j++)
					{
						DebugPrint1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", BufferMFT[j] & 0xff);
					}

				 	//Analysis MFT of NTFS File System..
				 	MFTDollarRootFileAnalysisBuffer(BufferMFT);
				 	//MFTDollarRootAnalysisBuffer(BufferMFTDollarRoot);	

					// data area start from 1824, HZK16 file start from 	FileBlockStart	block, so need to convert into sector by multi 8, block start number is 2 	
					// next state is to read FAT table
				 	sector_count = MBRSwitched.ReservedSelector;
				 	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: sector_count:%ld FileLength: %d MBRSwitched.ReservedSelector:%ld\n",  __LINE__, sector_count, FileLength, MBRSwitched.ReservedSelector);
		        }       
		    }

		    return EFI_SUCCESS;
		 } 		 
    }
    return EFI_SUCCESS;
}

//Note: ReadSize will be multi with 512
EFI_STATUS ReadDataFromPartition(UINT8 deviceID, UINT64 StartSectorNumber, UINT16 ReadSize, UINT8 *pBuffer)
{
	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: deviceID: %d StartSectorNumber: %ld ReadSize: %d\n", __LINE__, deviceID, StartSectorNumber, ReadSize);
	EFI_STATUS Status;
    UINTN NumHandles;
    EFI_BLOCK_IO_PROTOCOL *BlockIo;
    EFI_DISK_IO_PROTOCOL *DiskIo;
    EFI_HANDLE *ControllerHandle = NULL;
    
    Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiDiskIoProtocolGuid, NULL, &NumHandles, &ControllerHandle);
    if (EFI_ERROR(Status))
    {
        DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);	    
        return EFI_SUCCESS;
    }

 	Status = gBS->HandleProtocol(ControllerHandle[deviceID], &gEfiBlockIoProtocolGuid, (VOID * *) &BlockIo );   
    if (EFI_ERROR(Status))
    {
		DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		return EFI_SUCCESS;
    }
    
    Status = gBS->HandleProtocol( ControllerHandle[deviceID], &gEfiDiskIoProtocolGuid, (VOID * *) &DiskIo );     
    if (EFI_ERROR(Status))
    {
		DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		return EFI_SUCCESS;
    }

   	Status = DiskIo->ReadDisk(DiskIo, BlockIo->Media->MediaId, DISK_BUFFER_SIZE * (StartSectorNumber), DISK_BUFFER_SIZE * ReadSize, pBuffer);
 	if (EFI_ERROR(Status))
 	{
		DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X sector_count:%ld\n", __LINE__, Status, sector_count);
		return Status;
 	}
    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
    

}

EFI_STATUS NTFSRootPathIndexItemsRead(UINT8 i)
{
	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d DeviceType: %d, SectorCount: %lld\n", __LINE__, i);
    EFI_STATUS Status ;

	EFI_BLOCK_IO_PROTOCOL *BlockIo = NULL;
	EFI_DISK_IO_PROTOCOL *DiskIo = NULL;

	// cluster need to multi with 8 then it is sector.
   ReadDataFromPartition(i, A0Indexes[0].Offset * 8 , A0Indexes[0].OccupyCluster * 8, BufferMFT);

	for (int j = 0; j < 250; j++)
	{
		DebugPrint1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", BufferMFT[j] & 0xff);
	}

	MFTIndexItemsBufferAnalysis(BufferMFT);	
 	//sector_count = MBRSwitched.ReservedSelector;
 	//DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: sector_count:%ld FileLength: %d MBRSwitched.ReservedSelector:%ld\n",  __LINE__, sector_count, FileLength, MBRSwitched.ReservedSelector);

    return EFI_SUCCESS;
}

EFI_STATUS BufferAnalysis(UINT8 *p, MasterBootRecordSwitched *pMBRSwitched)
{
	MasterBootRecord *pMBR;
	
	pMBR = (MasterBootRecord *)AllocateZeroPool(DISK_BUFFER_SIZE);
	memcpy(pMBR, p, DISK_BUFFER_SIZE);

	// 大端字节序：低位字节在高地址，高位字节低地址上。这是人类读写数值的方法。
    // 小端字节序：与上面相反。低位字节在低地址，高位字节在高地址。
	/*DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "ReservedSelector:%02X%02X SectorsPerFat:%02X%02X%02X%02X BootPathStartCluster:%02X%02X%02X%02X NumFATS: %X", 
	                                    pMBR->ReservedSelector[0], pMBR->ReservedSelector[1], 
	                                    pMBR->SectorsPerFat[0], pMBR->SectorsPerFat[1], pMBR->SectorsPerFat[2], pMBR->SectorsPerFat[3],
	                                    pMBR->BootPathStartCluster[0], pMBR->BootPathStartCluster[1], pMBR->BootPathStartCluster[2], pMBR->BootPathStartCluster[3],
	                                    pMBR->NumFATS[0]);
	*/
	Transfer(pMBR, pMBRSwitched);

	FreePool(pMBR);
}

EFI_STATUS FirstSelectorAnalysis(UINT8 *p, MasterBootRecordSwitched *pMBRSwitched)
{
	MasterBootRecord *pMBR;
	
	pMBR = (MasterBootRecord *)AllocateZeroPool(DISK_BUFFER_SIZE);
	memcpy(pMBR, p, DISK_BUFFER_SIZE);

	// 大端字节序：低位字节在高地址，高位字节低地址上。这是人类读写数值的方法。
    // 小端字节序：与上面相反。低位字节在低地址，高位字节在高地址。
	/*DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "ReservedSelector:%02X%02X SectorsPerFat:%02X%02X%02X%02X BootPathStartCluster:%02X%02X%02X%02X NumFATS: %X", 
	                                    pMBR->ReservedSelector[0], pMBR->ReservedSelector[1], 
	                                    pMBR->SectorsPerFat[0], pMBR->SectorsPerFat[1], pMBR->SectorsPerFat[2], pMBR->SectorsPerFat[3],
	                                    pMBR->BootPathStartCluster[0], pMBR->BootPathStartCluster[1], pMBR->BootPathStartCluster[2], pMBR->BootPathStartCluster[3],
	                                    pMBR->NumFATS[0]);

	*/
	DOLLAR_BOOT *pDollarBoot;
	Transfer(pMBR, pMBRSwitched);

	FreePool(pMBR);
}

VOID TransferNTFS(DOLLAR_BOOT *pSource, DollarBootSwitched *pDest)
{
    pDest->BitsOfSector = BytesToInt2(pSource->BitsOfSector);
    pDest->SectorOfCluster = (UINT16)pSource->SectorOfCluster;
    pDest->AllSectorCount = BytesToInt8(pSource->AllSectorCount) + 1;
    pDest->MFT_StartCluster = BytesToInt8(pSource->MFT_StartCluster);
    pDest->MFT_MirrStartCluster = BytesToInt8(pSource->MFT_MirrStartCluster);
    
	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d BitsOfSector:%ld SectorOfCluster:%d AllSectorCount: %llu MFT_StartCluster:%llu MFT_MirrStartCluster:%llu", __LINE__,
											    pDest->BitsOfSector,
											    pDest->SectorOfCluster,
											    pDest->AllSectorCount,
											    pDest->MFT_StartCluster,
											    pDest->MFT_MirrStartCluster);
}


EFI_STATUS FirstSelectorAnalysisNTFS(UINT8 *p, DollarBootSwitched *pNTFSBootSwitched)
{
    DOLLAR_BOOT *pDollarBoot;
    
    pDollarBoot = (DOLLAR_BOOT *)AllocateZeroPool(DISK_BUFFER_SIZE);
    memcpy(pDollarBoot, p, DISK_BUFFER_SIZE);

    // 大端字节序：低位字节在高地址，高位字节低地址上。这是人类读写数值的方法。
    // 小端字节序：与上面相反。低位字节在低地址，高位字节在高地址。
    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d OEM:%c%c%c%c%c%c%c%c, BitsOfSector:%02X%02X SectorOfCluster:%02X ReservedSelector:%02X%02X Description: %X, size: %d", 
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
                                        						  
    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d AllSectorCount:%02X%02X%02X%02X%02X%02X%02X%02X, MFT_StartCluster:%02X%02X%02X%02X%02X%02X%02X%02X MFT_MirrStartCluster:%02X%02X%02X%02X%02X%02X%02X%02X ClusterPerMFT:%02X%02X%02X%02X ClusterPerIndex: %02X%02X%02X%02X", 
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

    TransferNTFS(pDollarBoot, pNTFSBootSwitched);

    FreePool(pDollarBoot);

}


// all partitions analysis
EFI_STATUS PartitionAnalysisFSM1(UINT16 DeviceType, long long SectorCount)
{    
    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d DeviceType: %d, SectorCount: %lld\n", __LINE__, DeviceType, SectorCount);
    DEBUG ((EFI_D_INFO, "PartitionUSBRead!!\r\n"));
    EFI_STATUS Status ;
    UINTN NumHandles, i;
    EFI_HANDLE *ControllerHandle = NULL;
    EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;
    EFI_BLOCK_IO_PROTOCOL           *BlockIo;
    UINT8 Buffer1[DISK_BUFFER_SIZE] = {0};
    UINT8 BufferBlock[DISK_BLOCK_BUFFER_SIZE] = {0};
    EFI_DISK_IO_PROTOCOL            *DiskIo;

    sector_count = 0;
    
    Status = gBS->LocateProtocol (&gEfiDevicePathToTextProtocolGuid, NULL, (VOID **) &DevPathToText);
    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
    if (EFI_ERROR(Status))
    {
    	 DEBUG ((EFI_D_INFO, "LocateProtocol1 error: %x\n", Status));    	 
    	 DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
        return Status;
    }
   
    Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiDiskIoProtocolGuid, NULL, &NumHandles, &ControllerHandle);
    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
    if (EFI_ERROR(Status))
    {
        DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
	    
        return EFI_SUCCESS;
    }

    DEBUG ((EFI_D_INFO, "Before for\n", Status));
    //DebugPrint1(350, 16 * 5, "%d: %x\n", __LINE__, Status);

	for (i = 8; i < 9; i++)
    //for (i = 0; i < NumHandles; i++)

    {
    	//DebugPrint1(350, 16 * 6, "%d: %x\n", __LINE__, Status);
        EFI_DEVICE_PATH_PROTOCOL *DiskDevicePath;
        Status = gBS->OpenProtocol(ControllerHandle[i],
                                   &gEfiDevicePathProtocolGuid,
                                   (VOID **)&DiskDevicePath,
                                   gImageHandle, 
                                   NULL,
                                   EFI_OPEN_PROTOCOL_GET_PROTOCOL);
        //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
        if (EFI_ERROR(Status))
        {
            DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
            return Status;
        }

        CHAR16 *TextDevicePath = DevPathToText->ConvertDevicePathToText(DiskDevicePath, TRUE, TRUE);

        // first display
    	 //DebugPrint1(DISK_READ_BUFFER_X, DISK_READ_BUFFER_Y + 16 * i, "%d: %s\n", __LINE__, TextDevicePath);
        //DEBUG ((EFI_D_INFO, "%s\n", TextDevicePath));

		 TextDevicePathAnalysisCHAR16(TextDevicePath, &device[i], i);
    	 
        if (TextDevicePath) gBS->FreePool(TextDevicePath);
        DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d device[i].DeviceType: %d, device[i].SectorCount: %lld\n", __LINE__, device[i].DeviceType, device[i].SectorCount);

		 // the USB we save our *.efi file and relative resource files..
		 //if (device[i].DeviceType == DeviceType && device[i].SectorCount == SectorCount)
		 //if (device[i].DeviceType == DeviceType && device[i].SectorCount == device[4].SectorCount)
		 //if (device[i].DeviceType == DeviceType && device[i].SectorCount == 522596352)
		 {
		    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		 	Status = gBS->HandleProtocol(ControllerHandle[i], &gEfiBlockIoProtocolGuid, (VOID * *) &BlockIo );                                                
		    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		    
		    if ( EFI_SUCCESS == Status )
		    {			        	 
		        Status = gBS->HandleProtocol( ControllerHandle[i], &gEfiDiskIoProtocolGuid, (VOID * *) &DiskIo ); 
		        //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		        
		        if ( Status == EFI_SUCCESS )
		        {
					 if (device[i].SectorCount <= sector_count)
					 {
						  DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: device[i].SectorCount <= sector_count \n", __LINE__);
						  continue;
					 }

					 // read from USB by sector(512B)
	        	    // Read FAT32 file system partition infomation , minimum unit is sector.
		        	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X sector_count:%ld\n", __LINE__, Status, sector_count);
	        	 	Status = DiskIo->ReadDisk( DiskIo, BlockIo->Media->MediaId, DISK_BUFFER_SIZE * sector_count, DISK_BUFFER_SIZE, Buffer1 );
		            if ( EFI_SUCCESS == Status )
		            {
						  for (int j = 0; j < 250; j++)
						  {
						  		DebugPrint1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", Buffer1[j] & 0xff);
						  }
				     }
		        	 
		            DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X sector_count:%ld BlockIo->Media->MediaId: %d\n", 
		            																   __LINE__, Status, sector_count, BlockIo->Media->MediaId);
															   
					// FAT32 file system
					if (Buffer1[0x52] == 'F' && Buffer1[0x53] == 'A' && Buffer1[0x54] == 'T' && Buffer1[0x55] == '3' && Buffer1[0x56] == '2')	
				 	{				 		
				 		DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: FAT32\n",  __LINE__);
				 		// analysis data area of patition
					 	FirstSelectorAnalysis(Buffer1, &MBRSwitched); 

					 	// data sector number start include: reserved selector, fat sectors(usually is 2: fat1 and fat2), and file system boot path start cluster(usually is 2, data block start number is 2)
					 	sector_count = MBRSwitched.ReservedSelector + MBRSwitched.SectorsPerFat * MBRSwitched.NumFATS + MBRSwitched.BootPathStartCluster - 2;
					 	BlockSize = MBRSwitched.SectorOfCluster * 512;
	               	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: sector_count:%ld BlockSize: %d\n",  __LINE__, sector_count, BlockSize);
               	}
               	// NTFS
               	else if (Buffer1[3] == 'N' && Buffer1[4] == 'T' && Buffer1[5] == 'F' && Buffer1[6] == 'S')
               	{
						FirstSelectorAnalysisNTFS(Buffer1, &NTFSBootSwitched);
						sector_count = NTFSBootSwitched.MFT_StartCluster * 8;
               		DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: NTFS sector_count:%llu\n",  __LINE__, sector_count);
               	}
               	else
               	{
               		return EFI_SUCCESS;
               	}               	
				  }       
		    }

		    return EFI_SUCCESS;
		 } 
    }
    return EFI_SUCCESS;
}


DisplayItemsOfPartition(UINT16 Index)
{
	// this code may be have some problems, because my USB file system is FAT32, my Disk file system is NTFS.
	// others use this code must be careful...
	if (device[Index].DeviceType == 1)
	{
		return;
		PartitionAnalysisFSM1(device[Index].DeviceType, device[Index].SectorCount);
		RootPathAnalysisFSM1(device[Index].DeviceType, device[Index].SectorCount);
		UINT16 valid_count = 0;

		for (UINT16 i = 0; i < 30; i++)
			if (pItems[i].FileName[0] != 0xE5 && (pItems[i].Attribute[0] == 0x20 
			    || pItems[i].Attribute[0] == 0x10))
	       {
	        	DebugPrint2(0, 8 * 16 + (valid_count) * 16, pMyComputerBuffer, "%d FileName:%2c%2c%2c%2c%2c%2c%2c%2c ExtensionName:%2c%2c%2c StartCluster:%02X%02X%02X%02X FileLength: %02X%02X%02X%02X Attribute: %02X    ", __LINE__,
	                                            pItems[i].FileName[0], pItems[i].FileName[1], pItems[i].FileName[2], pItems[i].FileName[3], pItems[i].FileName[4], pItems[i].FileName[5], pItems[i].FileName[6], pItems[i].FileName[7],
	                                            pItems[i].ExtensionName[0], pItems[i].ExtensionName[1],pItems[i].ExtensionName[2],
	                                            pItems[i].StartClusterHigh2B[0], pItems[i].StartClusterHigh2B[1],
	                                            pItems[i].StartClusterLow2B[0], pItems[i].StartClusterLow2B[1],
	                                            pItems[i].FileLength[0], pItems[i].FileLength[1], pItems[i].FileLength[2], pItems[i].FileLength[3],
	                                            pItems[i].Attribute[0]);
				
				valid_count++;
			}
		}
		else if (device[Index].DeviceType == 2)
		{
			PartitionAnalysisFSM1(device[Index].DeviceType, device[Index].SectorCount);
			MFTReadFromPartition(device[Index].DeviceType, device[Index].SectorCount);
		}
}

UINT8 MouseClickFlag = 0;
INT8 DisplayRootItemsFlag = 0;
UINT8 PreviousItem = -1;
VOID GraphicsLayerCompute(int iMouseX, int iMouseY, UINT8 MouseClickFlag)
{
	/*DebugPrint1(DISPLAY_X, DISPLAY_Y, "%d: pDeskDisplayBuffer: %X pDeskBuffer: %X ScreenWidth: %d ScreenHeight: %d pMouseBuffer: %X\n", __LINE__, 
																pDeskDisplayBuffer,
																pDeskBuffer,
																ScreenWidth,
																ScreenHeight,
																pMouseBuffer);
	*/
	//desk 
	GraphicsCopy(pDeskDisplayBuffer, pDeskBuffer, ScreenWidth, ScreenHeight, ScreenWidth, ScreenHeight, 0, 0);
    //DebugPrint1(DISPLAY_X, DISPLAY_Y, "%d: GraphicsLayerCompute\n", __LINE__);

	GraphicsCopy(pDeskDisplayBuffer, pMyComputerBuffer, ScreenWidth, ScreenHeight, MyComputerWidth, MyComputerHeight, MyComputerPositionX, MyComputerPositionY);

	//my computer
	if (MouseClickFlag == 1 && pDeskDisplayBuffer[(iMouseY * ScreenWidth + iMouseX) * 4 + 3] == GRAPHICS_LAYER_MY_COMPUTER)
	{
		MyComputerPositionX += x_move;
		MyComputerPositionY += y_move;
		
	//	GraphicsCopy(pDeskDisplayBuffer, pMyComputerBuffer, ScreenWidth, ScreenHeight, MyComputerWidth, MyComputerHeight, MyComputerPositionX, MyComputerPositionX);
	}
	x_move = 0;
	y_move = 0;
	// display graphics layer id mouse over, for mouse click event.
	//DebugPrint1(DISPLAY_X, DISPLAY_Y, "%d: Graphics Layer id: %d ", __LINE__, pDeskDisplayBuffer[(iMouseY * ScreenWidth + iMouseX) * 4 + 3]);
	
	int i, j;

	//16, ScreenHeight - 21, For event trigger

    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;  

	if (NULL == pMouseSelectedBuffer)
	{
		DEBUG ((EFI_D_INFO, "NULL == GraphicsLayerCompute"));
		return;
	}
    
    //DEBUG ((EFI_D_INFO, "Line: %d\n", __LINE__));

    for (i = 0; i < 16; i++)
        for (j = 0; j < 16; j++)
        {       
            pMouseBuffer[(i * 16 + j) * 4]     = 0xff;
            pMouseBuffer[(i * 16 + j) * 4 + 1] = 0xff;
            pMouseBuffer[(i * 16 + j) * 4 + 2] = 0;             
        }
	
    DrawChineseCharIntoBuffer2(pDeskBuffer,  16, ScreenHeight - 21,     (18 - 1) * 94 + 43 - 1, Color, ScreenWidth);
    DrawChineseCharIntoBuffer2(pDeskBuffer,  16 * 2, ScreenHeight - 21, (21 - 1) * 94 + 05 - 1, Color, ScreenWidth);
	
    Color.Red   = 0xff;
    Color.Green = 0x00;
    Color.Blue	  = 0x00;

	// this is draw a rectangle when mouse move on disk partition 
	for (UINT16 i = 0; i < NumHandles; i++)
	{		
		if (iMouseX >= MyComputerPositionX + 50 && iMouseX <= MyComputerPositionX + 50 + 16 * 6
			&& iMouseY >= MyComputerPositionY + i * 16 + 16 * 2 && iMouseY <= MyComputerPositionY + i * 16 + 16 * 3)
		{
			if (PreviousItem == i)
			{
				break;
			}
			
			RectangleDrawIntoBuffer(pMouseSelectedBuffer, 0,  0, 31, 15, 1,  Color, 32);
			DisplayItemsOfPartition(i);
			PreviousItem = i;
	       GraphicsCopy(pDeskDisplayBuffer, pMouseSelectedBuffer, ScreenWidth, ScreenHeight, 32, 16, MyComputerPositionX + 50, MyComputerPositionY  + i * (16 + 2) + 16 * 2);	
		}
	}
    
    //DebugPrint1(DISPLAY_X, DISPLAY_Y, "%d: GraphicsLayerCompute\n", __LINE__);
    if (iMouseX >= 16 && iMouseX <= 16 + 16 * 2
        && iMouseY >= ScreenHeight - 21 && iMouseY <= ScreenHeight)
    {	
		for (i = 0; i < 16; i++)
		{
			for (j = 0; j < 32; j++)
			{	
				pMouseSelectedBuffer[(i * 32 + j) * 4]     = pDeskDisplayBuffer[((MyComputerPositionX + 50 + i) * ScreenWidth +  MyComputerPositionY  + i * (16 + 2) + 16 * 2 + j) * 4];
				pMouseSelectedBuffer[(i * 32 + j) * 4 + 1] = pDeskDisplayBuffer[((MyComputerPositionX + 50 + i) * ScreenWidth +  MyComputerPositionY  + i * (16 + 2) + 16 * 2 + j) * 4 + 1];
				pMouseSelectedBuffer[(i * 32 + j) * 4 + 2] = pDeskDisplayBuffer[((MyComputerPositionX + 50 + i) * ScreenWidth +  MyComputerPositionY  + i * (16 + 2) + 16 * 2 + j) * 4 + 2];			
			}
		}
		 //RectangleFillIntoBuffer(UINT8 * pBuffer,IN UINTN x0,UINTN y0,UINTN x1,UINTN y1,IN UINTN BorderWidth,IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color)
		
        //RectangleFillIntoBuffer(pMouseSelectedBuffer, 0,  0, 32, 16, 1,  Color);

        // Draw a red rectangle when mouse move over left down (like menu button)
        RectangleDrawIntoBuffer(pMouseSelectedBuffer, 0,  0, 31, 15, 1,  Color, 32);

        //DebugPrint1(DISPLAY_X, DISPLAY_Y, "%d: GraphicsLayerCompute\n", __LINE__);

        if (MouseClickFlag == 1)
        {
			char *pClickMenuBuffer = (UINT8 *)AllocateZeroPool(16 * 16 * 4);
		    if (NULL != pClickMenuBuffer)
			{
				DEBUG ((EFI_D_INFO, "ScreenInit AllocatePool pDeskDisplayBuffer NULL\n"));

			    for(int i = 0; i < 16; i++)
			    	for(int j = 0; j < 16; j++)
			    	{
			    		pClickMenuBuffer[(i * 16 + j) * 4] = 0x33;
			    		pClickMenuBuffer[(i * 16 + j) * 4 + 1] = 0x33;
			    		pClickMenuBuffer[(i * 16 + j) * 4 + 2] = 0x33;
			    	}
				GraphicsCopy(pDeskDisplayBuffer, pClickMenuBuffer, ScreenWidth, ScreenHeight, 16, 16, 0, ScreenHeight - 22 - 16);
			}
        }
        
        GraphicsCopy(pDeskDisplayBuffer, pMouseSelectedBuffer, ScreenWidth, ScreenHeight, 32, 16, 15, ScreenHeight - 22);
    }
    
    // init mouse buffer for use in use
	DrawChineseCharIntoBuffer2(pMouseBuffer, 0, 0, 11 * 94 + 42, MouseColor, 16);
    //DebugPrint1(DISPLAY_X, DISPLAY_Y, "%d: GraphicsLayerCompute\n", __LINE__);

	GraphicsCopy(pDeskDisplayBuffer, pMouseBuffer, ScreenWidth, ScreenHeight, 16, 16, iMouseX, iMouseY);
    //DebugPrint1(DISPLAY_X, DISPLAY_Y, "%d: GraphicsLayerCompute\n", __LINE__);

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

    if (pBuffer == NULL)
    {
		return;
    }
    
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
            pBuffer[(j * ScreenWidth + i) * 4 + 3] =  Color.Reserved; //Red  
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




EFI_STATUS PartitionRead()
{
    //DebugPrint1(350, 0, "%d: 0 PartitionRead \n", __LINE__);
    DEBUG ((EFI_D_INFO, "PartitionRead!!\r\n"));
    EFI_STATUS Status ;
    UINTN NumHandles, i;
    EFI_HANDLE *ControllerHandle = NULL;
    EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;
    EFI_BLOCK_IO_PROTOCOL           *BlockIo;
    UINT8 Buffer1[DISK_BUFFER_SIZE];
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
    	 //DebugPrint1(TEXT_DEVICE_PATH_X, TEXT_DEVICE_PATH_Y + 16 * 4 * i, "%d: %s\n", __LINE__, TextDevicePath);
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
                Status = DiskIo->ReadDisk( DiskIo, BlockIo->Media->MediaId, 0, DISK_BUFFER_SIZE, Buffer1 );
                
                DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X BlockIo->Media->MediaId: %d\n", __LINE__, Status, BlockIo->Media->MediaId);

                //if ( EFI_SUCCESS == Status )
				 //	  for (int j = 0; j < 50; j++)
					  	   //DebugPrint1(DISK_READ_BUFFER_X + j * 8 * 3, TEXT_DEVICE_PATH_Y + 16 * (4 * i + 2) , "%02X ", Buffer1[j] & 0xff);
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
    UINT8 Buffer1[DISK_BUFFER_SIZE];
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

VOID Transfer(MasterBootRecord *pSource, MasterBootRecordSwitched *pDest)
{
    pDest->ReservedSelector = pSource->ReservedSelector[0] + pSource->ReservedSelector[1] * 16 * 16;
    pDest->SectorsPerFat    = (UINT16)pSource->SectorsPerFat[0] + (UINT16)(pSource->SectorsPerFat[1]) * 16 * 16 + pSource->SectorsPerFat[2] * 16 * 16 * 16 * 16 + pSource->SectorsPerFat[3] * 16 * 16 * 16 * 16 * 16 * 16;
    pDest->BootPathStartCluster = (UINT16)pSource->BootPathStartCluster[0] + pSource->BootPathStartCluster[1] * 16 * 16 + pSource->BootPathStartCluster[2] * 16 * 16 * 16 * 16, pSource->BootPathStartCluster[3] * 16 * 16 * 16 * 16 * 16 * 16;
    pDest->NumFATS      = pSource->NumFATS[0];
    pDest->SectorOfCluster = pSource->SectorOfCluster[0];
	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "ReservedSelector:%d SectorsPerFat:%d BootPathStartCluster: %d NumFATS:%d SectorOfCluster:%d", 
											    pDest->ReservedSelector,
											    pDest->SectorsPerFat,
											    pDest->BootPathStartCluster,
											    pDest->NumFATS,
											    pDest->SectorOfCluster);
}

EFI_STATUS ReadFileContentFromFAT32()
{
    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: ReadFileContentFromFAT32: %d \n",  __LINE__, sector_count);

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
    UINT8 Buffer1[DISK_BUFFER_SIZE];
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
        
       BlockIo2->ReadBlocksEx(BlockIo2, BlockIo2->Media->MediaId, LBA, &disk_handle_task.DiskIo2Token, DISK_BUFFER_SIZE, Buffer1);
        
        //gBS->SignalEvent (disk_handle_task.FileIoToken->Event);
        DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X BlockIo->Media->MediaId: %d\n", __LINE__, Status, BlockIo2->Media->MediaId);

        if ( EFI_SUCCESS == Status )
        {
              for (int j = 0; j < 250; j++)
              {
                    DebugPrint1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16) , "%02X ", Buffer1[j] & 0xff);
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

		        	 
		            Status = DiskIo2->ReadDiskEx( DiskIo2, BlockIo2->Media->MediaId, &disk_handle_task.DiskIo2Token, 0, DISK_BUFFER_SIZE, Buffer1 );

					
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

		           BlockIo2->ReadBlocksEx(BlockIo2, BlockIo2->Media->MediaId, LBA, &disk_handle_task.DiskIo2Token, DISK_BUFFER_SIZE, Buffer1);
		            
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

// all partitions analysis
EFI_STATUS PartitionAnalysis()
{    
    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d PartitionAnalysisFSM\n", __LINE__);
    EFI_STATUS Status ;
    UINTN i;
    EFI_HANDLE *ControllerHandle = NULL;
    EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;
    EFI_DISK_IO_PROTOCOL            *DiskIo;
    
    Status = gBS->LocateProtocol (&gEfiDevicePathToTextProtocolGuid, NULL, (VOID **) &DevPathToText);
    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
    if (EFI_ERROR(Status))
    {
    	 DEBUG ((EFI_D_INFO, "LocateProtocol1 error: %x\n", Status));    	 
    	 DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
        return Status;
    }
   
    Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiDiskIoProtocolGuid, NULL, &NumHandles, &ControllerHandle);
    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
    if (EFI_ERROR(Status))
    {
        DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);	    
        return EFI_SUCCESS;
    }

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
        //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
        if (EFI_ERROR(Status))
        {
            DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
            return Status;
        }

        CHAR16 *TextDevicePath = DevPathToText->ConvertDevicePathToText(DiskDevicePath, TRUE, TRUE);
		 DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X Partition: %s\n", __LINE__, Status, TextDevicePath);
		 TextDevicePathAnalysisCHAR16(TextDevicePath, &device[i], i);
    	            
        if (TextDevicePath) gBS->FreePool(TextDevicePath);		 
    }
    return EFI_SUCCESS;
}



// all partitions analysis
EFI_STATUS PartitionAnalysisFSM()
{    
    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d PartitionAnalysisFSM\n", __LINE__);
    DEBUG ((EFI_D_INFO, "PartitionUSBRead!!\r\n"));
    EFI_STATUS Status ;
    UINTN NumHandles, i;
    EFI_HANDLE *ControllerHandle = NULL;
    EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;
    EFI_BLOCK_IO_PROTOCOL           *BlockIo;
    UINT8 Buffer1[DISK_BUFFER_SIZE];
    UINT8 BufferBlock[DISK_BLOCK_BUFFER_SIZE];
    EFI_DISK_IO_PROTOCOL            *DiskIo;
    
    Status = gBS->LocateProtocol (&gEfiDevicePathToTextProtocolGuid, NULL, (VOID **) &DevPathToText);
    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
    if (EFI_ERROR(Status))
    {
    	 DEBUG ((EFI_D_INFO, "LocateProtocol1 error: %x\n", Status));    	 
    	 DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
        return Status;
    }
   
    Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiDiskIoProtocolGuid, NULL, &NumHandles, &ControllerHandle);
    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
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
        //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
        if (EFI_ERROR(Status))
        {
            DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
            return Status;
        }

        CHAR16 *TextDevicePath = DevPathToText->ConvertDevicePathToText(DiskDevicePath, TRUE, TRUE);

        // first display
    	 //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: %s\n", __LINE__, TextDevicePath);
        //DEBUG ((EFI_D_INFO, "%s\n", TextDevicePath));

		 TextDevicePathAnalysisCHAR16(TextDevicePath, &device[i], i);
    	 
        if (TextDevicePath) gBS->FreePool(TextDevicePath);

		 // the USB we save our *.efi file and relative resource files..
		 if (device[i].DeviceType == 1 && device[i].SectorCount == 915551)
		 {
		    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		 	Status = gBS->HandleProtocol(ControllerHandle[i], &gEfiBlockIoProtocolGuid, (VOID * *) &BlockIo );                                                
		    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		    
		    if ( EFI_SUCCESS == Status )
		    {			        	 
		        Status = gBS->HandleProtocol( ControllerHandle[i], &gEfiDiskIoProtocolGuid, (VOID * *) &DiskIo ); 
		        //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		        
		        if ( Status == EFI_SUCCESS )
		        {
					 if (device[i].SectorCount <= sector_count)
					 {
						  //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: device[i].SectorCount <= sector_count \n", __LINE__);
						  return EFI_SUCCESS;
					 }

					 // read from USB by sector(512B)
	        	    // Read FAT32 file system partition infomation , minimum unit is sector.
		        	 	//DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X sector_count:%ld\n", __LINE__, Status, sector_count);
	        	 	Status = DiskIo->ReadDisk( DiskIo, BlockIo->Media->MediaId, DISK_BUFFER_SIZE * sector_count, DISK_BUFFER_SIZE, Buffer1 );
		            if ( EFI_SUCCESS == Status )
		            {
						  for (int j = 0; j < 250; j++)
						  {
						  		DebugPrint1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", Buffer1[j] & 0xff);
						  }
				     }
		        	 
		            //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X sector_count:%ld BlockIo->Media->MediaId: %d\n", 
		            //																   __LINE__, Status, sector_count, BlockIo->Media->MediaId);
					  
				 	// analysis data area of patition
				 	FirstSelectorAnalysis(Buffer1, &MBRSwitched); 

				 	// data sector number start include: reserved selector, fat sectors(usually is 2: fat1 and fat2), and file system boot path start cluster(usually is 2, data block start number is 2)
				 	sector_count = MBRSwitched.ReservedSelector + MBRSwitched.SectorsPerFat * MBRSwitched.NumFATS + MBRSwitched.BootPathStartCluster - 2;
				 	BlockSize = MBRSwitched.SectorOfCluster * 512;
               	//DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: sector_count:%ld BlockSize: %d\n",  __LINE__, sector_count, BlockSize);
				  }       
		    }

		    return EFI_SUCCESS;
		 } 
    }
    return EFI_SUCCESS;
}

// analysis a partition 
EFI_STATUS RootPathAnalysisFSM()
{
    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d RootPathAnalysisFSM\n", __LINE__);
    //printf( "RootPathAnalysis\n" );
    DEBUG ((EFI_D_INFO, "PartitionUSBRead!!\r\n"));
    EFI_STATUS Status ;
    UINTN NumHandles, i;
    EFI_HANDLE *ControllerHandle = NULL;
    EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;
    EFI_BLOCK_IO_PROTOCOL           *BlockIo;
    UINT8 Buffer1[DISK_BUFFER_SIZE];
    UINT8 BufferBlock[DISK_BLOCK_BUFFER_SIZE];
    EFI_DISK_IO_PROTOCOL            *DiskIo;
    
    Status = gBS->LocateProtocol (&gEfiDevicePathToTextProtocolGuid, NULL, (VOID **) &DevPathToText);
    if (EFI_ERROR(Status))
    {
    	 DEBUG ((EFI_D_INFO, "LocateProtocol1 error: %x\n", Status));    	 
    	 //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
        return Status;
    }
   
    Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiDiskIoProtocolGuid, NULL, &NumHandles, &ControllerHandle);
    if (EFI_ERROR(Status))
    {
        //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
	    
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
    	 //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: %s\n", __LINE__, TextDevicePath);
        //DEBUG ((EFI_D_INFO, "%s\n", TextDevicePath));

		 TextDevicePathAnalysisCHAR16(TextDevicePath, &device[i], i);
    	            
        if (TextDevicePath) gBS->FreePool(TextDevicePath);

		 // the USB we save our *.efi file and relative resource files..
		 if (device[i].DeviceType == 1 && device[i].SectorCount == 915551)
		 {
		    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		 	Status = gBS->HandleProtocol(ControllerHandle[i], &gEfiBlockIoProtocolGuid, (VOID * *) &BlockIo );                                                
		    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		    
		    if ( EFI_SUCCESS == Status )
		    {			        	 
		        Status = gBS->HandleProtocol( ControllerHandle[i], &gEfiDiskIoProtocolGuid, (VOID * *) &DiskIo ); 
		        //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		        
		        if ( Status == EFI_SUCCESS )
		        {
					 if (device[i].SectorCount <= sector_count)
					 {
						  //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: device[i].SectorCount <= sector_count \n", __LINE__);
						  return EFI_SUCCESS;
					 }

	        	    // Read FAT32 file system partition infomation , minimum unit is sector.
	        	 	//DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X sector_count:%ld\n", __LINE__, Status, sector_count);
	        	 	Status = DiskIo->ReadDisk( DiskIo, BlockIo->Media->MediaId, DISK_BUFFER_SIZE * sector_count, DISK_BUFFER_SIZE, Buffer1 );
		            if ( EFI_SUCCESS == Status )
		            {
						  for (int j = 0; j < 250; j++)
						  {
						  		DebugPrint1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", Buffer1[j] & 0xff);
						  }
				     }
		        	 
				 	//When get root path data sector start number, we can get content of root path.
				 	RootPathAnalysis(Buffer1);	

					// data area start from 1824, HZK16 file start from 	FileBlockStart	block, so need to convert into sector by multi 8, block start number is 2 	
					// next state is to read FAT table
				 	sector_count = MBRSwitched.ReservedSelector;
				 	//DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: sector_count:%ld FileLength: %d MBRSwitched.ReservedSelector:%ld\n",  __LINE__, sector_count, FileLength, MBRSwitched.ReservedSelector);
		        }       
		    }

		    return EFI_SUCCESS;
		 } 		 
    }
    return EFI_SUCCESS;
}

// 
EFI_STATUS GetFatTableFSM()
{    
    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d GetFatTableFSM\n", __LINE__);
    //printf( "RootPathAnalysis\n" );
    DEBUG ((EFI_D_INFO, "PartitionUSBRead!!\r\n"));
    EFI_STATUS Status ;
    UINTN NumHandles, i;
    EFI_HANDLE *ControllerHandle = NULL;
    EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;
    EFI_BLOCK_IO_PROTOCOL           *BlockIo;
    UINT8 Buffer1[DISK_BUFFER_SIZE];
    UINT8 BufferBlock[DISK_BLOCK_BUFFER_SIZE];
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
    	 //DebugPrint1(DISK_READ_BUFFER_X, DISK_READ_BUFFER_Y + 16 * i, "%d: %s\n", __LINE__, TextDevicePath);
        //DEBUG ((EFI_D_INFO, "%s\n", TextDevicePath));

		 TextDevicePathAnalysisCHAR16(TextDevicePath, &device[i], i);
    	            
        if (TextDevicePath) gBS->FreePool(TextDevicePath);

		 // the USB we save our *.efi file and relative resource files..
		 if (device[i].DeviceType == 1 && device[i].SectorCount == 915551)
		 {
		    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		 	Status = gBS->HandleProtocol(ControllerHandle[i], &gEfiBlockIoProtocolGuid, (VOID * *) &BlockIo );                                                
		    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		    
		    if ( EFI_SUCCESS == Status )
		    {			        	 
		        Status = gBS->HandleProtocol( ControllerHandle[i], &gEfiDiskIoProtocolGuid, (VOID * *) &DiskIo ); 
		        //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		        
		        if ( Status == EFI_SUCCESS )
		        {
					 if (device[i].SectorCount <= sector_count)
					 {
						  //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: device[i].SectorCount <= sector_count \n", __LINE__);
						  return EFI_SUCCESS;
					 }

	        	    // Read FAT32 file system partition infomation , minimum unit is sector.
	        	 	//DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X sector_count:%ld\n", __LINE__, Status, sector_count);
	        	 	Status = DiskIo->ReadDisk( DiskIo, BlockIo->Media->MediaId, DISK_BUFFER_SIZE * sector_count, DISK_BUFFER_SIZE, Buffer1 );
		            if ( EFI_SUCCESS == Status )
		            {
		            	  FAT32_Table = (UINT8 *)AllocateZeroPool(DISK_BUFFER_SIZE + 1);
						  if (NULL == FAT32_Table)
						  {
							  DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: NULL == FAT32_Table\n", __LINE__);						  	  
						  }
		            	  
                        CopyMem(FAT32_Table, Buffer1, DISK_BUFFER_SIZE);
						  for (int j = 0; j < 250; j++)
						  {
						  		DebugPrint1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", Buffer1[j] & 0xff);
						  }
				     } 
				     
		            //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X sector_count:%ld BlockIo->Media->MediaId: %d\n", 
		            //																   __LINE__, Status, sector_count, BlockIo->Media->MediaId);

					  
				 	//When get root path data sector start number, we can get content of root path.
				 	//RootPathAnalysis(Buffer1);	

					// data area start from 1824, HZK16 file start from 	FileBlockStart	block, so need to convert into sector by multi 8, block start number is 2 	
					// next state is to read File content use block unit, and start with block start.
				 	sector_count = MBRSwitched.ReservedSelector + MBRSwitched.SectorsPerFat * MBRSwitched.NumFATS + MBRSwitched.BootPathStartCluster - 2 + (FileBlockStart - 2) * 8;
				 	PreviousBlockNumber = FileBlockStart;
				 	//DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: sector_count:%ld FileLength: %d PreviousBlockNumber: %d\n",  __LINE__, sector_count, FileLength, PreviousBlockNumber);
					 
		        }       
		    }

		    return EFI_SUCCESS;
		 } 		 
    }
    return EFI_SUCCESS;
}

UINT32 GetNextBlockNumber()
{
	//DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: PreviousBlockNumber: %d\n",  __LINE__, PreviousBlockNumber);
	FAT32_Table[PreviousBlockNumber * 4];

	if (FAT32_Table[PreviousBlockNumber * 4] == 0xff 
	    && FAT32_Table[PreviousBlockNumber * 4 + 1] == 0xff 
	    && FAT32_Table[PreviousBlockNumber * 4 + 2] == 0xff 
	    && FAT32_Table[PreviousBlockNumber * 4 + 3] == 0x0f)
	{
		//DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: PreviousBlockNumber: %d, PreviousBlockNumber: %llX\n",  __LINE__, PreviousBlockNumber, 0x0fffffff);
		return 0x0fffffff;
	}

	return FAT32_Table[PreviousBlockNumber * 4] + (UINT32)FAT32_Table[PreviousBlockNumber * 4 + 1] * 16 * 16 + (UINT32)FAT32_Table[PreviousBlockNumber * 4 + 2] * 16 * 16 * 16 * 16 + (UINT32)FAT32_Table[PreviousBlockNumber * 4 + 3] * 16 * 16 * 16 * 16 * 16 * 16;	
}

EFI_STATUS ChineseCharArrayInit()
{
	//DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: ChineseCharArrayInit\n",  __LINE__);
	DEBUG ((EFI_D_INFO, "%d ChineseCharArrayInit\n", __LINE__));
    // 87 line, 96 Chinese char each line, 32 Char each Chinese char
	UINT32 size = 267616;
    //UINT8 *sChineseChar;

    if (HZK16FileReadCount >= 50)
    {		
    	//DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: HZK16FileReadCount >= 60\n",  __LINE__);
		EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
		Color.Blue  = 0x00;
		Color.Red   = 0x00;
		Color.Green = 0x00;
		UINT16 x, y;
		x = 32;
		y = 20;
		for (UINT16 j = 13 ; j < 20 ; j++)
		{
			for (UINT16 i = 0 ; i < 37 ; i++)
			{	    
				DrawChineseCharIntoBuffer2(pDeskBuffer, x + i * 16, y, j * 94 + i, Color, ScreenWidth);
				//DEBUG ((EFI_D_INFO, "ChineseCharArrayInit: %x \n ", sChineseChar[1504 * 32 + i]));
			}
			x = 10;
			y += 16;
		}
    }

	if (NULL != sChineseChar)
    {
    	//DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: ChineseCharArrayInit\n",  __LINE__);
    	return EFI_SUCCESS;
    }

    
	sChineseChar = (UINT8 *)AllocateZeroPool(size);
	// sChineseChar = (UINT8 *)AllocatePages(65);
	if (NULL == sChineseChar)
    {
        DEBUG ((EFI_D_INFO, "ChineseCharArrayInit AllocateZeroPool Failed: %x!\n "));
        DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: ChineseCharArrayInit AllocateZeroPool failed\n",  __LINE__);
        //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: ChineseCharArrayInit AllocateZeroPool Failed: %x\n",  __LINE__);
        return (EFI_SUCCESS);
    }
    /*
	if (FileReadSelf2(L"HZK16", size, sChineseChar) == -1)
	{
		FreePool((VOID *)sChineseChar);
		sChineseChar = NULL;
		DEBUG ((EFI_D_INFO, "Read HZK16 failed\n"));
		return EFI_SUCCESS;
	}
	*/
    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: ChineseCharArrayInit finished...\n",  __LINE__);
	return EFI_SUCCESS;
}

void *memcopy(void *dest, const void *src, size_t count)
{
	char *d;
	const char *s;

	if (dest > (src + count) || (dest < src))
	{
		d = dest;
		s = src;
		while (count--)
			*d++ = *s++;        
	}
	else
	{
		d = (char *)(dest + count - 1);
		s = (char *)(src + count -1);
		while (count --)
			*d-- = *s--;
	}

	return dest;
}
//https://blog.csdn.net/goodwillyang/article/details/45559925

EFI_STATUS ReadFileFSM()
{    
    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d ReadFileFSM\n", __LINE__);
    DEBUG ((EFI_D_INFO, "PartitionUSBRead!!\r\n"));
    EFI_STATUS Status ;
    UINTN NumHandles, i;
    EFI_HANDLE *ControllerHandle = NULL;
    EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;
    EFI_BLOCK_IO_PROTOCOL           *BlockIo;
    UINT8 Buffer1[DISK_BUFFER_SIZE];
    UINT8 BufferBlock[DISK_BLOCK_BUFFER_SIZE];
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
    	 //DebugPrint1(DISK_READ_BUFFER_X, DISK_READ_BUFFER_Y + 16 * i, "%d: %s\n", __LINE__, TextDevicePath);
        //DEBUG ((EFI_D_INFO, "%s\n", TextDevicePath));

		 TextDevicePathAnalysisCHAR16(TextDevicePath, &device[i], i);
    	            
        if (TextDevicePath) gBS->FreePool(TextDevicePath);

		 // the USB we save our *.efi file and relative resource files..
		 if (device[i].DeviceType == 1 && device[i].SectorCount == 915551)
		 {
		    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		 	Status = gBS->HandleProtocol(ControllerHandle[i], &gEfiBlockIoProtocolGuid, (VOID * *) &BlockIo );                                                
		    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		    
		    if ( EFI_SUCCESS == Status )
		    {			        	 
		        Status = gBS->HandleProtocol( ControllerHandle[i], &gEfiDiskIoProtocolGuid, (VOID * *) &DiskIo ); 
		        //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		        
		        if ( Status == EFI_SUCCESS )
		        {
					 if (device[i].SectorCount <= sector_count)
					 {
						  //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: device[i].SectorCount <= sector_count \n", __LINE__);
						  return EFI_SUCCESS;
					 }

		        	 // read from USB by block(512 * 8)
	        	    // Read file content from FAT32(USB), minimum unit is block
	        	    for (int i = 0; i < FileLength / (512 * 8); i++)
	        	 	{
		        	 	//DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		        	 	Status = DiskIo->ReadDisk( DiskIo, BlockIo->Media->MediaId, DISK_BUFFER_SIZE * sector_count, DISK_BLOCK_BUFFER_SIZE, BufferBlock);
		        	 	//DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		        	 	
						if ( EFI_SUCCESS == Status )
						{  
						     ChineseCharArrayInit();
						     
	                       //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: HZK16FileReadCount: %d DISK_BLOCK_BUFFER_SIZE: %d\n", __LINE__, HZK16FileReadCount, DISK_BLOCK_BUFFER_SIZE);

							  //Copy buffer to ChineseBuffer
							  if (sChineseChar != NULL)
							  {
									//	UINT8 *p = memcopy(sChineseChar[HZK16FileReadCount * DISK_BLOCK_BUFFER_SIZE], BufferBlock, DISK_BLOCK_BUFFER_SIZE);
							  		for (UINT16 i = 0; i < DISK_BLOCK_BUFFER_SIZE; i++)
							  			sChineseChar[HZK16FileReadCount * DISK_BLOCK_BUFFER_SIZE + i] = BufferBlock[i];
	                             //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: sChineseChar: %X BufferBlock: %X\n", __LINE__, sChineseChar, BufferBlock);
							  		
							  }
							  for (int j = 0; j < 250; j++)
							  {
							  		//DebugPrint1(DISK_READ_BUFFER_X + (j % 32) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 32), "%02X ", BufferBlock[j] & 0xff);
							  }
							  HZK16FileReadCount++;
						 }

					 	 sector_count = MBRSwitched.ReservedSelector + MBRSwitched.SectorsPerFat * MBRSwitched.NumFATS + MBRSwitched.BootPathStartCluster - 2 + (GetNextBlockNumber() - 2) * 8;
					 	 PreviousBlockNumber = GetNextBlockNumber();
					     
			            //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: PreviousBlockNumber:%d sector_count:%ld HZK16FileReadCount: %d\n", 
			            //																   __LINE__, PreviousBlockNumber, sector_count, HZK16FileReadCount);
				  }
		        } 
		    }

		    return EFI_SUCCESS;
		 } 		 
    }
    return EFI_SUCCESS;
}

STATE_TRANS StatusTransitionTable[] =
{
	{ INIT_STATE,                READ_PATITION_EVENT,   GET_PARTITION_INFO_STATE, PartitionAnalysisFSM},
	{ GET_PARTITION_INFO_STATE,  READ_ROOT_PATH_EVENT,  GET_ROOT_PATH_INFO_STATE, RootPathAnalysisFSM},
	{ GET_ROOT_PATH_INFO_STATE,  READ_FAT_TABLE_EVENT,  GET_FAT_TABLE_STATE,      GetFatTableFSM},
	{ GET_FAT_TABLE_STATE,       READ_FILE_EVENT,       READ_FILE_STATE,          ReadFileFSM},
	{ READ_FILE_STATE,           READ_FILE_EVENT,       READ_FILE_STATE,          ReadFileFSM },
};

STATE	NextState = INIT_STATE;

int FileReadFSM(EVENT event)
{
    EFI_STATUS Status;
        
	if (HZK16FileReadCount > 64)
	{
		DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		return;
	}
	/*DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: FileReadFSM current event: %d, NextState: %d table event:%d table NextState: %d\n", 
							  __LINE__, 
                            event, 
                            NextState,
                            StatusTransitionTable[NextState].event,
                            StatusTransitionTable[NextState].NextState);
    */
	if ( event == StatusTransitionTable[NextState].event )
	{
		//DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		StatusTransitionTable[NextState].pFunc();
		NextState = StatusTransitionTable[NextState].NextState;
	}
	else  
	{
		//DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		NextState = INIT_STATE;
	}

	//DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
	StatusErrorCount++;
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

					//DecToCharBuffer(Buffer, DISK_BUFFER_SIZE, Bufferout);
			       
			       //ShowHex(Buffer, DISK_BUFFER_SIZE);
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

float MemoryParameterGet2()
{  
	EFI_STATUS                           Status;
	UINT8                                TmpMemoryMap[1];
	UINTN                                MapKey;
	UINTN                                DescriptorSize;
	UINT32                               DescriptorVersion;
	UINTN                                MemoryMapSize;
	EFI_MEMORY_DESCRIPTOR                *MemoryMap;
	EFI_MEMORY_DESCRIPTOR                *MemoryMapPtr;
	UINTN                                Index;
	struct efi_info_self                  *Efi;
	struct e820_entry_self               *LastE820;
	struct e820_entry_self               *E820;
	UINTN                                E820EntryCount;
	EFI_PHYSICAL_ADDRESS                 LastEndAddr;
	UINTN MemoryClassifySize[6] = {0};
    
    //
    // Get System MemoryMapSize
    //
    MemoryMapSize = sizeof (TmpMemoryMap);
    Status = gBS->GetMemoryMap (&MemoryMapSize,
			                    (EFI_MEMORY_DESCRIPTOR *)TmpMemoryMap,
			                    &MapKey,
			                    &DescriptorSize,
			                    &DescriptorVersion);
	//DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);			
	DEBUG ((EFI_D_INFO, "%d:  Status:%X \n", __LINE__, Status));
    ASSERT (Status == EFI_BUFFER_TOO_SMALL);
    //
    // Enlarge space here, because we will allocate pool now.
    //
    MemoryMapSize += EFI_PAGE_SIZE;
    Status = gBS->AllocatePool (EfiLoaderData,
			                    MemoryMapSize,
			                    (VOID **) &MemoryMap);
    ASSERT_EFI_ERROR (Status);
    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
    	
	DEBUG ((EFI_D_INFO, "%d:  Status:%X \n", __LINE__, Status));
    //
    // Get System MemoryMap
    //
    Status = gBS->GetMemoryMap (&MemoryMapSize,
			                    MemoryMap,
			                    &MapKey,
			                    &DescriptorSize,
			                    &DescriptorVersion);
    ASSERT_EFI_ERROR (Status);
    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
    	
	DEBUG ((EFI_D_INFO, "%d:  Status:%X \n", __LINE__, Status));
    
    LastE820 = NULL;
    E820EntryCount = 0;
    LastEndAddr = 0;
    MemoryMapPtr = MemoryMap;
    UINTN E820Type = 0;
    for (Index = 0; Index < (MemoryMapSize / DescriptorSize); Index++) 
    {
    	E820Type = 0;
      //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Index:%X \n", __LINE__, Index);
      	
	  //DEBUG ((EFI_D_INFO, "%d:  Status:%X \n", __LINE__, Status));
    
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
		        break;
	    
	      case EfiACPIMemoryNVS:	   // // Address space reserved for use by the firmware.
		        MemoryClassifySize[4] += MemoryMap->NumberOfPages;
		        E820Type = E820_NVS;
		        break;
	    
	      default:
		        MemoryClassifySize[5] += MemoryMap->NumberOfPages;
		        DEBUG ((DEBUG_ERROR,
		          "Invalid EFI memory descriptor type (0x%x)!\n",
		          MemoryMap->Type));
		          	
				   DEBUG ((EFI_D_INFO, "%d:  Invalid EFI memory descriptor type (0x%x)!\n", __LINE__, MemoryMap->Type));
		        continue;		  
      }
    
      	/*DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: %d: E820Type:%X Start:%X Number:%X End: %X\n", __LINE__, 
      	  																	Index,
      	  																	E820Type, 
      	  																	MemoryMap->PhysicalStart, 
      	  																	MemoryMap->NumberOfPages,
      	  																	MemoryMap->PhysicalStart + MemoryMap->NumberOfPages * 4 * 1024);

		*/
		DEBUG ((EFI_D_INFO, "%d: E820Type:%X Start:%X Number:%X\n", __LINE__, 
      	  																	E820Type, 
      	  																	MemoryMap->PhysicalStart, 
      	  																	MemoryMap->NumberOfPages));
		//
		// Get next item
		//
		MemoryMap = (EFI_MEMORY_DESCRIPTOR *)((UINTN)MemoryMap + DescriptorSize);
    }

    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: %d: (MemoryMapSize / DescriptorSize):%X MemoryClassifySize[0]:%d %d %d %d %d %d \n", __LINE__,
    																		    Index,
                                                                        (MemoryMapSize / DescriptorSize),
                                                                        MemoryClassifySize[0],
                                                                        MemoryClassifySize[1],
                                                                        MemoryClassifySize[2],
                                                                        MemoryClassifySize[3],
                                                                        MemoryClassifySize[4],
                                                                        MemoryClassifySize[5]
                                                                        );
	return  MemoryClassifySize[3];                                                                    
}

char *float2str(float val, int precision, char *buf)
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



EFI_STATUS WindowCreateUseBuffer(UINT8 *pBuffer, UINT8 *pParent, UINT16 Width, UINT16 Height, UINT16 Type, CHAR8 *pWindowTitle)
{	
    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Width: %d \n", __LINE__, Width);
	
	UINT16 i, j;

    if (NULL == pBuffer)
	{
		DEBUG ((EFI_D_INFO, "NULL == pBuffer"));
		return EFI_SUCCESS;
	}	
	
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    
	Color.Blue  = 0xff;
	Color.Red   = 0xff;
	Color.Green = 0xff;
	Color.Reserved = GRAPHICS_LAYER_MY_COMPUTER;

    // wo de dian nao
    DrawChineseCharIntoBuffer2(pBuffer, 3, 6,          (46 - 1 ) * 94 + 50 - 1, Color, Width);    
    DrawChineseCharIntoBuffer2(pBuffer, 3 + 16 , 6,     (21 - 1) * 94 + 36 - 1, Color, Width);
    DrawChineseCharIntoBuffer2(pBuffer, 3 + 16 * 2, 6, (21 - 1) * 94 + 71 - 1, Color, Width);
    DrawChineseCharIntoBuffer2(pBuffer, 3 + 16 * 3, 6, (36 - 1) * 94 + 52 - 1, Color, Width);

    // The Left of Window
	for (i = 23; i < Height; i++)
	{
		for (j = 0; j < Width / 3; j++)
		{
			pBuffer[(i * Width + j) * 4] = 214;
			pBuffer[(i * Width + j) * 4 + 1] = 211;
			pBuffer[(i * Width + j) * 4 + 2] = 204;
			pBuffer[(i * Width + j) * 4 + 3] = GRAPHICS_LAYER_MY_COMPUTER;
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
			pBuffer[(i * Width + j) * 4 + 3] = GRAPHICS_LAYER_MY_COMPUTER;
		}
	}

	// get partitions from api
	PartitionAnalysis();

	Color.Blue  = 0x00;
	Color.Red   = 0x00;
	Color.Green = 0x00;
	int x, y;

	for (UINT16 i = 0 ; i < NumHandles ; i++)
	{
		x = 50;
		y = i * 18 + 16 * 2;		

		if (device[i].DeviceType == 2)
		{
			//
			DrawChineseCharIntoBuffer2(pBuffer, x, y,          (20 - 1 ) * 94 + 37 - 1, Color, Width); 
			x += 16;
		}
		else
		{
			// U pan
			DrawChineseCharIntoBuffer2(pBuffer, x, y,          (51 - 1 ) * 94 + 37 - 1, Color, Width); 
			x += 16;
		}	
		//pan
		DrawChineseCharIntoBuffer2(pBuffer, x, y,          (37 - 1 ) * 94 + 44 - 1, Color, Width); 
		x += 16;

		//  2354 分 3988 区
		DrawChineseCharIntoBuffer2(pBuffer, x, y,          (23 - 1 ) * 94 + 54 - 1, Color, Width);    
		x += 16;
		
		DrawChineseCharIntoBuffer2(pBuffer, x, y,          (39 - 1 ) * 94 + 88 - 1, Color, Width);   
		x += 16;

		// 5027
		// 一
		// 2294
		// 二
		// 4093
		// 三
		if (device[i].PartitionID == 1)
		{
			DrawChineseCharIntoBuffer2(pBuffer, x, y,          (50 - 1 ) * 94 + 27 - 1, Color, Width);   
			x += 16;
		}
		else if (device[i].PartitionID == 2)
		{
			DrawChineseCharIntoBuffer2(pBuffer, x, y,          (22 - 1 ) * 94 + 94 - 1, Color, Width);   
			x += 16;
		}
		else if (device[i].PartitionID == 3)
		{
			DrawChineseCharIntoBuffer2(pBuffer, x, y,          (40 - 1 ) * 94 + 93 - 1, Color, Width);   
			x += 16;
		}
		
		UINT32 size = (UINT32)device[i].SectorCount / 2; //K
		char sizePostfix[3] = "MB";
		size /= 1024.0; //M
		if (size > 1024.0)
		{
			size /= 1024;	
			sizePostfix[3] = "GB";
		}

		//2083
		//大
		//4801
		//小
		DrawChineseCharIntoBuffer2(pBuffer, x, y,          (20 - 1 ) * 94 + 83 - 1, Color, Width);  
		x += 16;
		
		DrawChineseCharIntoBuffer2(pBuffer, x, y,          (48 - 1 ) * 94 + 1 - 1, Color, Width);  
		x += 16;

		//CHAR16 s[4] = L"大";
		DebugPrint2(x, y, pBuffer, "%d%a", size, sizePostfix);

	}
	
	y += 16;
	y += 16;
	//3658
	//内
	//2070
	//存
	DrawChineseCharIntoBuffer2(pBuffer, x, y,          (36 - 1 ) * 94 + 58 - 1, Color, Width);  
	x += 16;
	
	DrawChineseCharIntoBuffer2(pBuffer, x, y,          (20 - 1 ) * 94 + 70 - 1, Color, Width);  
	x += 16;
	
	// Get memory infomation
	x = 0;
	// Note: the other class memory can not use
	float size = (float)MemoryParameterGet2();
	size = size * 4;
	size = size / (1024 * 1024);
	CHAR8 buf[7] = {0};
	DebugPrint2(x, y, pBuffer, "%a", float2str(size, 3, buf));
	x += 10 * 8;
	
	DrawChineseCharIntoBuffer2(pBuffer, x, y,          (28 - 1 ) * 94 + 10 - 1, Color, Width);  
	x += 16;
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
	
    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: MyComputerWidth: %d \n", __LINE__, MyComputerWidth);
	WindowCreateUseBuffer(pMyComputerBuffer, pParent, MyComputerWidth, MyComputerHeight, Type, pWindowTitle);
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
			
		DebugPrint1(i * 4, 16 * 16, "%02x ", Buffer[i] );
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

    UINT8 Buffer1[DISK_BUFFER_SIZE];
    UINT8 Bufferout[DISK_BUFFER_SIZE * EXBYTE];

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

                Status = DiskIo->ReadDisk( DiskIo, BlockIo->Media->MediaId, 0, DISK_BUFFER_SIZE, Buffer1 );
                //Print( L"%d :%X\n", __LINE__ ,Status );
                DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X BlockIo->Media->MediaId: %d\n", __LINE__, Status, BlockIo->Media->MediaId);

                if ( EFI_SUCCESS == Status )
                {
                    //Buffer1[10] = '\0';
                    //Print( L"%d :%s\n", __LINE__ , Buffer );

					  for (int j = 0; j < 50; j++)
					  	   DebugPrint1(DISK_READ_BUFFER_X + j * 8 * 3, DISK_READ_BUFFER_Y + i * 16, "%02X ", Buffer1[j] & 0xff);

                    //DecToCharBuffer1(Buffer1, DISK_BUFFER_SIZE, Bufferout);
                   
                    //ShowHex1(Bufferout, DISK_BUFFER_SIZE);
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


int FSM_Event = READ_PATITION_EVENT;

int flag = 0;
EFIAPI HandleEnterPressed()
{
	DEBUG ((EFI_D_INFO, "%d HandleEnterPressed\n", __LINE__));
	
    //PartitionUSBReadSynchronous();
    //PartitionUSBReadAsynchronous();

	if (flag == 0)	
	{
		PartitionAnalysisFSM1(device[8].DeviceType, device[8].SectorCount);
		MFTReadFromPartition(device[8].DeviceType, device[8].SectorCount);
    	MFTDollarRootFileAnalysisBuffer(BufferMFT);
		flag = 1;
	}
	
    NTFSRootPathIndexItemsRead(8);

	DEBUG ((EFI_D_INFO, "%d HandleEnterPressed\n", __LINE__));
	return EFI_SUCCESS;
}

STATIC
VOID
EFIAPI
HandleKeyboardEvent (
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
    //DebugPrint1(DISPLAY_X, DISPLAY_Y, "%d:HandleKeyboardEvent \n", __LINE__);
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
               
               display_sector_number = uniChar - '0';
               
               if (display_sector_number > 10)
               {
                   display_sector_number = 10;                     
               }
               
               if (display_sector_number < 0)
               {
                   display_sector_number = 0;                     
               }
               
               DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: uniChar: %d display_sector_number: %d \n", __LINE__, uniChar, display_sector_number);
               

			    // Enter pressed
		     	 if (0x0D == uniChar)
		     	 {
		        	keyboard_input_count = 0;
		        	memset(pKeyboardInputBuffer, '\0', KEYBOARD_BUFFER_LENGTH);
		     	 	//DebugPrint1(DISPLAY_KEYBOARD_X, DISPLAY_KEYBOARD_Y, "%a keyboard_input_count: %04d enter pressed", pKeyboardInputBuffer, keyboard_input_count);

		     	 	HandleEnterPressed();
		     	 }
		     	 else
		     	 {
		     	 	DebugPrint1(DISPLAY_KEYBOARD_X, DISPLAY_KEYBOARD_Y, "%a keyboard_input_count: %04d ", pKeyboardInputBuffer, keyboard_input_count);
		     	 }
			}
		}    
    }  
	
	 //DrawAsciiCharUseBuffer(pDeskBuffer, DISPLAY_KEYBOARD_X, DISPLAY_KEYBOARD_Y, uniChar, Color);
	 
	 GraphicsLayerCompute(iMouseX, iMouseY, 0);
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
 
     UINT8 Buffer[DISK_BUFFER_SIZE];
 
     UINT8 Bufferout[DISK_BUFFER_SIZE * EXBYTE];
 
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
 
                 Status = DiskIo2->ReadDiskEx( DiskIo2, BlockIo2->Media->MediaId, disk_handle_task.DiskIo2Token.Event, 0, DISK_BUFFER_SIZE, Buffer );
				  
                 //Print( L"%d :%X\n", __LINE__ ,Status );
 
                 if ( EFI_SUCCESS == Status )
                 {
                     Buffer[10] = '\0';
                     //Print( L"%d :%s\n", __LINE__ , Buffer );
 
                     DecToCharBuffer1(Buffer, DISK_BUFFER_SIZE, Bufferout);
                    
                     ShowHex1(Bufferout, DISK_BUFFER_SIZE);
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
HandleMouseEvent (IN EFI_EVENT Event, IN VOID *Context)
{
	mouse_count++;
	//DebugPrint1(DISPLAY_X, DISPLAY_Y, "%d: HandleMouseEvent\n", __LINE__);
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
        DEBUG ((EFI_D_INFO, "Left button clicked\n"));
        
	    HandleMouseRightClick(iMouseX, iMouseY);

		if (MouseClickFlag == 0)
		    MouseClickFlag = 1;
		else if (MouseClickFlag == 1)
			MouseClickFlag = 0;
	    //DrawAsciiCharUseBuffer(GraphicsOutput, 20 + process2_i * 8 + 16, 60, 'E', Color);  
    }
    
    if (State.RightButton == 0x01)
    {
        DEBUG ((EFI_D_INFO, "Right button clicked\n"));
	    //DrawAsciiCharUseBuffer(GraphicsOutput, 20 + process2_i * 8 + 16, 60, 'R', Color);

	    HandleMouseRightClick(iMouseX, iMouseY);
	    
	    MouseClickFlag = 2;
    }
    //DebugPrint1(DISPLAY_X, DISPLAY_Y, "%d: HandleMouseEvent\n", __LINE__);
    //DEBUG ((EFI_D_INFO, "\n"));
    GraphicsLayerCompute(iMouseX, iMouseY, MouseClickFlag);
	
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
		
    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
	
	DEBUG ((EFI_D_INFO, "%d: MemoryParameterGet\n", __LINE__));
 	
	do 
	{
		DEBUG ((EFI_D_INFO, "%d: \n", __LINE__));
		EfiMemoryMap = AllocatePool (EfiMemoryMapSize);
		if (EfiMemoryMap == NULL)
		{
			DEBUG ((EFI_D_ERROR, "ERROR!! Null Pointer returned by AllocatePool ()\n"));
			ASSERT_EFI_ERROR (EFI_OUT_OF_RESOURCES);
			return ;
		}
		Status = gBS->GetMemoryMap (&EfiMemoryMapSize,
										EfiMemoryMap,
										&EfiMapKey,
										&EfiDescriptorSize,
										&EfiDescriptorVersion);
		DEBUG ((EFI_D_INFO, "%d: Status:%x\n", __LINE__, Status));
		if (EFI_ERROR(Status)) 
		{
			FreePool (EfiMemoryMap);
			//return EFI_SUCCESS;
		}
	} while (Status == EFI_BUFFER_TOO_SMALL);

	DEBUG((DEBUG_ERROR | DEBUG_PAGE, "%d EfiMemoryMapSize=0x%x EfiDescriptorSize=0x%x EfiMemoryMap=0x%x \n", __LINE__, EfiMemoryMapSize, EfiDescriptorSize, (UINTN)EfiMemoryMap));

	EFI_MEMORY_DESCRIPTOR *EfiMemoryMapEnd = (EFI_MEMORY_DESCRIPTOR *)((UINT8 *)EfiMemoryMap + EfiMemoryMapSize);
	EFI_MEMORY_DESCRIPTOR *EfiEntry = EfiMemoryMap;

	DEBUG((DEBUG_ERROR | DEBUG_PAGE,"%d ===========================%S============================== Start\n", __LINE__));

	UINT8 count = 0;
	while (EfiEntry < EfiMemoryMapEnd) 
	{
		DEBUG ((EFI_D_INFO, "%d Type:%3d Start:%16lx Count:%16lx count:%d \n", __LINE__, 
									EfiEntry->Type, 
									EfiEntry->PhysicalStart, 
									EfiEntry->NumberOfPages, 
									count++));
		EfiEntry++;
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
	parameter_count++;
	//DebugPrint1(DISPLAY_X, DISPLAY_Y, "%d: SystemParameterRead\n", __LINE__);
	//ShellServiceRead();
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
   
   /*DebugPrint1(DISPLAY_DESK_DATE_TIME_X - 250, DISPLAY_DESK_DATE_TIME_Y - 26, "%d: time: %8ld keyboard: %8ld mouse: %8ld parameter:%8ld\n", __LINE__,
   																				date_time_count,
   																				keyboard_count,
   																				mouse_count,
   																				parameter_count);*/
   GraphicsLayerCompute(iMouseX, iMouseY, 0);
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
		//DebugPrint1(DISPLAY_X, DISPLAY_Y, "%d: SystemTimeIntervalInit while\n", __LINE__);
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
    Color.Reserved = GRAPHICS_LAYER_DESK;
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

	HZK16FileReadCount = 0;
	FAT32_Table = NULL;

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

EFI_STATUS InitChineseChar()
{

	for (int i = 0; i < 5; i++)
    {
		DEBUG ((EFI_D_INFO, "%d HandleEnterPressed FSM_Event: %d\n", __LINE__, FSM_Event));
	    FileReadFSM(FSM_Event++);

	    if (READ_FILE_EVENT <= FSM_Event)
	    	FSM_Event = READ_FILE_EVENT;
	}

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
    
    //PartitionRead();    
    
    MultiProcessInit();

    InitChineseChar();
    
    MyComputerWindow(100, 100);

    SystemTimeIntervalInit();	
	
	//DebugPrint1(100, 100, "%d %d\n", __LINE__, Status);
	
    return EFI_SUCCESS;
}


