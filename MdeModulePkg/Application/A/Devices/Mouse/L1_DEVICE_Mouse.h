#pragma once

#include <Protocol/SimplePointer.h>
#include <Library/UefiBootServicesTableLib.h>


#define MOUSE_NO_CLICKED 0
#define MOUSE_LEFT_CLICKED 1
#define MOUSE_RIGHT_CLICKED 2

static UINT16 mouse_count = 0;
static UINT8 MouseClickFlag = MOUSE_NO_CLICKED;

static CHAR8 x_move = 0;
static CHAR8 y_move = 0;

static UINT16 iMouseX = 0;
static UINT16 iMouseY = 0;


EFI_STATUS L2_MOUSE_Init();

