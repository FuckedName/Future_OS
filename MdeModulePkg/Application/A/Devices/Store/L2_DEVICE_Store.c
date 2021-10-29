

#include <Protocol/DiskIo.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DiskIo2.h>
#include <Protocol/BlockIo2.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Graphics/L2_GRAPHICS.h>
#include <Global/Global.h>


#include "L2_DEVICE_Store.h"

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



