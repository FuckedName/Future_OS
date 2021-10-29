#pragma once

#include <Library/BaseLib.h>

#include <L1_GRAPHICS.h>



void L2_GRAPHICS_ParameterInit();

VOID EFIAPI L2_DEBUG_Print1 (UINT16 x, UINT16 y,  IN  CONST CHAR8  *Format, ...);

VOID EFIAPI L2_DEBUG_Print3 (UINT16 x, UINT16 y, WINDOW_LAYER_ITEM layer, IN  CONST CHAR8  *Format, ...);

