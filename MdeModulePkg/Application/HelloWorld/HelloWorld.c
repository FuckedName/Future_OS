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
    EFI_STATUS          RanStatus;
    UINT8 pApplicationCallInput[20];
    UINT8 (*pCallbackFunction)(EFI_STATUS ret);
}APPLICATION_CALL_DATA;

EFI_STATUS Callback(EFI_STATUS ret)
{
     Print(L"Call Print string successfully.!!!!\r\n");
     return EFI_SUCCESS;
}

// code refer from StartEfiApplication
//
EFI_STATUS EFIAPI UefiMain (IN EFI_HANDLE ParentImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
    Print(L"Hello Renqihong jiayou!!!!\r\n");

    APPLICATION_CALL_DATA *pApplicationCallData = (APPLICATION_CALL_DATA *)(0x40000000 - 0x1000);
    
    pApplicationCallData->ID = APPLICATION_CALL_ID_PRINT_STRING;
    
    pApplicationCallData->pApplicationCallInput[0] = 'T';
    pApplicationCallData->pApplicationCallInput[1] = 'e';
    pApplicationCallData->pApplicationCallInput[2] = 's';
    pApplicationCallData->pApplicationCallInput[3] = 't';
    pApplicationCallData->pApplicationCallInput[4] = 'A';
    pApplicationCallData->pApplicationCallInput[5] = 'P';
    pApplicationCallData->pApplicationCallInput[6] = 'I';
    pApplicationCallData->pApplicationCallInput[7] = '3';
    pApplicationCallData->pApplicationCallInput[8] = '\0';    

    pApplicationCallData->pCallbackFunction = Callback;
      
    Print(L"Hello Renqihong jiayou!!!!: %d\r\n", pApplicationCallData->RanStatus);
    
    return EFI_SUCCESS;

}
