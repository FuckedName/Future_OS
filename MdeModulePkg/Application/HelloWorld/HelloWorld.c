#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiRuntimeLib.h>
#include <Protocol/LoadedImage.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/Shell.h>

typedef enum
{
    APPLICATION_CALL_ID_INIT = 0,
    APPLICATION_CALL_ID_SHUTDOWN,
    APPLICATION_CALL_ID_PRINT_STRING,
    APPLICATION_CALL_ID_MAX
}APPLICATION_CALL_ID;

typedef struct
{
    APPLICATION_CALL_ID ID;
    UINT8 pApplicationCallInput[20];
    UINT8 (*pCallbackFunction)();
}APPLICATION_CALL_DATA;

APPLICATION_CALL_DATA *pApplicationCallData;


// code refer from StartEfiApplication
//
EFI_STATUS EFIAPI UefiMain (IN EFI_HANDLE ParentImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
    Print(L"Hello Renqihong jiayou!!!!\r\n");
    // ASSERT_EFI_ERROR(-1);
      EFI_SHELL_PROTOCOL    *EfiShellProtocol;
      EFI_STATUS            Status;
    //初始化协议
      Status = gBS->LocateProtocol (&gEfiShellProtocolGuid,
                                    NULL,
                                    (VOID **)&EfiShellProtocol);
    
      Print(L"%d Status: %d\r\n", __LINE__, Status);
      
      if (EFI_ERROR (Status)) 
      {
        Print(L"%d Status: %d\r\n", __LINE__, Status);
        return EFI_SUCCESS; 
      }
    
      EfiShellProtocol->Execute (&ParentImageHandle,
                                 L"fs0:",
                                 NULL,
                                 &Status);
                                 
      Print(L"%d Status: %d\r\n", __LINE__, Status);
      
      EfiShellProtocol->Execute (&ParentImageHandle,
                                 L"H.efi",
                                 NULL,
                                 &Status);
      Print(L"%d Status: %d\r\n", __LINE__, Status);
      
      return EFI_SUCCESS;
    //————————————————
    //版权声明：本文为CSDN博主「90geek」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
    //原文链接：https://blog.csdn.net/u011057409/article/details/122242621

}
