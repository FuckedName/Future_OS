
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

#include <Global/Global.h>


EFI_STATUS L2_STORE_Read(UINT8 deviceID, UINT64 StartSectorNumber, UINT16 ReadSize, UINT8 *pBuffer);


EFI_STATUS L2_STORE_Write(UINT8 deviceID, UINT64 StartSectorNumber, UINT16 WriteSize, UINT8 *pBuffer);


void L2_STORE_TextDevicePathAnalysis(CHAR16 *p, DEVICE_PARAMETER *device, UINTN count1);

EFI_STATUS L2_STORE_PartitionsParameterGet();

