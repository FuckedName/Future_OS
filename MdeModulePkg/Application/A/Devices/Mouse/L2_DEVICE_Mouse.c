#include <Library/BaseLib.h>

#include <Guid/ConsoleInDevice.h>
#include <Guid/ConsoleOutDevice.h>
#include <Guid/FileSystemVolumeLabelInfo.h>
#include <Guid/GlobalVariable.h>
#include <Guid/HiiBootMaintenanceFormset.h>
#include <Guid/MdeModuleHii.h>
#include <Guid/ShellLibHiiGuid.h>
#include <Guid/TtyTerm.h>
#include <IndustryStandard/Pci.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/FileExplorerLib.h>
#include <Library/FileHandleLib.h>
#include <Library/HandleParsingLib.h>
#include <Library/HiiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/PeCoffGetEntryPointLib.h>
#include <Library/PrintLib.h>
#include <Library/ShellCEntryLib.h> 
#include <Library/ShellCommandLib.h>
#include <Library/ShellLib.h>
#include <Library/SortLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootManagerLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Pi/PiFirmwareFile.h>
#include <Pi/PiFirmwareVolume.h>
#include <Protocol/AbsolutePointer.h>
#include <Protocol/BusSpecificDriverOverride.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DevicePathToText.h>
#include <Protocol/DriverDiagnostics2.h>
#include <Protocol/DriverDiagnostics.h>
#include <Protocol/DriverFamilyOverride.h>
#include <Protocol/DriverHealth.h>
#include <Protocol/DriverSupportedEfiVersion.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/FormBrowserEx2.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/LoadFile.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PlatformDriverOverride.h>
#include <Protocol/PlatformToDriverConfiguration.h>
#include <Protocol/SerialIo.h>
#include <Protocol/Shell.h>
#include <Protocol/ShellParameters.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/SimplePointer.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/UnicodeCollation.h>
#include <Protocol/DiskIo.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DiskIo2.h>
#include <Protocol/BlockIo2.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Global/Global.h>

#include <L2_DEVICE_Mouse.h>

#include <Graphics/L2_GRAPHICS.h>

// for mouse move & click
VOID
EFIAPI
L2_MOUSE_Event (IN EFI_EVENT Event, IN VOID *Context)
{
    mouse_count++;
    //L2_DEBUG_Print1(DISPLAY_X, DISPLAY_Y, "%d: HandleMouseEvent\n", __LINE__);
    EFI_STATUS Status;
    UINTN Index;
    EFI_SIMPLE_POINTER_STATE State;

    Status = gBS->CheckEvent(gMouse->WaitForInput); 
    if (Status == EFI_NOT_READY)
    {
        //return ;
    }
    
    Status = gMouse->GetState(gMouse, &State);
    if (Status == EFI_DEVICE_ERROR)
    {
        return ;
    }
    if (0 == State.RelativeMovementX && 0 == State.RelativeMovementY && 0 == State.LeftButton && 0 == State.RightButton)
    {
        return;
    }
    
    //X
    if (State.RelativeMovementX < 0)
    {
        int temp = State.RelativeMovementX >> 16;
        x_move = (0xFFFF - temp) & 0xff;  
        x_move = - x_move;
    }
    else if(State.RelativeMovementX > 0)
    {
        x_move = (State.RelativeMovementX >> 16) & 0xff;
    }

    //Y
    if (State.RelativeMovementY < 0)
    {       
        int temp = State.RelativeMovementY >> 16;
        y_move = (0xFFFF - temp) & 0xff;  
        y_move = - y_move;
    }
    else if(State.RelativeMovementY > 0)
    {
        y_move = (State.RelativeMovementY >> 16) & 0xff;
    }

    ////DEBUG ((EFI_D_INFO, "X: %X, Y: %X ", x_move, y_move));
    L2_DEBUG_Print1(0, ScreenHeight - 30 -  8 * 16, "%d: X: %04d, Y: %04d move X: %X move Y: %X", __LINE__, iMouseX, iMouseY, x_move, y_move );
    
    //L2_DEBUG_Print1(0, ScreenHeight - 30 -  8 * 16, "%d: X move: %d Y move: %d left: %d right: %d", __LINE__, State.RelativeMovementX, State.RelativeMovementY, State.LeftButton, State.RightButton);
    iMouseX = iMouseX + x_move * 3;
    iMouseY = iMouseY + y_move * 3; 
    

    if (iMouseX < 0)
        iMouseX = 0;
        
    if (iMouseX > ScreenWidth)
        iMouseX = ScreenWidth;

    if (iMouseY < 0)
        iMouseY = 0;

    if (iMouseY > ScreenHeight)
        iMouseY = ScreenHeight;

    //Button
    if (State.LeftButton == MOUSE_LEFT_CLICKED)
    {
        //DEBUG ((EFI_D_INFO, "Left button clicked\n"));
        
        L2_GRAPHICS_RightClickMenu(iMouseX, iMouseY);
        
        MouseClickFlag = (MouseClickFlag == MOUSE_NO_CLICKED) ? MOUSE_LEFT_CLICKED : MOUSE_NO_CLICKED;
        
        //DrawAsciiCharUseBuffer(GraphicsOutput, 20 + process2_i * 8 + 16, 60, 'E', Color);  
    }
    
    if (State.RightButton == 0x01)
    {
        //DEBUG ((EFI_D_INFO, "Right button clicked\n"));
        //DrawAsciiCharUseBuffer(GraphicsOutput, 20 + process2_i * 8 + 16, 60, 'R', Color);

        L2_GRAPHICS_RightClickMenu(iMouseX, iMouseY);
        
        MouseClickFlag = 2;
    }
    //L2_DEBUG_Print1(DISPLAY_X, DISPLAY_Y, "%d: HandleMouseEvent\n", __LINE__);
    ////DEBUG ((EFI_D_INFO, "\n"));
    L2_MOUSE_Move();
    
    gBS->WaitForEvent( 1, &gMouse->WaitForInput, &Index );
}



EFI_STATUS L2_DEVICE_MouseInit()
{
    EFI_STATUS                         Status;
    EFI_HANDLE                         *PointerHandleBuffer = NULL;
    UINTN                              i = 0;
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

