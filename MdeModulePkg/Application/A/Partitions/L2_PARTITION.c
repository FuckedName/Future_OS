
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





#include <Library/MemoryAllocationLib.h>

#include <Protocol/DevicePath.h>
#include <Protocol/DevicePathToText.h>
#include <L2_PARTITION.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/DevicePathToText.h>

#include <Graphics/L2_GRAPHICS.h>

//#include <Library/Math/L2_LIBRARY_Math.h>
//#include <Device/Store/L2_DEVICE_Store.h>
//#include <FAT32/L2_PARTITION_FAT32.h>
//#include <NTFS/L2_PARTITION_NTFS.h>

UINT32 BlockSize = 0;
DEVICE_PARAMETER device[10] = {0};
UINT32 FileBlockStart = 0;
MasterBootRecordSwitched MBRSwitched;
DollarBootSwitched NTFSBootSwitched;

// all partitions analysis



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
EFI_STATUS L2_FILE_PartitionTypeAnalysis(UINT16 DeviceID)
{    
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d DeviceID: %d\n", __LINE__, DeviceID);
    EFI_STATUS Status;
    UINT8 Buffer1[DISK_BUFFER_SIZE] = {0};

    sector_count = 0;

    Status = L1_STORE_READ(DeviceID, sector_count, 1, Buffer1 );    
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %X\n", __LINE__, Status);
        return Status;
    }

    // FAT32 file system
    if (Buffer1[0x52] == 'F' && Buffer1[0x53] == 'A' && Buffer1[0x54] == 'T' && Buffer1[0x55] == '3' && Buffer1[0x56] == '2')   
    {                   
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: FAT32\n",  __LINE__);
        // analysis data area of patition
        L1_FILE_FAT32_DataSectorAnalysis(Buffer1, &MBRSwitched); 

        // data sector number start include: reserved selector, fat sectors(usually is 2: fat1 and fat2), and file system boot path start cluster(usually is 2, data block start number is 2)
        sector_count = MBRSwitched.ReservedSelector + MBRSwitched.SectorsPerFat * MBRSwitched.NumFATS + (MBRSwitched.BootPathStartCluster - 2) * 8;
		device[DeviceID].StartSectorNumber = MBRSwitched.ReservedSelector + MBRSwitched.SectorsPerFat * MBRSwitched.NumFATS;
		device[DeviceID].FileSystemType = FILE_SYSTEM_FAT32;
        BlockSize = MBRSwitched.SectorOfCluster * 512;
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: sector_count:%ld BlockSize: %d\n",  __LINE__, sector_count, BlockSize);
        return FILE_SYSTEM_FAT32;
    }
    // NTFS
    else if (Buffer1[3] == 'N' && Buffer1[4] == 'T' && Buffer1[5] == 'F' && Buffer1[6] == 'S')
    {
        L2_FILE_NTFS_FirstSelectorAnalysis(Buffer1, &NTFSBootSwitched);
		device[DeviceID].StartSectorNumber = NTFSBootSwitched.MFT_StartCluster * 8;
		device[DeviceID].FileSystemType = FILE_SYSTEM_NTFS;
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: NTFS sector_count:%llu\n",  __LINE__, sector_count);
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
*  描述:   xxxxx
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
    UINT64    	  (*pFunctionStartSectorNumberGet)(UINT16); 
    EFI_STATUS    (*pFunctionBufferAnalysis)(UINT16, UINT8 *); 
}PARTITION_NAME_GET;




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
UINT64 L2_PARTITION_NameFAT32StartSectorNumberGet(UINT16 DeviceID)
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d DeviceID: %d StartSectorNumber: %d\n", __LINE__, DeviceID, device[DeviceID].StartSectorNumber);
	// Start with 2 cluster
	return device[DeviceID].StartSectorNumber;
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
UINT64 L2_PARTITION_NameNTFSStartSectorNumberGet(UINT16 DeviceID)
{	// Start with $volume
	return device[DeviceID].StartSectorNumber + 2 * MFT_ITEM_DOLLAR_VOLUME;
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
EFI_STATUS L2_PARTITION_NameFAT32Analysis(UINT16 DeviceID, UINT8 *Buffer)
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d %X %X %X %X\n", __LINE__, device[DeviceID].PartitionName[0], device[DeviceID].PartitionName[1], device[DeviceID].PartitionName[2], device[DeviceID].PartitionName[3]);

	for (UINT16 i = 0; i < 6; i++)
		device[DeviceID].PartitionName[i] = Buffer[i];
	device[DeviceID].PartitionName[6] = '\0';
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
EFI_STATUS L2_PARTITION_NameNTFSAnalysis(UINT16 DeviceID, UINT8 *Buffer)
{
	device[DeviceID].PartitionName[6] = '\0';
}


PARTITION_NAME_GET PartitionNameGet[]=
{
	{FILE_SYSTEM_FAT32, L2_PARTITION_NameFAT32StartSectorNumberGet,  L2_PARTITION_NameFAT32Analysis},  
	{FILE_SYSTEM_NTFS,  L2_PARTITION_NameNTFSStartSectorNumberGet,   L2_PARTITION_NameNTFSAnalysis}
};




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
EFI_STATUS L2_FILE_PartitionNameGet(UINT16 DeviceID)
{
	//L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d DeviceID: %X\n", __LINE__, DeviceID);
	UINT8 Buffer[DISK_BUFFER_SIZE] = {0};
	UINT64 StartSectorNumber = 0;
	EFI_STATUS Status;
	UINT16 FileSystemType = device[DeviceID].FileSystemType;
	//L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d FileSystemType: %llu\n", __LINE__, FileSystemType);

	StartSectorNumber = PartitionNameGet[FileSystemType].pFunctionStartSectorNumberGet(DeviceID);

	//L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d StartSectorNumber: %llu\n", __LINE__, StartSectorNumber);
	
	Status = L1_STORE_READ(DeviceID, StartSectorNumber, 1, Buffer);  
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
*  描述:   xxxxx
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
void L2_STORE_TextDevicePathAnalysis(CHAR16 *p, DEVICE_PARAMETER *device, UINTN count1)
{
    int length = StrLen(p);
    int i = 0;
    //L2_DEBUG_Print1(0, 11 * 16, "line: %d string: %s, length: %d\n", __LINE__, p, length);
    for (i = 0; i < length - 3; i++)
    { 
        //USB
        if (p[i] == 'U' && p[i + 1] == 'S' && p[i + 2] == 'B')
        {
            device->DeviceType = 1;
            break;
        }
            
        //Sata
        if (p[i] == 'S' && p[i + 1] == 'a' && p[i + 2] == 't' && p[i + 3] == 'a')
        {
            device->DeviceType = 2;
            break;
        }
        
        //NVMe
        if (p[i] == 'N' && p[i + 1] == 'V' && p[i + 2] == 'M' && p[i + 3] == 'e')
        {
            device->DeviceType = 2;
            break;
        }
    }

    //printf("line:%d i: %d\n", __LINE__, i);
    
    //HD
    while (i++)
        if (p[i] == 'H' && p[i + 1] == 'D')
            break;
            
    //printf("line:%d i: %d\n", __LINE__, i);
    if (i >= length)
        return;
                
    //printf("line:%d i: %d\n", __LINE__, i);
    device->PartitionID = p[i + 3] - '0';
    
    i+=3;
    
    while (i++)
    {
        //MBR
        if (p[i] == 'M' && p[i + 1] == 'B' && p[i + 2] == 'R')
        {
            device->PartitionType = 1;
            break;
        }
            
        //GPT
        if (p[i] == 'G' && p[i + 1] == 'P' && p[i + 2] == 'T')
        {
            device->PartitionType = 2;    
            break;
        }
    }
        
    i+=3;
    
    //printf("line:%d i: %d\n", __LINE__, i);
    int commalocation[2];
    int count = 0;
    for (int j = i + 1; p[j] != ')'; j++)
    {
        if (p[j] == ',')
        {
           commalocation [count++] = j;
           //printf("line:%d, j: %d, data: %c%c\n", __LINE__, j, p[j], p[j+1]);
        }
    }
    
    char temp[20];
    int j = 0;
    //start
    for (i = commalocation[0] + 1; i < commalocation[1]; i++)
    {
        temp[j++] = p[i];
        //printf("%c", p[i]);
    }
    temp[j] = '\0';
    //strtol(const char * nptr,char * * endptr,int base)
    //puts(temp);
    //DEBUG ((EFI_D_INFO, "line:%d device->StartSectorNumber: %a \n", __LINE__, temp));
    //printf("line:%d start: %d\n", __LINE__, strtol(temp));
    device->StartSectorNumber = L1_MATH_StringToLongLong(temp, NULL, 10);
    //putchar('\n');
    
    j = 0;
    //length
    for (i = commalocation[1] + 1; i < length - 1; i++)
    {
        temp[j++] = p[i];
        //printf("%c", p[i]);
    }
    temp[j] = '\0';
    //puts(temp);
    //printf("line:%d end: %d\n", __LINE__, strtol(temp));
    device->SectorCount = L1_MATH_StringToLongLong(temp, NULL, 10);;
    //DEBUG ((EFI_D_INFO, "line:%d device->SectorCount: %a \n", __LINE__, temp));
    //putchar('\n');
    
    //L2_DEBUG_Print1(0, 11 * 16, "line: %d string: %s, length: %d\n",       __LINE__, p, strlen(p));

    // second display
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: i: %d Start: %d Count: %d  DeviceType: %d PartitionType: %d PartitionID: %d\n",        
                                __LINE__, 
                                count1,
                                device->StartSectorNumber,
                                device->SectorCount,
                                device->DeviceType,
                                device->PartitionType,
                                device->PartitionID);
                                
    //DEBUG ((EFI_D_INFO, "line:%d device->DeviceType: %d \n", __LINE__, device->DeviceType));
    //DEBUG ((EFI_D_INFO, "line:%d device->PartitionType: %d \n", __LINE__, device->PartitionType));
    //DEBUG ((EFI_D_INFO, "line:%d device->PartitionID: %d \n", __LINE__, device->PartitionID));
    //DEBUG ((EFI_D_INFO, "line:%d device->StartSectorNumber: %d \n", __LINE__, device->StartSectorNumber));
    //DEBUG ((EFI_D_INFO, "line:%d device->SectorCount: %d \n", __LINE__, device->SectorCount));
    //putchar('\n');
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
EFI_STATUS L2_STORE_PartitionAnalysis()
{    
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d PartitionAnalysisFSM\n", __LINE__);
    EFI_STATUS Status ;
    UINTN i;
    EFI_HANDLE *ControllerHandle = NULL;
    EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;
    
    Status = gBS->LocateProtocol (&gEfiDevicePathToTextProtocolGuid, NULL, (VOID **) &DevPathToText);
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
    if (EFI_ERROR(Status))
    {
         //DEBUG ((EFI_D_INFO, "LocateProtocol1 error: %x\n", Status));
         L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
        return Status;
    }
   
    Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiDiskIoProtocolGuid, NULL, &PartitionCount, &ControllerHandle);
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);      
        return EFI_SUCCESS;
    }

    for (i = 0; i < PartitionCount; i++)
    {
        //L2_DEBUG_Print1(350, 16 * 6, "%d: %x\n", __LINE__, Status);
        EFI_DEVICE_PATH_PROTOCOL *DiskDevicePath;
        Status = gBS->OpenProtocol(ControllerHandle[i],
                                   &gEfiDevicePathProtocolGuid,
                                   (VOID **)&DiskDevicePath,
                                   gImageHandle, 
                                   NULL,
                                   EFI_OPEN_PROTOCOL_GET_PROTOCOL);
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
        if (EFI_ERROR(Status))
        {
            L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
            return Status;
        }

        CHAR16 *TextDevicePath = DevPathToText->ConvertDevicePathToText(DiskDevicePath, TRUE, TRUE);
         L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X Partition: %s\n", __LINE__, Status, TextDevicePath);
         L2_STORE_TextDevicePathAnalysis(TextDevicePath, &device[i], i);
                    
        if (TextDevicePath) gBS->FreePool(TextDevicePath);       
    }
    return EFI_SUCCESS;
}

