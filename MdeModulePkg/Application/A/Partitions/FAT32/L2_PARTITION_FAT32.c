
/*************************************************
    .
    File name:          *.*
    Author：                任启红
    ID：                    00001
    Date:                  202107
    Description:        
    Others:             无

    History:            无
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
*  描述:   需要进行大小端转换，这里边应该可以封装成接口
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
    pDest->ReservedSelector = L1_NETWORK_2BytesToUINT16(pSource->ReservedSelector);
    pDest->SectorsPerFat    = L1_NETWORK_4BytesToUINT32(pSource->SectorsPerFat);
    pDest->BootPathStartCluster = L1_NETWORK_4BytesToUINT32(pSource->BootPathStartCluster);
    pDest->FATCount      = pSource->FATCount[0];
    pDest->SectorOfCluster = pSource->SectorOfCluster[0];

    //Todo: the other parameters can compute like above too
    //Current only get parameters we need to use
    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d ReservedSelector:%d SectorsPerFat:%d BootPathStartCluster: %d NumFATS:%d SectorOfCluster:%d", 
                                                __LINE__,        
                                                pDest->ReservedSelector,
                                                pDest->SectorsPerFat,
                                                pDest->BootPathStartCluster,
                                                pDest->FATCount,
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
    
    //这种可能存在内存泄露，如果没次申请内存都不释放
    pMBR = (MasterBootRecord *)L2_MEMORY_Allocate("MBR Buffer", MEMORY_TYPE_PARTITION, DISK_BUFFER_SIZE);
    if (NULL == pMBR)
    {
        return EFI_SUCCESS;
    }

    //
    L1_MEMORY_Copy(pMBR, p, DISK_BUFFER_SIZE);
    
    // 大端字节序：低位字节在高地址，高位字节低地址上。这是人类读写数值的方法。
    // 小端字节序：与上面相反。低位字节在低地址，高位字节在高地址。
    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d ReservedSelector:%02X%02X SectorsPerFat:%02X%02X%02X%02X BootPathStartCluster:%02X%02X%02X%02X NumFATS: %X", 
                                        __LINE__,
                                        pMBR->ReservedSelector[0], pMBR->ReservedSelector[1], 
                                        pMBR->SectorsPerFat[0], pMBR->SectorsPerFat[1], pMBR->SectorsPerFat[2], pMBR->SectorsPerFat[3],
                                        pMBR->BootPathStartCluster[0], pMBR->BootPathStartCluster[1], pMBR->BootPathStartCluster[2], pMBR->BootPathStartCluster[3],
                                        pMBR->FATCount[0]);
    
    
    L2_FILE_Transfer(pMBR, pMBRSwitched);

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
    
    L1_MEMORY_SetValue(Buffer1, 0, DISK_BUFFER_SIZE);
    
    Status = L2_STORE_Read(DeviceID, sector_count, 1, Buffer1); 
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
        return Status;
    }
    
    L1_MEMORY_Memset(&pItems, 0, sizeof(pItems));
    L1_MEMORY_Copy(&pItems, Buffer1, DISK_BUFFER_SIZE);

    // data area start from 1824, HZK16 file start from     FileBlockStart  block, so need to convert into sector by multi 8, block start number is 2   
    // next state is to read FAT table
    //sector_count = device[DeviceID].stMBRSwitched.ReservedSelector;
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
        //这样写死8192或者1592，会有BUG
        //正常应该用这个：device[DeviceID].StartSectorNumber
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
        Status = L2_STORE_Read(PartitionItemID, StartSectorNumber, 1, Buffer); 
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
            case 0x20: L2_PARTITION_FileContentPrint(Buffer); 

                        //我们将从U盘读取的缓冲区，前5位，每位在原来的基础上+1
                        for (UINT16 i = 0; i < 5; i++)
                            Buffer[i] += 1;
                            
                        //然后写入U盘，这样可以看到U盘目录下文件的变化，不过这里边有点问题，我们修改文件正常还需要记录修改文件的日期，时间信息
                        L2_STORE_Write(PartitionItemID, StartSectorNumber, 1, Buffer);
                        break;
    
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
    EFI_STATUS Status;
    
    UINT8 Buffer[DISK_BUFFER_SIZE];
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_FILE_FAT32_FileDelete PartitionItemID: %d FolderItemID: %d \n", __LINE__, PartitionItemID, FolderItemID);
        
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

        switch(pItems[index].Attribute[0])
        {
            //如果是目录，则显示子目录
            case 0x10:  L1_MEMORY_Memset(&pItems, 0, sizeof(pItems));
                        L1_MEMORY_Copy(&pItems, Buffer, DISK_BUFFER_SIZE);
                        L2_STORE_FolderItemsPrint();
                        break;

            //如果是文件，则XXX
            case 0x20: 
                        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], 
                        "%d File: \n", __LINE__);
                       //打印从U盘读取的内容
                       L2_STORE_Read(2, 16384, 1, Buffer);
                       //L2_PARTITION_FileContentPrint(Buffer);

                       Buffer[0x20 * 4 + 0] = 0xE5;
                       //Buffer[0x20 * 4 + 0] = 'E';
                       //Buffer[0x20 * 4 + 1] = '5';
                       L2_STORE_Write(2, 16384, 1, Buffer);
                        break;

                       sector_count = device[DeviceID].stMBRSwitched.ReservedSelector;
                       L2_STORE_Read(DeviceID, sector_count, 1, Buffer);
                       
                       for (UINT8 i = 0; i < 4; i++)
                       {
                            Buffer[StartCluster * 4 + i] = 0;
                       }
                       
                       L2_STORE_Write(DeviceID, sector_count, 1, Buffer);
                       ;
                    break;

            default: break;
        }
        
    }

}
