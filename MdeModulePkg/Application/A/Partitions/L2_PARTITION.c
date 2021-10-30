
#include <Library/MemoryAllocationLib.h>

#include <Protocol/DevicePath.h>
#include <Protocol/DevicePathToText.h>
#include <L2_PARTITION.h>

#include <Graphics/L2_GRAPHICS.h>

//#include <Library/Math/L2_LIBRARY_Math.h>
//#include <Device/Store/L2_DEVICE_Store.h>
//#include <FAT32/L2_PARTITION_FAT32.h>
//#include <NTFS/L2_PARTITION_NTFS.h>


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
        BlockSize = MBRSwitched.SectorOfCluster * 512;
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: sector_count:%ld BlockSize: %d\n",  __LINE__, sector_count, BlockSize);
        return FILE_SYSTEM_FAT32;
    }
    // NTFS
    else if (Buffer1[3] == 'N' && Buffer1[4] == 'T' && Buffer1[5] == 'F' && Buffer1[6] == 'S')
    {
        L2_FILE_NTFS_FirstSelectorAnalysis(Buffer1, &NTFSBootSwitched);
		device[DeviceID].StartSectorNumber = NTFSBootSwitched.MFT_StartCluster * 8;
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

