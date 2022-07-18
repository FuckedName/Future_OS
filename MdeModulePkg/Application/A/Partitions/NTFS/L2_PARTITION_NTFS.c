
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





#include <string.h>

#include <L2_PARTITION_NTFS.h>

#include <Devices/Store/L2_DEVICE_Store.h>

#include <Libraries/Network/L1_LIBRARY_Network.h>
#include <Graphics/L2_GRAPHICS.h>
#include <Global/Global.h>


extern UINT8 BufferMFT[DISK_BUFFER_SIZE * 2];

// Index of A0 attribute
IndexInformation A0Indexes[10] = {0};

//���ڷ����ļ�ϵͳʹ�ã�����ÿ�ζ������ͷţ��˷�ʱ�䣬���Խ�һ�����õ�buffer
UINT8 pBufferTemp[DISK_BUFFER_SIZE * 2];

UINT64 FileContentRelativeSector;

UINT8 pItem[DISK_BUFFER_SIZE * 2] = {0};



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
EFI_STATUS L2_FILE_NTFS_MFT_Item_Read(UINT16 DeviceID, UINT64 SectorStartNumber)
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d DeviceID: %d\n", __LINE__, DeviceID);
    //printf( "RootPathAnalysis\n" );
    //DEBUG ((EFI_D_INFO, "PartitionUSBRead!!\r\n"));
    EFI_STATUS Status ;
    EFI_HANDLE *ControllerHandle = NULL;

    //sector_count is MFT start sector, 5 * 2 means $ROOT sector...
    //Every MFT Item use 2 sector .
    Status = L2_STORE_Read(DeviceID, SectorStartNumber, 2, BufferMFT); 
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
        return Status;
    }
    
     for (int j = 0; j < 250; j++)
     {
        //L2_DEBUG_Print1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", BufferMFT[j] & 0xff);
     }  

    //Analysis MFT of NTFS File System..
    //L2_FILE_NTFS_MFTDollarRootFileAnalysis(BufferMFT);
    //MFTDollarRootAnalysisBuffer(BufferMFTDollarRoot); 

    // data area start from 1824, HZK16 file start from     FileBlockStart  block, so need to convert into sector by multi 8, block start number is 2   
    // next state is to read FAT table
    // sector_count = MBRSwitched.ReservedSelector;
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: sector_count:%ld FileLength: %d MBRSwitched.ReservedSelector:%ld\n",  __LINE__, sector_count, FileLength, device[DeviceID].stMBRSwitched.ReservedSelector);
      
    return EFI_SUCCESS;
}


UINT16 Index = 0;


// Find $Root file from all MFT(may be 15 file,)
// pBuffer store all MFT
//�Ƚ���Ҫ�ļ���������0X30�ļ������ԣ����м�¼�Ÿ�Ŀ¼�����ļ����ļ�����
//0X80�������Լ�¼���ļ��е����ݣ�0X90���������ԣ�����Ÿ�Ŀ¼�µ���Ŀ¼�����ļ��������
//��ĳ��Ŀ¼�µ����ݱȽ϶࣬�Ӷ�����0X90�����޷���ȫ���ʱ��0XA0���Ի�ָ��һ�������������������������˸�Ŀ¼������ʣ�����ݵ������
EFI_STATUS  L2_FILE_NTFS_MFTDollarRootFileAnalysis(UINT8 *pBuffer)
{
    UINT8 *p = NULL;
    UINT8 pItem[200] = {0}; // to save one item analysised
    
    p = (UINT8 *)AllocateZeroPool(DISK_BUFFER_SIZE * 2);
    if (p == NULL)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: p == NULL \n", __LINE__);
        return EFI_SUCCESS;
    }

    for (int i = 0; i < DISK_BUFFER_SIZE * 2; i++)
        p[i] = pBuffer[i];

    Index = 0;
    
    // File header length
    UINT16 AttributeOffset = L1_NETWORK_2BytesToUINT16(((NTFS_FILE_HEADER *)p)->AttributeOffset);
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: AttributeOffset:%X \n", __LINE__, AttributeOffset);

    // location of a0 attribute may be in front of 10 
    // ofcourse mybe some bugs...
    for (int i = 0; i < 10; i++)
    {
        UINT8 size[4];

        // get Attribute size
        for (int i = 0; i < 4; i++)
            size[i] = p[AttributeOffset + 4 + i];
            
        //INFO_SELF("%02X%02X%02X%02X\n", size[0], size[1], size[2], size[3]);
        UINT16 AttributeSize = L1_NETWORK_4BytesToUINT32(size);

        // Copy attribute to buffer
        for (int i = 0; i < AttributeSize; i++)
            pItem[i] = p[AttributeOffset + i];

        // after buffer copied, we can get information in item
        UINT16  NameSize = ((NTFS_FILE_ATTRIBUTE_HEADER *)pItem)->NameSize;
        
        UINT16  NameOffset = L1_NETWORK_2BytesToUINT16(((NTFS_FILE_ATTRIBUTE_HEADER *)pItem)->NameOffset);
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Type[0]: %02X AttributeSize: %02X NameSize: %02X NameOffset: %02X\n", __LINE__, 
                                                            ((NTFS_FILE_ATTRIBUTE_HEADER *)pItem)->Type[0],
                                                            AttributeSize,
                                                            NameSize,
                                                            NameOffset);   
                                                            
         //A0 attribute is very important for us to analysis root path items(file or folder)
         // ofcourse the other parameter of attribut can analysis, if you want
         if (0xA0 == ((NTFS_FILE_ATTRIBUTE_HEADER *)pItem)->Type[0])
         {
            // every name char use two bytes.
            UINT16 DataRunsSize = AttributeSize - NameOffset - NameSize * 2;
            if (DataRunsSize > 100 || DataRunsSize < 0)
            {
                L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: DataRunsSize illegal.", __LINE__);
                return ;
            }
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: A0 attribute has been found: ", __LINE__);
            //for (int i = NameOffset; i < NameOffset + NameSize * 2; i++)
            //    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: %02X ", __LINE__, pItem[i] & 0xff);

            int j = 0;

            //get data runs
            UINT8 DataRuns[20] = {0};
            for (int i = NameOffset + NameSize * 2; i < AttributeSize; i++)
            {           
                DataRuns[j] = pItem[i] & 0xff;
                L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: %02X ", __LINE__, DataRuns[j] & 0xff);   
                j++;
            }
            L2_FILE_NTFS_DollarRootA0DatarunAnalysis(DataRuns);
            break;
         }
         
        AttributeOffset += AttributeSize;
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
EFI_STATUS L2_FILE_NTFS_RootPathItemsRead(UINT8 PartitionID)
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d DeviceType: %d, SectorCount: %lld\n", __LINE__, PartitionID);
    EFI_STATUS Status ;
    UINT8 BufferBlock[DISK_BLOCK_BUFFER_SIZE];

    UINT8 k = 0;
    UINT16 lastOffset = 0;
   
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Offset:%ld OccupyCluster:%ld\n",  __LINE__, 
                                                                     (A0Indexes[k].Offset + lastOffset) * 8, 
                                                                     A0Indexes[k].OccupyCluster * 8);
    
    // cluster need to multi with 8 then it is sector.
    // ��ȡ��Ŀ¼���INDEX ITEM ���̻���
    Status = L2_STORE_Read(PartitionID, (A0Indexes[k].Offset + lastOffset) * 8 , A0Indexes[k].OccupyCluster * 8, BufferBlock);
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
        return Status;
    }

    // ����INDEX ITEM ����
    L2_FILE_NTFS_MFTIndexItemsAnalysis(BufferBlock, PartitionID);    
    
    lastOffset = A0Indexes[k].Offset;
    
    return EFI_SUCCESS;
}



