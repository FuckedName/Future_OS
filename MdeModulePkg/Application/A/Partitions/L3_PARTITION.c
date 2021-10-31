#include <L3_PARTITION.h>

#include <Graphics/L2_GRAPHICS.h>
#include <Devices/Store/L2_DEVICE_Store.h>
#include <Global/Global.h>

int READ_FILE_FSM_Event = READ_PATITION_EVENT;


// all partitions analysis
EFI_STATUS L2_STORE_PartitionAnalysisFSM()
{    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d PartitionAnalysisFSM\n", __LINE__);
    EFI_STATUS Status ;
    UINT8 Buffer1[DISK_BUFFER_SIZE];
    
    for (int i = 0; i < PartitionCount; i++)
    {
        if (device[i].DeviceType == 1 && device[i].SectorCount == STORE_EFI_PATH_PARTITION_SECTOR_COUNT)
        {
             Status = L1_STORE_READ(i, sector_count, 1, Buffer1); 
             if ( EFI_SUCCESS == Status )
            {
                L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
                  
                  // analysis data area of patition
                  L1_FILE_FAT32_DataSectorAnalysis(Buffer1, &MBRSwitched); 

                  // data sector number start include: reserved selector, fat sectors(usually is 2: fat1 and fat2), and file system boot path start cluster(usually is 2, data block start number is 2)
                  sector_count = MBRSwitched.ReservedSelector + MBRSwitched.SectorsPerFat * MBRSwitched.NumFATS + (MBRSwitched.BootPathStartCluster - 2) * 8;
                  BlockSize = MBRSwitched.SectorOfCluster * 512;
                  L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: sector_count:%ld BlockSize: %d\n",  __LINE__, sector_count, BlockSize);
             }           
            break;
         }       
    }

    return EFI_SUCCESS;
}

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



//delete blanks of file name and file extension name
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

EFI_STATUS L1_FILE_RootPathAnalysis(UINT8 *p)
{
    memcpy(&pItems, p, DISK_BUFFER_SIZE);
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

            //for (int j = 0; j < 5; j++)
            //  L2_DEBUG_Print1(j * 3 * 8, 16 * 40 + valid_count * 16, "%02X ", pItems[i].FileName[j]);
            if (L1_STRING_Compare(FileName2, ReadFileName, ReadFileNameLength) == EFI_SUCCESS)
            {
                L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d FileName:%2c%2c%2c%2c%2c%2c%2c%2c ExtensionName:%2c%2c%2c StartCluster:%02X%02X%02X%02X FileLength: %02X%02X%02X%02X Attribute: %02X    ",  __LINE__,
                                pItems[i].FileName[0], pItems[i].FileName[1], pItems[i].FileName[2], pItems[i].FileName[3], pItems[i].FileName[4], pItems[i].FileName[5], pItems[i].FileName[6], pItems[i].FileName[7],
                                pItems[i].ExtensionName[0], pItems[i].ExtensionName[1],pItems[i].ExtensionName[2],
                                pItems[i].StartClusterHigh2B[0], pItems[i].StartClusterHigh2B[1],
                                pItems[i].StartClusterLow2B[0], pItems[i].StartClusterLow2B[1],
                                pItems[i].FileLength[0], pItems[i].FileLength[1], pItems[i].FileLength[2], pItems[i].FileLength[3],
                                pItems[i].Attribute[0]);
                
              FileBlockStart = (UINT32)pItems[i].StartClusterHigh2B[0] * 16 * 16 * 16 * 16 + (UINT32)pItems[i].StartClusterHigh2B[1] * 16 * 16 * 16 * 16 * 16 * 16 + pItems[i].StartClusterLow2B[0] + (UINT32)pItems[i].StartClusterLow2B[1] * 16 * 16;
              FileLength = pItems[i].FileLength[0] + (UINT32)pItems[i].FileLength[1] * 16 * 16 + (UINT32)pItems[i].FileLength[2] * 16 * 16 * 16 * 16 + (UINT32)pItems[i].FileLength[3] * 16 * 16 * 16 * 16 * 16 * 16;

              L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FileBlockStart: %d FileLength: %ld\n", __LINE__, FileBlockStart, FileLength);
            }
        }

}


// analysis a partition 
EFI_STATUS L2_STORE_RootPathAnalysisFSM()
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d RootPathAnalysisFSM\n", __LINE__);
    EFI_STATUS Status ;
    UINT8 Buffer1[DISK_BUFFER_SIZE];
    
    for (int i = 0; i < PartitionCount; i++)
    {
        if (device[i].DeviceType == 1 && device[i].SectorCount == STORE_EFI_PATH_PARTITION_SECTOR_COUNT)
        {
            Status = L1_STORE_READ(i, sector_count, 1, Buffer1); 
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
            break;
         }       
    }

    return EFI_SUCCESS;
}


