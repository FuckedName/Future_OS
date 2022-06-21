
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




#include <L3_PARTITION.h>

#include <Graphics/L2_GRAPHICS.h>
#include <Devices/Store/L2_DEVICE_Store.h>
#include <Libraries/String/L1_LIBRARY_String.h>
#include <Global/Global.h>

int READ_FILE_FSM_Event = READ_PATITION_INFO_EVENT;

//整个系统硬盘、U盘等等外存的分区总数，注：一个硬盘或一个U盘可以分成多个分区
UINTN PartitionCount = 0;

READ_FILE_STATE   NextState = READ_FILE_INIT_STATE;

UINT64 FileReadCount = 0;

UINT64 FileLength = 0;

UINT64 PreviousBlockNumber = 0;

UINT8 ReadFileNameLength = 0;

UINT8 ReadFileName[20];

//记录下次读取第多少号扇区
UINT64 sector_count = 0;
UINT32 ReadFilePartitionID = 0;

//记录文件所在目录最大目录深度
#define FILE_PATH_COUNT 10

//记录文件所在目录最大目录长度
#define FILE_PATH_LENGTH 20

//记录文件名长度
#define FILE_NAME_LENGTH 50

// all partitions analysis

BOOLEAN TestFlag = FALSE;


typedef struct
{
	//文件路径和文件名信息
	//因为单独从文件名来看，不能确认是否是路径还是文件名
	UINT8 FilePaths[FILE_PATH_COUNT][FILE_PATH_LENGTH]; 
	UINT8 *pDestBuffer; //文件读取后存放的缓冲区
	UINT16 CurrentPathID; //当前操作的路径编号
	UINT16 PathCount;
    UINT16 CurrentPartitionID; //当前操作分区编号 
	UINT8 *pItemBuffer; //存放文件所在目录，目录对应的磁盘缓冲区
	UINT8 *pItemID; //存放文件所在目录的编号

}FILE_READ_DATA;



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
EFI_STATUS L2_STORE_PartitionAnalysisFSM()
{    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d PartitionAnalysisFSM\n", __LINE__);
    EFI_STATUS Status ;
    UINT8 Buffer1[DISK_BUFFER_SIZE];
    UINT16 i = ReadFilePartitionID;
    
    Status = L2_STORE_Read(i, sector_count, 1, Buffer1); 
    if (EFI_SUCCESS == Status)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
          
        // analysis data area of patition
        //L1_FILE_FAT32_DataSectorAnalysis(Buffer1, &MBRSwitched); 
        
        // data sector number start include: reserved selector, fat sectors(usually is 2: fat1 and fat2), and file system boot path start cluster(usually is 2, data block start number is 2)
        sector_count = device[i].stMBRSwitched.ReservedSelector + device[i].stMBRSwitched.SectorsPerFat * device[i].stMBRSwitched.FATCount + (device[i].stMBRSwitched.BootPathStartCluster - 2) * 8;
        BlockSize = device[i].stMBRSwitched.SectorOfCluster * 512;
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: sector_count:%ld BlockSize: %d\n",  __LINE__, sector_count, BlockSize);
     }           

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
void L1_FILE_NameGet(UINT8 ItemID, UINT8 *FileName)
 {    
    UINT8 count = 0;
    while (pItems[ItemID].FileName[count] != 0)
    {
    	if (count >= 8)
		{
			break;
    	}
        FileName[count] = pItems[ItemID].FileName[count];
        count++;
    }
	
    if (pItems[ItemID].ExtensionName[0] != 0)
    {
        FileName[count] = ' ';
        count++;
    }

	UINT8 count2 = 0;
    while (pItems[ItemID].ExtensionName[count2] != 0)
    {
    	if (count2 >= 3)
		{
			break;
    	}
    
        FileName[count] = pItems[ItemID].ExtensionName[count2];
        count++;
        count2++;
    }
	
	FileName[12] = 0;
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
void L1_FILE_NameMerge(UINT8 ItemID, UINT8 *FileName)
 {    
    UINT8 count = 0;
    while (pItems[ItemID].FileName[count] != 0)
    {
    	if (count >= 8 || pItems[ItemID].FileName[count] == 0x20)
		{
			break;
    	}
        FileName[count] = pItems[ItemID].FileName[count];
        count++;
    }
	
    if (pItems[ItemID].ExtensionName[0] != 0)
    {
        FileName[count] = '.';
        count++;
    }

	UINT8 count2 = 0;
    while (pItems[ItemID].ExtensionName[count2] != 0)
    {
    	if (count2 >= 3 || pItems[ItemID].ExtensionName[count2] == 0x20)
		{
			break;
    	}
    
        FileName[count] = pItems[ItemID].ExtensionName[count2];
        count++;
        count2++;
    }

    count = (count >= 11) ? 11 : count;
	
	FileName[count] = 0;
 }


//delete blanks of file name and file extension name


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
void L1_FILE_NameGetUseItem(FAT32_ROOTPATH_SHORT_FILE_ITEM pItem, UINT8 *FileName)
 {
 	if (NULL == FileName)
 	{
 		return;
 	}
 
    UINT16 i = 0;
    while (TRUE)
    {
    	if (i >= 8 || pItem.FileName[i] == 0x20)
		{
			break;
    	}
        FileName[i] = pItem.FileName[i];
        i++;
    }
				
	//表示没有后缀名
	if (pItem.ExtensionName[0] == 0x20)
	{
		FileName[i] = '\0';
		return;
	}

	//有后缀名，则需要加点号
    FileName[i] = '.';
    i++;

	UINT8 count2 = 0;
    while (pItem.ExtensionName[count2] != 0x20)
    {
    	if (count2 >= 3)
		{
			break;
    	}
    
        FileName[i] = pItem.ExtensionName[count2];
        i++;
        count2++;
    }

    i = (i >= 12) ? 12 : i;
	
	FileName[i] = '\0';
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
void L1_FILE_NameGet2(UINT8 deviceID, UINT8 *FileName)
{    
    UINT8 s[12] = {0};
    for (UINT8 i = 0; i < 11; i++)
    {       
        s[i] = pItems[deviceID].FileName[i];
    }
    UINT8 j = 0;
    for (UINT8 i = 0; i < 11; )
    {
        if (s[i] != 32)
            FileName[j++] = s[i++];
        else
            i++;
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
EFI_STATUS L1_FILE_RootPathAnalysis(UINT8 *p)
{
    L1_MEMORY_Copy(&pItems, p, DISK_BUFFER_SIZE);
    UINT16 valid_count = 0;
    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: .: %d\n", __LINE__, ReadFileName[7]);

    //display filing file and subpath. 
    for (int i = 0; i < 30; i++)
        if (pItems[i].FileName[0] != 0xE5 && (pItems[i].Attribute[0] == 0x20 
            || pItems[i].Attribute[0] == 0x10))
       {
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d FileName:%2c%2c%2c%2c%2c%2c%2c%2c ExtensionName:%2c%2c%2c StartCluster:%02X%02X%02X%02X FileLength: %02X%02X%02X%02X Attribute: %02X    ", __LINE__,
                                            pItems[i].FileName[0], pItems[i].FileName[1], pItems[i].FileName[2], pItems[i].FileName[3], pItems[i].FileName[4], pItems[i].FileName[5], pItems[i].FileName[6], pItems[i].FileName[7],
                                            pItems[i].ExtensionName[0], pItems[i].ExtensionName[1],pItems[i].ExtensionName[2],
                                            pItems[i].StartClusterHigh2B[0], pItems[i].StartClusterHigh2B[1],
                                            pItems[i].StartClusterLow2B[0], pItems[i].StartClusterLow2B[1],
                                            pItems[i].FileLength[0], pItems[i].FileLength[1], pItems[i].FileLength[2], pItems[i].FileLength[3],
                                            pItems[i].Attribute[0]);
            
            valid_count++;
            
            UINT8 FileName[13] = {0};
            UINT8 FileName2[13] = {0};
            L1_FILE_NameGet(i, FileName);
            L1_FILE_NameGet2(i, FileName2);
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FileName: %a\n", __LINE__, FileName);
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FileName2: %a\n", __LINE__, FileName2);
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: ReadFileName: %a\n", __LINE__, ReadFileName);

            //这里写的不太好，应该只需要两个参数就行了，不应该有Length这个参数
            if (L1_STRING_Compare(FileName2, ReadFileName, ReadFileNameLength) == EFI_SUCCESS)
            {
                L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d FileName:%2c%2c%2c%2c%2c%2c%2c%2c ExtensionName:%2c%2c%2c StartCluster:%02X%02X%02X%02X FileLength: %02X%02X%02X%02X Attribute: %02X    ",  __LINE__,
                                pItems[i].FileName[0], pItems[i].FileName[1], pItems[i].FileName[2], pItems[i].FileName[3], pItems[i].FileName[4], pItems[i].FileName[5], pItems[i].FileName[6], pItems[i].FileName[7],
                                pItems[i].ExtensionName[0], pItems[i].ExtensionName[1],pItems[i].ExtensionName[2],
                                pItems[i].StartClusterHigh2B[0], pItems[i].StartClusterHigh2B[1],
                                pItems[i].StartClusterLow2B[0], pItems[i].StartClusterLow2B[1],
                                pItems[i].FileLength[0], pItems[i].FileLength[1], pItems[i].FileLength[2], pItems[i].FileLength[3],
                                pItems[i].Attribute[0]);
                
                FileBlockStart = (UINT64)pItems[i].StartClusterLow2B[0] | (UINT64)pItems[i].StartClusterLow2B[1] << 8 | (UINT64)pItems[i].StartClusterHigh2B[0] << 16 | (UINT64)pItems[i].StartClusterHigh2B[1] << 24;
                FileLength = (UINT64)pItems[i].FileLength[0] | (UINT64)pItems[i].FileLength[1] << 8 | (UINT64)pItems[i].FileLength[2] << 16 | (UINT64)pItems[i].FileLength[3] << 24;

                L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FileBlockStart: %d FileLength: %ld\n", __LINE__, FileBlockStart, FileLength);
            }
        }

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
EFI_STATUS L2_STORE_RootPathAnalysisFSM()
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d RootPathAnalysisFSM\n", __LINE__);
    EFI_STATUS Status ;
    UINT8 Buffer1[DISK_BUFFER_SIZE];
    UINT16 i = ReadFilePartitionID;
    
    Status = L2_STORE_Read(i, sector_count, 1, Buffer1); 
    if ( EFI_SUCCESS == Status )
    {
         //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X\n", __LINE__, Status);
          
          //When get root path data sector start number, we can get content of root path.
            L1_FILE_RootPathAnalysis(Buffer1);  

            // data area start from 1824, HZK16 file start from     FileBlockStart  block, so need to convert into sector by multi 8, block start number is 2   
            // next state is to read FAT table
            sector_count = device[i].stMBRSwitched.ReservedSelector;
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: sector_count:%ld FileLength: %d MBRSwitched.ReservedSelector:%ld\n",  __LINE__, sector_count, FileLength, device[i].stMBRSwitched.ReservedSelector);
     }    

    return EFI_SUCCESS;
}



/****************************************************************************
*
*  描述: 从分区读取FAT表内容，这里需要注意，如果FAT表信息有变化，则需要重新读取
         FAT个一般有两个，我们只读一个，所以我们在文件操作新建，修改的时候，若涉及FAT表写入的时候，也需要注意写入这个FAT
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L2_STORE_FatTableRead(DEVICE_PARAMETER *pDevice, FILE_READ_DATA FileReadData)
{	
	if (NULL == pDevice->pFAT_TableBuffer)
	{
		pDevice->pFAT_TableBuffer = L2_MEMORY_Allocate("FAT Table Buffer", MEMORY_TYPE_APPLICATION, DISK_BUFFER_SIZE * pDevice->stMBRSwitched.SectorsPerFat);
		EFI_STATUS Status = L2_STORE_Read(FileReadData.CurrentPartitionID, pDevice->stMBRSwitched.ReservedSelector,  pDevice->stMBRSwitched.SectorsPerFat, pDevice->pFAT_TableBuffer);	
	} 
	
	//L2_PARTITION_BufferPrint(pDevice->pFAT_TableBuffer, 512);	
	//L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: pDevice->pFAT_TableBuffer: %x Status: %d\n", __LINE__, pDevice->pFAT_TableBuffer, Status); 
}



/****************************************************************************
*
*  描述:     把FAT表指定BLOCK数值设置为0
*
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L2_STORE_FatTableSetZero(DEVICE_PARAMETER *pDevice, FILE_READ_DATA FileReadData, UINT64 BlockNumber)
{	
	if (NULL == pDevice->pFAT_TableBuffer)
	{
		for (UINT16 i = 0; i < 4; i++)
			pDevice->pFAT_TableBuffer[4 * BlockNumber + i] = 0;
		//pDevice->pFAT_TableBuffer = L2_MEMORY_Allocate("FAT Table Buffer", MEMORY_TYPE_APPLICATION, DISK_BUFFER_SIZE * pDevice->stMBRSwitched.SectorsPerFat);
		EFI_STATUS Status = L2_STORE_Write(FileReadData.CurrentPartitionID, pDevice->stMBRSwitched.ReservedSelector,  pDevice->stMBRSwitched.SectorsPerFat, pDevice->pFAT_TableBuffer);	
	} 
	
	//L2_PARTITION_BufferPrint(pDevice->pFAT_TableBuffer, 512);	
	//L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: pDevice->pFAT_TableBuffer: %x Status: %d\n", __LINE__, pDevice->pFAT_TableBuffer, Status); 
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
EFI_STATUS L2_STORE_GetFatTableFSM()
{    
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d GetFatTableFSM\n", __LINE__);
    EFI_STATUS Status ;
    UINT16 i = ReadFilePartitionID;
    
    if (NULL != device[i].pFAT_TableBuffer)
    {
        // start sector of file
        sector_count = device[i].stMBRSwitched.ReservedSelector + device[i].stMBRSwitched.SectorsPerFat * device[i].stMBRSwitched.FATCount + device[i].stMBRSwitched.BootPathStartCluster - 2 + (FileBlockStart - 2) * 8;
        
        // for FAT32_Table get next block number
        PreviousBlockNumber = FileBlockStart;

        return EFI_SUCCESS;
    }
        
     //start block need to recompute depends on file block start number 
     //FileBlockStart;
     L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d MBRSwitched.SectorsPerFat: %d\n", __LINE__, device[i].stMBRSwitched.SectorsPerFat);
    
     // 512 = 16 * 32 = 4 item * 32
    //FAT32_Table = (UINT8 *)AllocateZeroPool(DISK_BUFFER_SIZE * device[i].stMBRSwitched.SectorsPerFat);
    device[i].pFAT_TableBuffer = L2_MEMORY_Allocate("FAT Table Buffer", MEMORY_TYPE_APPLICATION, DISK_BUFFER_SIZE * device[i].stMBRSwitched.SectorsPerFat);
     if (NULL == device[i].pFAT_TableBuffer)
     {
         L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: NULL == FAT32_Table\n", __LINE__);                             
     }             
     
    Status = L2_STORE_Read(i, sector_count,  device[i].stMBRSwitched.SectorsPerFat, device[i].pFAT_TableBuffer); 
    if ( EFI_SUCCESS == Status )
    {
          //CopyMem(FAT32_Table, Buffer1, DISK_BUFFER_SIZE * MBRSwitched.SectorsPerFat);
          for (int j = 0; j < 250; j++)
          {
                //L2_DEBUG_Print1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", Buffer1[j] & 0xff);
          }
     }           
    
    // start sector of file
    sector_count = device[i].stMBRSwitched.ReservedSelector + device[i].stMBRSwitched.SectorsPerFat * device[i].stMBRSwitched.FATCount + device[i].stMBRSwitched.BootPathStartCluster - 2 + (FileBlockStart - 2) * 8;
    
    // for FAT32_Table get next block number
    PreviousBlockNumber = FileBlockStart;
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: sector_count:%ld FileLength: %d PreviousBlockNumber: %d\n",  __LINE__, sector_count, FileLength, PreviousBlockNumber);

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
UINT32 L2_FILE_GetNextBlockNumber()
{
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: PreviousBlockNumber: %d\n",  __LINE__, PreviousBlockNumber);

    if (PreviousBlockNumber == 0)
    {
        return 0x0fffffff;
    }
    UINT16 i = ReadFilePartitionID;
    
    if (device[i].pFAT_TableBuffer[PreviousBlockNumber * 4] == 0xff 
        && device[i].pFAT_TableBuffer[PreviousBlockNumber * 4 + 1] == 0xff 
        && device[i].pFAT_TableBuffer[PreviousBlockNumber * 4 + 2] == 0xff 
        && device[i].pFAT_TableBuffer[PreviousBlockNumber * 4 + 3] == 0x0f)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: PreviousBlockNumber: %d, PreviousBlockNumber: %llX\n",  __LINE__, PreviousBlockNumber, 0x0fffffff);
        return 0x0fffffff;
    }
    
    return device[i].pFAT_TableBuffer[PreviousBlockNumber  * 4] | (UINT32)device[i].pFAT_TableBuffer[PreviousBlockNumber * 4 + 1] << 8 | (UINT32)device[i].pFAT_TableBuffer[PreviousBlockNumber * 4 + 2] << 16 | (UINT32)device[i].pFAT_TableBuffer[PreviousBlockNumber * 4 + 3] << 24;  
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
EFI_STATUS L2_STORE_ReadFileFSM()
{    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d ReadFileFSM: PartitionCount: %d FileLength: %d sector_count: %llu\n", __LINE__, PartitionCount, FileLength, sector_count);
    ////DEBUG ((EFI_D_INFO, "PartitionUSBRead!!\r\n"));
    EFI_STATUS Status ;
    UINT8 BufferBlock[DISK_BLOCK_BUFFER_SIZE];
    UINT16 i = ReadFilePartitionID;
    
    // read from USB by block(512 * 8)
    // Read file content from FAT32(USB), minimum unit is block

    UINT8 AddOneFlag = (FileLength % (512 * 8)) == 0 ? 0 : 1;

    //把整个文件从外存读取到内存
    for (UINT16 k = 0; k < FileLength / (512 * 8) + AddOneFlag; k++)
    {
        Status = L2_STORE_Read(i, sector_count, 8, BufferBlock); 
        if (EFI_ERROR(Status))
        {
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
            return Status;
        }           

        /*
        BufferBlock[0] = 0xff;
        BufferBlock[1] = 0xff;

        Status = L2_STORE_Write(i, sector_count, 8, BufferBlock); 
        if (EFI_ERROR(Status))
        {
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
            return Status;
        }     
        */           
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: HZK16FileReadCount: %d DISK_BLOCK_BUFFER_SIZE: %d\n", __LINE__, HZK16FileReadCount, DISK_BLOCK_BUFFER_SIZE);

          //Copy buffer to ChineseBuffer
          if (pReadFileDestBuffer != NULL)
          {
                for (UINT16 j = 0; j < DISK_BLOCK_BUFFER_SIZE; j++)
                    pReadFileDestBuffer[FileReadCount * DISK_BLOCK_BUFFER_SIZE + j] = BufferBlock[j];
         }
          
          for (int j = 0; j < 250; j++)
          {
                //L2_DEBUG_Print1(DISK_READ_BUFFER_X + (j % 32) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 32), "%02X ", BufferBlock[j] & 0xff);
          }
          
          FileReadCount++;
          UINT32 NextBlockNumber = L2_FILE_GetNextBlockNumber();
          sector_count = device[i].stMBRSwitched.ReservedSelector + device[i].stMBRSwitched.SectorsPerFat * device[i].stMBRSwitched.FATCount + device[i].stMBRSwitched.BootPathStartCluster - 2 + (NextBlockNumber - 2) * 8;
          PreviousBlockNumber = NextBlockNumber;
          //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: NextBlockNumber: %llu\n",  __LINE__, NextBlockNumber);
     }
     

    return EFI_SUCCESS;
}