//Print ROOT Path items.
/****************************************************************************
*
*  ����:   ����NTFS�ļ�ϵͳ��������
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS  L2_FILE_NTFS_MFTIndexItemsAnalysis(UINT8 *pBuffer, UINT8 DeviceID)
{
    UINT8 *p = NULL;
    
    p = (UINT8 *)AllocateZeroPool(DISK_BUFFER_SIZE * 8);

    if (NULL == p)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d NULL == p\n", __LINE__);
        return EFI_SUCCESS;
    }
    
    //memcpy(p, pBuffer[512 * 2], DISK_BUFFER_SIZE);
    for (UINT16 i = 0; i < 512 * 8; i++)
        p[i] = pBuffer[i];

    //IndexEntryOffset:�������ƫ�� ����ڵ�ǰλ��
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d IndexEntryOffset: %llu IndexEntrySize: %llu\n", __LINE__, 
                                                                     L1_NETWORK_4BytesToUINT32(((NTFS_INDEX_HEADER *)p)->IndexEntryOffset),
                                                                     L1_NETWORK_4BytesToUINT32(((NTFS_INDEX_HEADER *)p)->IndexEntrySize));

    // ����ڵ�ǰλ�� need to add size before this Byte.
    // ��ȡ����ͷ��ߵ�������ƫ���ֶΡ�
    //
    UINT8 IndexEntryOffset = L1_NETWORK_4BytesToUINT32(((NTFS_INDEX_HEADER *)p)->IndexEntryOffset) + 24;
    
    UINT8 pItem[200] = {0};
    UINT16 index = IndexEntryOffset;

    //L1_MEMORY_Memset(&pItems, 0, sizeof(pItems));
    UINT8 j = 0;

    pCommonStorageItems[0].ItemCount = 0;

    //��INDEX ITEM�����������������д�빫�����ļ����ļ��������ݽṹ���Ա�������ҵĵ�����ʾ
    for (UINT8 i = 0; ; i++)
    {    
         if (index >= L1_NETWORK_4BytesToUINT32(((NTFS_INDEX_HEADER *)p)->IndexEntrySize))
            break;
            
         //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: index: %d\n", __LINE__,  index);  
         // length use 2b at 8, 9 bit
         UINT16 IndexItemLength = pBuffer[index + 8] + pBuffer[index + 9] * 16;
         if (0 == IndexItemLength)
         {
             return;
         }

         // copy item into pItem buffer
        for (int i = 0; i < IndexItemLength; i++)
            pItem[i] = pBuffer[index + i];
            
         UINT8 FileNameSize = ((NTFS_INDEX_ITEM *)pItem)->FileNameSize;

         //ǰ�����ֽڡ���
         FileContentRelativeSector = L1_NETWORK_6BytesToUINT64(((NTFS_INDEX_ITEM *)pItem)->MFTReferNumber);
         /*L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: attribut length2: %d FileNameSize: %d\n", __LINE__, 
                                                                   length2,
                                                                   FileNameSize);  */  
         UINT8 attributeName[20];                                                                    
         for (int i = 0; i < FileNameSize; i++)
         {
             //every name char use 2 Bytes.
             // Item before name use 82 Bytes. 
            attributeName[i] = pItem[82 + 2 * i];
         }

         UINT64 FileFlag = L1_NETWORK_8BytesToUINT64(((NTFS_INDEX_ITEM *)pItem)->FileFlag);
         L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Name: %a, Sector: %llu, FileFlag: %llX\n", __LINE__, attributeName, FileContentRelativeSector, FileFlag);
         //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%s attributeName: %a\n", __LINE__,  attributeName);  

        if (FileFlag == 0x20) //File
        { 
            pCommonStorageItems[j].Type = COMMON_STORAGE_ITEM_FILE;
            L1_MEMORY_Copy(pCommonStorageItems[j].Name, attributeName, L1_STRING_Length(attributeName));
            pCommonStorageItems[j].Name[L1_STRING_Length(attributeName)] = 0;
            pCommonStorageItems[j].Size = 100;
            pCommonStorageItems[j].FileContentRelativeSector = FileContentRelativeSector;
            pCommonStorageItems[0].ItemCount++;
            j++;
        }
        else if (FileFlag == 0x10000000) //Folder
        {
            pCommonStorageItems[j].Type = COMMON_STORAGE_ITEM_FOLDER;
            L1_MEMORY_Copy(pCommonStorageItems[j].Name, attributeName, L1_STRING_Length(attributeName));
            pCommonStorageItems[j].Name[L1_STRING_Length(attributeName)] = 0;
            pCommonStorageItems[j].Size = 200;
            pCommonStorageItems[0].ItemCount++;
            pCommonStorageItems[j].FileContentRelativeSector = FileContentRelativeSector;
            j++;
        }
                
        index += IndexItemLength;
    }

    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: ItemCount: %d\n", __LINE__, pCommonStorageItems[0].ItemCount);
         
}



