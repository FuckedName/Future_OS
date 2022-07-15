#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>

#include <Library/UefiRuntimeLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/LoadedImage.h>
#include <Protocol/Shell.h>



EFI_STATUS EFIAPI UefiMain (IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
    Print(L"Hello Renqihong jiayou!!!!\r\n");

    EFI_STATUS Status;

	//INT32 i = 1000000;
	
    //while(i-- > 0);
	
	EFI_SHELL_PROTOCOL	  *EfiShellProtocol;

    //初始化协议
    Status = gBS->LocateProtocol (&gEfiShellProtocolGuid,
                                NULL,
                                (VOID **)&EfiShellProtocol);
	    
    if (EFI_ERROR (Status)) 
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_ApplicationRun Status: %d\n", __LINE__, Status);
        return EFI_SUCCESS; 
    }
    return EFI_SUCCESS;
}
