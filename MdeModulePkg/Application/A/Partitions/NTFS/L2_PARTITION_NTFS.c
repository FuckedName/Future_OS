
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





#include <string.h>

#include <L2_PARTITION_NTFS.h>

#include <Devices/Store/L2_DEVICE_Store.h>

#include <Libraries/Network/L1_LIBRARY_Network.h>
#include <Graphics/L2_GRAPHICS.h>
#include <Global/Global.h>


extern UINT8 BufferMFT[DISK_BUFFER_SIZE * 2];

// Index of A0 attribute
IndexInformation A0Indexes[10] = {0};

//用于分析文件系统使用，避免每次都申请释放，浪费时间，所以建一个公用的buffer
UINT8 pBufferTemp[DISK_BUFFER_SIZE * 2];

UINT64 FileContentRelativeSector;

UINT8 pItem[DISK_BUFFER_SIZE * 2] = {0};

// NTFS Main File Table items analysis
// MFT_Item_ID: 0 $MFT
/*             1 $MFTMirr
                 2 $LogFile
                 3 $Volume
                 4 $AttrDef
               5 $ROOT
               etc 
*/



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


UINT16 Index = 0;


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
*  描述:   xxxxx
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
    // 读取根目录项的INDEX ITEM 磁盘缓存
    Status = L1_STORE_READ(PartitionID, (A0Indexes[k].Offset + lastOffset) * 8 , A0Indexes[k].OccupyCluster * 8, BufferBlock);
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
        return Status;
    }

	// 分析INDEX ITEM 缓存
    L2_FILE_NTFS_MFTIndexItemsAnalysis(BufferBlock, PartitionID);    
    
    lastOffset = A0Indexes[k].Offset;
    
    return EFI_SUCCESS;
}



