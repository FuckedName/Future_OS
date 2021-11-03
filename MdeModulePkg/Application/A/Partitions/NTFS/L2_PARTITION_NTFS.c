
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





#include <string.h>

#include <L2_PARTITION_NTFS.h>

#include <Devices/Store/L2_DEVICE_Store.h>

#include <Libraries/Network/L1_LIBRARY_Network.h>
#include <Graphics/L2_GRAPHICS.h>
#include <Global/Global.h>


extern UINT8 BufferMFT[DISK_BUFFER_SIZE * 2];

// Index of A0 attribute
IndexInformation A0Indexes[10] = {0};

UINT64 FileContentRelativeSector;

// NTFS Main File Table items analysis
// MFT_Item_ID: 0 $MFT
/*             1 $MFTMirr
                 2 $LogFile
                 3 $Volume
                 4 $AttrDef
               5 $ROOT
               etc 
*/
EFI_STATUS L2_FILE_NTFS_MFT_Item_Read(UINT16 DeviceID, UINT64 SectorStartNumber)
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d DeviceID: %d\n", __LINE__, DeviceID);
    //printf( "RootPathAnalysis\n" );
    //DEBUG ((EFI_D_INFO, "PartitionUSBRead!!\r\n"));
    EFI_STATUS Status ;
    EFI_HANDLE *ControllerHandle = NULL;

    //sector_count is MFT start sector, 5 * 2 means $ROOT sector...
    //Every MFT Item use 2 sector .
    Status = L1_STORE_READ(DeviceID, SectorStartNumber, 2, BufferMFT); 
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
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: sector_count:%ld FileLength: %d MBRSwitched.ReservedSelector:%ld\n",  __LINE__, sector_count, FileLength, MBRSwitched.ReservedSelector);
      
    return EFI_SUCCESS;
}


// Find $Root file from all MFT(may be 15 file,)
// pBuffer store all MFT
//比较重要的几个属性如0X30文件名属性，其中记录着该目录或者文件的文件名；
//0X80数据属性记录着文件中的数据；0X90索引根属性，存放着该目录下的子目录和子文件的索引项；
//当某个目录下的内容比较多，从而导致0X90属性无法完全存放时，0XA0属性会指向一个索引区域，这个索引区域包含了该目录下所有剩余内容的索引项。
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

    // Root file buffer copy
//  memcpy(p, pBuffer[512 * 2 * 5], DISK_BUFFER_SIZE * 2);

    for (int i = 0; i < DISK_BUFFER_SIZE * 2; i++)
        p[i] = pBuffer[i];

    //for (int i = 0; i < 7; i++)
    //{
        //for (int j = 0; j < 512; j++)
        //{
           //%02X: 8 * 3, 
          //L2_DEBUG_Print1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", p[j] & 0xff);
        //}
    //}
    
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
            UINT8 DataRuns[20] = {0};
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: A0 attribute has been found: ", __LINE__);
            for (int i = NameOffset; i < NameOffset + NameSize * 2; i++)
                L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: %02X ", __LINE__, pItem[i] & 0xff);

            int j = 0;

            //get data runs
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
    Status = L1_STORE_READ(PartitionID, (A0Indexes[k].Offset + lastOffset) * 8 , A0Indexes[k].OccupyCluster * 8, BufferBlock);
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
        return Status;
    }

    L2_FILE_NTFS_MFTIndexItemsAnalysis(BufferBlock, PartitionID);    
    
    lastOffset = A0Indexes[k].Offset;
    
    return EFI_SUCCESS;
}

//Print ROOT Path items.
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

    //IndexEntryOffset:索引项的偏移 相对于当前位置
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d IndexEntryOffset: %llu IndexEntrySize: %llu\n", __LINE__, 
                                                                     L1_NETWORK_4BytesToUINT32(((NTFS_INDEX_HEADER *)p)->IndexEntryOffset),
                                                                     L1_NETWORK_4BytesToUINT32(((NTFS_INDEX_HEADER *)p)->IndexEntrySize));

    // 相对于当前位置 need to add size before this Byte.
    UINT8 length = L1_NETWORK_4BytesToUINT32(((NTFS_INDEX_HEADER *)p)->IndexEntryOffset) + 24;
    UINT8 pItem[200] = {0};
    UINT16 index = length;

	//
    for (UINT8 i = 0; ; i++)
    {    
         if (index >= L1_NETWORK_4BytesToUINT32(((NTFS_INDEX_HEADER *)p)->IndexEntrySize))
            break;
            
         //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: index: %d\n", __LINE__,  index);  
         // length use 2b at 8, 9 bit
         UINT16 length2 = pBuffer[index + 8] + pBuffer[index + 9] * 16;

		 // copy item into pItem buffer
        for (int i = 0; i < length2; i++)
            pItem[i] = pBuffer[index + i];
            
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
         //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%s attributeName: %a\n", __LINE__,  attributeName);  
         index += length2;
    }
}


// Analysis attribut A0 of $Root
EFI_STATUS  L2_FILE_NTFS_DollarRootA0DatarunAnalysis(UINT8 *p)
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: string length: %d\n", __LINE__,  L1_STRING_Length(p));

    UINT16 length = L1_STRING_Length(p);
    UINT8 occupyCluster = 0;
    UINT16 offset = 0;

    UINT16 Index = 0;
	UINT16 i = 0;
    while(i < length)
    {
    	// for exampleData runs:11 01 24
        UINT8  offsetLength  = p[i] >> 4;
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


EFI_STATUS L2_FILE_NTFS_FirstSelectorAnalysis(UINT8 *p, DollarBootSwitched *pNTFSBootSwitched)
{
    DOLLAR_BOOT *pDollarBoot;
    
    pDollarBoot = (DOLLAR_BOOT *)AllocateZeroPool(DISK_BUFFER_SIZE);
    L1_MEMORY_Copy(pDollarBoot, p, DISK_BUFFER_SIZE);
  
    L1_FILE_NTFS_DollerRootTransfer(pDollarBoot, pNTFSBootSwitched);

    FreePool(pDollarBoot);
}

VOID L1_FILE_NTFS_DollerRootTransfer(DOLLAR_BOOT *pSource, DollarBootSwitched *pDest)
{
    pDest->BitsOfSector = L1_NETWORK_2BytesToUINT16(pSource->BitsOfSector);
    pDest->SectorOfCluster = (UINT16)pSource->SectorOfCluster;
    pDest->AllSectorCount = L1_NETWORK_8BytesToUINT64(pSource->AllSectorCount) + 1;
    pDest->MFT_StartCluster = L1_NETWORK_8BytesToUINT64(pSource->MFT_StartCluster);
    pDest->MFT_MirrStartCluster = L1_NETWORK_8BytesToUINT64(pSource->MFT_MirrStartCluster);
    
}

