
/*************************************************
    .
    File name:      	*.*
    Author��	        	������
    ID��					00001
    Date:          		202107
    Description:    	
    Others:         	��

    History:        	��
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
#include <Library/BaseLib.h>
#include <Uefi/UefiBaseType.h>

#include <Global/Global.h>

VOID L2_ApplicationInit();

VOID L2_ApplicationRun(EFI_EVENT Event,     CHAR16  *FileName);

VOID L2_APPLICATIONS_SetHandle(EFI_HANDLE          ImageHandle);

