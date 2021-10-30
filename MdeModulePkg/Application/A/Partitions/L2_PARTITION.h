#pragma once

#include <L1_PARTITION.h>
#include <Devices/Store/L2_DEVICE_Store.h>



// To save folder or file after read from file system.
typedef struct
{
	UINT8 Name[100]; 
	UINT8 Type[100]; // 1 file; 2 folder; 
	UINT16 Size;
	UINT16 CreateTime;
	UINT16 LatestModifiedTime;
}COMMON_STORAGE_ITEM;

EFI_STATUS L2_FILE_PartitionTypeAnalysis(UINT16 DeviceID);

