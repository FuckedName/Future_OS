
/*************************************************
    .
    File name:      	*.*
    Author：	        	任启红
    ID：					00001
    Date:          		202107
    Description:    	
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




#include <L2_PARTITION_FAT32.h>
#include <string.h>

//http://quwei.911cha.com/



/****************************************************************************
*
*  描述:   xxxxx
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID L2_FILE_Transfer(MasterBootRecord *pSource, MasterBootRecordSwitched *pDest)
{
    pDest->ReservedSelector = (UINT16)pSource->ReservedSelector[0] | (UINT16)pSource->ReservedSelector[1] << 8;
    pDest->SectorsPerFat    = (UINT32)pSource->SectorsPerFat[0] | (UINT32)(pSource->SectorsPerFat[1]) << 8 | (UINT32)(pSource->SectorsPerFat[2]) << 16 | (UINT32)(pSource->SectorsPerFat[3]) << 24;
    pDest->BootPathStartCluster = (UINT32)pSource->BootPathStartCluster[0] | (UINT32)pSource->BootPathStartCluster[1] << 8 | (UINT32)pSource->BootPathStartCluster[2] << 16 | (UINT32)pSource->BootPathStartCluster[3] << 24;
    pDest->NumFATS      = pSource->NumFATS[0];
    pDest->SectorOfCluster = pSource->SectorOfCluster[0];

    //Todo: the other parameters can compute like above too
    //Current only get parameters we need to use
    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "ReservedSelector:%d SectorsPerFat:%d BootPathStartCluster: %d NumFATS:%d SectorOfCluster:%d", 
                                                pDest->ReservedSelector,
                                                pDest->SectorsPerFat,
                                                pDest->BootPathStartCluster,
                                                pDest->NumFATS,
                                                pDest->SectorOfCluster);
}





/****************************************************************************
*
*  描述:   xxxxx
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L1_FILE_FAT32_DataSectorAnalysis(UINT8 *p, MasterBootRecordSwitched *pMBRSwitched)
{
    MasterBootRecord *pMBR;
    
    //pMBR = (MasterBootRecord *)AllocateZeroPool(DISK_BUFFER_SIZE);
    pMBR = (MasterBootRecord *)L2_MEMORY_Allocate("MBR Buffer", MEMORY_TYPE_PARTITION, DISK_BUFFER_SIZE);
    if (NULL == pMBR)
    {
        return EFI_SUCCESS;
    }
    L1_MEMORY_Copy(pMBR, p, DISK_BUFFER_SIZE);
	
    // 大端字节序：低位字节在高地址，高位字节低地址上。这是人类读写数值的方法。
    // 小端字节序：与上面相反。低位字节在低地址，高位字节在高地址。
    /*
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d ReservedSelector:%02X%02X SectorsPerFat:%02X%02X%02X%02X BootPathStartCluster:%02X%02X%02X%02X NumFATS: %X", 
                                        __LINE__,
                                        pMBR->ReservedSelector[0], pMBR->ReservedSelector[1], 
                                        pMBR->SectorsPerFat[0], pMBR->SectorsPerFat[1], pMBR->SectorsPerFat[2], pMBR->SectorsPerFat[3],
                                        pMBR->BootPathStartCluster[0], pMBR->BootPathStartCluster[1], pMBR->BootPathStartCluster[2], pMBR->BootPathStartCluster[3],
                                        pMBR->NumFATS[0]);
    */
    
    L2_FILE_Transfer(pMBR, pMBRSwitched);

    //FreePool(pMBR);
}


// analysis a partition 



