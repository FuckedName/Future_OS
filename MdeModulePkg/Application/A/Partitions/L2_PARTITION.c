
/*************************************************
    .
    File name:      	*.*
    Author：	        	任启红
    ID：					00001
    Date:          		202107
    Description:    	
    Others:         	无

    History:        	无
	    1.  Date:
		    Author: 
		    ID:
		    Modification:
		    
	    2.  Date:
		    Author: 
		    ID:
		    Modification:
*************************************************/





#include <Library/MemoryAllocationLib.h>

#include <Protocol/DevicePath.h>
#include <Protocol/DevicePathToText.h>
#include <L2_PARTITION.h>
#include <Partitions/NTFS/L2_PARTITION_NTFS.h>
#include <Partitions/FAT32/L2_PARTITION_FAT32.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/DevicePathToText.h>

#include <Graphics/L2_GRAPHICS.h>

//#include <Library/Math/L2_LIBRARY_Math.h>
//#include <Device/Store/L2_DEVICE_Store.h>
//#include <FAT32/L2_PARTITION_FAT32.h>
//#include <NTFS/L2_PARTITION_NTFS.h>

UINT32 BlockSize = 0;

//定义整个系统分区数，类似C，D，E，U盘等等，注意，一块硬盘或U盘可以分成多个分区
DEVICE_PARAMETER device[50] = {0};
UINT64 FileBlockStart = 0;
DollarBootSwitched NTFSBootSwitched;

//定义当前操作系统读取系统文件所在的分区名称，系统文件包含：背景图片，图标，HZK16文件等等这些
UINT8 EFI_FILE_STORE_PATH_PARTITION_NAME[50] = "OS";

UINT8 Buffer1[DISK_BUFFER_SIZE];




//通用文件系统文件或者目录
//因为当前系统支持NTFS和FAT32，实际上后边可能会支持其他类型的文件系统，不同文件系统，
//读取出来的文件都会有文件名、大小、类型等等，所以抽象出来一个公共的数据结构。
COMMON_STORAGE_ITEM pCommonStorageItems[100];


// all partitions analysis