//可优化，同一个分区，第一次读入后，不需要再初始化一遍
//这里需要注意，FAT表格是变化的，因为如果涉及文件写入，删除，新建文件，这些场景FAT表会变化
STATE_TRANSFORM FileReadTransitionTable[] =
{
    { READ_FILE_INIT_STATE,                READ_PATITION_INFO_EVENT,   READ_FILE_GET_PARTITION_INFO_STATE, L2_STORE_PartitionAnalysisFSM},
    { READ_FILE_GET_PARTITION_INFO_STATE,  READ_ROOT_PATH_EVENT,  READ_FILE_GET_ROOT_PATH_INFO_STATE, L2_STORE_RootPathAnalysisFSM},
    { READ_FILE_GET_ROOT_PATH_INFO_STATE,  READ_FAT_TABLE_EVENT,  READ_FILE_GET_FAT_TABLE_STATE,      L2_STORE_GetFatTableFSM},
    { READ_FILE_GET_FAT_TABLE_STATE,       READ_FILE_EVENT,       READ_FILE_GET_DATA_STATE,               L2_STORE_ReadFileFSM},
    { READ_FILE_GET_DATA_STATE,            READ_FILE_EVENT,       READ_FILE_GET_DATA_STATE,               L2_STORE_ReadFileFSM },
};




