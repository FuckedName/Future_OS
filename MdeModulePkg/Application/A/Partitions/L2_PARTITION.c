
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





#include <Library/MemoryAllocationLib.h>

#include <Protocol/DevicePath.h>
#include <Protocol/DevicePathToText.h>
#include <Partitions/NTFS/L2_PARTITION_NTFS.h>
#include <Partitions/FAT32/L2_PARTITION_FAT32.h>

#include "L2_PARTITION.h"

#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/DevicePathToText.h>

#include <Graphics/L2_GRAPHICS.h>

//#include <Library/Math/L2_LIBRARY_Math.h>
//#include <Device/Store/L2_DEVICE_Store.h>
//#include <FAT32/L2_PARTITION_FAT32.h>
//#include <NTFS/L2_PARTITION_NTFS.h>

UINT32 BlockSize = 0;


//��������ϵͳ������������C��D��E��U�̵ȵȣ�ע�⣬һ��Ӳ�̻�U�̿��Էֳɶ������
DEVICE_PARAMETER device[PARTITION_COUNT] = {0};
UINT64 FileBlockStart = 0;
DollarBootSwitched NTFSBootSwitched;

//���嵱ǰ����ϵͳ��ȡϵͳ�ļ����ڵķ������ƣ�ϵͳ�ļ�����������ͼƬ��ͼ�꣬HZK16�ļ��ȵ���Щ
UINT8 EFI_FILE_STORE_PATH_PARTITION_NAME[50] = "OS";

UINT8 Buffer1[DISK_BUFFER_SIZE];




//ͨ���ļ�ϵͳ�ļ�����Ŀ¼
//��Ϊ��ǰϵͳ֧��NTFS��FAT32��ʵ���Ϻ�߿��ܻ�֧���������͵��ļ�ϵͳ����ͬ�ļ�ϵͳ��
//��ȡ�������ļ��������ļ�������С�����͵ȵȣ����Գ������һ�����������ݽṹ��
COMMON_STORAGE_ITEM pCommonStorageItems[100];


// all partitions analysis



