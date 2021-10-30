#pragma once



EFI_STATUS L2_COMMON_MemoryAllocate();

UINT8 *L2_MEMORY_Allocate(char *pApplicationName, UINT16 type, UINT32 SizeRequired);