//Print ROOT Path items.
/****************************************************************************
*
*  描述:   分析NTFS文件系统的索引项
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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

    //IndexEntryOffset:索引项的偏移 相对于当前位置
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d IndexEntryOffset: %llu IndexEntrySize: %llu\n", __LINE__, 
                                                                     L1_NETWORK_4BytesToUINT32(((NTFS_INDEX_HEADER *)p)->IndexEntryOffset),
                                                                     L1_NETWORK_4BytesToUINT32(((NTFS_INDEX_HEADER *)p)->IndexEntrySize));

    // 相对于当前位置 need to add size before this Byte.
    // 获取索引头里边的索引项偏移字段。
    //
    UINT8 IndexEntryOffset = L1_NETWORK_4BytesToUINT32(((NTFS_INDEX_HEADER *)p)->IndexEntryOffset) + 24;
	
    UINT8 pItem[200] = {0};
    UINT16 index = IndexEntryOffset;

	//L1_MEMORY_Memset(&pItems, 0, sizeof(pItems));
	UINT8 j = 0;

	pCommonStorageItems[0].ItemCount = 0;

	//把INDEX ITEM索引项分析出来，并写入公共的文件、文件夹项数据结构，以便后续在我的电脑显示
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

		 //前六个字节。。
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
EFI_STATUS L2_FILE_NTFS_FirstSelectorAnalysis(UINT8 *p, DollarBootSwitched *pNTFSBootSwitched)
{
    DOLLAR_BOOT *pDollarBoot;
    
    pDollarBoot = (DOLLAR_BOOT *)AllocateZeroPool(DISK_BUFFER_SIZE);
    L1_MEMORY_Copy(pDollarBoot, p, DISK_BUFFER_SIZE);
  
    L1_FILE_NTFS_DollerRootTransfer(pDollarBoot, pNTFSBootSwitched);

    FreePool(pDollarBoot);
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
	
    // 相对于当前位置的偏移，所以需要加上当前位置前面的24个字节。 need to add size before this Byte.
    UINT16 HeaderLength = L1_NETWORK_4BytesToUINT32(((NTFS_INDEX_HEADER *)p)->IndexEntryOffset) + 24;

	return HeaderLength;
}

EFI_STATUS  L2_FILE_NTFS_IndexItemAttributeAnalysis(UINT8 *pBuffer, UINT16 Offset)
{
	NTFS_INDEX_ITEM;

	if (Offset >= L1_NETWORK_4BytesToUINT32(((NTFS_INDEX_HEADER *)pBuffer)->IndexEntrySize))
            return -1;
	
	UINT8 pItem[200] = {0};

	// 索引项大小
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


//NTFS文件系统 索引项分析
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

	// 获取索引头相关参数
	UINT16 Offset = L2_FILE_NTFS_IndexItemHeaderAnalysis(p);

	// 获取各个索引项相关的参数
	for (UINT16 i = 0; i < 10; i++)
	{
		UINT16 AttributeSize = L2_FILE_NTFS_IndexItemAttributeAnalysis(p, Offset);
		
	    Offset += AttributeSize;
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
*  描述: 分析NTFS的磁盘FILE项（注意不是目录下的文件）里边的属性，每个FILE项一般有很多个属性，到少有4，5个
*
*  //比较重要的几个属性如0X30文件名属性，其中记录着该目录或者文件的文件名；
*  //0X80数据属性记录着文件中的数据；
*  //0X90索引根属性，存放着该目录下的子目录和子文件的索引项；当某个目录下的内容比较多，从而导致0X90属性无法完全存放时，
*  //0XA0属性会指向一个索引区域，这个索引区域包含了该目录下所有剩余内容的索引项。
*  //属性有常驻属性和非常驻属性之分，当一个属性的数据能够在1KB的文件记录中保存的时候，该属性为常驻属性；
*  //而当属性的数据无法在文件记录中存放，需要存放到MFT外的其他位置时，该属性为非常驻属性。常驻属性和非常驻属性的头部结构定义如下：
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX  
*
*
*****************************************************************************/
UINT16  L2_FILE_NTFS_FileItemAttributeAnalysis(UINT8 *p, UINT16 AttributeOffset, NTFS_FILE_ATTRIBUTE_HEADER_SWITCHED *pAttributeHeaderSwitched)
{
	//L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FUNCTION: %a", __LINE__, __FUNCTION__);
	
	UINT8 pItem[200] = {0};
    UINT8 size[4];
	UINT16 AttributeSize;
	UINT16 i;

    pAttributeHeaderSwitched->Type = p[AttributeOffset];
	if (0 == pAttributeHeaderSwitched->Type || MFT_ATTRIBUTE_DOLLAR_EA <= pAttributeHeaderSwitched->Type)
	{
		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: MFT_ATTRIBUTE_INVALID: %X ", __LINE__, __FUNCTION__, pAttributeHeaderSwitched->Type);
		return 0;
	}
    // 属性头和属性体总长度
    for (i = 0; i < 4; i++)
        size[i] = p[AttributeOffset + 4 + i];
        
    AttributeSize = L1_NETWORK_4BytesToUINT32(size);
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: AttributeSize: %02X AttributeType: %02X", __LINE__, AttributeSize, p[AttributeOffset]);
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
				UINT8 DataRunSize = AttributeSize - NameOffset - NameSize * 2;
				L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: DataRunSize: %02X ", __LINE__, DataRunSize);
				//每个字符占用两个字节
				for(UINT8 i = 0; i < DataRunSize; i++)
				{
					//Get data runs data
					pAttributeHeaderSwitched->Data[i] = pItem[pAttributeHeaderSwitched->NameOffset + pAttributeHeaderSwitched->NameSize * 2 + i];
				}

				//用于测试获取的值是否正确
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
				//L2_FILE_NTFS_A0AttributeAnalysis();
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

		//90 属性分析，有些复杂，里边有些BUG，需要根据实际情况适配下
		//90 属性包含：NTFS_FILE_ATTRIBUTE_HEADER ResidentAttributeHeader INDEX_ROOT INDEX_ENTRY 这4个结构体。
		case MFT_ATTRIBUTE_DOLLAR_INDEX_ROOT:
			{				
			
				L2_PARTITION_BufferPrint(pItem, 0x120);
				L1_MEMORY_Memset((void *)pCommonStorageItems, 0, 10 * sizeof(COMMON_STORAGE_ITEM));
						
				if (pAttributeHeaderSwitched->ResidentFlag == 0) //常驻
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
					UINT64 IE_MftReferNumber = L1_NETWORK_8BytesToUINT64(((INDEX_ENTRY *)pItem[Offset])->IE_MftReferNumber);
					while (IE_MftReferNumber != 0)
					{						
						UINT8 IE_FileNameSize = ((INDEX_ENTRY *)pItem[Offset])->IE_FileNameSize;
						UINT8 IE_FileNamespace = ((INDEX_ENTRY *)pItem[Offset])->IE_FileNamespace;
						UINT64 IE_FileFlag = L1_NETWORK_8BytesToUINT64(((INDEX_ENTRY *)pItem[Offset])->IE_FileFlag);
						UINT16 IE_Size = L1_NETWORK_2BytesToUINT16(((INDEX_ENTRY *)pItem[Offset])->IE_Size);
	
						L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: NameSize: %d Namespace: %d IE_FileFlag: %X IE_MftReferNumber: %X ", __LINE__, IE_FileNameSize, IE_FileNamespace, IE_FileFlag, IE_MftReferNumber);
							
						for(UINT8 i = 0; i < IE_FileNameSize; i++)
						{
							pAttributeHeaderSwitched->Data[i] = pItem[Offset + 0x52 + i * 2];
							pCommonStorageItems[j].Name[i] = pItem[Offset + 0x52 + i * 2];
						}
						
						pCommonStorageItems[j].Type = COMMON_STORAGE_ITEM_FILE;
						pCommonStorageItems[j].FileContentRelativeSector = IE_MftReferNumber;
	
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
						IE_MftReferNumber = L1_NETWORK_8BytesToUINT64(((INDEX_ENTRY *)pItem[Offset])->IE_MftReferNumber);
						j++;
						if (j > 1)
						{
							break;
						}
					}

					break;
				}
				
				pAttributeHeaderSwitched->DataSize = 4;

			}
					
			
		default:;
	}		 

	return AttributeSize;
}