// Analysis attribut A0 of $Root
EFI_STATUS  L2_FILE_NTFS_DollarRootA0DatarunAnalysis(UINT8 *p)
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: string length: %d\n", __LINE__,  L1_STRING_Length(p));

    UINT16 length = L1_STRING_Length(p);
    UINT8  occupyCluster = 0;
    UINT32 offset = 0;

    UINT16 i = 0;
    while(i < length)
    {
        // for exampleData runs:11 01 24
        // Hight 4 bit
        UINT8  offsetLength  = p[i] >> 4;

        //Low 4bit
        UINT8  occupyClusterLength = p[i] & 0x0f;
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: OccupyClusterLength: %d offsetLength: %d\n", __LINE__,  occupyClusterLength, offsetLength);
        
        i++;

        // Current only handle value is 1
        if (occupyClusterLength == 1)
            A0Indexes[Index].OccupyCluster = p[i];
        
        i++;
        //INFO_SELF(" i: %d\n", i);
        if (offsetLength == 1)
        {
            offset = p[i];
        }
        else if (offsetLength == 2)
        {
            UINT8 size[2];
            size[0] = p[i];
            size[1] = p[i + 1];
            offset = L1_NETWORK_2BytesToUINT16(size);
        }
        else if (offsetLength == 3)
        {
            UINT8 size[3];
            size[0] = p[i];
            size[1] = p[i + 1];
            size[2] = p[i + 2];
            offset = L1_NETWORK_3BytesToUINT32(size);
        }
        A0Indexes[Index].Offset = offset;
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d offset:%d \n", __LINE__, offset);
        i += offsetLength;
        Index++;
        //INFO_SELF(" i: %d\n", i);
    }

}