//准备把文件读取功能重构：
//1、原来的ACTION函数没有入参，准备新写一个带入参，减少全局变量，便于后期维护；
//2、原来文件读取不支持带文件路径，新的版本准备带文件路径；
//3、为了更好的对图形化界面文件新增、修改、删除、查询的操作；
//4、更好的支持独立目录、文件单独读取
STATE_TRANSFORM_NEW FileReadNewTransitionTable[] =
{
    { READ_FILE_INIT_STATE,                READ_PATITION_INFO_EVENT,   READ_FILE_GET_PARTITION_INFO_STATE, L2_STORE_PartitionAnalysisFSM},
    { READ_FILE_GET_PARTITION_INFO_STATE,  READ_ROOT_PATH_EVENT,  READ_FILE_GET_ROOT_PATH_INFO_STATE, L2_STORE_RootPathAnalysisFSM},
    { READ_FILE_GET_ROOT_PATH_INFO_STATE,  READ_FAT_TABLE_EVENT,  READ_FILE_GET_FAT_TABLE_STATE,      L2_STORE_GetFatTableFSM},
    { READ_FILE_GET_FAT_TABLE_STATE,       READ_FILE_EVENT,       READ_FILE_GET_DATA_STATE,               L2_STORE_ReadFileFSM},
    { READ_FILE_GET_DATA_STATE,            READ_FILE_EVENT,       READ_FILE_GET_DATA_STATE,               L2_STORE_ReadFileFSM },
};



