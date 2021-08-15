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

#define INFO_SELF(...)   \
			do {   \
				  Print(L"%d %a %a: ",__LINE__, __FILE__, __FUNCTION__);  \
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
#define EXBYTE 4

#define FILE_SYSTEM_OTHER   0xff
#define FILE_SYSTEM_FAT32  1
#define FILE_SYSTEM_NTFS   2


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

UINTN PartitionCount = 0;

CHAR8 x_move = 0;
CHAR8 y_move = 0;

UINT16 DebugPrintX = 0;
UINT16 DebugPrintY = 0; 

CHAR8    AsciiBuffer[0x100] = {0};

EFI_GRAPHICS_OUTPUT_PROTOCOL       *GraphicsOutput = NULL;
EFI_SIMPLE_POINTER_PROTOCOL        *gMouse;

extern EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *gSimpleFileSystem; 


EFI_HANDLE *handle;

UINT8 *pDeskBuffer = NULL; //Desk layer: 1
UINT8 *pMyComputerBuffer = NULL; // MyComputer layer: 2
UINT8 *pDeskDisplayBuffer = NULL; //desk display after multi graphicses layers compute
UINT8 *pMouseSelectedBuffer = NULL;  // after mouse selected
UINT8 *pMouseClickBuffer = NULL; // for mouse click 
UINT8 *pDateTimeBuffer = NULL; //Mouse layer: 3
UINT8 *pMouseBuffer = NULL; //Mouse layer: 4

// Read File 
UINT8 ReadFileName[20];
UINT8 *pReadFileDestBuffer = NULL;
UINT8 ReadFileNameLength = 0;
UINT16 FileReadCount = 0;

UINT8 *pDeskWallpaperBuffer = NULL;


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
	UINT8 JMP[3] ; // 0x00 3 ��תָ�������ͷһ������
	UINT8 OEM[8] ; // 0x03 8 OEM���Ƴ���ֵ��MSDOS5.0.
	UINT8 BitsOfSector[2] ; // 0x0b 2 ÿ���������ֽ�����ȡֵֻ�������¼��֣�512��1024��2048����4096����Ϊ512��ȡ����õļ�����
	UINT8 SectorOfCluster[1] ; // 0x0d 1 ÿ���������� ��ֵ������2�������η���ͬʱ��Ҫ��֤ÿ�ص��ֽ������ܳ���32K
	UINT8 ReservedSelector[2] ; // 0x0e 2 ��������������������������������Ϊ0��FAT12/FAT16����Ϊ1��FAT32�ĵ���ֵȡΪ32
	UINT8 NumFATS[1] ; // 0x10 1 �ļ��������Ŀ�� NumFATS���κ�FAT��ʽ������Ϊ2
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

/*
    struct for NTFS file system
*/

//
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


typedef struct 
{
	UINT16 BitsOfSector;
	UINT16 SectorOfCluster;	
	UINT64 AllSectorCount;
	UINT64 MFT_StartCluster;
	UINT64 MFT_MirrStartCluster;
}DollarBootSwitched;


// MFT��¼��������������ļ� (����MFT���������ļ����ļ��еȵ�) ��Ϣ�������ռ�ռ�ã��ļ��������ԣ��ļ�λ������������ʱ
// MFT �ĵ�һ���¼$MFT����������������MFT�������ı��Ϊ0��MFT���ͷ���������½ṹ��
typedef struct {
	 UINT8    mark[4];             // "FILE" ��־ 
	 UINT8    UsnOffset[2];        // �������к�ƫ�� ��������30 00
	 UINT8    usnSize[2];          // �������������С+1 �� 03 00
	 UINT8    LSN[8];              // ��־�ļ����к�(ÿ�μ�¼�޸ĺ�ı�)  58 8E 0F 34 00 00 00 00
	// 0x10
	 UINT8    SN[2];               // ���к� �����ļ����¼���ô���������
	 UINT8    linkNum[2];          // Ӳ������ (����Ŀ¼ָ����ļ�) 01 00
	 UINT8    firstAttr[2];        // ��һ�����Ե�ƫ�ơ���38 00
	 UINT8    flags[2];            // 0��ɾ�� 1�����ļ� 2��ɾ��Ŀ¼ 3Ŀ¼��ʹ��
	// 0x18
	 UINT8    MftUseLen[4];        // ��¼��Ч����   ��A8 01 00 00
	 UINT8    maxLen[4];            // ��¼ռ�ó��� �� 00 04 00 00
	// 0x20
	 UINT8    baseRecordNum[8];     // ����������¼, ����ǻ�����¼��Ϊ0
	 UINT8    nextAttrId[2];        // ��һ����Id����07 00
	 UINT8    border[2];            //
	 UINT8    xpRecordNum[4];       // ����xp, ��¼��
	// 0x30
	 UINT8    USN[8];                 // �������к�(2B) �� ������������
}MFT_HEADER;

/*
	MFT ����һ���� MFT ��(��¼)����ɵģ�����ÿ���С�ǹ̶���(һ��Ϊ1KB = 2 * 512)��MFT������ǰ16�����������ļ���¼����ΪԪ���ݣ�
	Ԫ�����ڴ����������������ģ����Ϊ0~15�����$MFT��ƫ��Ϊ0x0C0000000, ��ô��һ���ƫ�ƾ���0x0C0000400������һ�����
	0x0C0000800�ȵȣ�
*/

//------------------  ����ͷͨ�ýṹ ----
typedef struct  //����ƫ������Ϊ������������� Type ��ƫ����
{
	 UINT8 Type[4];           // �������� 0x10, 0x20, 0x30, 0x40,...,0xF0,0x100
	 UINT8 Length[4];         // ���Եĳ���
	 UINT8 NonResidentFiag;   // �Ƿ��Ƿǳ�פ���ԣ�l Ϊ�ǳ�פ���ԣ�0 Ϊ��פ���� 00
	 UINT8 NameLength;        // �������Ƴ��ȣ�������������ƣ���ֵΪ 00
	 UINT8 ContentOffset[2];  // �������ݵ�ƫ����  18 00
	 UINT8 CompressedFiag[2]; // ���ļ���¼��ʾ���ļ������Ƿ�ѹ���� 00 00
	 UINT8 Identify[2];       // ʶ���־  00 00
	//--- 0ffset: 0x10 ---
	//--------  ��פ���Ժͷǳ�פ���ԵĹ������� ----
	union CCommon
	{
	
		//---- ���������Ϊ ��פ ����ʱʹ�øýṹ ----
		struct CResident
		{
			 UINT8 StreamLength[4];        // ����ֵ�ĳ���, �����Ծ������ݵĳ��ȡ�"48 00 00 00"
			 UINT8 StreamOffset[2];        // ����ֵ��ʼƫ����  "18 00"
			 UINT8 IndexFiag[2];           // �����Ƿ�����������������������һ������(��Ŀ¼)�Ļ������  00 00
		};
		
		//------- ���������Ϊ �ǳ�פ ����ʱʹ�øýṹ ----
		struct CNonResident
		{
			 UINT8 StartVCN[8];            // ��ʼ�� VCN ֵ(����غţ���һ���ļ��е��ڲ��ر��,0��
			 UINT8 LastVCN[8];             // ���� VCN ֵ
			 UINT8 RunListOffset[2];       // �����б��ƫ����
			 UINT8 CompressEngineIndex[2]; // ѹ�����������ֵ��ָѹ��ʱʹ�õľ������档
			 UINT8 Reserved[4];
			 UINT8 StreamAiiocSize[8];     // Ϊ����ֵ����Ŀռ� ����λΪB��ѹ���ļ�����ֵС��ʵ��ֵ
			 UINT8 StreamRealSize[8];      // ����ֵʵ��ʹ�õĿռ䣬��λΪB
			 UINT8 StreamCompressedSize[8]; // ����ֵ����ѹ����Ĵ�С, ��δѹ��, ��ֵΪʵ��ֵ
		};
	};
}NTFSAttribute;

struct Value0x10
{
    UINT8 fileCreateTime[8];    // �ļ�����ʱ��
    UINT8 fileChangeTime[8];    // �ļ��޸�ʱ��
    UINT8 MFTChangeTime[8];     // MFT�޸�ʱ��
    UINT8 fileLatVisited[8];    // �ļ�������ʱ��
    UINT8 tranAtrribute[4];     // �ļ���ͳ����
    UINT8 otherInfo[28];        // �汾�������ߣ�����ȫ�ȵ���Ϣ(��ϸ��)
    UINT8 updataNum[8];         // �ļ��������к�
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

/*Ŀ¼�����ֽں���*/
enum DirFirstChar
{
	DirUnUsed            = 0x00,        /*������û��ʹ��*/
	DirCharE5            = 0x05,        /*���ַ�Ϊ0xe5*/
	DirisSubDir          = 0x2e,        /*��һ����Ŀ¼ .,..Ϊ��Ŀ¼*/
	DirFileisDeleted     = 0xe5        /*�ļ���ɾ��*/
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

UINT8 *FAT32_Table = NULL;

UINT8 *sChineseChar = NULL;
UINT8 *pWallPaperBuffer = NULL;
//UINT8 sChineseChar[267616];

static UINTN ScreenWidth, ScreenHeight;  
UINT16 MyComputerWidth = 16 * 50;
UINT16 MyComputerHeight = 16 * 40;
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



int iMouseX = 0;
int iMouseY = 0;

#pragma  pack(1)
typedef struct     //����ṹ����Ƕ������Ǹ�ͼ��һ����װ��
{
    //bmp header
    UINT8  Signatue[2] ;   // B  M
    UINT8 FileSize[4] ;     //�ļ���С
    UINT8 Reserv1[2] ; 
    UINT8 Reserv2[2] ; 
    UINT8 FileOffset[4] ;   //�ļ�ͷƫ����

    //DIB header
    UINT8 DIBHeaderSize[4] ; //DIBͷ��С
    UINT8 ImageWidth[4]   ;  //�ļ����
    UINT8 ImageHight[4]   ;  //�ļ��߶�
    UINT8 Planes[2]       ; 
    UINT8 BPP[2]          ;  //ÿ�����ص��λ��
    UINT8 Compression[4]  ; 
    UINT8 ImageSize[4]    ;  //ͼ�ļ���С
    UINT8 XPPM[4] ; 
    UINT8 YPPM[4] ; 
    UINT8 CCT[4] ; 
    UINT8 ICC[4] ;          
}BMP_HEADER;
#pragma  pack()


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


// we need a FSM��Finite State Machine��to Read content of file

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


// С��ģʽ
// byteתint  
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

	if (NULL == pDeskBuffer)
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
	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: StrCmpSelf\n", __LINE__);
	
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
	memcpy(&pItems, p, DISK_BUFFER_SIZE);
	UINT16 valid_count = 0;
	
	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: .: %d\n", __LINE__, ReadFileName[7]);

	//display filing file and subpath. 
	for (int i = 0; i < 30; i++)
		if (pItems[i].FileName[0] != 0xE5 && (pItems[i].Attribute[0] == 0x20 
		    || pItems[i].Attribute[0] == 0x10))
       {
        	DebugPrint1(DISK_MBR_X, 16 * 37 + (valid_count) * 16, "%d FileName:%2c%2c%2c%2c%2c%2c%2c%2c ExtensionName:%2c%2c%2c StartCluster:%02X%02X%02X%02X FileLength: %02X%02X%02X%02X Attribute: %02X    ", __LINE__,
                                            pItems[i].FileName[0], pItems[i].FileName[1], pItems[i].FileName[2], pItems[i].FileName[3], pItems[i].FileName[4], pItems[i].FileName[5], pItems[i].FileName[6], pItems[i].FileName[7],
                                            pItems[i].ExtensionName[0], pItems[i].ExtensionName[1],pItems[i].ExtensionName[2],
                                            pItems[i].StartClusterHigh2B[0], pItems[i].StartClusterHigh2B[1],
                                            pItems[i].StartClusterLow2B[0], pItems[i].StartClusterLow2B[1],
                                            pItems[i].FileLength[0], pItems[i].FileLength[1], pItems[i].FileLength[2], pItems[i].FileLength[3],
                                            pItems[i].Attribute[0]);
			
			valid_count++;

			UINT8 FileName[12] = {0};
			int count = 0;
			while (pItems[i].FileName[count] != 0)
			{
				FileName[count] = pItems[i].FileName[count];
				count++;
			}

			if (pItems[i].ExtensionName[0] != 0)
			{
				FileName[count] = ' ';
				count++;
			}
			int count2 = 0;
			while (pItems[i].ExtensionName[count2] != 0)
			{
				FileName[count] = pItems[i].ExtensionName[count2];
				count++;
				count2++;
			}

			DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: FileName: %a\n", __LINE__, FileName);
			DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: ReadFileName: %a\n", __LINE__, ReadFileName);

			//for (int j = 0; j < 5; j++)
			//	DebugPrint1(j * 3 * 8, 16 * 40 + valid_count * 16, "%02X ", pItems[i].FileName[j]);
			if (StrCmpSelf(FileName, ReadFileName, ReadFileNameLength) == EFI_SUCCESS)			
			{
	        	DebugPrint1(DISK_MBR_X, 16 * 30 + (valid_count) * 16 + 4 * 16, "%d FileName:%2c%2c%2c%2c%2c%2c%2c%2c ExtensionName:%2c%2c%2c StartCluster:%02X%02X%02X%02X FileLength: %02X%02X%02X%02X Attribute: %02X    ",  __LINE__,
                                pItems[i].FileName[0], pItems[i].FileName[1], pItems[i].FileName[2], pItems[i].FileName[3], pItems[i].FileName[4], pItems[i].FileName[5], pItems[i].FileName[6], pItems[i].FileName[7],
                                pItems[i].ExtensionName[0], pItems[i].ExtensionName[1],pItems[i].ExtensionName[2],
                                pItems[i].StartClusterHigh2B[0], pItems[i].StartClusterHigh2B[1],
                                pItems[i].StartClusterLow2B[0], pItems[i].StartClusterLow2B[1],
                                pItems[i].FileLength[0], pItems[i].FileLength[1], pItems[i].FileLength[2], pItems[i].FileLength[3],
                                pItems[i].Attribute[0]);
				
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
		//if (pItems[i].FileName[0] != 0xE5 && (pItems[i].Attribute[0] == 0x20 
		//    || pItems[i].Attribute[0] == 0x10))
       {
        	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d FileName:%2c%2c%2c%2c%2c%2c%2c%2c ExtensionName:%2c%2c%2c StartCluster:%02X%02X%02X%02X FileLength: %02X%02X%02X%02X Attribute: %02X    ",  __LINE__,
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

// �ļ���¼ͷ
typedef struct
{
	/*+0x00*/ UINT8 Type[4];    // �̶�ֵ'FILE'
	/*+0x04*/ UINT8 USNOffset[2]; // �������к�ƫ��, �����ϵͳ�й�
	/*+0x06*/ UINT8 USNCount[2]; // �̶��б��СSize in words of Update Sequence Number & Array (S)
	/*+0x08*/ UINT8 Lsn[8]; // ��־�ļ����к�(LSN)
	/*+0x10*/ UINT8 SequenceNumber[2]; // ���к�(���ڼ�¼�ļ�������ʹ�õĴ���)
	/*+0x12*/ UINT8 LinkCount[2];// Ӳ������
	/*+0x14*/ UINT8 AttributeOffset[2]; // ��һ������ƫ��
	/*+0x16*/ UINT8 Flags[2];// flags, 00��ʾɾ���ļ�,01��ʾ�����ļ�,02��ʾɾ��Ŀ¼,03��ʾ����Ŀ¼
	/*+0x18*/ UINT8 BytesInUse[4]; // �ļ���¼ʵʱ��С(�ֽ�) ��ǰMFT�����,��FFFFFF�ĳ���+4
	/*+0x1C*/ UINT8 BytesAllocated[4]; // �ļ���¼�����С(�ֽ�)
	/*+0x20*/ UINT8 BaseFileRecord[8]; // = 0 �����ļ���¼ File reference to the base FILE record
	/*+0x28*/ UINT8 NextAttributeNumber[2]; // ��һ������ID��
	/*+0x2A*/ UINT8 Pading[2]; // �߽�
	/*+0x2C*/ UINT8 MFTRecordNumber[4]; // windows xp��ʹ��,��MFT��¼��
	/*+0x30*/ UINT8 USN[2]; // �������к�
	/*+0x32*/ UINT8 UpdateArray[0]; // ��������
 } FILE_HEADER, *pFILE_HEADER; 

typedef struct  
{
	UINT8 Type[4];   //��������
	UINT8 Size[4];   //����ͷ����������ܳ���
	UINT8 ResidentFlag; //�Ƿ��ǳ�פ���ԣ�0��פ 1�ǳ�פ��
	UINT8 NameSize;   //�������ĳ���
	UINT8 NameOffset[2]; //��������ƫ�� ���������ͷ
	UINT8 Flags[2]; //��־��0x0001ѹ�� 0x4000���� 0x8000ϡ�裩
	UINT8 Id[2]; //����ΨһID
}CommonAttributeHeader;

int display_sector_number = 0;
typedef struct 
{
	UINT8 OccupyCluster;
	UINT64 Offset; //Please Note: The first item is start offset, and next item is relative offset....
}IndexInformation;

UINT16 BytesToInt3(UINT8 *bytes)
{
	//INFO_SELF("%x %x %x\n", bytes[0], bytes[1], bytes[2]);
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
        //INFO_SELF(" i: %d\n", i);
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
        //INFO_SELF(" i: %d\n", i);
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

	// location of a0 attribute may be in front of 10 
	for (int i = 0; i < 10; i++)
	{
		UINT8 size[4];
		for (int i = 0; i < 4; i++)
			size[i] = p[AttributeOffset + 4 + i];
		//INFO_SELF("%02X%02X%02X%02X\n", size[0], size[1], size[2], size[3]);
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
    UINT8 Flag[4]; //�̶�ֵ "INDX"
    UINT8 USNOffset[2];//�������к�ƫ��
    UINT8 USNSize[2];//�������кź͸��������С
    UINT8 LogSequenceNumber[8]; // ��־�ļ����к�(LSN)
    UINT8 IndexCacheVCN[8];//�����������������������е�VCN
    UINT8 IndexEntryOffset[4];//�������ƫ�� ����ڵ�ǰλ��
    UINT8 IndexEntrySize[4];//������Ĵ�С
    UINT8 IndexEntryAllocSize[4];//���������Ĵ�С
    UINT8 HasLeafNode;//��һ ��ʾ���ӽڵ�
    UINT8 Fill[3];//���
    UINT8 USN[2];//�������к�
    UINT8 USNArray[0];//������������
}INDEX_HEADER;

typedef struct {
     UINT8 MFTReferNumber[8];//�ļ���MFT�ο���
     UINT8 IndexEntrySize[2];//������Ĵ�С
     UINT8 FileNameAttriBodySize[2];//�ļ���������Ĵ�С
     UINT8 IndexFlag[2];//������־
     UINT8 Fill[2];//���
     UINT8 FatherDirMFTReferNumber[8];//��Ŀ¼MFT�ļ��ο���
     UINT8 CreatTime[8];//�ļ�����ʱ�� 8
     UINT8 AlterTime[8];//�ļ�����޸�ʱ��
     UINT8 MFTChgTime[8];//�ļ���¼����޸�ʱ��
     UINT8 ReadTime[8];//�ļ�������ʱ��
     UINT8 FileAllocSize[8];//�ļ������С
     UINT8 FileRealSize[8];//�ļ�ʵ�ʴ�С
     UINT8 FileFlag[8];//�ļ���־
     UINT8 FileNameSize;//�ļ�������
     UINT8 FileNamespace;//�ļ������ռ�
     UINT8 FileNameAndFill[0];//�ļ��������
}INDEX_ITEM;


EFI_STATUS  MFTIndexItemsBufferAnalysis(UINT8 *pBuffer)
{
	UINT8 *p = NULL;
	
	p = (UINT8 *)AllocateZeroPool(DISK_BUFFER_SIZE * 8);

	if (NULL == p)
	{
		DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d NULL == p\n", __LINE__);
		return EFI_SUCCESS;
	}
	
	//memcpy(p, pBuffer[512 * 2], DISK_BUFFER_SIZE);
	for (UINT16 i = 0; i < 512 * 8; i++)
		p[i] = pBuffer[i];

	//IndexEntryOffset:�������ƫ�� ����ڵ�ǰλ��
	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d IndexEntryOffset: %llu IndexEntrySize: %llu\n", __LINE__, 
																     BytesToInt4(((INDEX_HEADER *)p)->IndexEntryOffset),
																     BytesToInt4(((INDEX_HEADER *)p)->IndexEntrySize));

	// ����ڵ�ǰλ�� need to add size before this Byte.
	UINT8 length = BytesToInt4(((INDEX_HEADER *)p)->IndexEntryOffset) + 24;
    UINT8 pItem[200] = {0};
    UINT16 index = length;

	for (UINT8 i = 0; ; i++)
	{    
		 if (index >= BytesToInt4(((INDEX_HEADER *)p)->IndexEntrySize))
			break;
			
		 //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%s index: %d\n", __LINE__,  index);  
		 UINT16 length2 = pBuffer[index + 8] + pBuffer[index + 9] * 16;
		 
        for (int i = 0; i < length2; i++)
            pItem[i] = pBuffer[index + i];
            
		 UINT8 FileNameSize =	 ((INDEX_ITEM *)pItem)->FileNameSize;
		 //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d attribut length2: %d FileNameSize: %d\n", __LINE__, 
		//														     length2,
		//														     FileNameSize);    
		 UINT8 attributeName[20];																     
		 for (int i = 0; i < FileNameSize; i++)
		 {
		 	attributeName[i] = pItem[82 + 2 * i];
		 }
		 DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: attributeName: %a\n", __LINE__, attributeName);
		 //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%s attributeName: %a\n", __LINE__,  attributeName);  
		 index += length2;
	}
}


long long Multi(long x, long y)
{
	return x * y;
}

// string transform into long long
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

//Note: ReadSize will be multi with 512
EFI_STATUS ReadDataFromPartition(UINT8 deviceID, UINT64 StartSectorNumber, UINT16 ReadSize, UINT8 *pBuffer)
{
	if (StartSectorNumber > device[deviceID].SectorCount)
	{
		DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: deviceID: %d StartSectorNumber: %ld ReadSize: %d\n", __LINE__, deviceID, StartSectorNumber, ReadSize);
		return -1;
	}

	//DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: deviceID: %d StartSectorNumber: %ld ReadSize: %d\n", __LINE__, deviceID, StartSectorNumber, ReadSize);
	EFI_STATUS Status;
    UINTN NumHandles;
    EFI_BLOCK_IO_PROTOCOL *BlockIo;
    EFI_DISK_IO_PROTOCOL *DiskIo;
    EFI_HANDLE *ControllerHandle = NULL;
    
    Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiDiskIoProtocolGuid, NULL, &NumHandles, &ControllerHandle);
    if (EFI_ERROR(Status))
    {
        DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);	    
        return Status;
    }

 	Status = gBS->HandleProtocol(ControllerHandle[deviceID], &gEfiBlockIoProtocolGuid, (VOID * *) &BlockIo );   
    if (EFI_ERROR(Status))
    {
		DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		return Status;
    }
    
    Status = gBS->HandleProtocol( ControllerHandle[deviceID], &gEfiDiskIoProtocolGuid, (VOID * *) &DiskIo );     
    if (EFI_ERROR(Status))
    {
		DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		return Status;
    }

   	Status = DiskIo->ReadDisk(DiskIo, BlockIo->Media->MediaId, DISK_BUFFER_SIZE * (StartSectorNumber), DISK_BUFFER_SIZE * ReadSize, pBuffer);
 	if (EFI_ERROR(Status))
 	{
		DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X sector_count:%ld\n", __LINE__, Status, sector_count);
		return Status;
 	}
 	
	for (int j = 0; j < 250; j++)
	{
		DebugPrint1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", pBuffer[j] & 0xff);
	}
	//INFO_SELF("\n");
    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
    return EFI_SUCCESS;
}




UINT8 BufferMFT[DISK_BUFFER_SIZE * 2];

// NTFS Main File Table items analysis
EFI_STATUS MFTReadFromPartition(UINT16 DeviceID)
{
    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d DeviceID: %d\n", __LINE__, DeviceID);
    //printf( "RootPathAnalysis\n" );
    DEBUG ((EFI_D_INFO, "PartitionUSBRead!!\r\n"));
    EFI_STATUS Status ;
    EFI_HANDLE *ControllerHandle = NULL;
        
    Status = ReadDataFromPartition(DeviceID, sector_count + 5 *2, 2, BufferMFT); 
    if (EFI_ERROR(Status))
    {
    	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d Status: %X\n", __LINE__, Status);
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
      
    return EFI_SUCCESS;
}


// analysis a partition 
EFI_STATUS RootPathAnalysisFSM1(UINT16 DeviceID)
{
    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d DeviceID: %d\n", __LINE__, DeviceID);
    //printf( "RootPathAnalysis\n" );
    EFI_STATUS Status ;
    UINT8 Buffer1[DISK_BUFFER_SIZE];
    
    Status = ReadDataFromPartition(DeviceID, sector_count, 1, Buffer1); 
	if (EFI_ERROR(Status))
    {
    	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d Status: %X\n", __LINE__, Status);
    	return Status;
    }
    
 	//When get root path data sector start number, we can get content of root path.
 	RootPathAnalysis1(Buffer1);	

	// data area start from 1824, HZK16 file start from 	FileBlockStart	block, so need to convert into sector by multi 8, block start number is 2 	
	// next state is to read FAT table
 	sector_count = MBRSwitched.ReservedSelector;
 	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: sector_count:%ld FileLength: %d MBRSwitched.ReservedSelector:%ld\n",  __LINE__, sector_count, FileLength, MBRSwitched.ReservedSelector);

    return EFI_SUCCESS;
}

EFI_STATUS NTFSRootPathIndexItemsRead(UINT8 i)
{
	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d DeviceType: %d, SectorCount: %lld\n", __LINE__, i);
    EFI_STATUS Status ;
    UINT8 BufferBlock[DISK_BLOCK_BUFFER_SIZE];

	UINT8 k = 0;
	UINT16 lastOffset = 0;
	
	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Offset:%ld OccupyCluster:%ld\n",  __LINE__, 
																	 (A0Indexes[k].Offset + lastOffset) * 8, 
																	 A0Indexes[k].OccupyCluster * 8);
	// cluster need to multi with 8 then it is sector.
   Status = ReadDataFromPartition(i, (A0Indexes[k].Offset + lastOffset) * 8 , A0Indexes[k].OccupyCluster * 8, BufferBlock);
    if (EFI_ERROR(Status))
    {
        DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d Status: %X\n", __LINE__, Status);
        return Status;
    }

	MFTIndexItemsBufferAnalysis(BufferBlock);	
	
 	lastOffset = A0Indexes[k].Offset;
	
    return EFI_SUCCESS;
}

EFI_STATUS BufferAnalysis(UINT8 *p, MasterBootRecordSwitched *pMBRSwitched)
{
	MasterBootRecord *pMBR;
	
	pMBR = (MasterBootRecord *)AllocateZeroPool(DISK_BUFFER_SIZE);
	memcpy(pMBR, p, DISK_BUFFER_SIZE);

	// ����ֽ��򣺵�λ�ֽ��ڸߵ�ַ����λ�ֽڵ͵�ַ�ϡ����������д��ֵ�ķ�����
    // С���ֽ����������෴����λ�ֽ��ڵ͵�ַ����λ�ֽ��ڸߵ�ַ��
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

	// ����ֽ��򣺵�λ�ֽ��ڸߵ�ַ����λ�ֽڵ͵�ַ�ϡ����������д��ֵ�ķ�����
    // С���ֽ����������෴����λ�ֽ��ڵ͵�ַ����λ�ֽ��ڸߵ�ַ��
	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d ReservedSelector:%02X%02X SectorsPerFat:%02X%02X%02X%02X BootPathStartCluster:%02X%02X%02X%02X NumFATS: %X", 
	                                    __LINE__,
	                                    pMBR->ReservedSelector[0], pMBR->ReservedSelector[1], 
	                                    pMBR->SectorsPerFat[0], pMBR->SectorsPerFat[1], pMBR->SectorsPerFat[2], pMBR->SectorsPerFat[3],
	                                    pMBR->BootPathStartCluster[0], pMBR->BootPathStartCluster[1], pMBR->BootPathStartCluster[2], pMBR->BootPathStartCluster[3],
	                                    pMBR->NumFATS[0]);

	
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

    // ����ֽ��򣺵�λ�ֽ��ڸߵ�ַ����λ�ֽڵ͵�ַ�ϡ����������д��ֵ�ķ�����
    // С���ֽ����������෴����λ�ֽ��ڵ͵�ַ����λ�ֽ��ڸߵ�ַ��
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

#define FILE_SYSTEM_OTHER   0xff
#define FILE_SYSTEM_FAT32  1
#define FILE_SYSTEM_NTFS   2


// all partitions analysis
EFI_STATUS PartitionAnalysisFSM1(UINT16 DeviceID)
{    
    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d DeviceID: %d\n", __LINE__, DeviceID);
    EFI_STATUS Status;
    UINT8 Buffer1[DISK_BUFFER_SIZE] = {0};

    sector_count = 0;

	Status = ReadDataFromPartition(DeviceID, sector_count, 1, Buffer1 );    
    if (EFI_ERROR(Status))
    {
        DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d Status: %X\n", __LINE__, Status);
        return Status;
    }

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
   		return FILE_SYSTEM_FAT32;
	}
   	// NTFS
   	else if (Buffer1[3] == 'N' && Buffer1[4] == 'T' && Buffer1[5] == 'F' && Buffer1[6] == 'S')
   	{
		FirstSelectorAnalysisNTFS(Buffer1, &NTFSBootSwitched);
		sector_count = NTFSBootSwitched.MFT_StartCluster * 8;
   		DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: NTFS sector_count:%llu\n",  __LINE__, sector_count);
   		return FILE_SYSTEM_NTFS;
   	}
   	else
   	{
   		DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: \n",  __LINE__);
   		return FILE_SYSTEM_OTHER;
   	}               	

    return EFI_SUCCESS;

}

DisplayItemsOfPartition(UINT16 Index)
{
	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: \n",  __LINE__);
	// this code may be have some problems, because my USB file system is FAT32, my Disk file system is NTFS.
	// others use this code must be careful...
	UINT8 FileSystemType = PartitionAnalysisFSM1(Index);

	if (FileSystemType == FILE_SYSTEM_FAT32)
	{
		RootPathAnalysisFSM1(Index);
		UINT16 valid_count = 0;

		
		for (UINT16 i = 0; i < 32; i++)
			//if (pItems[i].FileName[0] != 0xE5 && (pItems[i].Attribute[0] == 0x20 
			//    || pItems[i].Attribute[0] == 0x10))
	       {
	        	DebugPrint2(0, 8 * 16 + (valid_count) * 16, pMyComputerBuffer, "%d %2c%2c%2c%2c%2c%2c%2c%2c.%2c%2c%2c FileLength: %d Attribute: %02X    ", __LINE__,
	                                            pItems[i].FileName[0], pItems[i].FileName[1], pItems[i].FileName[2], pItems[i].FileName[3], pItems[i].FileName[4], pItems[i].FileName[5], pItems[i].FileName[6], pItems[i].FileName[7],
	                                            pItems[i].ExtensionName[0], pItems[i].ExtensionName[1],pItems[i].ExtensionName[2],
	                                            BytesToInt4(pItems[i].FileLength),
	                                            pItems[i].Attribute[0]);
				
				valid_count++;
			}
		
	}
	else if (FileSystemType == FILE_SYSTEM_NTFS)
	{
		MFTReadFromPartition(Index);
    	MFTDollarRootFileAnalysisBuffer(BufferMFT);			
	    NTFSRootPathIndexItemsRead(Index);
	}
	else if (FileSystemType == FILE_SYSTEM_OTHER)
	{
		return;
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
		MyComputerPositionX += x_move * 3;
		MyComputerPositionY += y_move * 3;
		
	//	GraphicsCopy(pDeskDisplayBuffer, pMyComputerBuffer, ScreenWidth, ScreenHeight, MyComputerWidth, MyComputerHeight, MyComputerPositionX, MyComputerPositionX);
	}
	x_move = 0;
	y_move = 0;
	// display graphics layer id mouse over, for mouse click event.
	//DebugPrint1(DISPLAY_X, DISPLAY_Y, "%d: Graphics Layer id: %d ", __LINE__, pDeskDisplayBuffer[(iMouseY * ScreenWidth + iMouseX) * 4 + 3]);
	
	int i, j;

	GraphicsLayerMouseMove();
    
    // init mouse buffer with cursor
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

MouseMoveoverResponse()
{
 	//DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: iMouseX: %d iMouseY: %d \n",  __LINE__, iMouseX, iMouseY);
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
	Color.Red = 0xff;
	Color.Green= 0xff;
	Color.Blue= 0x00;
	
	//start button
    if (iMouseX >= 0 && iMouseX <= 16 + 16 * 2
        && iMouseY >= ScreenHeight - 21 && iMouseY <= ScreenHeight)
    {   
    	//DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: iMouseX: %d iMouseY: %d \n",  __LINE__, iMouseX, iMouseY);
		RectangleDrawIntoBuffer(pMouseSelectedBuffer, 0,  0, 31, 15, 1,  Color, 32);
    	//MenuButtonClickResponse();
    	Color.Red   = 0xFF;
	    Color.Green = 0xFF;
	    Color.Blue	= 0xFF;
	    //RectangleFillIntoBuffer(pDeskBuffer, 3,     ScreenHeight - 21, 13,     ScreenHeight - 11, 1, Color);
    	GraphicsCopy(pDeskDisplayBuffer, pMouseSelectedBuffer, ScreenWidth, ScreenHeight, 32, 16, 3, ScreenHeight - 21);
    }
    
	// this is draw a rectangle when mouse move on disk partition in my computer window
	for (UINT16 i = 0; i < PartitionCount; i++)
	{		
		if (iMouseX >= MyComputerPositionX + 50 && iMouseX <= MyComputerPositionX + 50 + 16 * 6
			&& iMouseY >= MyComputerPositionY + i * 16 + 16 * 2 && iMouseY <= MyComputerPositionY + i * 16 + 16 * 3)
		{
			
			if (PreviousItem == i)
			{
				break;
			}
			
			RectangleDrawIntoBuffer(pMouseSelectedBuffer, 0,  0, 31, 15, 1,  Color, 32);

			// need to save result into cache, for next time to read, reduce disk operation
			DisplayItemsOfPartition(i);
			PreviousItem = i;
	       GraphicsCopy(pDeskDisplayBuffer, pMouseSelectedBuffer, ScreenWidth, ScreenHeight, 32, 16, MyComputerPositionX + 50, MyComputerPositionY  + i * (16 + 2) + 16 * 2);	
		}
	}

}

MouseClickResponse()
{
    //my computer
    if (MouseClickFlag == 1 && pDeskDisplayBuffer[(iMouseY * ScreenWidth + iMouseX) * 4 + 3] == GRAPHICS_LAYER_MY_COMPUTER)
    {
        MyComputerPositionX += x_move * 3;
        MyComputerPositionY += y_move * 3;
        
    //  GraphicsCopy(pDeskDisplayBuffer, pMyComputerBuffer, ScreenWidth, ScreenHeight, MyComputerWidth, MyComputerHeight, MyComputerPositionX, MyComputerPositionX);
    }
    x_move = 0;
    y_move = 0;

}

MenuButtonClickResponse()
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

VOID GraphicsLayerMouseMove()
{	
    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: GraphicsLayerMouseMove\n",  __LINE__);
	
	// display graphics layer id mouse over, for mouse click event.
	//DebugPrint1(DISPLAY_X, DISPLAY_Y, "%d: Graphics Layer id: %d ", __LINE__, pDeskDisplayBuffer[(iMouseY * ScreenWidth + iMouseX) * 4 + 3]);
	
	MouseClickResponse();
    
	MouseMoveoverResponse();
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

EFI_EVENT MultiTaskTriggerGroup1Event;
EFI_EVENT MultiTaskTriggerGroup2Event;

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

Network()
{}

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


// all partitions analysis
EFI_STATUS PartitionAnalysis()
{    
    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d PartitionAnalysisFSM\n", __LINE__);
    EFI_STATUS Status ;
    UINTN i;
    EFI_HANDLE *ControllerHandle = NULL;
    EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;
    
    Status = gBS->LocateProtocol (&gEfiDevicePathToTextProtocolGuid, NULL, (VOID **) &DevPathToText);
    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
    if (EFI_ERROR(Status))
    {
    	 DEBUG ((EFI_D_INFO, "LocateProtocol1 error: %x\n", Status));    	 
    	 DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
        return Status;
    }
   
    Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiDiskIoProtocolGuid, NULL, &PartitionCount, &ControllerHandle);
    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
    if (EFI_ERROR(Status))
    {
        DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);	    
        return EFI_SUCCESS;
    }

    for (i = 0; i < PartitionCount; i++)
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
    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d PartitionAnalysisFSM\n", __LINE__);
    EFI_STATUS Status ;
    UINT8 Buffer1[DISK_BUFFER_SIZE];
    
    for (int i = 0; i < PartitionCount; i++)
	{
		if (device[i].DeviceType == 1 && device[i].SectorCount == 915551)
		{
        	 Status = ReadDataFromPartition(i, sector_count, 1, Buffer1); 
			 if ( EFI_SUCCESS == Status )
            {
	            DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
				  
			 	  // analysis data area of patition
			 	  FirstSelectorAnalysis(Buffer1, &MBRSwitched); 

			 	  // data sector number start include: reserved selector, fat sectors(usually is 2: fat1 and fat2), and file system boot path start cluster(usually is 2, data block start number is 2)
			 	  sector_count = MBRSwitched.ReservedSelector + MBRSwitched.SectorsPerFat * MBRSwitched.NumFATS + MBRSwitched.BootPathStartCluster - 2;
			 	  BlockSize = MBRSwitched.SectorOfCluster * 512;
	       		  DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: sector_count:%ld BlockSize: %d\n",  __LINE__, sector_count, BlockSize);
		     }        	 
            break;
		 }       
    }

    return EFI_SUCCESS;
}

// analysis a partition 
EFI_STATUS RootPathAnalysisFSM()
{
    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d RootPathAnalysisFSM\n", __LINE__);
    EFI_STATUS Status ;
    UINT8 Buffer1[DISK_BUFFER_SIZE];
    
    for (int i = 0; i < PartitionCount; i++)
    {
        if (device[i].DeviceType == 1 && device[i].SectorCount == 915551)
        {
            Status = ReadDataFromPartition(i, sector_count, 1, Buffer1); 
            if ( EFI_SUCCESS == Status )
            {
                 //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X\n", __LINE__, Status);
                  
                  //When get root path data sector start number, we can get content of root path.
				 	RootPathAnalysis(Buffer1);	

					// data area start from 1824, HZK16 file start from 	FileBlockStart	block, so need to convert into sector by multi 8, block start number is 2 	
					// next state is to read FAT table
				 	sector_count = MBRSwitched.ReservedSelector;
				 	//DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: sector_count:%ld FileLength: %d MBRSwitched.ReservedSelector:%ld\n",  __LINE__, sector_count, FileLength, MBRSwitched.ReservedSelector);
             }           
            break;
         }       
    }

    return EFI_SUCCESS;
}

// 
EFI_STATUS GetFatTableFSM()
{    
    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d GetFatTableFSM\n", __LINE__);
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
        if (device[i].DeviceType == 1 && device[i].SectorCount == 915551)
        {
        	 //start block need to recompute depends on file block start number 
        	 //FileBlockStart;
        	 DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d MBRSwitched.SectorsPerFat: %d\n", __LINE__, MBRSwitched.SectorsPerFat);
            
             // 512 = 16 * 32 = 4 item * 32
            FAT32_Table = (UINT8 *)AllocateZeroPool(DISK_BUFFER_SIZE * MBRSwitched.SectorsPerFat);
             if (NULL == FAT32_Table)
             {
                 DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: NULL == FAT32_Table\n", __LINE__);                             
             }             
        	 
            Status = ReadDataFromPartition(i, sector_count,  MBRSwitched.SectorsPerFat, FAT32_Table); 
            if ( EFI_SUCCESS == Status )
            {
                //CopyMem(FAT32_Table, Buffer1, DISK_BUFFER_SIZE * MBRSwitched.SectorsPerFat);
				  for (int j = 0; j < 250; j++)
				  {
				  		//DebugPrint1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", Buffer1[j] & 0xff);
				  }

				  // start sector of file
				  sector_count = MBRSwitched.ReservedSelector + MBRSwitched.SectorsPerFat * MBRSwitched.NumFATS + MBRSwitched.BootPathStartCluster - 2 + (FileBlockStart - 2) * 8;

				  // for FAT32_Table get next block number
				  PreviousBlockNumber = FileBlockStart;
				  //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: sector_count:%ld FileLength: %d PreviousBlockNumber: %d\n",  __LINE__, sector_count, FileLength, PreviousBlockNumber);
             }           
            break;
         }       
    }

    return EFI_SUCCESS;
}

UINT32 GetNextBlockNumber()
{
	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: PreviousBlockNumber: %d\n",  __LINE__, PreviousBlockNumber);
	
	if (FAT32_Table[PreviousBlockNumber * 4] == 0xff 
	    && FAT32_Table[PreviousBlockNumber * 4 + 1] == 0xff 
	    && FAT32_Table[PreviousBlockNumber * 4 + 2] == 0xff 
	    && FAT32_Table[PreviousBlockNumber * 4 + 3] == 0x0f)
	{
		DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: PreviousBlockNumber: %d, PreviousBlockNumber: %llX\n",  __LINE__, PreviousBlockNumber, 0x0fffffff);
		return 0x0fffffff;
	}
	
	return FAT32_Table[PreviousBlockNumber  * 4] + (UINT32)FAT32_Table[PreviousBlockNumber * 4 + 1] * 16 * 16 + (UINT32)FAT32_Table[PreviousBlockNumber * 4 + 2] * 16 * 16 * 16 * 16 + (UINT32)FAT32_Table[PreviousBlockNumber * 4 + 3] * 16 * 16 * 16 * 16 * 16 * 16;	
}

EFI_STATUS ChineseCharArrayInit()
{
	//DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: ChineseCharArrayInit\n",  __LINE__);
	DEBUG ((EFI_D_INFO, "%d ChineseCharArrayInit\n", __LINE__));
    // 87 line, 96 Chinese char each line, 32 Char each Chinese char
	
    
    //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: ChineseCharArrayInit finished...\n",  __LINE__);
	return EFI_SUCCESS;
}

void *memcopy(UINT8 *dest, const UINT8 *src, UINT8 count)
{
	UINT8 *d;
	const UINT8 *s;

	d = dest;
	s = src;
	while (count--)
		*d++ = *s++;        
	
	return dest;
}
//https://blog.csdn.net/goodwillyang/article/details/45559925

EFI_STATUS ReadFileFSM()
{    
    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d ReadFileFSM: PartitionCount: %d FileLength: %d\n", __LINE__, PartitionCount, FileLength);
    //DEBUG ((EFI_D_INFO, "PartitionUSBRead!!\r\n"));
    EFI_STATUS Status ;
    UINT8 BufferBlock[DISK_BLOCK_BUFFER_SIZE];
            
	for (int i = 0; i < PartitionCount; i++)
	{
		if (device[i].DeviceType == 1 && device[i].SectorCount == 915551)
		{
			// read from USB by block(512 * 8)
    	    // Read file content from FAT32(USB), minimum unit is block
    	    for (UINT16 k = 0; k < FileLength / (512 * 8); k++)
    	 	{
        	 	Status = ReadDataFromPartition(i, sector_count, 8, BufferBlock); 
				if (EFI_ERROR(Status))
			    {
			    	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d Status: %X\n", __LINE__, Status);
			    	return Status;
			    }        	 	
				
			     ChineseCharArrayInit();
			     
                //DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: HZK16FileReadCount: %d DISK_BLOCK_BUFFER_SIZE: %d\n", __LINE__, HZK16FileReadCount, DISK_BLOCK_BUFFER_SIZE);

				  //Copy buffer to ChineseBuffer
				  if (pReadFileDestBuffer != NULL)
				  {
				  		for (UINT16 j = 0; j < DISK_BLOCK_BUFFER_SIZE; j++)
				  			pReadFileDestBuffer[FileReadCount * DISK_BLOCK_BUFFER_SIZE + j] = BufferBlock[j];
                 }
				  
				  for (int j = 0; j < 250; j++)
				  {
				  		//DebugPrint1(DISK_READ_BUFFER_X + (j % 32) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 32), "%02X ", BufferBlock[j] & 0xff);
				  }
				  
				  FileReadCount++;
				  UINT32 NextBlockNumber = GetNextBlockNumber();
		 	 	  sector_count = MBRSwitched.ReservedSelector + MBRSwitched.SectorsPerFat * MBRSwitched.NumFATS + MBRSwitched.BootPathStartCluster - 2 + (NextBlockNumber - 2) * 8;
		 	 	  PreviousBlockNumber = NextBlockNumber;
		 	 	  DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: NextBlockNumber: %llu\n",  __LINE__, NextBlockNumber);
			 }
			 
			break;
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
    
	if (FileReadCount > FileLength / (512 * 8))
	{
		DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		return;
	}
	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: FileReadFSM current event: %d, NextState: %d table event:%d table NextState: %d\n", 
							  __LINE__, 
                            event, 
                            NextState,
                            StatusTransitionTable[NextState].event,
                            StatusTransitionTable[NextState].NextState);
    
	if ( event == StatusTransitionTable[NextState].event )
	{
		DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		StatusTransitionTable[NextState].pFunc();
		NextState = StatusTransitionTable[NextState].NextState;
	}
	else  
	{
		DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
		NextState = INIT_STATE;
	}

	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: Status:%X \n", __LINE__, Status);
	StatusErrorCount++;
}

UINT32 TimerSliceCount = 0;
VOID EFIAPI TimeSlice(
	IN EFI_EVENT Event,
	IN VOID           *Context
	)
{
    //DebugPrint1(0, 5 * 16, "%d TimeSlice %x %lu \n", __LINE__, Context, *((UINT32 *)Context));
    //DebugPrint1(0, 6 * 16, "%d TimerSliceCount: %lu \n", __LINE__, TimerSliceCount);
    //Print(L"%lu\n", *((UINT32 *)Context));
    if (TimerSliceCount % 2 == 0)
       gBS->SignalEvent (MultiTaskTriggerGroup1Event);
       
    if (TimerSliceCount % 20 == 0)
       gBS->SignalEvent (MultiTaskTriggerGroup2Event);
    
    //DEBUG ((EFI_D_INFO, "System time slice Loop ...\n"));
    //gBS->SignalEvent (MultiTaskTriggerEvent);

	TimerSliceCount++;
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

	Color.Blue  = 0x00;
	Color.Red   = 0x00;
	Color.Green = 0x00;
	int x, y;

	for (UINT16 i = 0 ; i < PartitionCount; i++)
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

		//  2354 �� 3988 ��
		DrawChineseCharIntoBuffer2(pBuffer, x, y,          (23 - 1 ) * 94 + 54 - 1, Color, Width);    
		x += 16;
		
		DrawChineseCharIntoBuffer2(pBuffer, x, y,          (39 - 1 ) * 94 + 88 - 1, Color, Width);   
		x += 16;

		// 5027
		// һ
		// 2294
		// ��
		// 4093
		// ��
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
			sizePostfix[0] = 'G';
		}

		//2083
		//��
		//4801
		//С
		DrawChineseCharIntoBuffer2(pBuffer, x, y,          (20 - 1 ) * 94 + 83 - 1, Color, Width);  
		x += 16;
		
		DrawChineseCharIntoBuffer2(pBuffer, x, y,          (48 - 1 ) * 94 + 1 - 1, Color, Width);  
		x += 16;

		//CHAR16 s[4] = L"��";
		DebugPrint2(x, y, pBuffer, "%d%a", size, sizePostfix);

	}
	
	y += 16;
	y += 16;
	
	x = 50;
	//3658
	//��
	//2070
	//��
	DrawChineseCharIntoBuffer2(pBuffer, x, y,          (36 - 1 ) * 94 + 58 - 1, Color, Width);  
	x += 16;
	
	DrawChineseCharIntoBuffer2(pBuffer, x, y,          (20 - 1 ) * 94 + 70 - 1, Color, Width);  
	x += 16;
	
	// Get memory infomation
	//x = 0;
	// Note: the other class memory can not use
	float size = (float)MemoryParameterGet2();
	size = size * 4;
	size = size / (1024 * 1024);
	CHAR8 buf[7] = {0};
	DebugPrint2(x, y, pBuffer, "%a", float2str(size, 3, buf));
	x += 5 * 8;

	//g
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
 
int FSM_Event = READ_PATITION_EVENT;

int flag = 0;

EFI_STATUS ReadFileSelf(UINT8 *FileName, UINT8 NameLength, UINT8 *pBuffer)
{
	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: FileName: %a \n", __LINE__, FileName);
	if (pBuffer == NULL)
	{
		DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d:  \n", __LINE__);
		return -1;
	}
	
	memcopy(ReadFileName, FileName, NameLength);
	pReadFileDestBuffer = pBuffer;
	ReadFileNameLength = NameLength;

	FileReadCount = 0;
	sector_count = 0;
	PreviousBlockNumber = 0;
	FileBlockStart = 0;
	NextState = INIT_STATE;
	FSM_Event = 0;

	for (int i = 0; i < 5; i++)
    {
    	DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: i: %d \n", __LINE__, i);
		DEBUG ((EFI_D_INFO, "%d HandleEnterPressed FSM_Event: %d\n", __LINE__, FSM_Event));
	    FileReadFSM(FSM_Event++);

	    if (READ_FILE_EVENT <= FSM_Event)
	    	FSM_Event = READ_FILE_EVENT;
	}

	
}


EFIAPI HandleEnterPressed()
{
	DEBUG ((EFI_D_INFO, "%d HandleEnterPressed\n", __LINE__));
	
		
    //PartitionUSBReadSynchronous();
    //PartitionUSBReadAsynchronous();
    
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
    GraphicsLayerMouseMove();
	
	gBS->WaitForEvent( 1, &gMouse->WaitForInput, &Index );
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
   /*
   GraphicsOutput->Blt(GraphicsOutput, 
			            (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) pDateTimeBuffer,
			            EfiBltBufferToVideo,
			            0, 0, 
			            0, 16 * 8, 
			            8 * 50, 16, 0);*/
}

EFI_STATUS MultiProcessInit ()
{
    UINT16 i;

    // task group for mouse keyboard
	EFI_GUID gMultiProcessGroup1Guid  = { 0x0579257E, 0x1843, 0x45FB, { 0x83, 0x9D, 0x6B, 0x79, 0x09, 0x38, 0x29, 0xA9 } };
	
    // task group for display date time
	EFI_GUID gMultiProcessGroup2Guid  = { 0x0579257E, 0x1843, 0x45FB, { 0x83, 0x9D, 0x6B, 0x79, 0x09, 0x38, 0x29, 0xAA } };
	
	//DrawChineseCharIntoBuffer2(pMouseBuffer, 0, 0, 11 * 94 + 42, Color, 16);
	
    //DrawChineseCharIntoBuffer(pMouseBuffer, 0, 0, 0, Color, 16);
    
    EFI_EVENT_NOTIFY       TaskProcessesGroup1[] = {HandleKeyboardEvent, HandleMouseEvent};

    EFI_EVENT_NOTIFY       TaskProcessesGroup2[] = {DisplaySystemDateTime};

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


EFI_STATUS SystemTimeIntervalInit()
{
    EFI_STATUS	Status;
	EFI_HANDLE	TimerOne	= NULL;
	static const UINTN TimeInterval = 20000;
	
	UINT32 *TimerCount;

	TimerCount = (UINT32 *)AllocateZeroPool(4);
	if (NULL == TimerCount)
	{
		DEBUG(( EFI_D_INFO, "%d, NULL == TimerCount \r\n", __LINE__));
		return;
	}

	Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL | EVT_TIMER,
                       		TPL_CALLBACK,
                       		TimeSlice,
                       		(VOID *)TimerCount,
                       		&TimerOne);

	if ( EFI_ERROR( Status ) )
	{
		DEBUG(( EFI_D_INFO, "Create Event Error! \r\n" ));
		return(1);
	}

	Status = gBS->SetTimer(TimerOne,
						  TimerPeriodic,
						  MultU64x32( TimeInterval, 1)); // will multi 100, ns

	if ( EFI_ERROR( Status ) )
	{
		DEBUG(( EFI_D_INFO, "Set Timer Error! \r\n" ));
		return(2);
	}

	while (1)
	{
		*TimerCount = *TimerCount + 1;
		//DebugPrint1(DISPLAY_X, DISPLAY_Y, "%d: SystemTimeIntervalInit while\n", __LINE__);
		//if (*TimerCount % 1000000 == 0)
	       //DebugPrint1(0, 4 * 16, "%d: while (1) p:%x %lu \n", __LINE__, TimerCount, *TimerCount);
	}
	
	gBS->SetTimer( TimerOne, TimerCancel, 0 );
	gBS->CloseEvent( TimerOne );    

	return EFI_SUCCESS;
}


EFI_STATUS ScreenInit()
{
	EFI_STATUS status = 0;
	//UINT32 i = 0;
	//UINT32 j = 0;
	UINT32 x = ScreenWidth;
	UINT32 y = ScreenHeight;
	//UINT8 p[100];
	
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
/*
	if (StatusErrorCount % 61 == 0)
	{
		for (int j = 0; j < ScreenHeight - 25; j++)
		{
			for (int i = 0; i < ScreenWidth; i++)
			{
				pDeskBuffer[(j * ScreenWidth + i) * 4]     = 0x84;
				pDeskBuffer[(j * ScreenWidth + i) * 4 + 1] = 0x84;
				pDeskBuffer[(j * ScreenWidth + i) * 4 + 2] = 0x00;
			}
		}		
	}
	*/
	StatusErrorCount = 0;
    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: FAT32\n",  __LINE__);

  	status = ReadFileSelf("ZHUFENG BMP", 11, pDeskWallpaperBuffer);
	if (EFI_ERROR(status))
	{
		INFO_SELF(L"ReadFileSelf error.\r\n");
  		return;
	}

   for (int j = 0; j < 250; j++)
   {
       DebugPrint1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", pDeskWallpaperBuffer[j] & 0xff);
   }
	
  	
	for (int i = 0; i < ScreenHeight; i++)
		for (int j = 0; j < ScreenWidth; j++)
		{
			// BMP 3bits, and desk buffer 4bits
			pDeskBuffer[(i * ScreenWidth + j) * 4]     = pDeskWallpaperBuffer[0x36 + ((ScreenHeight - i) * 1920 + j) * 3 ];
			pDeskBuffer[(i * ScreenWidth + j) * 4 + 1] = pDeskWallpaperBuffer[0x36 + ((ScreenHeight - i) * 1920 + j) * 3 + 1];
			pDeskBuffer[(i * ScreenWidth + j) * 4 + 2] = pDeskWallpaperBuffer[0x36 + ((ScreenHeight - i) * 1920 + j) * 3 + 2];
		}
 	/*
    Color.Red   = 0x00;
    Color.Green = 0x84;
    Color.Blue	= 0x84;
    Color.Reserved = GRAPHICS_LAYER_DESK;
    RectangleFillIntoBuffer(pDeskBuffer, 0,     0,      x -  1, y - 29, 1, Color);
    
    */
    
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

    DrawChineseCharIntoBuffer2(pDeskBuffer,  16, ScreenHeight - 21,     (18 - 1) * 94 + 43 - 1, Color, ScreenWidth);
    DrawChineseCharIntoBuffer2(pDeskBuffer,  16 * 2, ScreenHeight - 21, (21 - 1) * 94 + 05 - 1, Color, ScreenWidth);
    
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
	
    INFO_SELF(L"\r\n");
    
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

    // BMP header size: 0x36
    UINT32 DeskWallpaperBufferSize = 1920 * 1080 * 3 + 0x36;
	//bmp size, is so big
	pDeskWallpaperBuffer = (UINT8 *)AllocatePool(DeskWallpaperBufferSize); 
	if (NULL == pDeskWallpaperBuffer)
	{
		DEBUG ((EFI_D_INFO, "ScreenInit AllocatePool pDeskDisplayBuffer NULL\n"));
		return -1;
	}

	pMouseClickBuffer = (UINT8 *)AllocatePool(MouseClickWindowWidth * MouseClickWindowHeight * 4); 
	if (pMouseClickBuffer == NULL)
	{
		return -1;
	}   
	
	pDateTimeBuffer = (UINT8 *)AllocateZeroPool(8 * 16 * 50 * 4); 
	if (pDateTimeBuffer == NULL)
	{
		return -1;
	}   
	
    pMouseBuffer = (UINT8 *)AllocateZeroPool(16 * 16 * 4);
    if (NULL == pMouseBuffer)
	{
		DEBUG ((EFI_D_INFO, "MultiProcessInit pMouseBuffer pDeskDisplayBuffer NULL\n"));
		return -1;
	}
	
	pMouseSelectedBuffer = (UINT8 *)AllocateZeroPool(16 * 16 * 2 * 4);
    if (NULL == pMouseSelectedBuffer)
	{
		DEBUG ((EFI_D_INFO, "ScreenInit AllocatePool pDeskDisplayBuffer NULL\n"));
		return -1;
	}

	UINT32 size = 267616;
    
	sChineseChar = (UINT8 *)AllocateZeroPool(size);
	if (NULL == sChineseChar)
    {
        DEBUG ((EFI_D_INFO, "ChineseCharArrayInit AllocateZeroPool Failed: %x!\n "));
        DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d: ChineseCharArrayInit AllocateZeroPool failed\n",  __LINE__);
        return -1;
    }		
    
	MouseColor.Blue  = 0xff;
    MouseColor.Red   = 0xff;
    MouseColor.Green = 0xff;

	iMouseX = ScreenWidth / 2;
	iMouseY = ScreenHeight / 2;

	FileReadCount = 0;
	FAT32_Table = NULL;

	
    INFO_SELF(L"\r\n");

	return EFI_SUCCESS;
}



EFI_STATUS InitChineseChar()
{
    DebugPrint1(DISPLAY_ERROR_STATUS_X, DISPLAY_ERROR_STATUS_Y, "%d:  \n", __LINE__);

	ReadFileSelf("HZK16", 5, sChineseChar);
}

EFI_STATUS
EFIAPI
Main (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_STATUS  Status;    

    Status = gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (VOID **) &GraphicsOutput);    
    INFO_SELF(L"\r\n");    
    if (EFI_ERROR (Status)) 
    {
    	 INFO_SELF(L"%X\n", Status);
        return EFI_UNSUPPORTED;
    }
    
    ScreenWidth  = GraphicsOutput->Mode->Info->HorizontalResolution;
    ScreenHeight = GraphicsOutput->Mode->Info->VerticalResolution;

    INFO_SELF(L"\r\n");

    ParametersInitial();
                
    INFO_SELF(L"\r\n");
	MouseInit();
    
    INFO_SELF(L"\r\n");
    
	// get partitions from api
	PartitionAnalysis();
	
	INFO_SELF(L"\r\n");
    MultiProcessInit();
    
    INFO_SELF(L"\r\n");
    InitChineseChar();
    
    INFO_SELF(L"\r\n");
    ScreenInit();
    
    INFO_SELF(L"\r\n");
    MyComputerWindow(100, 100);
    
    INFO_SELF(L"\r\n");
    SystemTimeIntervalInit();	

    //GraphicsLayerCompute(iMouseX, iMouseY, 0);
	
	//DebugPrint1(100, 100, "%d %d\n", __LINE__, Status);
	
    return EFI_SUCCESS;
}


