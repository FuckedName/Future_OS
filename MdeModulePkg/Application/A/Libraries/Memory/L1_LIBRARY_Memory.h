#pragma once


void L1_MEMORY_SetValue(UINT8 *pBuffer, UINT32 Length, UINT8 Value);

void *L1_MEMORY_Copy(UINT8 *dest, const UINT8 *src, UINT8 count);

VOID L1_MEMORY_Memset(void *s, UINT8 c, UINT32 n);


