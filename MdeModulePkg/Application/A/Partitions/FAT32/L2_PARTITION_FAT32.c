
/*************************************************
    .
    File name:          *.*
    Author��                ������
    ID��                    00001
    Date:                  202107
    Description:        
    Others:             ��

    History:            ��
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
*  ����:   ��Ҫ���д�С��ת���������Ӧ�ÿ��Է�װ�ɽӿ�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
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
*  ����:   xxxxx
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L1_FILE_FAT32_DataSectorAnalysis(UINT8 *p, MasterBootRecordSwitched *pMBRSwitched)
{
    MasterBootRecord *pMBR;
    
    //pMBR = (MasterBootRecord *)AllocateZeroPool(DISK_BUFFER_SIZE);
    
    //���ֿ��ܴ����ڴ�й¶�����û�������ڴ涼���ͷ�
    pMBR = (MasterBootRecord *)L2_MEMORY_Allocate("MBR Buffer", MEMORY_TYPE_PARTITION, DISK_BUFFER_SIZE);
    if (NULL == pMBR)
    {
        return EFI_SUCCESS;
    }

    //
    L1_MEMORY_Copy(pMBR, p, DISK_BUFFER_SIZE);
    
    // ����ֽ��򣺵�λ�ֽ��ڸߵ�ַ����λ�ֽڵ͵�ַ�ϡ����������д��ֵ�ķ�����
    // С���ֽ����������෴����λ�ֽ��ڵ͵�ַ����λ�ֽ��ڸߵ�ַ��
    
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
*  ����:   xxxxx
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
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
*  ����:   xxxxx
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
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

    //��Ҫ�һ�ȡ��Ч��������
    UINT16 index = FolderItemValidIndexArray[FolderItemID];
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d index: %d\n", __LINE__, index);
        
    //FAT32�ļ�ϵͳ��ʽ
    if (device[PartitionItemID].FileSystemType == FILE_SYSTEM_FAT32)
    {

        UINT16 High2B = L1_NETWORK_2BytesToUINT16(pItems[index].StartClusterHigh2B);
        UINT16 Low2B  = L1_NETWORK_2BytesToUINT16(pItems[index].StartClusterLow2B);
        UINT32 StartCluster = (UINT32)High2B << 16 | (UINT32)Low2B;

        // Start cluster id is 2, exclude 0,1
        //����д��8192����1592������BUG
        //����Ӧ���������device[DeviceID].StartSectorNumber
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
            //�����Ŀ¼������ʾ��Ŀ¼
            case 0x10:  L1_MEMORY_Memset(&pItems, 0, sizeof(pItems));
                        L1_MEMORY_Copy(&pItems, Buffer, DISK_BUFFER_SIZE);
                        L2_STORE_FolderItemsPrint();
                        break;

            //������ļ�������ʾ�ļ�����
            case 0x20: L2_PARTITION_FileContentPrint(Buffer); 

                        //���ǽ���U�̶�ȡ�Ļ�������ǰ5λ��ÿλ��ԭ���Ļ�����+1
                        for (UINT16 i = 0; i < 5; i++)
                            Buffer[i] += 1;
                            
                        //Ȼ��д��U�̣��������Կ���U��Ŀ¼���ļ��ı仯������������е����⣬�����޸��ļ���������Ҫ��¼�޸��ļ������ڣ�ʱ����Ϣ
                        L2_STORE_Write(PartitionItemID, StartSectorNumber, 1, Buffer);
                        break;
    
            default: break;
        }
        
    }


}

/****************************************************************************
*
*  ����:   xxxxx
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_FILE_FAT32_FileAdd(UINT16 DeviceID)
{}


/****************************************************************************
*
*  ����:   xxxxx
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_FILE_FAT32_FileOpen(UINT16 DeviceID)
{}


/****************************************************************************
*
*  ����:   xxxxx
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_FILE_FAT32_FileDelete(UINT16 DeviceID)
{
    //��ʱ�Ȳ�ʵ��
    EFI_STATUS Status;
    
    UINT8 Buffer[DISK_BUFFER_SIZE];
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_FILE_FAT32_FileDelete PartitionItemID: %d FolderItemID: %d \n", __LINE__, PartitionItemID, FolderItemID);
        
    if (0xffff == PartitionItemID || 0xffff == FolderItemID)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d 0xffff == PartitionItemID\n", __LINE__);
        return -1;
    }

    //��Ҫ�һ�ȡ��Ч��������
    UINT16 index = FolderItemValidIndexArray[FolderItemID];
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d index: %d\n", __LINE__, index);
        
    //FAT32�ļ�ϵͳ��ʽ
    if (device[PartitionItemID].FileSystemType == FILE_SYSTEM_FAT32)
    {       

        UINT16 High2B = L1_NETWORK_2BytesToUINT16(pItems[index].StartClusterHigh2B);
        UINT16 Low2B  = L1_NETWORK_2BytesToUINT16(pItems[index].StartClusterLow2B);
        UINT32 StartCluster = (UINT32)High2B << 16 | (UINT32)Low2B;

        // Start cluster id is 2, exclude 0,1
        //����д��8192������BUG
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
            //�����Ŀ¼������ʾ��Ŀ¼
            case 0x10:  L1_MEMORY_Memset(&pItems, 0, sizeof(pItems));
                        L1_MEMORY_Copy(&pItems, Buffer, DISK_BUFFER_SIZE);
                        L2_STORE_FolderItemsPrint();
                        break;

            //������ļ�����XXX
            case 0x20: 
                        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], 
                        "%d File: \n", __LINE__);
                       //��ӡ��U�̶�ȡ������
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
