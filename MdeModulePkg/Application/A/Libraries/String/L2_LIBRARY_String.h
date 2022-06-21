
/*************************************************
    .
    File name:      	*.*
    Author£∫	        	»Œ∆Ù∫Ï
    ID£∫					00001
    Date:          		202107
    Description:    	
    Others:         	Œﬁ

    History:        	Œﬁ
	    1.  Date:
		    Author: 
		    ID:
		    Modification:
		    
	    2.  Date:
		    Author: 
		    ID:
		    Modification:
*************************************************/




#pragma once

#include <Protocol/GraphicsOutput.h>

#include <L1_LIBRARY_String.h>
#include <Graphics/L1_GRAPHICS.h>
#include <Graphics/Window/L2_GRAPHICS_Window.h>


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

