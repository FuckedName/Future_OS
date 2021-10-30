#pragma once

#include <L1_PARTITION_NTFS.h>

EFI_STATUS L2_FILE_NTFS_MFT_Item_Read(UINT16 DeviceID, UINT64 SectorStartNumber);

EFI_STATUS  L2_FILE_NTFS_MFTDollarRootFileAnalysis(UINT8 *pBuffer);

EFI_STATUS L2_FILE_NTFS_RootPathItemsRead(UINT8 PartitionID);

EFI_STATUS  L2_FILE_NTFS_MFTIndexItemsAnalysis(UINT8 *pBuffer, UINT8 DeviceID);

EFI_STATUS  L2_FILE_NTFS_DollarRootA0DatarunAnalysis(UINT8 *p);