/****************************************************************************
*
*  描述:   仔细看了下状态机代码，发现代码有问题，CurrentState这个变量没有使用，不过原来只需要状态依次从小到大触发就可以
*        所以这块代码是有缺陷的
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
int L2_STORE_FileRead(EVENT event)
{
    EFI_STATUS Status;

    //如果读取的扇区数量，超过文件长度除以每个扇区所占用的字节，则停止读取
    if (FileReadCount > FileLength / (512 * 8))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
        return;
    }
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FileReadFSM current event: %d, NextState: %d table event:%d table NextState: %d\n", 
                              __LINE__, 
                            event, 
                            NextState,
                            FileReadTransitionTable[NextState].event,
                            FileReadTransitionTable[NextState].NextState);
    
    if ( event == FileReadTransitionTable[NextState].event)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
        FileReadTransitionTable[NextState].pAction();
        NextState = FileReadTransitionTable[NextState].NextState;
    }
    else  
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
        NextState = READ_FILE_INIT_STATE;
    }

    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
    StatusErrorCount++;
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
EFI_STATUS L3_APPLICATION_ReadFile(UINT8 *FileName, UINT8 NameLength, UINT8 *pBuffer)
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FileName: %a \n", __LINE__, FileName);
    if (pBuffer == NULL)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d:  \n", __LINE__);
        return -1;
    }
    
    L1_MEMORY_Copy(ReadFileName, FileName, NameLength);
    pReadFileDestBuffer = pBuffer;
    ReadFileNameLength = NameLength;

    FileReadCount = 0;
    sector_count = 0;
    PreviousBlockNumber = 0;
    FileBlockStart = 0;
    NextState = READ_FILE_INIT_STATE;
    READ_FILE_FSM_Event = 0;

    //当前读取系统文件，都是在"OS"这个分区
    for (int i = 0; i < PartitionCount; i++)
    {
        if (device[i].PartitionName[0] == EFI_FILE_STORE_PATH_PARTITION_NAME[0] && device[i].PartitionName[1] == EFI_FILE_STORE_PATH_PARTITION_NAME[1])
        {
            ReadFilePartitionID = i;
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: ReadFilePartitionID: %d \n", __LINE__, ReadFilePartitionID);
            break;
        }
    }

    //默认第一个分区是FAT32
    //ReadFilePartitionID = 1;

    for (int i = 0; i < 5; i++)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: i: %d \n", __LINE__, i);
        //DEBUG ((EFI_D_INFO, "%d HandleEnterPressed FSM_Event: %d\n", __LINE__, READ_FILE_FSM_Event));

        //这里就是按照读取文件的状态机，一个一个事件的触发
        L2_STORE_FileRead(READ_FILE_FSM_Event++);

        //前几个事件只需要触发一次，但是文件读取的时候，需要触发很多次
        if (READ_FILE_EVENT <= READ_FILE_FSM_Event)
            READ_FILE_FSM_Event = READ_FILE_EVENT;
    }

    
}



/****************************************************************************
*
*  描述:    解析文件所在的路径列表，以/分割，把根目录写到FilePaths[0]，下一级目录写到FilePaths[1]，依次类推
*         示例："/OS/resource/zhufeng.bmp"，把OS写到FilePaths[0]，resource写到FilePaths[1]
*
*  参数1： 示例："/OS/resource/zhufeng.bmp"，其中/OS是指系统目录
*  参数2： pBuffer
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
UINT16 L3_APPLICATION_GetFilePaths(UINT8 *pPath, FILE_READ_DATA *FileReadData)
{
    UINT16 i = 0;
    UINT16 j = 0;
    UINT16 PathCount = 0;

	//如果不是以/开头的路径，我们认为不合法
    if ('/' != pPath[0])
    {
    	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d File name does not start with '/'. \n", __LINE__);
   
        return -1;
    }

    for (i = 1; i <= AsciiStrLen(pPath); i++)
    {
    	//从第2个字符开始往后找，如果找到/，则表示已经找到一个路径
        if('/' == pPath[i] || '\0' == pPath[i])
        {
            FileReadData->FilePaths[PathCount][j] = '\0';
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Found path: %a\n", __LINE__, FileReadData->FilePaths[PathCount]);
    
            j = 0;
            PathCount++;
        }
        else
        {
            FileReadData->FilePaths[PathCount][j++] = L1_STRING_UpperCaseString(pPath[i]);
        }
    }

	//用于记录一共找到路径个数
	FileReadData->PathCount = PathCount;
}



/****************************************************************************
*
*  描述:    解析文件名，是从最后一个字符解析
*
*  参数1： 示例："/OS/resource/zhufeng.bmp"，其中/OS是指系统目录
*  参数2： pBuffer
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
UINT16 L3_APPLICATION_GetFileName(UINT8 *pPath, UINT8 *FileName)
{
    UINT16 Length = AsciiStrLen(pPath);
    UINT16 j = 0;
    
    for (UINT16 i = Length - 1; i >= 0; i--)
    {
    	//从最后一位，往前找，如果找到/表示都是文件名，文件名不合法的场景暂时没有判断，比如是否有：字符等等
        if ('/' == pPath[i])
        {
            //因为取文件名是从最后一位往前放文件名，所以需要倒序下
            L1_STRING_Reverse(FileName);
			
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d File name: %a\n", __LINE__, FileName);
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d filename start location: %d\n", __LINE__, i);

            break;
        }
        FileName[j++] = pPath[i];
    }

}

EFI_STATUS L2_STORE_PartitionMBRAnalysis(UINT8 *Buffer, DEVICE_PARAMETER *pDevice)
{ 
	// analysis data area of patition
	L1_FILE_FAT32_DataSectorAnalysis(Buffer, &pDevice->stMBRSwitched); 
	
	// data sector number start include: reserved selector, fat sectors(usually is 2: fat1 and fat2), and file system boot path start cluster(usually is 2, data block start number is 2)
	UINT64 DataAreaStartSector = pDevice->stMBRSwitched.ReservedSelector + pDevice->stMBRSwitched.SectorsPerFat * pDevice->stMBRSwitched.FATCount + (pDevice->stMBRSwitched.BootPathStartCluster - 2) * 8;
	//BlockSize = pstMBRSwitched->SectorOfCluster * 512;
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: DataAreaStartSector:%ld BlockSize: %d\n",  __LINE__, DataAreaStartSector, BlockSize);

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
UINT32 L2_FILE_GetNextBlockNumber2(UINT16 PartitionID, UINT64 PreviousBlockNumber)
{
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: PartitionID: %d PreviousBlockNumber: %d\n",  __LINE__, PartitionID, PreviousBlockNumber);

    if (PreviousBlockNumber == 0)
    {
        return 0x0fffffff;
    }
    UINT16 i = PartitionID;
    
    if (device[i].pFAT_TableBuffer[PreviousBlockNumber * 4] == 0xff 
        && device[i].pFAT_TableBuffer[PreviousBlockNumber * 4 + 1] == 0xff 
        && device[i].pFAT_TableBuffer[PreviousBlockNumber * 4 + 2] == 0xff 
        && device[i].pFAT_TableBuffer[PreviousBlockNumber * 4 + 3] == 0x0f)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: PreviousBlockNumber: %d, PreviousBlockNumber: %llX\n",  __LINE__, PreviousBlockNumber, 0x0fffffff);
        return 0x0fffffff;
    }
    
    return device[i].pFAT_TableBuffer[PreviousBlockNumber  * 4] | (UINT32)device[i].pFAT_TableBuffer[PreviousBlockNumber * 4 + 1] << 8 | (UINT32)device[i].pFAT_TableBuffer[PreviousBlockNumber * 4 + 2] << 16 | (UINT32)device[i].pFAT_TableBuffer[PreviousBlockNumber * 4 + 3] << 24;  
}


/****************************************************************************
*
*  描述:    通过文件所在的完整路径，读取文件
*
*  参数1： 示例："/OS/resource/zhufeng.bmp"，其中/OS是指系统目录
*  参数2： pBuffer
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
UINT16 L3_APPLICATION_PartitionQueryByPath(DEVICE_PARAMETER *pDevice, FILE_READ_DATA *pFileReadData)
{
	UINT16 j = 0;
	
	//找到对应的分区
    for (UINT16 i = 0; i < PartitionCount; i++)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FileInPartitionID: %a \n", __LINE__, pDevice[i].PartitionName);
            
        //分区都在FilePaths第一个字符串
        for (j = 0; L1_STRING_IsValidNameChar(pDevice[i].PartitionName[j]) && L1_STRING_IsValidNameChar(pFileReadData->FilePaths[0][j]); j++)
        {
            if (pDevice[i].PartitionName[j] != pFileReadData->FilePaths[0][j])            
            {
                break;
            }
        }

        //这里需要注意，分区名称如果为空，值是0x20
        if (pDevice[i].PartitionName[j] == 0x20 && pFileReadData->FilePaths[0][j] == 0)
        {
			pFileReadData->CurrentPartitionID = i;
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FileInPartitionID: %d \n", __LINE__, pFileReadData->CurrentPartitionID);
            return i;
        }
    }

	return 0xffff;
}



UINT16 L3_APPLICATION_FileDataGetByItemID(FAT32_ROOTPATH_SHORT_FILE_ITEM *pItemsInPath , FILE_READ_DATA *pFileReadData, UINT64 *pNextReadSectorNumber, UINT16 j)
{
    UINT16 FileInPartitionID = pFileReadData->CurrentPartitionID;
	
	//获取下一次访问的扇区编号
	UINT64 BlockNumber = (UINT64)pItemsInPath[j].StartClusterLow2B[0] | (UINT64)pItemsInPath[j].StartClusterLow2B[1] << 8 | (UINT64)pItemsInPath[j].StartClusterHigh2B[0] << 16 | (UINT64)pItemsInPath[j].StartClusterHigh2B[1] << 24;
	
	*pNextReadSectorNumber = device[FileInPartitionID].stMBRSwitched.ReservedSelector + device[FileInPartitionID].stMBRSwitched.SectorsPerFat * device[FileInPartitionID].stMBRSwitched.FATCount + device[FileInPartitionID].stMBRSwitched.BootPathStartCluster - 2 + (BlockNumber - 2) * 8;			
	
	FileLength = (UINT64)pItemsInPath[j].FileLength[0] | (UINT64)pItemsInPath[j].FileLength[1] << 8 | (UINT64)pItemsInPath[j].FileLength[2] << 16 | (UINT64)pItemsInPath[j].FileLength[3] << 24;

	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: File In Items: %d NextReadSectorNumber: %d FileLength: %d\n", __LINE__, j, *pNextReadSectorNumber, FileLength);

	//文件
	if (pItemsInPath[j].Attribute[0] == 0x20)
	{
		UINT64 SectorCount = FileLength / (512 * 8);
		UINT8 AddOneFlag = (FileLength % (512 * 8) == 0) ? 0 : 1; 
		UINT8 BufferBlock[DISK_BUFFER_SIZE * 8];


		for (UINT64 ReadTimes = 0; ReadTimes < FileLength / (512 * 8); ReadTimes++)
		{
			//这样读取文件会有问题，如果文件是连续存放，则没有问题，如果是非连续存放，则不行
			EFI_STATUS Status = L2_STORE_Read(FileInPartitionID, *pNextReadSectorNumber, 8, BufferBlock); 
			if (EFI_SUCCESS != Status)
			{
				L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Read from device error: Status:%X \n", __LINE__, Status);
				return Status;
			}
			
			//L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: NextReadSectorNumber: %llu BlockNumber: %llu\n", __LINE__, *pNextReadSectorNumber, BlockNumber);
												

			for (UINT16 BufferCopy = 0; BufferCopy < 512 * 8; BufferCopy++)
				pFileReadData->pDestBuffer[ReadTimes * 512 * 8 + BufferCopy] = BufferBlock[BufferCopy];

			BlockNumber = L2_FILE_GetNextBlockNumber2(FileInPartitionID, BlockNumber);
			*pNextReadSectorNumber = device[FileInPartitionID].stMBRSwitched.ReservedSelector + device[FileInPartitionID].stMBRSwitched.SectorsPerFat * device[FileInPartitionID].stMBRSwitched.FATCount + device[FileInPartitionID].stMBRSwitched.BootPathStartCluster - 2 + (BlockNumber - 2) * 8;
		}			
	}
}


UINT16 L3_APPLICATION_ItemFindByName(FAT32_ROOTPATH_SHORT_FILE_ITEM *pItemsInPath , FILE_READ_DATA *pFileReadData)
{
	UINT64 FileBlockStartNumber = 0;
	UINT64 FileLength = 0;
    UINT16 FileInPartitionID = pFileReadData->CurrentPartitionID;
	UINT16 CurrentPathID  = pFileReadData->CurrentPathID;
	UINT64 NextReadSectorNumber = 0;
	
	//解析目录下的项目，因为每项32个字节长度，我们读取的512字节，所以512/32=16
	for (UINT16 j = 0; j < DISK_BUFFER_SIZE * 2 / 32; j++)
	{
		//外层循环一共是32项，但实际目录下可能没有存放32个项，比如少于32项
		if (pItemsInPath[j].FileName[0] == 0 )
		{
			L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: pItemsInPath[j].FileName[0] == 0 || pItemsInPath[j].FileName[0] == 0xE5\n", __LINE__);

			break;
		}

		//要么是文件，要么是文件夹
		if (pItemsInPath[j].FileName[0] != 0xE5 && (pItemsInPath[j].Attribute[0] == 0x20 || pItemsInPath[j].Attribute[0] == 0x10))
		{
			UINT16 k;
			//8位文件名+.+3位后缀名=12位，加'\0'共13位
			//当前暂不处理长文件名，长目录名
			UINT8 FileName[14] = {0};
			L1_FILE_NameGetUseItem(pItemsInPath[j], FileName);
			//L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FileName: %a\n", __LINE__, FileName);

			L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Name:%c%c%c%c%c%c%c%c Extension:%c%c%c Start:%02X%02X%02X%02X Length: %02X%02X%02X%02X A: %02X %a ", __LINE__,
											pItemsInPath[j].FileName[0], pItemsInPath[j].FileName[1], pItemsInPath[j].FileName[2], pItemsInPath[j].FileName[3], pItemsInPath[j].FileName[4], pItemsInPath[j].FileName[5], pItemsInPath[j].FileName[6], pItemsInPath[j].FileName[7],
											pItemsInPath[j].ExtensionName[0], pItemsInPath[j].ExtensionName[1],pItemsInPath[j].ExtensionName[2],
											pItemsInPath[j].StartClusterHigh2B[0], pItemsInPath[j].StartClusterHigh2B[1],
											pItemsInPath[j].StartClusterLow2B[0], pItemsInPath[j].StartClusterLow2B[1],
											pItemsInPath[j].FileLength[0], pItemsInPath[j].FileLength[1], pItemsInPath[j].FileLength[2], pItemsInPath[j].FileLength[3],
											pItemsInPath[j].Attribute[0],
											FileName);

			//查找CurrentPathID对应的路径
			for (k = 0; L1_STRING_IsValidNameChar(FileName[k]) && L1_STRING_IsValidNameChar(pFileReadData->FilePaths[CurrentPathID][k]); k++)
			{
				if (FileName[k] != pFileReadData->FilePaths[CurrentPathID][k])			
				{
					break;
				}
			}

			//这里需要注意，分区名称如果为空，值是0x20
			if (FileName[k] == 0 && pFileReadData->FilePaths[CurrentPathID][k] == 0)
			{					
				pFileReadData->pItemBuffer = pItemsInPath;
				pFileReadData->pItemID = j;
				return j;
			}
		}	
	}

}




EFI_STATUS L3_APPLICATION_FileReadWithPath(UINT8 *pPath, UINT8 *pDestBuffer)
{
	FILE_READ_DATA FileReadData;
	FileReadData.pDestBuffer = pDestBuffer;
    UINT16 FileInPartitionID = 0xffff;
	UINT8 Buffer[DISK_BUFFER_SIZE * 2];
	UINT64 NextReadSectorNumber = 0;
	
    if (pPath[0] != '/')
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Path error!\n", __LINE__);
        return;
    }

    UINT16 Length = AsciiStrLen(pPath);
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d string length: %d\n", __LINE__, Length);
        
	//分析文件路径，把路径和文件名按照/拆分，方便后续操作
    L3_APPLICATION_GetFilePaths(pPath, &FileReadData);
	
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FileReadData.FilePaths[0]: %a \n", __LINE__, FileReadData.FilePaths[0]);

	//找到分区对应的设备索引，后续对文件进行读写的时候需要。
	FileInPartitionID = L3_APPLICATION_PartitionQueryByPath(&device, &FileReadData);
        	
	//读取第一个扇区，分析分区参数，比如：FAT表大小，FAT表个数，保留扇区数
	EFI_STATUS Status = L2_STORE_Read(FileInPartitionID, 0, 1, Buffer); 
    if (EFI_SUCCESS != Status)
    {
		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Read from device error: Status:%X \n", __LINE__, Status);
    	return Status;
    }
	
	L2_STORE_PartitionMBRAnalysis(Buffer, &device[FileInPartitionID]);

	//读取FAT表
	L2_STORE_FatTableRead(&device[FileInPartitionID], FileReadData);

	NextReadSectorNumber = device[FileInPartitionID].stMBRSwitched.ReservedSelector + device[FileInPartitionID].stMBRSwitched.SectorsPerFat * device[FileInPartitionID].stMBRSwitched.FATCount + (device[FileInPartitionID].stMBRSwitched.BootPathStartCluster - 2) * 8;;

	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: NextReadSectorNumber: %d \n", __LINE__, NextReadSectorNumber);

	//用于存放从目录下读取文件或目录数据，用于解析
    FAT32_ROOTPATH_SHORT_FILE_ITEM pItemsInPath[32];

	//解析各个路径参数
	//这里需要注意，第一个路径是分区名字，所以根目录下的路径需要从第二个名字开始找，索引号是1
	for (UINT16 i = 1; i < FileReadData.PathCount; i++)
	{
		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FilePaths: %a \n", __LINE__, FileReadData.FilePaths[i]);
            
		//当前只读一个扇区，这样这里是有缺陷的，如果路径里的项目超过16个，则不能正常读取
		EFI_STATUS Status = L2_STORE_Read(FileInPartitionID, NextReadSectorNumber, 2, Buffer); 
		if (EFI_SUCCESS != Status)
		{
			L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Read from device error: Status:%X \n", __LINE__, Status);
			return Status;
		}
		
		L1_MEMORY_Copy(&pItemsInPath, Buffer, DISK_BUFFER_SIZE * 2);
		FileReadData.CurrentPathID = i;
		
		UINT16 index = L3_APPLICATION_ItemFindByName(&pItemsInPath, &FileReadData);

		L3_APPLICATION_FileDataGetByItemID(&pItemsInPath, &FileReadData, &NextReadSectorNumber, index);
	}

}


/****************************************************************************
*
*  描述:    通过文件所在的完整路径，删除文件
*
*  参数1： 示例："/OS/resource/zhufeng.bmp"，其中/OS是指系统目录
*  参数2： pBuffer
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L3_APPLICATION_FileDeleteWithPath(UINT8 *pPath, UINT8 *pDestBuffer)
{
	FILE_READ_DATA FileReadData;
	FileReadData.pDestBuffer = pDestBuffer;
    UINT16 FileInPartitionID = 0xffff;
	UINT8 Buffer[DISK_BUFFER_SIZE * 2];
	UINT64 NextReadSectorNumber = 0;
	
    if (pPath[0] != '/')
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Path error!\n", __LINE__);
        return;
    }

    UINT16 Length = AsciiStrLen(pPath);
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d string length: %d\n", __LINE__, Length);
        
	//分析文件路径，把路径和文件名按照/拆分，方便后续操作
    L3_APPLICATION_GetFilePaths(pPath, &FileReadData);
	
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FileReadData.FilePaths[0]: %a \n", __LINE__, FileReadData.FilePaths[0]);

	//找到分区对应的设备索引，后续对文件进行读写的时候需要。
	FileInPartitionID = L3_APPLICATION_PartitionQueryByPath(&device, &FileReadData);
        	
	//读取第一个扇区，分析分区参数，比如：FAT表大小，FAT表个数，保留扇区数
	EFI_STATUS Status = L2_STORE_Read(FileInPartitionID, 0, 1, Buffer); 
    if (EFI_SUCCESS != Status)
    {
		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Read from device error: Status:%X \n", __LINE__, Status);
    	return Status;
    }
	
	L2_STORE_PartitionMBRAnalysis(Buffer, &device[FileInPartitionID]);

	//读取FAT表
	L2_STORE_FatTableRead(&device[FileInPartitionID], FileReadData);

	NextReadSectorNumber = device[FileInPartitionID].stMBRSwitched.ReservedSelector + device[FileInPartitionID].stMBRSwitched.SectorsPerFat * device[FileInPartitionID].stMBRSwitched.FATCount + (device[FileInPartitionID].stMBRSwitched.BootPathStartCluster - 2) * 8;;

	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: NextReadSectorNumber: %d \n", __LINE__, NextReadSectorNumber);

	//用于存放从目录下读取文件或目录数据，用于解析
    FAT32_ROOTPATH_SHORT_FILE_ITEM pItemsInPath[32];

	//解析各个路径参数
	//这里需要注意，第一个路径是分区名字，所以根目录下的路径需要从第二个名字开始找，索引号是1
	for (UINT16 i = 1; i < FileReadData.PathCount; i++)
	{
		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FilePaths: %a \n", __LINE__, FileReadData.FilePaths[i]);
            
		//当前只读一个扇区，这样这里是有缺陷的，如果路径里的项目超过16个，则不能正常读取
		EFI_STATUS Status = L2_STORE_Read(FileInPartitionID, NextReadSectorNumber, 2, Buffer); 
		if (EFI_SUCCESS != Status)
		{
			L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Read from device error: Status:%X \n", __LINE__, Status);
			return Status;
		}
		
		L1_MEMORY_Copy(&pItemsInPath, Buffer, DISK_BUFFER_SIZE * 2);
		FileReadData.CurrentPathID = i;
		
		UINT16 index = L3_APPLICATION_ItemFindByName(&pItemsInPath, &FileReadData);
		
		//1、把目录项名称的第一个字节修改为0xE5
		pItemsInPath[index].FileName[0] = 0xE5;
		L1_MEMORY_Copy(Buffer, &pItemsInPath, DISK_BUFFER_SIZE * 2);
		
		Status = L2_STORE_Write(FileInPartitionID, 0, 1, Buffer); 

		L3_APPLICATION_FileDataGetByItemID(&pItemsInPath, &FileReadData, &NextReadSectorNumber, index);
		
		//获取文件所在的块号
		UINT64 BlockNumber = (UINT64)pItemsInPath[index].StartClusterLow2B[0] | (UINT64)pItemsInPath[index].StartClusterLow2B[1] << 8 | (UINT64)pItemsInPath[index].StartClusterHigh2B[0] << 16 | (UINT64)pItemsInPath[index].StartClusterHigh2B[1] << 24;


		for (UINT64 ReadTimes = 0; ReadTimes < FileLength / (512 * 8); ReadTimes++)
		{				
			BlockNumber = L2_FILE_GetNextBlockNumber2(FileInPartitionID, BlockNumber);
			L2_STORE_FatTableSetZero(&device[FileInPartitionID], FileReadData,  BlockNumber);
		}
		
	}

	//删除文件需要的操作：
	//2、把FAT Table对应的索引清空
	//3、
}


/****************************************************************************
*
*  描述:    通过文件所在的完整路径，删除文件
*
*  参数1： 示例："/OS/resource/zhufeng.bmp"，其中/OS是指系统目录
*  参数2： pBuffer
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L3_APPLICATION_FileAddWithPath(UINT8 *pPath, UINT8 *pDestBuffer)
{
    FILE_READ_DATA FileReadData;
	FileReadData.pDestBuffer = pDestBuffer;
    UINT16 FileInPartitionID = 0xffff;
	UINT8 Buffer[DISK_BUFFER_SIZE * 2];
	UINT64 NextReadSectorNumber = 0;
	
    if (pPath[0] != '/')
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Path error!\n", __LINE__);
        return;
    }

    UINT16 Length = AsciiStrLen(pPath);
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d string length: %d\n", __LINE__, Length);
        
	//分析文件路径，把路径和文件名按照/拆分，方便后续操作
    L3_APPLICATION_GetFilePaths(pPath, &FileReadData);
	
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FileReadData.FilePaths[0]: %a \n", __LINE__, FileReadData.FilePaths[0]);

	//找到分区对应的设备索引，后续对文件进行读写的时候需要。
	FileInPartitionID = L3_APPLICATION_PartitionQueryByPath(&device, &FileReadData);
        	
	//读取第一个扇区，分析分区参数，比如：FAT表大小，FAT表个数，保留扇区数
	EFI_STATUS Status = L2_STORE_Read(FileInPartitionID, 0, 1, Buffer); 
    if (EFI_SUCCESS != Status)
    {
		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Read from device error: Status:%X \n", __LINE__, Status);
    	return Status;
    }
	
	L2_STORE_PartitionMBRAnalysis(Buffer, &device[FileInPartitionID]);

	//读取FAT表
	L2_STORE_FatTableRead(&device[FileInPartitionID], FileReadData);

	NextReadSectorNumber = device[FileInPartitionID].stMBRSwitched.ReservedSelector + device[FileInPartitionID].stMBRSwitched.SectorsPerFat * device[FileInPartitionID].stMBRSwitched.FATCount + (device[FileInPartitionID].stMBRSwitched.BootPathStartCluster - 2) * 8;;

	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: NextReadSectorNumber: %d \n", __LINE__, NextReadSectorNumber);

	//用于存放从目录下读取文件或目录数据，用于解析
    FAT32_ROOTPATH_SHORT_FILE_ITEM pItemsInPath[32];

	//解析各个路径参数
	//这里需要注意，第一个路径是分区名字，所以根目录下的路径需要从第二个名字开始找，索引号是1
	for (UINT16 i = 1; i < FileReadData.PathCount; i++)
	{
		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FilePaths: %a \n", __LINE__, FileReadData.FilePaths[i]);
            
		//当前只读一个扇区，这样这里是有缺陷的，如果路径里的项目超过16个，则不能正常读取
		EFI_STATUS Status = L2_STORE_Read(FileInPartitionID, NextReadSectorNumber, 2, Buffer); 
		if (EFI_SUCCESS != Status)
		{
			L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Read from device error: Status:%X \n", __LINE__, Status);
			return Status;
		}
		
		L1_MEMORY_Copy(&pItemsInPath, Buffer, DISK_BUFFER_SIZE * 2);
		FileReadData.CurrentPathID = i;
		
		UINT16 index = L3_APPLICATION_ItemFindByName(&pItemsInPath, &FileReadData);

		L3_APPLICATION_FileDataGetByItemID(&pItemsInPath, &FileReadData, &NextReadSectorNumber, index);
	}

}


/****************************************************************************
*
*  描述:    通过文件所在的完整路径，修改文件
*
*  参数1： 示例："/OS/resource/zhufeng.bmp"，其中/OS是指系统目录
*  参数2： pBuffer
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L3_APPLICATION_FileModifyWithPath(UINT8 *pPath, UINT8 *pDestBuffer)
{
    FILE_READ_DATA FileReadData;
	FileReadData.pDestBuffer = pDestBuffer;
    UINT16 FileInPartitionID = 0xffff;
	UINT8 Buffer[DISK_BUFFER_SIZE * 2];
	UINT64 NextReadSectorNumber = 0;
	
    if (pPath[0] != '/')
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Path error!\n", __LINE__);
        return;
    }

    UINT16 Length = AsciiStrLen(pPath);
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d string length: %d\n", __LINE__, Length);
        
	//分析文件路径，把路径和文件名按照/拆分，方便后续操作
    L3_APPLICATION_GetFilePaths(pPath, &FileReadData);
	
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FileReadData.FilePaths[0]: %a \n", __LINE__, FileReadData.FilePaths[0]);

	//找到分区对应的设备索引，后续对文件进行读写的时候需要。
	FileInPartitionID = L3_APPLICATION_PartitionQueryByPath(&device, &FileReadData);
        	
	//读取第一个扇区，分析分区参数，比如：FAT表大小，FAT表个数，保留扇区数
	EFI_STATUS Status = L2_STORE_Read(FileInPartitionID, 0, 1, Buffer); 
    if (EFI_SUCCESS != Status)
    {
		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Read from device error: Status:%X \n", __LINE__, Status);
    	return Status;
    }
	
	L2_STORE_PartitionMBRAnalysis(Buffer, &device[FileInPartitionID]);

	//读取FAT表
	L2_STORE_FatTableRead(&device[FileInPartitionID], FileReadData);

	NextReadSectorNumber = device[FileInPartitionID].stMBRSwitched.ReservedSelector + device[FileInPartitionID].stMBRSwitched.SectorsPerFat * device[FileInPartitionID].stMBRSwitched.FATCount + (device[FileInPartitionID].stMBRSwitched.BootPathStartCluster - 2) * 8;;

	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: NextReadSectorNumber: %d \n", __LINE__, NextReadSectorNumber);

	//用于存放从目录下读取文件或目录数据，用于解析
    FAT32_ROOTPATH_SHORT_FILE_ITEM pItemsInPath[32];

	//解析各个路径参数
	//这里需要注意，第一个路径是分区名字，所以根目录下的路径需要从第二个名字开始找，索引号是1
	for (UINT16 i = 1; i < FileReadData.PathCount; i++)
	{
		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FilePaths: %a \n", __LINE__, FileReadData.FilePaths[i]);
            
		//当前只读一个扇区，这样这里是有缺陷的，如果路径里的项目超过16个，则不能正常读取
		EFI_STATUS Status = L2_STORE_Read(FileInPartitionID, NextReadSectorNumber, 2, Buffer); 
		if (EFI_SUCCESS != Status)
		{
			L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Read from device error: Status:%X \n", __LINE__, Status);
			return Status;
		}
		
		L1_MEMORY_Copy(&pItemsInPath, Buffer, DISK_BUFFER_SIZE * 2);
		FileReadData.CurrentPathID = i;
		
		UINT16 index = L3_APPLICATION_ItemFindByName(&pItemsInPath, &FileReadData);

		L3_APPLICATION_FileDataGetByItemID(&pItemsInPath, &FileReadData, &NextReadSectorNumber, index);
	}
}




UINT8 PreviousItem = -1;

// Display mouse move over's a partition's root path items.



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
EFI_STATUS L3_PARTITION_RootPathAccess()
{	
	EFI_STATUS Status;
	UINT8 Buffer1[512];
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
	
    Color.Red = 0xff;
    Color.Green= 0x00;
    Color.Blue= 0x00;
    Color.Reserved = 0x00;

	UINT16 i = 0;
	
    for (UINT16 i = 0; i < PartitionCount; i++)
    {   
        UINT16 MyComputerPositionX = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartX;
        UINT16 MyComputerPositionY = WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW].StartY;
        
        if (iMouseX >= MyComputerPositionX + 50 && iMouseX <= MyComputerPositionX + 50 + 16 * 6
            && iMouseY >= MyComputerPositionY + i * 16 + 16 * 2 && iMouseY <= MyComputerPositionY + i * 16 + 16 * 3)
        {   
            if (PreviousItem == i)
            {
                break;
            }
            
            //L2_GRAPHICS_RectangleDraw(pMouseSelectedBuffer, 0,  0, 31, 15, 1,  Color, 32);
            L2_STORE_PartitionItemsPrint(i);
            PreviousItem = i;
            L2_GRAPHICS_Copy(pDeskDisplayBuffer, pMouseSelectedBuffer, ScreenWidth, ScreenHeight, 32, 16, MyComputerPositionX + 50, MyComputerPositionY  + i * (16 + 2) + 16 * 2);   
        }
    }
	/*
	Status = L2_STORE_Read(i, sector_count, 1, Buffer1); 
	if ( EFI_SUCCESS == Status )
	{
		//L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X\n", __LINE__, Status);

		//When get root path data sector start number, we can get content of root path.
		L1_FILE_RootPathAnalysis(Buffer1);  

		// data area start from 1824, HZK16 file start from     FileBlockStart  block, so need to convert into sector by multi 8, block start number is 2   
		// next state is to read FAT table
		sector_count = MBRSwitched.ReservedSelector;
		//L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: sector_count:%ld FileLength: %d MBRSwitched.ReservedSelector:%ld\n",  __LINE__, sector_count, FileLength, MBRSwitched.ReservedSelector);
	}           
	*/
}