/****************************************************************************
*
*  ����:   ��ȡ���������̡�U�̵ȵȣ��ļ�ϵͳ���ͣ�NTFS/FAT32�ȵ�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_FILE_PartitionTypeAnalysis(UINT16 DeviceID)
{    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d DeviceID: %d\n", __LINE__, DeviceID);
    EFI_STATUS Status;

    sector_count = 0;

    L1_MEMORY_SetValue(Buffer1, 0, DISK_BUFFER_SIZE);

    Status = L2_STORE_Read(DeviceID, 0, 1, Buffer1 );  
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
        device[DeviceID].StartSectorNumber = device[DeviceID].stMBRSwitched.ReservedSelector + device[DeviceID].stMBRSwitched.SectorsPerFat * device[DeviceID].stMBRSwitched.FATCount;
        sector_count = device[DeviceID].StartSectorNumber + (device[DeviceID].stMBRSwitched.BootPathStartCluster - 2) * 8;
        device[DeviceID].FileSystemType = FILE_SYSTEM_FAT32;
        BlockSize = device[DeviceID].stMBRSwitched.SectorOfCluster * 512; 
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: sector_count:%ld BlockSize: %d StartSectorNumber: %llu FileSystemType: %d\n",  __LINE__, sector_count, BlockSize, device[DeviceID].StartSectorNumber, device[DeviceID].FileSystemType);


        
        return FILE_SYSTEM_FAT32;
    }
    // NTFS
    else if (Buffer1[3] == 'N' && Buffer1[4] == 'T' && Buffer1[5] == 'F' && Buffer1[6] == 'S')
    {
        L2_FILE_NTFS_FirstSelectorAnalysis(Buffer1, &NTFSBootSwitched);
        device[DeviceID].StartSectorNumber = NTFSBootSwitched.MFT_StartCluster * 8;
        device[DeviceID].FileSystemType = FILE_SYSTEM_NTFS;
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: sector_count:%ld BlockSize: %d StartSectorNumber: %llu FileSystemType: %d\n",  __LINE__, sector_count, BlockSize, device[DeviceID].StartSectorNumber, device[DeviceID].FileSystemType);
        return FILE_SYSTEM_NTFS;
    }
    // the other file system can add at this place
    else
    {
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: \n",  __LINE__);
        return FILE_SYSTEM_MAX;
    }                   

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
EFI_STATUS L2_FILE_PartitionNameAnalysis(UINT16 DeviceID, UINT8 *pBuffer)
{
    switch (device[DeviceID].FileSystemType)
    {
        case FILE_SYSTEM_FAT32:
            break;
    }
}

typedef struct
{
    FILE_SYSTEM_TYPE FileSystemType;
    UINT64          (*pFunctionStartSectorNumberGet)(UINT16); 
    EFI_STATUS    (*pFunctionBufferAnalysis)(UINT16, UINT8 *); 
}PARTITION_NAME_GET;




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
UINT64 L2_PARTITION_NameFAT32StartSectorNumberGet(UINT16 DeviceID)
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d DeviceID: %d StartSectorNumber: %d\n", __LINE__, DeviceID, device[DeviceID].StartSectorNumber);
    // Start with 2 cluster
    return device[DeviceID].StartSectorNumber;
}




/****************************************************************************
*
*  ����:   ��ȡ������ڵ�MFT,��������Ԫ�����ļ���MFT_ITEM_DOLLAR_VOLUME��
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
UINT64 L2_PARTITION_NameNTFSStartSectorNumberGet(UINT16 DeviceID)
{    // Start with $volume
    return device[DeviceID].StartSectorNumber + 2 * MFT_ITEM_DOLLAR_VOLUME;
}





/****************************************************************************
*
*  ����:   
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_PARTITION_NameFAT32Analysis(UINT16 DeviceID, UINT8 *Buffer)
{
    L1_MEMORY_SetValue(device[DeviceID].PartitionName, 0, PARTITION_NAME_LENGTH);

    //�ڶ�������ǰ�����ַ�����FAT32���������֣����ҵ�һ���ؾ��ǵڶ����أ���Ϊû��0��1��
    for (UINT16 i = 0; i < 6; i++)
        device[DeviceID].PartitionName[i] = Buffer[i];
    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d %X %X %X %X\n", __LINE__, device[DeviceID].PartitionName[0], device[DeviceID].PartitionName[1], device[DeviceID].PartitionName[2], device[DeviceID].PartitionName[3]);

    device[DeviceID].PartitionName[6] = '\0'; //?��???BUG��???��????6??3��??
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
EFI_STATUS L2_PARTITION_NameNTFSAnalysis(UINT16 DeviceID, UINT8 *Buffer)
{
    NTFS_FILE_SWITCHED NTFSFileSwitched = {0};
    L1_MEMORY_SetValue(device[DeviceID].PartitionName, 0, PARTITION_NAME_LENGTH);


    //��ǰ���ԣ�ֻ��ʾһ���豸����ʾ����豸���Ի�Ƚ��鷳
    //if (3 == DeviceID)
        L2_PARTITION_FileContentPrint(Buffer);
    
    L2_FILE_NTFS_FileItemBufferAnalysis(Buffer, &NTFSFileSwitched);

    UINT8 j;

    //һ��FILE����кܶ�����ԣ�����VOLUME��������6�����ң�������Щ���Բ�һ���У���������ȡС�ڵ���5����һ���ķ���
    for (UINT8 i = 0; i < 5; i++)
    {
        if (NTFSFileSwitched.NTFSFileAttributeHeaderSwitched[i].Type == MFT_ATTRIBUTE_DOLLAR_VOLUME_NAME)
        {
            for (j = 0; NTFSFileSwitched.NTFSFileAttributeHeaderSwitched[i].Data[j] != '\0'; j++)                
                device[DeviceID].PartitionName[j] = NTFSFileSwitched.NTFSFileAttributeHeaderSwitched[i].Data[j];

            break;
        }
    }
    device[DeviceID].PartitionName[j] = '\0';
}




/****************************************************************************
*
*  ����: ��һ���ǲ�ͬ���ļ�ϵͳ���ͣ��ڶ�������Ҫ��ȡ�����ţ��������ǽ����Ӵ洢�������ڴ档
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
PARTITION_NAME_GET PartitionNameGet[]=
{
    {FILE_SYSTEM_MIN, NULL, NULL},
    {FILE_SYSTEM_FAT32, L2_PARTITION_NameFAT32StartSectorNumberGet,  L2_PARTITION_NameFAT32Analysis},  
    {FILE_SYSTEM_NTFS,  L2_PARTITION_NameNTFSStartSectorNumberGet,   L2_PARTITION_NameNTFSAnalysis}
};




/****************************************************************************
*
*  ����:   ��ȡ���������֣������WINDOWS����ϵͳ����������C�̡�D�̵ȵ��̵����ƣ�����������������Ϸ����ȵȣ���ǰ�ݲ�֧�����ļ���
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_FILE_PartitionNameGet(UINT16 DeviceID)
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d DeviceID: %X\n", __LINE__, DeviceID);
    UINT8 Buffer[DISK_BUFFER_SIZE * 2] = {0};
    UINT64 StartSectorNumber = 0;
    EFI_STATUS Status;
    UINT16 FileSystemType;
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d FileSystemType: %llu\n", __LINE__, FileSystemType);

    
    FileSystemType = device[DeviceID].FileSystemType;
    if (FileSystemType == FILE_SYSTEM_MIN || FileSystemType == FILE_SYSTEM_MAX)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d FileSystemType: %X FileSystemType == FILE_SYSTEM_MIN || FileSystemType == FILE_SYSTEM_MAX\n", __LINE__, FileSystemType);
        return;
    }

    StartSectorNumber = PartitionNameGet[FileSystemType].pFunctionStartSectorNumberGet(DeviceID);

    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d DeviceID: %d FileSystemType: %d, StartSectorNumber: %llu\n", __LINE__, DeviceID,FileSystemType, StartSectorNumber);

    //һ��Ԫ�����Ƕ�ȡ��������
    Status = L2_STORE_Read(DeviceID, StartSectorNumber, 2, Buffer);  
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
        return Status;
    }

    PartitionNameGet[FileSystemType].pFunctionBufferAnalysis(DeviceID, Buffer);
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
EFI_STATUS L2_STORE_PartitionParameterInitial()
{
    for (UINT16 i = 0; i < PARTITION_COUNT; i++)
    {
        device[i].pFAT_TableBuffer = NULL;
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
EFI_STATUS L2_STORE_PartitionAnalysis()
{    
    L2_STORE_PartitionsParameterGet();
}