// 
EFI_STATUS L2_STORE_GetFatTableFSM()
{    
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d GetFatTableFSM\n", __LINE__);
    EFI_STATUS Status ;
    if (NULL != FAT32_Table)
    {
        // start sector of file
        sector_count = MBRSwitched.ReservedSelector + MBRSwitched.SectorsPerFat * MBRSwitched.NumFATS + MBRSwitched.BootPathStartCluster - 2 + (FileBlockStart - 2) * 8;
        
        // for FAT32_Table get next block number
        PreviousBlockNumber = FileBlockStart;

        return EFI_SUCCESS;
    }
    
    for (int i = 0; i < PartitionCount; i++)
    {
        if (device[i].DeviceType == 1 && device[i].SectorCount == STORE_EFI_PATH_PARTITION_SECTOR_COUNT)
        {
             //start block need to recompute depends on file block start number 
             //FileBlockStart;
             L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d MBRSwitched.SectorsPerFat: %d\n", __LINE__, MBRSwitched.SectorsPerFat);
            
             // 512 = 16 * 32 = 4 item * 32
            FAT32_Table = (UINT8 *)AllocateZeroPool(DISK_BUFFER_SIZE * MBRSwitched.SectorsPerFat);
             if (NULL == FAT32_Table)
             {
                 L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: NULL == FAT32_Table\n", __LINE__);                             
             }             
             
            Status = L1_STORE_READ(i, sector_count,  MBRSwitched.SectorsPerFat, FAT32_Table); 
            if ( EFI_SUCCESS == Status )
            {
                  //CopyMem(FAT32_Table, Buffer1, DISK_BUFFER_SIZE * MBRSwitched.SectorsPerFat);
                  for (int j = 0; j < 250; j++)
                  {
                        //L2_DEBUG_Print1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", Buffer1[j] & 0xff);
                  }

                  // start sector of file
                  sector_count = MBRSwitched.ReservedSelector + MBRSwitched.SectorsPerFat * MBRSwitched.NumFATS + MBRSwitched.BootPathStartCluster - 2 + (FileBlockStart - 2) * 8;

                  // for FAT32_Table get next block number
                  PreviousBlockNumber = FileBlockStart;
                  //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: sector_count:%ld FileLength: %d PreviousBlockNumber: %d\n",  __LINE__, sector_count, FileLength, PreviousBlockNumber);
             }           
            break;
         }       
    }

    return EFI_SUCCESS;
}

UINT32 L2_FILE_GetNextBlockNumber()
{
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: PreviousBlockNumber: %d\n",  __LINE__, PreviousBlockNumber);

    if (PreviousBlockNumber == 0)
    {
        return 0x0fffffff;
    }
    
    if (FAT32_Table[PreviousBlockNumber * 4] == 0xff 
        && FAT32_Table[PreviousBlockNumber * 4 + 1] == 0xff 
        && FAT32_Table[PreviousBlockNumber * 4 + 2] == 0xff 
        && FAT32_Table[PreviousBlockNumber * 4 + 3] == 0x0f)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: PreviousBlockNumber: %d, PreviousBlockNumber: %llX\n",  __LINE__, PreviousBlockNumber, 0x0fffffff);
        return 0x0fffffff;
    }
    
    return FAT32_Table[PreviousBlockNumber  * 4] + (UINT32)FAT32_Table[PreviousBlockNumber * 4 + 1] * 16 * 16 + (UINT32)FAT32_Table[PreviousBlockNumber * 4 + 2] * 16 * 16 * 16 * 16 + (UINT32)FAT32_Table[PreviousBlockNumber * 4 + 3] * 16 * 16 * 16 * 16 * 16 * 16;  
}


