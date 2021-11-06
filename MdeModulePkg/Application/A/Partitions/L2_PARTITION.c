
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

#include <Graphics/L2_GRAPHICS.h>

//#include <Library/Math/L2_LIBRARY_Math.h>
//#include <Device/Store/L2_DEVICE_Store.h>
//#include <FAT32/L2_PARTITION_FAT32.h>
//#include <NTFS/L2_PARTITION_NTFS.h>

UINT32 BlockSize = 0;
DEVICE_PARAMETER device[10] = {0};
UINT32 FileBlockStart = 0;
MasterBootRecordSwitched MBRSwitched;
DollarBootSwitched NTFSBootSwitched;

// all partitions analysis
EFI_STATUS L2_FILE_PartitionTypeAnalysis(UINT16 DeviceID)
{    
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d DeviceID: %d\n", __LINE__, DeviceID);
    EFI_STATUS Status;
    UINT8 Buffer1[DISK_BUFFER_SIZE] = {0};

    sector_count = 0;

    Status = L1_STORE_READ(DeviceID, sector_count, 1, Buffer1 );    
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
        return Status;
    }

    // FAT32 file system
    if (Buffer1[0x52] == 'F' && Buffer1[0x53] == 'A' && Buffer1[0x54] == 'T' && Buffer1[0x55] == '3' && Buffer1[0x56] == '2')   
    {                   
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FAT32\n",  __LINE__);
        // analysis data area of patition
        L1_FILE_FAT32_DataSectorAnalysis(Buffer1, &MBRSwitched); 

        // data sector number start include: reserved selector, fat sectors(usually is 2: fat1 and fat2), and file system boot path start cluster(usually is 2, data block start number is 2)
        sector_count = MBRSwitched.ReservedSelector + MBRSwitched.SectorsPerFat * MBRSwitched.NumFATS + (MBRSwitched.BootPathStartCluster - 2) * 8;
		device[DeviceID].StartSectorNumber = MBRSwitched.ReservedSelector + MBRSwitched.SectorsPerFat * MBRSwitched.NumFATS;
		device[DeviceID].FileSystemType = FILE_SYSTEM_FAT32;
        BlockSize = MBRSwitched.SectorOfCluster * 512;
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: sector_count:%ld BlockSize: %d\n",  __LINE__, sector_count, BlockSize);
        return FILE_SYSTEM_FAT32;
    }
    // NTFS
    else if (Buffer1[3] == 'N' && Buffer1[4] == 'T' && Buffer1[5] == 'F' && Buffer1[6] == 'S')
    {
        L2_FILE_NTFS_FirstSelectorAnalysis(Buffer1, &NTFSBootSwitched);
		device[DeviceID].StartSectorNumber = NTFSBootSwitched.MFT_StartCluster * 8;
		device[DeviceID].FileSystemType = FILE_SYSTEM_NTFS;
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: NTFS sector_count:%llu\n",  __LINE__, sector_count);
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

UINT64 L2_PARTITION_NameFAT32StartSectorNumberGet(UINT16 DeviceID)
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d DeviceID: %d StartSectorNumber: %d\n", __LINE__, DeviceID, device[DeviceID].StartSectorNumber);
	// Start with 2 cluster
	return device[DeviceID].StartSectorNumber;
}

UINT64 L2_PARTITION_NameNTFSStartSectorNumberGet(UINT16 DeviceID)
{	// Start with $volume
	return device[DeviceID].StartSectorNumber + 2 * MFT_ITEM_DOLLAR_VOLUME;
}


EFI_STATUS L2_PARTITION_NameFAT32Analysis(UINT16 DeviceID, UINT8 *Buffer)
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d %X %X %X %X\n", __LINE__, device[DeviceID].PartitionName[0], device[DeviceID].PartitionName[1], device[DeviceID].PartitionName[2], device[DeviceID].PartitionName[3]);

	for (UINT16 i = 0; i < 6; i++)
		device[DeviceID].PartitionName[i] = Buffer[i];
	device[DeviceID].PartitionName[6] = '\0';
}

EFI_STATUS L2_PARTITION_NameNTFSAnalysis(UINT16 DeviceID, UINT8 *Buffer)
{
	device[DeviceID].PartitionName[6] = '\0';
}


PARTITION_NAME_GET PartitionNameGet[]=
{
	{FILE_SYSTEM_FAT32, L2_PARTITION_NameFAT32StartSectorNumberGet,  L2_PARTITION_NameFAT32Analysis},  
	{FILE_SYSTEM_NTFS,  L2_PARTITION_NameNTFSStartSectorNumberGet,   L2_PARTITION_NameNTFSAnalysis}
};

EFI_STATUS L2_FILE_PartitionNameGet(UINT16 DeviceID)
{
	//L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d DeviceID: %X\n", __LINE__, DeviceID);
	UINT8 Buffer[DISK_BUFFER_SIZE] = {0};
	UINT64 StartSectorNumber = 0;
	EFI_STATUS Status;
	UINT16 FileSystemType = device[DeviceID].FileSystemType;
	//L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d FileSystemType: %llu\n", __LINE__, FileSystemType);

	StartSectorNumber = PartitionNameGet[FileSystemType].pFunctionStartSectorNumberGet(DeviceID);

	//L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d StartSectorNumber: %llu\n", __LINE__, StartSectorNumber);
	
	Status = L1_STORE_READ(DeviceID, StartSectorNumber, 1, Buffer);  
	if (EFI_ERROR(Status))
	{
		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
		return Status;
	}

	PartitionNameGet[FileSystemType].pFunctionBufferAnalysis(DeviceID, Buffer);
	return EFI_SUCCESS;
}