/****************************************************************************
*
*  描述:   获取分区（磁盘、U盘等等）文件系统类型：NTFS/FAT32等等
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L2_FILE_PartitionTypeAnalysis(UINT16 DeviceID)
{    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d DeviceID: %d\n", __LINE__, DeviceID);
    EFI_STATUS Status;

    sector_count = 0;

    L1_MEMORY_SetValue(Buffer1, 0, DISK_BUFFER_SIZE);

    Status = L1_STORE_READ(DeviceID, 0, 1, Buffer1 );  
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
        return Status;
    }

    // FAT32 file system
    if (Buffer1[0x52] == 'F' && Buffer1[0x53] == 'A' && Buffer1[0x54] == 'T' && Buffer1[0x55] == '3' && Buffer1[0x56] == '2')   
    {                   
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FAT32\n",  __LINE__);
        // analysis data area of patition
        L1_FILE_FAT32_DataSectorAnalysis(Buffer1, &(device[DeviceID].stMBRSwitched)); 

        // data sector number start include: reserved selector, fat sectors(usually is 2: fat1 and fat2), and file system boot path start cluster(usually is 2, data block start number is 2)
        device[DeviceID].StartSectorNumber = device[DeviceID].stMBRSwitched.ReservedSelector + device[DeviceID].stMBRSwitched.SectorsPerFat * device[DeviceID].stMBRSwitched.FATCount;
		sector_count = device[DeviceID].StartSectorNumber + (device[DeviceID].stMBRSwitched.BootPathStartCluster - 2) * 8;
		device[DeviceID].FileSystemType = FILE_SYSTEM_FAT32;
        BlockSize = device[DeviceID].stMBRSwitched.SectorOfCluster * 512; 
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: sector_count:%ld BlockSize: %d StartSectorNumber: %llu FileSystemType: %d\n",  __LINE__, sector_count, BlockSize, device[DeviceID].StartSectorNumber, device[DeviceID].FileSystemType);


        
        return FILE_SYSTEM_FAT32;
    }
    // NTFS
    else if (Buffer1[3] == 'N' && Buffer1[4] == 'T' && Buffer1[5] == 'F' && Buffer1[6] == 'S')
    {
        L2_FILE_NTFS_FirstSelectorAnalysis(Buffer1, &NTFSBootSwitched);
		device[DeviceID].StartSectorNumber = NTFSBootSwitched.MFT_StartCluster * 8;
		device[DeviceID].FileSystemType = FILE_SYSTEM_NTFS;
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: sector_count:%ld BlockSize: %d StartSectorNumber: %llu FileSystemType: %d\n",  __LINE__, sector_count, BlockSize, device[DeviceID].StartSectorNumber, device[DeviceID].FileSystemType);
        return FILE_SYSTEM_NTFS;
    }
    // the other file system can add at this place
    else
    {
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: \n",  __LINE__);
        return FILE_SYSTEM_MAX;
    }                   

    return EFI_SUCCESS;

}




/****************************************************************************
*
*  描述:   xxxxx
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L2_FILE_PartitionNameAnalysis(UINT16 DeviceID, UINT8 *pBuffer)
{
	switch (device[DeviceID].FileSystemType)
	{
		case FILE_SYSTEM_FAT32:
			break;
	}
}

typedef struct
{
	FILE_SYSTEM_TYPE FileSystemType;
    UINT64    	  (*pFunctionStartSectorNumberGet)(UINT16); 
    EFI_STATUS    (*pFunctionBufferAnalysis)(UINT16, UINT8 *); 
}PARTITION_NAME_GET;




/****************************************************************************
*
*  描述:   xxxxx
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
UINT64 L2_PARTITION_NameFAT32StartSectorNumberGet(UINT16 DeviceID)
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d DeviceID: %d StartSectorNumber: %d\n", __LINE__, DeviceID, device[DeviceID].StartSectorNumber);
	// Start with 2 cluster
	return device[DeviceID].StartSectorNumber;
}




/****************************************************************************
*
*  描述:   获取卷标所在的MFT,即第四项元数据文件：MFT_ITEM_DOLLAR_VOLUME。
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
UINT64 L2_PARTITION_NameNTFSStartSectorNumberGet(UINT16 DeviceID)
{	// Start with $volume
	return device[DeviceID].StartSectorNumber + 2 * MFT_ITEM_DOLLAR_VOLUME;
}





/****************************************************************************
*
*  描述:   
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L2_PARTITION_NameFAT32Analysis(UINT16 DeviceID, UINT8 *Buffer)
{
	L1_MEMORY_SetValue(device[DeviceID].PartitionName, 0, PARTITION_NAME_LENGTH);

	//第二个簇区前几个字符就是FAT32分区的名字，并且第一个簇就是第二个簇，因为没有0，1号
	for (UINT16 i = 0; i < 6; i++)
		device[DeviceID].PartitionName[i] = Buffer[i];
	
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d %X %X %X %X\n", __LINE__, device[DeviceID].PartitionName[0], device[DeviceID].PartitionName[1], device[DeviceID].PartitionName[2], device[DeviceID].PartitionName[3]);
    device[DeviceID].PartitionName[6] = '\0';
}




/****************************************************************************
*
*  描述:   xxxxx
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L2_PARTITION_NameNTFSAnalysis(UINT16 DeviceID, UINT8 *Buffer)
{
	NTFS_FILE_SWITCHED NTFSFileSwitched = {0};
	L1_MEMORY_SetValue(device[DeviceID].PartitionName, 0, PARTITION_NAME_LENGTH);


	//当前测试，只显示一个设备，显示多个设备测试会比较麻烦
	//if (3 == DeviceID)
		L2_PARTITION_FileContentPrint(Buffer);
	
	L2_FILE_NTFS_FileItemBufferAnalysis(Buffer, &NTFSFileSwitched);

	UINT8 j;

	//一个FILE里边有很多个属性，正常VOLUME属性在在6个左右，不过有些属性不一定有，所以这里取小于等于5，有一定的风险
	for (UINT8 i = 0; i < 5; i++)
	{
		if (NTFSFileSwitched.NTFSFileAttributeHeaderSwitched[i].Type == MFT_ATTRIBUTE_DOLLAR_VOLUME_NAME)
		{
			for (j = 0; NTFSFileSwitched.NTFSFileAttributeHeaderSwitched[i].Data[j] != '\0'; j++)				
				device[DeviceID].PartitionName[j] = NTFSFileSwitched.NTFSFileAttributeHeaderSwitched[i].Data[j];

			break;
		}
	}
	device[DeviceID].PartitionName[j] = '\0';
}




/****************************************************************************
*
*  描述: 第一列是不同的文件系统类型，第二列是需要读取扇区号，第三列是解析从存储分区的内存。
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
PARTITION_NAME_GET PartitionNameGet[]=
{
    {FILE_SYSTEM_MIN, NULL, NULL},
	{FILE_SYSTEM_FAT32, L2_PARTITION_NameFAT32StartSectorNumberGet,  L2_PARTITION_NameFAT32Analysis},  
	{FILE_SYSTEM_NTFS,  L2_PARTITION_NameNTFSStartSectorNumberGet,   L2_PARTITION_NameNTFSAnalysis}
};




/****************************************************************************
*
*  描述:   获取分区的名字，这个从WINDOWS操作系统看到的类似C盘、D盘等等盘的名称（比如软件分区，资料分区等等，当前暂不支持中文件）
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L2_FILE_PartitionNameGet(UINT16 DeviceID)
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d DeviceID: %X\n", __LINE__, DeviceID);
	UINT8 Buffer[DISK_BUFFER_SIZE * 2] = {0};
	UINT64 StartSectorNumber = 0;
	EFI_STATUS Status;
	UINT16 FileSystemType;
	//L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d FileSystemType: %llu\n", __LINE__, FileSystemType);

    
	FileSystemType = device[DeviceID].FileSystemType;
	if (FileSystemType == FILE_SYSTEM_MIN || FileSystemType == FILE_SYSTEM_MAX)
	{
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d FileSystemType: %X FileSystemType == FILE_SYSTEM_MIN || FileSystemType == FILE_SYSTEM_MAX\n", __LINE__, FileSystemType);
        return;
	}

	StartSectorNumber = PartitionNameGet[FileSystemType].pFunctionStartSectorNumberGet(DeviceID);

	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d DeviceID: %d FileSystemType: %d, StartSectorNumber: %llu\n", __LINE__, DeviceID,FileSystemType, StartSectorNumber);

	//一个元数据是读取两个扇区
	Status = L1_STORE_READ(DeviceID, StartSectorNumber, 2, Buffer);  
	if (EFI_ERROR(Status))
	{
		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
		return Status;
	}

	PartitionNameGet[FileSystemType].pFunctionBufferAnalysis(DeviceID, Buffer);
	return EFI_SUCCESS;
}




/****************************************************************************
*
*  描述:   xxxxx
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L2_STORE_PartitionAnalysis()
{
	L2_STORE_PartitionsParameterGet();
}
