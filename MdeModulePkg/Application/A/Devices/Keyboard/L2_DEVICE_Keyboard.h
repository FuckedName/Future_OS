#pragma once
#include <Library/UefiLib.h>

#include <L1_DEVICE_Keyboard.h>

VOID L2_KEYBOARD_KeyPressed();

VOID
EFIAPI
L2_KEYBOARD_Event (
  IN EFI_EVENT Event,
  IN VOID      *Context
  );

