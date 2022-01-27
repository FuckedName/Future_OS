#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>

EFI_STATUS EFIAPI UefiMain (IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
    Print(L"Hello Renqihong jiayou!!!!\r\n");
   // ASSERT_EFI_ERROR(-1);
    return EFI_SUCCESS;
}
