#pragma once
#include <L1_DEVICE_Mouse.h>
#include <L1_DEVICE_Screen.h>


// for mouse move & click
VOID L2_MOUSE_Event (EFI_EVENT Event,  VOID *Context);

EFI_STATUS L2_DEVICE_MouseInit();