/****************************************************************************
*
*  描述:   xxxxx
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L2_FILE_FAT32_DataSectorHandle(UINT16 DeviceID)
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_FILE_FAT32_DataSectorHandle DeviceID: %d sector_count: %u\n", __LINE__, DeviceID, sector_count);
    //printf( "RootPathAnalysis\n" );
    EFI_STATUS Status ;
    UINT8 Buffer1[DISK_BUFFER_SIZE];
    
    Status = L1_STORE_READ(DeviceID, sector_count, 1, Buffer1); 
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
        return Status;
    }
    
    //When get root path data sector start number, we can get content of root path.
    //L1_FILE_FAT32_DataSectorAnalysis(Buffer1, &device[DeviceID].stMBRSwitched);    
    
    L1_MEMORY_Memset(&pItems, 0, sizeof(pItems));
    L1_MEMORY_Copy(&pItems, Buffer1, DISK_BUFFER_SIZE);

    // data area start from 1824, HZK16 file start from     FileBlockStart  block, so need to convert into sector by multi 8, block start number is 2   
    // next state is to read FAT table
    sector_count = device[DeviceID].stMBRSwitched.ReservedSelector;
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: sector_count:%ld FileLength: %d MBRSwitched.ReservedSelector:%ld\n",  __LINE__, sector_count, FileLength, device[DeviceID].stMBRSwitched.ReservedSelector);

    return EFI_SUCCESS;
}

/****************************************************************************
*
*  描述:   xxxxx
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L2_FILE_FAT32_FileModify(UINT16 DeviceID)
{
    EFI_STATUS Status ;
    UINT8 Buffer[DISK_BUFFER_SIZE];
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_FILE_FAT32_FileModify PartitionItemID: %d FolderItemID: %d \n", __LINE__, PartitionItemID, FolderItemID);
        
    if (0xffff == PartitionItemID || 0xffff == FolderItemID)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d 0xffff == PartitionItemID\n", __LINE__);
        return -1;
    }

    //需要找获取有效的项索引
    UINT16 index = FolderItemValidIndexArray[FolderItemID];
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d index: %d\n", __LINE__, index);
        
    //FAT32文件系统格式
    if (device[PartitionItemID].FileSystemType == FILE_SYSTEM_FAT32)
    {

        UINT16 High2B = L1_NETWORK_2BytesToUINT16(pItems[index].StartClusterHigh2B);
        UINT16 Low2B  = L1_NETWORK_2BytesToUINT16(pItems[index].StartClusterLow2B);
        UINT32 StartCluster = (UINT32)High2B << 16 | (UINT32)Low2B;

        // Start cluster id is 2, exclude 0,1
        //这样写死8192，会有BUG
        UINT32 StartSectorNumber = 15920 + (StartCluster - 2) * 8;
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], 
                        "%d High2B: %X Low2B: %X StartCluster: %X StartSectorNumber: %X\n", 
                        __LINE__, 
                        High2B,
                        Low2B,
                        StartCluster,
                        StartSectorNumber);

        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], 
                        "%d ItemName:%c%c%c%c%c%c%c%c, extension Name: %c%c%c\n", 
                        __LINE__, 
                        pItems[index].FileName[0],
                        pItems[index].FileName[1],
                        pItems[index].FileName[2],
                        pItems[index].FileName[3],
                        pItems[index].FileName[4],
                        pItems[index].FileName[5],
                        pItems[index].FileName[6],
                        pItems[index].FileName[7],
                        pItems[index].ExtensionName[0],
                        pItems[index].ExtensionName[1],
                        pItems[index].ExtensionName[2]);

        // Read data from partition(disk or USB etc..)                  
        Status = L1_STORE_READ(PartitionItemID, StartSectorNumber, 1, Buffer); 
        if (EFI_ERROR(Status))
        {
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
            return Status;
        }

        switch(pItems[index].Attribute[0])
        {
            //如果是目录，则显示子目录
            case 0x10:  L1_MEMORY_Memset(&pItems, 0, sizeof(pItems));
                        L1_MEMORY_Copy(&pItems, Buffer, DISK_BUFFER_SIZE);
                        L2_STORE_FolderItemsPrint();
                        break;

            //如果是文件，则显示文件内容
            case 0x20: L2_PARTITION_FileContentPrint(Buffer); break;

            default: break;
        }
        
    }


}

/****************************************************************************
*
*  描述:   xxxxx
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L2_FILE_FAT32_FileAdd(UINT16 DeviceID)
{}


/****************************************************************************
*
*  描述:   xxxxx
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L2_FILE_FAT32_FileOpen(UINT16 DeviceID)
{}


/****************************************************************************
*
*  描述:   xxxxx
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L2_FILE_FAT32_FileDelete(UINT16 DeviceID)
{
    //暂时先不实现
    return;
    UINT8 Buffer1[512];
    EFI_STATUS Status;
    Status = L1_STORE_READ(DeviceID, 0, 1, Buffer1 );  
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
        return Status;
    }

    // FAT32 file system
    if (Buffer1[0x52] == 'F' && Buffer1[0x53] == 'A' && Buffer1[0x54] == 'T' && Buffer1[0x55] == '3' && Buffer1[0x56] == '2')   
    {                   
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FAT32\n",  __LINE__);
        // analysis data area of patition
        L1_FILE_FAT32_DataSectorAnalysis(Buffer1, &(device[DeviceID].stMBRSwitched)); 

        // data sector number start include: reserved selector, fat sectors(usually is 2: fat1 and fat2), and file system boot path start cluster(usually is 2, data block start number is 2)
        device[DeviceID].StartSectorNumber = device[DeviceID].stMBRSwitched.ReservedSelector + device[DeviceID].stMBRSwitched.SectorsPerFat * device[DeviceID].stMBRSwitched.NumFATS;
		sector_count = device[DeviceID].StartSectorNumber + (device[DeviceID].stMBRSwitched.BootPathStartCluster - 2) * 8;
		device[DeviceID].FileSystemType = FILE_SYSTEM_FAT32;
        BlockSize = device[DeviceID].stMBRSwitched.SectorOfCluster * 512; 
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: sector_count:%ld BlockSize: %d StartSectorNumber: %llu FileSystemType: %d\n",  __LINE__, sector_count, BlockSize, device[DeviceID].StartSectorNumber, device[DeviceID].FileSystemType);
        return FILE_SYSTEM_FAT32;
    }
    
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_MOUSE_MyComputerFolderItemClicked\n", __LINE__);
    //FolderItemID;
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d DeviceID: %d\n", __LINE__, DeviceID);
    //printf( "RootPathAnalysis\n" );
    UINT8 Buffer[DISK_BUFFER_SIZE];
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d PartitionItemID: %d FolderItemID: %d \n", __LINE__, PartitionItemID, FolderItemID);
        
    if (0xffff == PartitionItemID || 0xffff == FolderItemID)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d 0xffff == PartitionItemID\n", __LINE__);
        return -1;
    }

    //需要找获取有效的项索引
    UINT16 index = FolderItemValidIndexArray[FolderItemID];
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d index: %d\n", __LINE__, index);
        
    //FAT32文件系统格式
    if (device[PartitionItemID].FileSystemType == FILE_SYSTEM_FAT32)
    {       

        UINT16 High2B = L1_NETWORK_2BytesToUINT16(pItems[index].StartClusterHigh2B);
        UINT16 Low2B  = L1_NETWORK_2BytesToUINT16(pItems[index].StartClusterLow2B);
        UINT32 StartCluster = (UINT32)High2B << 16 | (UINT32)Low2B;

        // Start cluster id is 2, exclude 0,1
        //这样写死8192，会有BUG
        UINT32 StartSectorNumber = 8192 + (StartCluster - 2) * 8;
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], 
                        "%d High2B: %X Low2B: %X StartCluster: %X StartSectorNumber: %X\n", 
                        __LINE__, 
                        High2B,
                        Low2B,
                        StartCluster,
                        StartSectorNumber);

        //如果是efi文件，认为是可执行文件
        if (pItems[index].ExtensionName[0] == 'E' && pItems[index].ExtensionName[1] == 'F' && pItems[index].ExtensionName[2] == 'I' )
        {
            pItems[index].ExtensionName[0] = 'e';
            pItems[index].ExtensionName[1] = 'f';
            pItems[index].ExtensionName[2] = 'i';
            
            UINT8 FileName[13] = {0};
            L1_FILE_NameMerge(index, FileName);
            CHAR16 wcFileName[13] = {0};

            for (UINT8 i = 0; '\0' != FileName[i]; i++)
            {
                wcFileName[i] = FileName[i];
            }
                            
            EFI_EVENT       Event;
            L2_ApplicationRun(Event, wcFileName);
        }

        // Read data from partition(disk or USB etc..)                  
        Status = L1_STORE_READ(PartitionItemID, StartSectorNumber, 1, Buffer); 
        if (EFI_ERROR(Status))
        {
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
            return Status;
        }

        switch(pItems[index].Attribute[0])
        {
            //如果是目录，则显示子目录
            case 0x10:  L1_MEMORY_Memset(&pItems, 0, sizeof(pItems));
                        L1_MEMORY_Copy(&pItems, Buffer, DISK_BUFFER_SIZE);
                        L2_STORE_FolderItemsPrint();
                        break;

            //如果是文件，则显示文件内容
            case 0x20: L2_PARTITION_FileContentPrint(Buffer); break;

            default: break;
        }
        
    }

}