// Print Folder item in graphics layer 




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
EFI_STATUS L3_PARTITION_FileAccess(UINT16 DeviceID)
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L3_PARTITION_FileAccess\n", __LINE__);
	UINT8 Buffer1[512];
	
	EFI_STATUS Status = L2_STORE_Read(DeviceID, sector_count, 1, Buffer1 );  
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
        return Status;
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
EFI_STATUS L3_PARTITION_SubPathAccess()
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L3_PARTITION_SubPathAccess\n", __LINE__);
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
EFI_STATUS L3_PARTITION_AccessFinish()
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L3_PARTITION_AccessFinish\n", __LINE__);
		
	PARTITION_ITEM_ACCESS_STATE PartitionItemAccessNextState = INIT_ACCESS_STATE;
	PARTITION_ITEM_ACCESS_EVENT PartitionItemAccessEvent = ROOT_PATH_ACCESS_EVENT;
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
EFI_STATUS L3_PARTITION_ParentPathAccess()
{
}


// need a stack to save current detailed path
PARTITION_ITEM_ACCESS_STATE_TRANSFORM PartitionItemAccessStateTransformTable[] =
{
	//Current state             Trigger Event          //Next state              //Handle function
    // init state
    {INIT_ACCESS_STATE,         ROOT_PATH_ACCESS_EVENT, ROOT_PATH_STATE,        L3_PARTITION_RootPathAccess},

    // root path
    {ROOT_PATH_STATE,           FILE_ACCESS_EVENT,   FILE_ITEM_ACCESS_STATE,    L3_PARTITION_FileAccess},
    {ROOT_PATH_STATE,           FOLDER_ACCESS_EVENT, FOLDER_ITEM_ACCESS_STATE,  L3_PARTITION_SubPathAccess},
    {ROOT_PATH_STATE,           PARENT_ACCESS_EVENT, ROOT_PATH_STATE,           L3_PARTITION_RootPathAccess},
    {ROOT_PATH_STATE,           CLOSE_ACCESS_EVENT,  INIT_ACCESS_STATE,         L3_PARTITION_AccessFinish},
    
    // some folder
    {FOLDER_ITEM_ACCESS_STATE,  FILE_ACCESS_EVENT,   FILE_ITEM_ACCESS_STATE,    L3_PARTITION_FileAccess}, // open a file and next state is current folder
    {FOLDER_ITEM_ACCESS_STATE,  FOLDER_ACCESS_EVENT, FOLDER_ITEM_ACCESS_STATE,  L3_PARTITION_SubPathAccess}, // a folder.
    {FOLDER_ITEM_ACCESS_STATE,  PARENT_ACCESS_EVENT, FOLDER_ITEM_ACCESS_STATE,  L3_PARTITION_ParentPathAccess}, // access parent path
    {FOLDER_ITEM_ACCESS_STATE,  CLOSE_ACCESS_EVENT,  INIT_ACCESS_STATE,         L3_PARTITION_AccessFinish},

    // some file
    {FILE_ITEM_ACCESS_STATE,    FILE_ACCESS_EVENT,   INIT_ACCESS_STATE,    		L3_PARTITION_FileAccess},
    {FILE_ITEM_ACCESS_STATE,    FOLDER_ACCESS_EVENT, INIT_ACCESS_STATE,  		L3_PARTITION_SubPathAccess},
    {FILE_ITEM_ACCESS_STATE,    PARENT_ACCESS_EVENT, FOLDER_ITEM_ACCESS_STATE,  L3_PARTITION_ParentPathAccess},
    {FILE_ITEM_ACCESS_STATE,    CLOSE_ACCESS_EVENT,  INIT_ACCESS_STATE,         L3_PARTITION_AccessFinish}
};