IndexInformation *L2_FILE_NTFS_GetA0Indexes()
{    
    return A0Indexes;
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
EFI_STATUS L2_FILE_NTFS_FirstSelectorAnalysis(UINT8 *p, DollarBootSwitched *pNTFSBootSwitched)
{
    DOLLAR_BOOT *pDollarBoot;
    
    pDollarBoot = (DOLLAR_BOOT *)L2_MEMORY_Allocate("DOLLAR_BOOT Buffer", MEMORY_TYPE_PARTITION, DISK_BUFFER_SIZE);
    //pDollarBoot = (DOLLAR_BOOT *)AllocateZeroPool(DISK_BUFFER_SIZE);
    L1_MEMORY_Copy(pDollarBoot, p, DISK_BUFFER_SIZE);
  
    L1_FILE_NTFS_DollerRootTransfer(pDollarBoot, pNTFSBootSwitched);

    //FreePool(pDollarBoot);
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
VOID L1_FILE_NTFS_DollerRootTransfer(DOLLAR_BOOT *pSource, DollarBootSwitched *pDest)
{
    pDest->BitsOfSector = L1_NETWORK_2BytesToUINT16(pSource->BitsOfSector);
    pDest->SectorOfCluster = (UINT16)pSource->SectorOfCluster;
    pDest->AllSectorCount = L1_NETWORK_8BytesToUINT64(pSource->AllSectorCount) + 1;
    pDest->MFT_StartCluster = L1_NETWORK_8BytesToUINT64(pSource->MFT_StartCluster);
    pDest->MFT_MirrStartCluster = L1_NETWORK_8BytesToUINT64(pSource->MFT_MirrStartCluster);
    
}




// L2_FILE_NTFS_MFTIndexItemsAnalysis
UINT16  L2_FILE_NTFS_IndexItemHeaderAnalysis(UINT8 *p)
{
    NTFS_INDEX_HEADER;
    
    // ����ڵ�ǰλ�õ�ƫ�ƣ�������Ҫ���ϵ�ǰλ��ǰ���24���ֽڡ� need to add size before this Byte.
    UINT16 HeaderLength = L1_NETWORK_4BytesToUINT32(((NTFS_INDEX_HEADER *)p)->IndexEntryOffset) + 24;

    return HeaderLength;
}

EFI_STATUS  L2_FILE_NTFS_IndexItemAttributeAnalysis(UINT8 *pBuffer, UINT16 Offset)
{
    NTFS_INDEX_ITEM;

    if (Offset >= L1_NETWORK_4BytesToUINT32(((NTFS_INDEX_HEADER *)pBuffer)->IndexEntrySize))
            return -1;
    
    UINT8 pItem[200] = {0};

    // �������С
    UINT16 length2 = pBuffer[Offset + 8] + pBuffer[Offset + 9] * 16;

    // copy item into pItem buffer
    for (int i = 0; i < length2; i++)
        pItem[i] = pBuffer[Offset + i];

    UINT8 FileNameSize = ((NTFS_INDEX_ITEM *)pItem)->FileNameSize;
    FileContentRelativeSector = L1_NETWORK_6BytesToUINT64(((NTFS_INDEX_ITEM *)pItem)->MFTReferNumber);
    /*L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: attribut length2: %d FileNameSize: %d\n", __LINE__, 
                                                           length2,
                                                           FileNameSize);  */  
    UINT8 attributeName[20];                                                                    
    for (int i = 0; i < FileNameSize; i++)
    {
        //every name char use 2 Bytes.
        // Item before name use 82 Bytes. 
        attributeName[i] = pItem[82 + 2 * i];
    }

    UINT8 IndexFlag = L1_NETWORK_2BytesToUINT16(((NTFS_INDEX_ITEM *)pItem)->IndexFlag);
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Name: %a, RelativeSector: %llu, IndexFlag: %d\n", __LINE__, attributeName, FileContentRelativeSector, IndexFlag);

    return length2;    
}


//NTFS�ļ�ϵͳ ���������
//
EFI_STATUS  L2_FILE_NTFS_IndexItemBufferAnalysis(UINT8 *pBuffer)
{
    NTFS_INDEX_HEADER;
    NTFS_INDEX_ITEM;
    
    UINT8 *p = NULL;
        
    p = (UINT8 *)AllocateZeroPool(DISK_BUFFER_SIZE * 8);

    if (NULL == p)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d NULL == p\n", __LINE__);
        return EFI_SUCCESS;
    }
    
    //memcpy(p, pBuffer[512 * 2], DISK_BUFFER_SIZE);
    for (UINT16 i = 0; i < 512 * 8; i++)
        p[i] = pBuffer[i];

    // ��ȡ����ͷ��ز���
    UINT16 Offset = L2_FILE_NTFS_IndexItemHeaderAnalysis(p);

    // ��ȡ������������صĲ���
    for (UINT16 i = 0; i < 10; i++)
    {
        UINT16 AttributeSize = L2_FILE_NTFS_IndexItemAttributeAnalysis(p, Offset);
        
        Offset += AttributeSize;
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
UINT16 L2_FILE_NTFS_FileItemHeaderAnalysis(UINT8 *pBuffer, NTFS_FILE_SWITCHED *pNTFSFileSwitched)
{
    // File header length
    UINT16 AttributeOffset = L1_NETWORK_2BytesToUINT16(((NTFS_FILE_HEADER *)pBuffer)->AttributeOffset);
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: AttributeOffset:%X \n", __LINE__, AttributeOffset);

    pNTFSFileSwitched->NTFSFileHeaderSwitched.AttributeOffset = AttributeOffset;
    
    return AttributeOffset;
}

EFI_STATUS  L2_FILE_NTFS_FileItemNonResidentAttributeAnalysis(pBuffer, Offset)
{
}


EFI_STATUS  L2_FILE_NTFS_FileItemResidentAttributeAnalysis(pBuffer, Offset)
{
}


EFI_STATUS  L2_FILE_NTFS_DollarVolumeNameAttributeAnalysis(UINT8 *pBuffer, UINT8 *pData)
{
}


/****************************************************************************
*
*  ����: ����NTFS�Ĵ���FILE�ע�ⲻ��Ŀ¼�µ��ļ�����ߵ����ԣ�ÿ��FILE��һ���кܶ�����ԣ�������4��5��
*
*  //�Ƚ���Ҫ�ļ���������0X30�ļ������ԣ����м�¼�Ÿ�Ŀ¼�����ļ����ļ�����
*  //0X80�������Լ�¼���ļ��е����ݣ�
*  //0X90���������ԣ�����Ÿ�Ŀ¼�µ���Ŀ¼�����ļ����������ĳ��Ŀ¼�µ����ݱȽ϶࣬�Ӷ�����0X90�����޷���ȫ���ʱ��
*  //0XA0���Ի�ָ��һ�������������������������˸�Ŀ¼������ʣ�����ݵ������
*  //�����г�פ���Ժͷǳ�פ����֮�֣���һ�����Ե������ܹ���1KB���ļ���¼�б����ʱ�򣬸�����Ϊ��פ���ԣ�
*  //�������Ե������޷����ļ���¼�д�ţ���Ҫ��ŵ�MFT�������λ��ʱ��������Ϊ�ǳ�פ���ԡ���פ���Ժͷǳ�פ���Ե�ͷ���ṹ�������£�
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX  
*
*
*****************************************************************************/
UINT16  L2_FILE_NTFS_FileItemAttributeAnalysis(UINT8 *p, UINT16 AttributeOffset, NTFS_FILE_ATTRIBUTE_HEADER_SWITCHED *pAttributeHeaderSwitched)
{
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FUNCTION: %a", __LINE__, __FUNCTION__);
    
    UINT8 size[4];
    UINT16 AttributeSize;
    UINT16 i;

    L1_MEMORY_Memset(pItem, 0, DISK_BUFFER_SIZE * 2);

    pAttributeHeaderSwitched->Type = p[AttributeOffset];
    if (0 == pAttributeHeaderSwitched->Type || MFT_ATTRIBUTE_DOLLAR_EA <= pAttributeHeaderSwitched->Type)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MFT_ATTRIBUTE_INVALID: %X ", __LINE__, __FUNCTION__, pAttributeHeaderSwitched->Type);
        return 0;
    }
    // ����ͷ���������ܳ���
    for (i = 0; i < 4; i++)
        size[i] = p[AttributeOffset + 4 + i];
        
    AttributeSize = L1_NETWORK_4BytesToUINT32(size);

    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: AttributeType: %02X AttributeSize: %02X ", __LINE__, p[AttributeOffset], AttributeSize);
    if (0 == AttributeSize)
        return AttributeSize;
    
    // Copy attribute to buffer
    for (i = 0; i < AttributeSize; i++)
        pItem[i] = p[AttributeOffset + i];
    
    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X ", __LINE__, pItem[0], 
                    pItem[1],
                    pItem[2],
                    pItem[3],
                    pItem[4],
                    pItem[5],
                    pItem[6],
                    pItem[7],
                    pItem[8],
                    pItem[9],
                    pItem[10],
                    pItem[11],
                    pItem[12],
                    pItem[13],
                    pItem[14],
                    pItem[15]);
                            
    // after buffer copied, we can get information in item
    pAttributeHeaderSwitched->NameSize = ((NTFS_FILE_ATTRIBUTE_HEADER *)pItem)->NameSize;
    
    pAttributeHeaderSwitched->NameOffset = L1_NETWORK_2BytesToUINT16(((NTFS_FILE_ATTRIBUTE_HEADER *)pItem)->NameOffset);
                                                        
    pAttributeHeaderSwitched->ResidentFlag = ((NTFS_FILE_ATTRIBUTE_HEADER *)pItem)->ResidentFlag;
    
    
    UINT16 NameOffset = pAttributeHeaderSwitched->NameOffset;
    UINT8 NameSize = pAttributeHeaderSwitched->NameSize;
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Type: %02X ResidentFlag: %d ", __LINE__, pAttributeHeaderSwitched->Type, pAttributeHeaderSwitched->ResidentFlag);

    switch (pAttributeHeaderSwitched->Type)
    {
        //A0 Attribute
        case MFT_ATTRIBUTE_DOLLAR_INDEX_ALLOCATION:
            {                
                int j = 0;
                UINT8 DataRuns[20] = {0};
                for (int i = NameOffset + NameSize * 2; i < AttributeSize; i++)
                {           
                    DataRuns[j] = pItem[i] & 0xff;
                    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: %02X ", __LINE__, DataRuns[j] & 0xff);   
                    j++;
                }

                //���ڲ��Ի�ȡ��ֵ�Ƿ���ȷ
                /*L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: %02X %02X %02X %02X %02X %02X %02X %02X\n", __LINE__, 
                                                            pItem[pAttributeHeaderSwitched->NameOffset + pAttributeHeaderSwitched->NameSize * 2 + 0],
                                                            pItem[pAttributeHeaderSwitched->NameOffset + pAttributeHeaderSwitched->NameSize * 2 + 1],
                                                            pItem[pAttributeHeaderSwitched->NameOffset + pAttributeHeaderSwitched->NameSize * 2 + 2],
                                                            pItem[pAttributeHeaderSwitched->NameOffset + pAttributeHeaderSwitched->NameSize * 2 + 3],
                                                            pItem[pAttributeHeaderSwitched->NameOffset + pAttributeHeaderSwitched->NameSize * 2 + 4],
                                                            pItem[pAttributeHeaderSwitched->NameOffset + pAttributeHeaderSwitched->NameSize * 2 + 5],
                                                            pItem[pAttributeHeaderSwitched->NameOffset + pAttributeHeaderSwitched->NameSize * 2 + 6],
                                                            pItem[pAttributeHeaderSwitched->NameOffset + pAttributeHeaderSwitched->NameSize * 2 + 7]);
                */
                
                Index = 0;
                L2_FILE_NTFS_DollarRootA0DatarunAnalysis(DataRuns);
            }
            break;

        case MFT_ATTRIBUTE_DOLLAR_VOLUME_NAME:
            for(UINT8 i = 0; i * 2 < AttributeSize - pAttributeHeaderSwitched->NameOffset; i++)
            {
                pAttributeHeaderSwitched->Data[i] = pItem[pAttributeHeaderSwitched->NameOffset + i * 2];
            }
            break;
            
        case MFT_ATTRIBUTE_DOLLAR_DATA:
            for(UINT8 i = 0; i < AttributeSize - pAttributeHeaderSwitched->NameOffset; i++)
            {
                pAttributeHeaderSwitched->Data[i] = pItem[pAttributeHeaderSwitched->NameOffset + i];
            }
            pAttributeHeaderSwitched->DataSize = AttributeSize - pAttributeHeaderSwitched->NameOffset;
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: %02X %02X %02X %02X %02X %02X",
                                pItem[pAttributeHeaderSwitched->NameOffset + 0],
                                pItem[pAttributeHeaderSwitched->NameOffset + 1],
                                pItem[pAttributeHeaderSwitched->NameOffset + 2],
                                pItem[pAttributeHeaderSwitched->NameOffset + 3],
                                pItem[pAttributeHeaderSwitched->NameOffset + 4],
                                pItem[pAttributeHeaderSwitched->NameOffset + 5]);
            break;

        //90 ���Է�������Щ���ӣ������ЩBUG����Ҫ����ʵ�����������
        //90 ���԰�����NTFS_FILE_ATTRIBUTE_HEADER ResidentAttributeHeader INDEX_ROOT INDEX_ENTRY ��4���ṹ�塣
        case MFT_ATTRIBUTE_DOLLAR_INDEX_ROOT:
            {    
                //������Գ���̫�̣���ʾ��Ų������ݣ�����������BUG
                if (AttributeSize <= 0x58)
                {
                    return AttributeSize;
                }                    
            
                //return AttributeSize;
                //L2_PARTITION_BufferPrint(pItem, 0x120);
                L1_MEMORY_Memset((void *)pCommonStorageItems, 0, 32 * sizeof(COMMON_STORAGE_ITEM));
                for (UINT8 i = 0; i < 32 ; i++)
                {
                    for (UINT8 j = 0; j < 100; j++)
                    {
                        pCommonStorageItems[i].Name[j] = 0;
                    }
                    pCommonStorageItems[i].FileContentRelativeSector = 0;
                    pCommonStorageItems[i].ItemCount = 0;
                    pCommonStorageItems[i].Type = COMMON_STORAGE_ITEM_MAX;
                }

                //L2_PARTITION_BufferPrint(pItem, AttributeSize);
                
                if (pAttributeHeaderSwitched->ResidentFlag == 0) //��פ
                {
                    UINT16 AttributeHeaderSize = sizeof(NTFS_FILE_ATTRIBUTE_HEADER);
                    UINT16 Offset = AttributeHeaderSize;
                    unsigned long DataSize = L1_NETWORK_4BytesToUINT32(((ResidentAttributeHeader *)pItem[Offset])->ATTR_DataSize);
                                        
                    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Offset: %d ", __LINE__, Offset);
                        
                    UINT16 ResidentAttributeHeaderSize = sizeof(ResidentAttributeHeader);
                    Offset += ResidentAttributeHeaderSize;
                            
                    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Offset: %d ", __LINE__, Offset);
                        
                    unsigned long AttributeType = L1_NETWORK_4BytesToUINT32(((INDEX_ROOT *)pItem[Offset])->IR_AttributeType);

                    
                    UINT16 IndexRootSize = sizeof(INDEX_ROOT);
                    Offset += IndexRootSize;        
                    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Offset: %d ", __LINE__, Offset);
                        
                    UINT8 j = 0;

                    // INDEX_ENTRY
                    INDEX_ENTRY;
                    while (TRUE)
                    {            
                        UINT64 IE_MftReferNumber = L1_NETWORK_8BytesToUINT64(pItem[Offset]);
                        UINT8 IE_FileNameSize = pItem[Offset + 0x50];
                        UINT8 IE_FileNamespace = pItem[Offset + 0x51];
                        UINT64 IE_FileFlag = pItem[Offset + 0x48] | (UINT64)pItem[Offset + 0x48 + 1] << 8 | (UINT64)pItem[Offset + 0x48 + 2]  << 16 |  
                                             (UINT64)pItem[Offset + 0x48 + 3]  << 24 | (UINT64)pItem[Offset + 0x48 + 4]  << 32 |  
                                             (UINT64)pItem[Offset + 0x48 + 5]  << 40 | (UINT64)pItem[Offset + 0x48 + 6]  << 48 |  
                                             (UINT64)pItem[Offset + 0x48 + 7]  << 56 ;
                        UINT16 IE_Size = (UINT16)pItem[Offset + 0x8] | (UINT16)pItem[Offset + 0x9] << 8;
    
                        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: NameSize: %02X Namespace: %02X IE_FileFlag: %X IE_Size: %X ", __LINE__, IE_FileNameSize, IE_FileNamespace, IE_FileFlag, IE_Size);
                            
                        for(UINT8 i = 0; i < IE_FileNameSize; i++)
                        {
                            pAttributeHeaderSwitched->Data[i] = pItem[Offset + 0x52 + i * 2];
                            pCommonStorageItems[j].Name[i] = pItem[Offset + 0x52 + i * 2];
                        }
                        
                        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: %c%c%c%c%c%c%c%c%c ", __LINE__, 
                                                                                                                                pCommonStorageItems[j].Name[0], 
                                                                                                                                pCommonStorageItems[j].Name[1], 
                                                                                                                                pCommonStorageItems[j].Name[2], 
                                                                                                                                pCommonStorageItems[j].Name[3], 
                                                                                                                                pCommonStorageItems[j].Name[4], 
                                                                                                                                pCommonStorageItems[j].Name[5], 
                                                                                                                                pCommonStorageItems[j].Name[6], 
                                                                                                                                pCommonStorageItems[j].Name[7], 
                                                                                                                                pCommonStorageItems[j].Name[8]);
                        pCommonStorageItems[j].Type = COMMON_STORAGE_ITEM_FILE;
                        
                        //����д�����������ÿ���������Ч�����Ż�
                        UINT64 RelativeSector = pItem[Offset] + (UINT64)pItem[Offset + 1] * 256 + (UINT64)pItem[Offset + 2] * 256 * 256 + (UINT64)pItem[Offset + 3] * 256 * 256 * 256 + (UINT64)pItem[Offset + 4] * 256 * 256 * 256 * 256 + (UINT64)pItem[Offset + 5] * 256 * 256 * 256 * 256 * 256;
                        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: RelativeSector: %LLX", __LINE__, 
                                                                                                                                RelativeSector);
                        pCommonStorageItems[j].FileContentRelativeSector = RelativeSector;
                        

                        //��ߺ����е�BUG��IE_FileFlag��ȡ��ֵ��̫��
                        switch (IE_FileFlag)
                        {
                            case 0x20:
                                pCommonStorageItems[j].Type = COMMON_STORAGE_ITEM_FILE;
                                break;
                                
                            case 0x10000000:
                                pCommonStorageItems[j].Type = COMMON_STORAGE_ITEM_FOLDER;
                                break;
                                
    
                            default:;
                        }
                        
                        Offset += IE_Size;
                        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: %X ", __LINE__, pItem[Offset]);
                        
                        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: %02X %02X %02X %02X ", __LINE__, 
                                                                                                                                pItem[Offset + 0], 
                                                                                                                                pItem[Offset + 1], 
                                                                                                                                pItem[Offset + 2], 
                                                                                                                                pItem[Offset + 3]);
                        j++;
                        if (0 == pItem[Offset + 0] && 0 == pItem[Offset + 1] && 0 == pItem[Offset + 2] && 0 == pItem[Offset + 3])
                        {
                            return AttributeSize;
                        }
                    }

                    break;
                }
                
                pAttributeHeaderSwitched->DataSize = 4;

            }
            break;
                    
        default:
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Default Attribute Type: %d ", __LINE__, pAttributeHeaderSwitched->Type);
    }         

    return AttributeSize;
}



