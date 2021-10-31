#pragma once
#include <L1_DEVICE_Mouse.h>
//#include <L1_DEVICE_Screen.h>


// for mouse move & click
VOID
EFIAPI
L2_MOUSE_Event (IN EFI_EVENT Event, IN VOID *Context);


EFI_STATUS L2_DEVICE_MouseInit();