EFI_STATUS L2_STORE_ReadFileFSM()
{    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d ReadFileFSM: PartitionCount: %d FileLength: %d sector_count: %llu\n", __LINE__, PartitionCount, FileLength, sector_count);
    ////DEBUG ((EFI_D_INFO, "PartitionUSBRead!!\r\n"));
    EFI_STATUS Status ;
    UINT8 BufferBlock[DISK_BLOCK_BUFFER_SIZE];
            
    for (int i = 0; i < PartitionCount; i++)
    {
        if (device[i].DeviceType == 1 && device[i].SectorCount == STORE_EFI_PATH_PARTITION_SECTOR_COUNT)
        {
            // read from USB by block(512 * 8)
            // Read file content from FAT32(USB), minimum unit is block

            UINT8 AddOneFlag = (FileLength % (512 * 8)) == 0 ? 0 : 1;
            
            for (UINT16 k = 0; k < FileLength / (512 * 8) + AddOneFlag; k++)
            {
                Status = L1_STORE_READ(i, sector_count, 8, BufferBlock); 
                if (EFI_ERROR(Status))
                {
                    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
                    return Status;
                }           

                /*
                BufferBlock[0] = 0xff;
                BufferBlock[1] = 0xff;

                Status = L1_STORE_Write(i, sector_count, 8, BufferBlock); 
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
                  sector_count = MBRSwitched.ReservedSelector + MBRSwitched.SectorsPerFat * MBRSwitched.NumFATS + MBRSwitched.BootPathStartCluster - 2 + (NextBlockNumber - 2) * 8;
                  PreviousBlockNumber = NextBlockNumber;
                  //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: NextBlockNumber: %llu\n",  __LINE__, NextBlockNumber);
             }
             
            break;
        }

    }

    return EFI_SUCCESS;
}


STATE_TRANS StatusTransitionTable[] =
{
    { INIT_STATE,                READ_PATITION_EVENT,   GET_PARTITION_INFO_STATE, L2_STORE_PartitionAnalysisFSM},
    { GET_PARTITION_INFO_STATE,  READ_ROOT_PATH_EVENT,  GET_ROOT_PATH_INFO_STATE, L2_STORE_RootPathAnalysisFSM},
    { GET_ROOT_PATH_INFO_STATE,  READ_FAT_TABLE_EVENT,  GET_FAT_TABLE_STATE,      L2_STORE_GetFatTableFSM},
    { GET_FAT_TABLE_STATE,       READ_FILE_EVENT,       READ_FILE_STATE,          L2_STORE_ReadFileFSM},
    { READ_FILE_STATE,           READ_FILE_EVENT,       READ_FILE_STATE,          L2_STORE_ReadFileFSM },
};


int L2_STORE_FileRead(EVENT event)
{
    EFI_STATUS Status;
    
    if (FileReadCount > FileLength / (512 * 8))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
        return;
    }
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FileReadFSM current event: %d, NextState: %d table event:%d table NextState: %d\n", 
                              __LINE__, 
                            event, 
                            NextState,
                            StatusTransitionTable[NextState].event,
                            StatusTransitionTable[NextState].NextState);
    
    if ( event == StatusTransitionTable[NextState].event )
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
        StatusTransitionTable[NextState].pFunc();
        NextState = StatusTransitionTable[NextState].NextState;
    }
    else  
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
        NextState = INIT_STATE;
    }

    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
    StatusErrorCount++;
}


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
    NextState = INIT_STATE;
    READ_FILE_FSM_Event = 0;

    for (int i = 0; i < 5; i++)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: i: %d \n", __LINE__, i);
        //DEBUG ((EFI_D_INFO, "%d HandleEnterPressed FSM_Event: %d\n", __LINE__, READ_FILE_FSM_Event));
        L2_STORE_FileRead(READ_FILE_FSM_Event++);

        if (READ_FILE_EVENT <= READ_FILE_FSM_Event)
            READ_FILE_FSM_Event = READ_FILE_EVENT;
    }

    
}


UINT8 PreviousItem = -1;

// Display mouse move over's a partition's root path items.
EFI_STATUS L3_PARTITION_RootPathAccess()
{	
	EFI_STATUS Status;
	UINT8 Buffer1[512];
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
	
    Color.Red = 0xff;
    Color.Green= 0x00;
    Color.Blue= 0x00;

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
            
            L2_GRAPHICS_RectangleDraw(pMouseSelectedBuffer, 0,  0, 31, 15, 1,  Color, 32);
            L2_STORE_PartitionItemsPrint(i);
            PreviousItem = i;
            L2_GRAPHICS_Copy(pDeskDisplayBuffer, pMouseSelectedBuffer, ScreenWidth, ScreenHeight, 32, 16, MyComputerPositionX + 50, MyComputerPositionY  + i * (16 + 2) + 16 * 2);   
        }
    }
	/*
	Status = L1_STORE_READ(i, sector_count, 1, Buffer1); 
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

EFI_STATUS L3_PARTITION_FileAccess(UINT16 DeviceID)
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L3_PARTITION_FileAccess\n", __LINE__);
	UINT8 Buffer1[512];
	
	EFI_STATUS Status = L1_STORE_READ(DeviceID, sector_count, 1, Buffer1 );  
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
        return Status;
    }
}


EFI_STATUS L3_PARTITION_SubPathAccess()
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L3_PARTITION_SubPathAccess\n", __LINE__);
}


EFI_STATUS L3_PARTITION_AccessFinish()
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L3_PARTITION_AccessFinish\n", __LINE__);
		
	PARTITION_ITEM_ACCESS_STATE PartitionItemAccessNextState = INIT_ACCESS_STATE;
	PARTITION_ITEM_ACCESS_EVENT PartitionItemAccessEvent = ROOT_PATH_ACCESS_EVENT;
}

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


VOID L3_PARTITION_Access()
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

