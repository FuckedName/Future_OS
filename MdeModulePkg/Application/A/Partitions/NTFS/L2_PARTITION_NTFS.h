
/*************************************************
    .
    File name:      	A.c
    Author：	        任启红
    ID：		00001
    Date:          	202107
    Description:    	整个模块的主入口函数
    Others:         	无
    History:        	无
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

#include <L1_PARTITION_NTFS.h>

#include <Global/Global.h>
EFI_STATUS L2_FILE_NTFS_MFT_Item_Read(UINT16 DeviceID, UINT64 SectorStartNumber);

EFI_STATUS  L2_FILE_NTFS_MFTDollarRootFileAnalysis(UINT8 *pBuffer);

EFI_STATUS L2_FILE_NTFS_RootPathItemsRead(UINT8 PartitionID);

EFI_STATUS  L2_FILE_NTFS_MFTIndexItemsAnalysis(UINT8 *pBuffer, UINT8 DeviceID);

EFI_STATUS  L2_FILE_NTFS_DollarRootA0DatarunAnalysis(UINT8 *p);

EFI_STATUS L2_FILE_NTFS_FirstSelectorAnalysis(UINT8 *p, DollarBootSwitched *pNTFSBootSwitched);