//File record in NTFS file system means data on disk, not file in folder.. 
//L2_FILE_NTFS_MFTDollarRootFileAnalysis
/****************************************************************************
*
*  描述: 分析NTFS的磁盘FILE项（注意不是目录下的文件），每个FILE项一般占用两个扇区
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS  L2_FILE_NTFS_FileItemBufferAnalysis(UINT8 *pBuffer, NTFS_FILE_SWITCHED *pNTFSFileSwitched)
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: ", __LINE__);
	UINT16 AttributeSize;
	
	for (UINT16 i = 0; i < DISK_BUFFER_SIZE * 2; i++)
		pBufferTemp[i] = pBuffer[i];
	
	// 通过属性头计算第一个属性偏移
	// 获取文件头信息
	UINT16 Offset = L2_FILE_NTFS_FileItemHeaderAnalysis(pBufferTemp, pNTFSFileSwitched);
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Offset: %d", __LINE__, Offset);
		
	// 获取每个属性信息
	// 当前只计算10个属性，其实是有缺陷的
	for (UINT16 i = 0; ; i++)
	{
		AttributeSize = L2_FILE_NTFS_FileItemAttributeAnalysis(pBuffer, Offset, &(pNTFSFileSwitched->NTFSFileAttributeHeaderSwitched[i]));
		if (AttributeSize == 0 || AttributeSize > 0x200 || Offset >= DISK_BUFFER_SIZE * 2)
			break;
		
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
*  描述: 分析NTFS的磁盘FILE项（注意不是目录下的文件）里边的属性，每个FILE项一般有很多个属性，到少有4，5个
*
*  //比较重要的几个属性如0X30文件名属性，其中记录着该目录或者文件的文件名；
*  //0X80数据属性记录着文件中的数据；
*  //0X90索引根属性，存放着该目录下的子目录和子文件的索引项；当某个目录下的内容比较多，从而导致0X90属性无法完全存放时，
*  //0XA0属性会指向一个索引区域，这个索引区域包含了该目录下所有剩余内容的索引项。
*  //属性有常驻属性和非常驻属性之分，当一个属性的数据能够在1KB的文件记录中保存的时候，该属性为常驻属性；
*  //而当属性的数据无法在文件记录中存放，需要存放到MFT外的其他位置时，该属性为非常驻属性。常驻属性和非常驻属性的头部结构定义如下：
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX  
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
	
    // 属性头和属性体总长度
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
				//每个字符占用两个字节
				for(UINT8 i = 0; i < DataRunSize; i++)
				{
					//Get data runs data
					pAttributeHeaderSwitched->Data[i] = pItem[pAttributeHeaderSwitched->NameOffset + pAttributeHeaderSwitched->NameSize * 2 + i];
				}

				//用于测试获取的值是否正确
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
				//L2_FILE_NTFS_A0AttributeAnalysis();
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

		//90 属性分析，有些复杂，里边有些BUG，需要根据实际情况适配下
		//90 属性包含：NTFS_FILE_ATTRIBUTE_HEADER ResidentAttributeHeader INDEX_ROOT INDEX_ENTRY 这4个结构体。
		case MFT_ATTRIBUTE_DOLLAR_INDEX_ROOT:
			{				
			
				//return AttributeSize;
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

				L2_PARTITION_BufferPrint(pItem, AttributeSize);
				
				if (pAttributeHeaderSwitched->ResidentFlag == 0) //常驻
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
						
					
					// 取前六位
					UINT64 IE_MftReferNumber = L1_NETWORK_6BytesToUINT64(((INDEX_ENTRY *)pItem[Offset])->IE_MftReferNumber);
					L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: IE_MftReferNumber: %X ", __LINE__, IE_MftReferNumber);
					
					L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: %02X %02X %02X %02X %02X %02X %02X %02X %02X ", __LINE__, 
																															pItem[Offset + 0], 
																															pItem[Offset + 1], 
																															pItem[Offset + 2], 
																															pItem[Offset + 3], 
																															pItem[Offset + 4], 
																															pItem[Offset + 5], 
																															pItem[Offset + 6], 
																															pItem[Offset + 7], 
																															pItem[Offset + 8]);
					
					UINT8 j = 0;
					
					while (IE_MftReferNumber != 0xffffff)
					{						
						if (0xff == pItem[Offset] && 0xff == pItem[Offset + 1] && 0xff == pItem[Offset + 2] && 0xff == pItem[Offset + 3])
						{
							L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: end item, j: %d", j);
							return AttributeSize;
						}
						UINT8 IE_FileNameSize = pItem[Offset + 0x50];
						UINT8 IE_FileNamespace = pItem[Offset + 0x51];
						UINT64 IE_FileFlag = L1_NETWORK_8BytesToUINT64(pItem[Offset + 0x48]);
						UINT16 IE_Size = pItem[Offset + 0x8] + pItem[Offset + 0x9] * 16 * 16;
	
						L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: NameSize: %02X Namespace: %02X IE_FileFlag: %X IE_Size: %X ", __LINE__, IE_FileNameSize, IE_FileNamespace, IE_FileFlag, IE_Size);
							
						for(UINT8 i = 0; i < IE_FileNameSize; i++)
						{
							pAttributeHeaderSwitched->Data[i] = pItem[Offset + 0x52 + i * 2];
							pCommonStorageItems[j].Name[i] = pItem[Offset + 0x52 + i * 2];
						}
						
						pCommonStorageItems[j].Type = COMMON_STORAGE_ITEM_FILE;
						pCommonStorageItems[j].FileContentRelativeSector = IE_MftReferNumber;
	
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
						IE_MftReferNumber = L1_NETWORK_8BytesToUINT64(((INDEX_ENTRY *)pItem[Offset])->IE_MftReferNumber);
						j++;
						if (j > 3)
						{
							break;
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
	
	// 通过属性头计算第一个属性偏移
	// 获取文件头信息
	UINT16 Offset = L2_FILE_NTFS_FileItemHeaderAnalysis2(pBufferTemp, pNTFSFileSwitched);
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Offset: %d ", __LINE__, Offset);
	
	// 获取每个属性信息
	// 当前只计算10个属性，其实是有缺陷的
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

