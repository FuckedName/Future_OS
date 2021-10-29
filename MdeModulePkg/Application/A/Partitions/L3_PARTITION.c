#include <L3_PARTITION.h>

#include <Graphics/L2_GRAPHICS.h>
#include <Devices/Store/L2_DEVICE_Store.h>
#include <Global/Global.h>


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

