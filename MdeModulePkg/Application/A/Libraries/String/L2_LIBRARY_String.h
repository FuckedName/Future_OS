#pragma once

#include <Protocol/GraphicsOutput.h>

#include <L1_LIBRARY_String.h>
#include <Graphics/L1_GRAPHICS.h>


VOID L2_STRING_Maker (UINT16 x, UINT16 y,
  IN  CONST CHAR8   *Format,
  IN  VA_LIST       VaList
  );

VOID L2_STRING_Maker2 (UINT16 x, UINT16 y, WINDOW_LAYER_ITEM layer,
  IN  CONST CHAR8   *Format,
  IN  VA_LIST       VaList
  );

VOID L2_STRING_Maker3 (UINT16 x, UINT16 y, WINDOW_LAYER_ITEM layer,
  IN  CONST CHAR8   *Format,
  IN  VA_LIST       VaList
  );