//File record in NTFS file system means data on disk, not file in folder.. 
//L2_FILE_NTFS_MFTDollarRootFileAnalysis
/****************************************************************************
*
*  ����: ����NTFS�Ĵ���FILE�ע�ⲻ��Ŀ¼�µ��ļ�����ÿ��FILE��һ��ռ����������
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS  L2_FILE_NTFS_FileItemBufferAnalysis(UINT8 *pBuffer, NTFS_FILE_SWITCHED *pNTFSFileSwitched)
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: ", __LINE__);
    UINT16 AttributeSize;
    
    for (UINT16 i = 0; i < DISK_BUFFER_SIZE * 2; i++)
        pBufferTemp[i] = pBuffer[i];
    
    // ͨ������ͷ�����һ������ƫ��
    // ��ȡ�ļ�ͷ��Ϣ
    UINT16 Offset = L2_FILE_NTFS_FileItemHeaderAnalysis(pBufferTemp, pNTFSFileSwitched);
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Offset: %d", __LINE__, Offset);
        
    // ��ȡÿ��������Ϣ
    // ��ǰֻ����10�����ԣ���ʵ����ȱ�ݵ�
    for (UINT16 i = 0; ; i++)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: For loop analysis attribute count: %d ", __LINE__, i + 1);
        AttributeSize = L2_FILE_NTFS_FileItemAttributeAnalysis(pBuffer, Offset, &(pNTFSFileSwitched->NTFSFileAttributeHeaderSwitched[i]));
        if (AttributeSize == 0 || AttributeSize > 0x200 || Offset >= DISK_BUFFER_SIZE * 2)
        {
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: For loop end AttributeSize: %d ", __LINE__, AttributeSize);
            break;
        }            
        
        Offset += AttributeSize;
    }
}



UINT16 L2_FILE_NTFS_FileItemHeaderAnalysis2(UINT8 *pBuffer, NTFS_FILE_SWITCHED *pNTFSFileSwitched)
{
    // File header length
    UINT16 AttributeOffset = L1_NETWORK_2BytesToUINT16(((NTFS_FILE_HEADER *)pBuffer)->AttributeOffset);
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: AttributeOffset:%X \n", __LINE__, AttributeOffset);

    pNTFSFileSwitched->NTFSFileHeaderSwitched.AttributeOffset = AttributeOffset;
    
    return AttributeOffset;
}



/****************************************************************************
*
*  ����: ����NTFS�Ĵ���FILE�ע�ⲻ��Ŀ¼�µ��ļ�����ߵ����ԣ�ÿ��FILE��һ���кܶ�����ԣ�������4��5��
*
*  //�Ƚ���Ҫ�ļ���������0X30�ļ������ԣ����м�¼�Ÿ�Ŀ¼�����ļ����ļ�����
*  //0X80�������Լ�¼���ļ��е����ݣ�
*  //0X90���������ԣ�����Ÿ�Ŀ¼�µ���Ŀ¼�����ļ����������ĳ��Ŀ¼�µ����ݱȽ϶࣬�Ӷ�����0X90�����޷���ȫ���ʱ��
*  //0XA0���Ի�ָ��һ�������������������������˸�Ŀ¼������ʣ�����ݵ������
*  //�����г�פ���Ժͷǳ�פ����֮�֣���һ�����Ե������ܹ���1KB���ļ���¼�б����ʱ�򣬸�����Ϊ��פ���ԣ�
*  //�������Ե������޷����ļ���¼�д�ţ���Ҫ��ŵ�MFT�������λ��ʱ��������Ϊ�ǳ�פ���ԡ���פ���Ժͷǳ�פ���Ե�ͷ���ṹ�������£�
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX  
*
*
*****************************************************************************/
UINT16  L2_FILE_NTFS_FileItemAttributeAnalysis2(UINT8 *p, UINT16 AttributeOffset, NTFS_FILE_ATTRIBUTE_HEADER_SWITCHED *pAttributeHeaderSwitched)
{
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FUNCTION: %a", __LINE__, __FUNCTION__);
    
    UINT8 size[4];
    UINT16 AttributeSize;
    UINT16 i;

    L1_MEMORY_Memset(pItem, 0, DISK_BUFFER_SIZE * 2);

    pAttributeHeaderSwitched->Type = p[AttributeOffset];
    if (0 == pAttributeHeaderSwitched->Type || MFT_ATTRIBUTE_DOLLAR_EA <= pAttributeHeaderSwitched->Type)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MFT_ATTRIBUTE_INVALID: %X ", __LINE__, __FUNCTION__, pAttributeHeaderSwitched->Type);
        return 0;
    }
    
    // ����ͷ���������ܳ���
    for (i = 0; i < 4; i++)
        size[i] = p[AttributeOffset + 4 + i];
        
    AttributeSize = L1_NETWORK_4BytesToUINT32(size);

    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: AttributeType: %02X AttributeSize: %02X ", __LINE__, p[AttributeOffset], AttributeSize);
    if (0 == AttributeSize)
        return AttributeSize;
    
    // Copy attribute to buffer
    for (i = 0; i < AttributeSize; i++)
        pItem[i] = p[AttributeOffset + i];
    
    /*
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X ", __LINE__, pItem[0], 
                    pItem[1],
                    pItem[2],
                    pItem[3],
                    pItem[4],
                    pItem[5],
                    pItem[6],
                    pItem[7],
                    pItem[8],
                    pItem[9],
                    pItem[10],
                    pItem[11],
                    pItem[12],
                    pItem[13],
                    pItem[14],
                    pItem[15]);
    */                        
    // after buffer copied, we can get information in item
    //return AttributeSize;
    pAttributeHeaderSwitched->NameOffset = L1_NETWORK_2BytesToUINT16(((NTFS_FILE_ATTRIBUTE_HEADER *)pItem)->NameOffset);
                                                   
    pAttributeHeaderSwitched->ResidentFlag = ((NTFS_FILE_ATTRIBUTE_HEADER *)pItem)->ResidentFlag;
    
    
    UINT16 NameOffset = pAttributeHeaderSwitched->NameOffset;
    
    pAttributeHeaderSwitched->NameSize = ((NTFS_FILE_ATTRIBUTE_HEADER *)pItem)->NameSize;
    UINT8 NameSize = pAttributeHeaderSwitched->NameSize;
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Type: %02X ResidentFlag: %d ", __LINE__, pAttributeHeaderSwitched->Type, pAttributeHeaderSwitched->ResidentFlag);
    
    //return AttributeSize;     

    switch (pAttributeHeaderSwitched->Type)
    {
        //A0 Attribute
        case MFT_ATTRIBUTE_DOLLAR_INDEX_ALLOCATION:
            {
                UINT8 DataRunSize = AttributeSize - NameOffset - NameSize * 2;
                L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: DataRunSize: %02X ", __LINE__, DataRunSize);
                
                int j = 0;

                //get data runs
                UINT8 DataRuns[20] = {0};
                for (int i = NameOffset + NameSize * 2; i < AttributeSize; i++)
                {           
                    DataRuns[j] = pItem[i] & 0xff;
                    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: %02X ", __LINE__, DataRuns[j] & 0xff);   
                    j++;
                }

                //���ڲ��Ի�ȡ��ֵ�Ƿ���ȷ
                /*L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: %02X %02X %02X %02X %02X %02X %02X %02X\n", __LINE__, 
                                                            pItem[pAttributeHeaderSwitched->NameOffset + pAttributeHeaderSwitched->NameSize * 2 + 0],
                                                            pItem[pAttributeHeaderSwitched->NameOffset + pAttributeHeaderSwitched->NameSize * 2 + 1],
                                                            pItem[pAttributeHeaderSwitched->NameOffset + pAttributeHeaderSwitched->NameSize * 2 + 2],
                                                            pItem[pAttributeHeaderSwitched->NameOffset + pAttributeHeaderSwitched->NameSize * 2 + 3],
                                                            pItem[pAttributeHeaderSwitched->NameOffset + pAttributeHeaderSwitched->NameSize * 2 + 4],
                                                            pItem[pAttributeHeaderSwitched->NameOffset + pAttributeHeaderSwitched->NameSize * 2 + 5],
                                                            pItem[pAttributeHeaderSwitched->NameOffset + pAttributeHeaderSwitched->NameSize * 2 + 6],
                                                            pItem[pAttributeHeaderSwitched->NameOffset + pAttributeHeaderSwitched->NameSize * 2 + 7]);
                */
                L2_FILE_NTFS_DollarRootA0DatarunAnalysis(DataRuns);
            }
            break;

        case MFT_ATTRIBUTE_DOLLAR_VOLUME_NAME:
            for(UINT8 i = 0; i * 2 < AttributeSize - pAttributeHeaderSwitched->NameOffset; i++)
            {
                pAttributeHeaderSwitched->Data[i] = pItem[pAttributeHeaderSwitched->NameOffset + i * 2];
            }
            break;
            
        case MFT_ATTRIBUTE_DOLLAR_DATA:
            for(UINT8 i = 0; i < AttributeSize - pAttributeHeaderSwitched->NameOffset; i++)
            {
                pAttributeHeaderSwitched->Data[i] = pItem[pAttributeHeaderSwitched->NameOffset + i];
            }
            pAttributeHeaderSwitched->DataSize = AttributeSize - pAttributeHeaderSwitched->NameOffset;
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: %02X %02X %02X %02X %02X %02X",
                                pItem[pAttributeHeaderSwitched->NameOffset + 0],
                                pItem[pAttributeHeaderSwitched->NameOffset + 1],
                                pItem[pAttributeHeaderSwitched->NameOffset + 2],
                                pItem[pAttributeHeaderSwitched->NameOffset + 3],
                                pItem[pAttributeHeaderSwitched->NameOffset + 4],
                                pItem[pAttributeHeaderSwitched->NameOffset + 5]);
            break;

        //90 ���Է�������Щ���ӣ������ЩBUG����Ҫ����ʵ�����������
        //90 ���԰�����NTFS_FILE_ATTRIBUTE_HEADER ResidentAttributeHeader INDEX_ROOT INDEX_ENTRY ��4���ṹ�塣
        case MFT_ATTRIBUTE_DOLLAR_INDEX_ROOT:
            {                
                if (AttributeSize <= 0x58)
                {
                    return AttributeSize;
                }
                //L2_PARTITION_BufferPrint(pItem, 0x120);
                L1_MEMORY_Memset((void *)pCommonStorageItems, 0, 10 * sizeof(COMMON_STORAGE_ITEM));
                for (UINT8 i = 0; i < 10 ; i++)
                {
                    for (UINT8 j = 0; j < 100; j++)
                    {
                        pCommonStorageItems[i].Name[j] = 0;
                    }
                    pCommonStorageItems[i].FileContentRelativeSector = 0;
                    pCommonStorageItems[i].ItemCount = 0;
                    pCommonStorageItems[i].Type = COMMON_STORAGE_ITEM_MAX;
                }

                //L2_PARTITION_BufferPrint(pItem, AttributeSize);
                
                if (pAttributeHeaderSwitched->ResidentFlag == 0) //��פ
                {
                    UINT16 AttributeHeaderSize = sizeof(NTFS_FILE_ATTRIBUTE_HEADER);
                    UINT16 Offset = AttributeHeaderSize;
                    unsigned long DataSize = L1_NETWORK_4BytesToUINT32(((ResidentAttributeHeader *)pItem[Offset])->ATTR_DataSize);
                                        
                    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Offset: %d ", __LINE__, Offset);
                        
                    UINT16 ResidentAttributeHeaderSize = sizeof(ResidentAttributeHeader);
                    Offset += ResidentAttributeHeaderSize;
                            
                    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Offset: %d ", __LINE__, Offset);
                        
                    unsigned long AttributeType = L1_NETWORK_4BytesToUINT32(((INDEX_ROOT *)pItem[Offset])->IR_AttributeType);

                    
                    UINT16 IndexRootSize = sizeof(INDEX_ROOT);
                    Offset += IndexRootSize;        
                    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Offset: %d ", __LINE__, Offset);
                        
                    
                    UINT8 j = 0;

                    // INDEX_ENTRY
                    INDEX_ENTRY;
                    while (TRUE)
                    {            
                        UINT64 IE_MftReferNumber = L1_NETWORK_8BytesToUINT64(pItem[Offset]);
                        UINT8 IE_FileNameSize = pItem[Offset + 0x50];
                        UINT8 IE_FileNamespace = pItem[Offset + 0x51];
                        UINT64 IE_FileFlag = L1_NETWORK_8BytesToUINT64(pItem[Offset + 0x48]);
                        UINT16 IE_Size = (UINT16)pItem[Offset + 0x8] | (UINT16)pItem[Offset + 0x9] << 8;
    
                        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: NameSize: %02X Namespace: %02X IE_FileFlag: %X IE_Size: %X ", __LINE__, IE_FileNameSize, IE_FileNamespace, IE_FileFlag, IE_Size);
                            
                        for(UINT8 i = 0; i < IE_FileNameSize; i++)
                        {
                            pAttributeHeaderSwitched->Data[i] = pItem[Offset + 0x52 + i * 2];
                            pCommonStorageItems[j].Name[i] = pItem[Offset + 0x52 + i * 2];
                        }
                        
                        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: %c%c%c%c%c%c%c%c%c ", __LINE__, 
                                                                                                                                pCommonStorageItems[j].Name[0], 
                                                                                                                                pCommonStorageItems[j].Name[1], 
                                                                                                                                pCommonStorageItems[j].Name[2], 
                                                                                                                                pCommonStorageItems[j].Name[3], 
                                                                                                                                pCommonStorageItems[j].Name[4], 
                                                                                                                                pCommonStorageItems[j].Name[5], 
                                                                                                                                pCommonStorageItems[j].Name[6], 
                                                                                                                                pCommonStorageItems[j].Name[7], 
                                                                                                                                pCommonStorageItems[j].Name[8]);
                        pCommonStorageItems[j].Type = COMMON_STORAGE_ITEM_FILE;
                        UINT64 RelativeSector = pItem[Offset] + (UINT64)pItem[Offset + 1] * 256 + (UINT64)pItem[Offset + 2] * 256 * 256 + (UINT64)pItem[Offset + 3] * 256 * 256 * 256 + (UINT64)pItem[Offset + 4] * 256 * 256 * 256 * 256 + (UINT64)pItem[Offset + 5] * 256 * 256 * 256 * 256 * 256;
                        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: RelativeSector: %LLX", __LINE__, 
                                                                                                                                RelativeSector);
                        pCommonStorageItems[j].FileContentRelativeSector = RelativeSector;
                        

                        //��ߺ����е�BUG��IE_FileFlag��ȡ��ֵ��̫��
                        switch (IE_FileFlag)
                        {
                            case 0x20:
                                pCommonStorageItems[j].Type = COMMON_STORAGE_ITEM_FILE;
                                break;
                                
                            case 0x10000000:
                                pCommonStorageItems[j].Type = COMMON_STORAGE_ITEM_FOLDER;
                                break;
                                
    
                            default:;
                        }
                        
                        Offset += IE_Size;
                        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: %X ", __LINE__, pItem[Offset]);
                        
                        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: %02X %02X %02X %02X ", __LINE__, 
                                                                                                                                pItem[Offset + 0], 
                                                                                                                                pItem[Offset + 1], 
                                                                                                                                pItem[Offset + 2], 
                                                                                                                                pItem[Offset + 3]);
                        j++;
                        if (0 == pItem[Offset + 0] && 0 == pItem[Offset + 1] && 0 == pItem[Offset + 2] && 0 == pItem[Offset + 3])
                        {
                            return 0;
                        }
                    }

                    break;
                }
                
                pAttributeHeaderSwitched->DataSize = 4;

            }
                    
            
        default: ;
    }         

    return AttributeSize;
}


