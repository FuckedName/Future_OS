#include <L1_DEVICE_Mouse.h>

EFI_SIMPLE_POINTER_PROTOCOL  *gMouse;

EFI_STATUS L2_MOUSE_Init()
{
    EFI_STATUS                         Status;
    EFI_HANDLE                         *PointerHandleBuffer = NULL;
    UINTN                              i = 0;
	MouseClickFlag 					   = MOUSE_NO_CLICKED;
    UINTN                              HandleCount = 0;
    
    //get the handles which supports
    Status = gBS->LocateHandleBuffer(
                                    ByProtocol,
                                    &gEfiSimplePointerProtocolGuid,
                                    NULL,
                                    &HandleCount,
                                    &PointerHandleBuffer
                                    );
        
    if (EFI_ERROR(Status))  return Status;      //unsupport
    
    for (i = 0; i < HandleCount; i++)
    {
        Status = gBS->HandleProtocol(PointerHandleBuffer[i],
                                    &gEfiSimplePointerProtocolGuid,
                                    (VOID**)&gMouse);
            
        if (EFI_ERROR(Status))  
            continue;
        
        else
        {
            return EFI_SUCCESS;
        }
    }   
    
    return EFI_SUCCESS;

}


