#pragma once
#include <L1_PROCESS_Multi.h>

#include <Library/UefiLib.h>

EFI_STATUS L2_COMMON_MultiProcessInit ();

VOID EFIAPI L2_TIMER_Slice(
    IN EFI_EVENT Event,
    IN VOID           *Context
    );

VOID
EFIAPI
L2_SYSTEM_Start (IN EFI_EVENT Event, IN VOID *Context);