EFI_STATUS  L2_FILE_NTFS_FileItemBufferAnalysis2(UINT8 *pBuffer, NTFS_FILE_SWITCHED *pNTFSFileSwitched)
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: ", __LINE__);
    UINT16 AttributeSize;
    
    for (UINT16 i = 0; i < DISK_BUFFER_SIZE * 2; i++)
        pBufferTemp[i] = pBuffer[i];
    
    // ͨ������ͷ�����һ������ƫ��
    // ��ȡ�ļ�ͷ��Ϣ
    UINT16 Offset = L2_FILE_NTFS_FileItemHeaderAnalysis(pBufferTemp, pNTFSFileSwitched);
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Offset: %d ", __LINE__, Offset);
    
    // ��ȡÿ��������Ϣ
    for (UINT16 i = 0; ; i++)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: For loop analysis attribute count: %d ", __LINE__, i + 1);
        AttributeSize = L2_FILE_NTFS_FileItemAttributeAnalysis2(pBuffer, Offset, &(pNTFSFileSwitched->NTFSFileAttributeHeaderSwitched[i]));
        if (AttributeSize == 0 || AttributeSize > 0x200 || Offset >= DISK_BUFFER_SIZE * 2)
        {
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: For loop end AttributeSize: %d ", __LINE__, AttributeSize);
            break;
        }
        
        Offset += AttributeSize;
    }
}