PARTITION_ITEM_ACCESS_STATE PartitionItemAccessNextState = INIT_ACCESS_STATE;
PARTITION_ITEM_ACCESS_EVENT PartitionItemAccessEvent = ROOT_PATH_ACCESS_EVENT;


//我的电脑窗口，对文件处理的状态机，开始可以只对FAT32文件系统格式操作，后续可以增加对NTFS文件系统对应操作，因为FAT32相对简单一些
//入参：目录、文件（或：未选中文件，因为可以在某个目录创建文件，这个场景不需要选择某文件）
//事件：新增、打开、删除、修改
//操作处理函数

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
VOID L3_PARTITION_Access() //暂时未使用
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L3_PARTITION_Access\n", __LINE__);
	
    PARTITION_ITEM_ACCESS_STATE     CurrentState;
	UINT16 RootFlag = TRUE;

    for (UINT16 i = 0; i < sizeof(PartitionItemAccessStateTransformTable) / sizeof(PartitionItemAccessStateTransformTable[0]); i++ )
    {
        if (PartitionItemAccessStateTransformTable[i].CurrentState == PartitionItemAccessNextState 
            && PartitionItemAccessEvent == PartitionItemAccessStateTransformTable[i].event )
        {
        	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L3_PARTITION_Access For\n", __LINE__);
            PartitionItemAccessNextState = PartitionItemAccessStateTransformTable[i].NextState;

            // need to check the return value after function runs..... 
            PartitionItemAccessStateTransformTable[i].pFunc();
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, 
                                                        WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], 
                                                        "%d: L3_PARTITION_Access For: %d L3_PARTITION_Access For: %d\n", 
                                                        __LINE__, 
                                                        PartitionItemAccessEvent, 
                                                        PartitionItemAccessNextState);
            break;
        }   
    }
    
}

