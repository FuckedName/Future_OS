#pragma once
#include <L1_PARTITION_FAT32.h>
#include <Global/Global.h>

EFI_STATUS L1_FILE_FAT32_DataSectorAnalysis(UINT8 *p, MasterBootRecordSwitched *pMBRSwitched);

EFI_STATUS L2_FILE_FAT32_DataSectorHandle(UINT16 DeviceID);

