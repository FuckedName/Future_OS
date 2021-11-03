#pragma once

#include <Global/Global.h>


EFI_STATUS L1_STORE_READ(UINT8 deviceID, UINT64 StartSectorNumber, UINT16 ReadSize, UINT8 *pBuffer);


EFI_STATUS L1_STORE_Write(UINT8 deviceID, UINT64 StartSectorNumber, UINT16 WriteSize, UINT8 *pBuffer);

EFI_STATUS L2_STORE_PartitionAnalysis();

void L2_STORE_TextDevicePathAnalysis(CHAR16 *p, DEVICE_PARAMETER *device, UINTN count1);

