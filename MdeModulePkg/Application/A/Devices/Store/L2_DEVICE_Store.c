

#include <Protocol/DiskIo.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DiskIo2.h>
#include <Protocol/BlockIo2.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/DevicePathToText.h>

#include <Graphics/L2_GRAPHICS.h>
#include <Libraries/Math/L1_LIBRARY_Math.h>
#include <L2_DEVICE_Store.h>
#include <L2_GRAPHICS.h>


//Note: ReadSize will be multi with 512
EFI_STATUS L1_STORE_READ(UINT8 deviceID, UINT64 StartSectorNumber, UINT16 ReadSize, UINT8 *pBuffer)
{
    if (StartSectorNumber > device[deviceID].SectorCount)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: deviceID: %d StartSectorNumber: %ld ReadSize: %d\n", __LINE__, deviceID, StartSectorNumber, ReadSize);
        return -1;
    }

    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: deviceID: %d StartSectorNumber: %ld ReadSize: %d\n", __LINE__, deviceID, StartSectorNumber, ReadSize);
    EFI_STATUS Status;
    UINTN NumHandles;
    EFI_BLOCK_IO_PROTOCOL *BlockIo;
    EFI_DISK_IO_PROTOCOL *DiskIo;
    EFI_HANDLE *ControllerHandle = NULL;
    
    Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiDiskIoProtocolGuid, NULL, &NumHandles, &ControllerHandle);
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);      
        return Status;
    }

    Status = gBS->HandleProtocol(ControllerHandle[deviceID], &gEfiBlockIoProtocolGuid, (VOID * *) &BlockIo );   
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
        return Status;
    }
    
    Status = gBS->HandleProtocol( ControllerHandle[deviceID], &gEfiDiskIoProtocolGuid, (VOID * *) &DiskIo );     
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
        return Status;
    }

    Status = DiskIo->ReadDisk(DiskIo, BlockIo->Media->MediaId, DISK_BUFFER_SIZE * (StartSectorNumber), DISK_BUFFER_SIZE * ReadSize, pBuffer);
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X sector_count:%ld\n", __LINE__, Status, sector_count);
        return Status;
    }
    
    for (int j = 0; j < 250; j++)
    {
        //L2_DEBUG_Print1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", pBuffer[j] & 0xff);
    }
    //INFO_SELF("\n");
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
    return EFI_SUCCESS;
}


//Note: ReadSize will be multi with 512
EFI_STATUS L1_STORE_Write(UINT8 deviceID, UINT64 StartSectorNumber, UINT16 WriteSize, UINT8 *pBuffer)
{
    if (StartSectorNumber > device[deviceID].SectorCount)
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: deviceID: %d StartSectorNumber: %ld ReadSize: %d\n", __LINE__, deviceID, StartSectorNumber, WriteSize);
        return -1;
    }

    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: deviceID: %d StartSectorNumber: %ld ReadSize: %d\n", __LINE__, deviceID, StartSectorNumber, ReadSize);
    EFI_STATUS Status;
    UINTN NumHandles;
    EFI_BLOCK_IO_PROTOCOL *BlockIo;
    EFI_DISK_IO_PROTOCOL *DiskIo;
    EFI_HANDLE *ControllerHandle = NULL;
    
    Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiDiskIoProtocolGuid, NULL, &NumHandles, &ControllerHandle);
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);      
        return Status;
    }

    Status = gBS->HandleProtocol(ControllerHandle[deviceID], &gEfiBlockIoProtocolGuid, (VOID * *) &BlockIo );   
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
        return Status;
    }
    
    Status = gBS->HandleProtocol( ControllerHandle[deviceID], &gEfiDiskIoProtocolGuid, (VOID * *) &DiskIo );     
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
        return Status;
    }

    Status = DiskIo->WriteDisk(DiskIo, BlockIo->Media->MediaId, DISK_BUFFER_SIZE * (StartSectorNumber), DISK_BUFFER_SIZE * WriteSize, pBuffer);
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X sector_count:%ld\n", __LINE__, Status, sector_count);
        return Status;
    }
    
    for (int j = 0; j < 250; j++)
    {
        //L2_DEBUG_Print1(DISK_READ_BUFFER_X + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", pBuffer[j] & 0xff);
    }
    //INFO_SELF("\n");
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
    return EFI_SUCCESS;
}


// all partitions analysis
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

