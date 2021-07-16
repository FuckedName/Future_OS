#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>

GLOBAL_REMOVE_IF_UNREFERENCED EFI_STRING_ID mStringHelpTokenId = STRING_TOKEN (STR_HELLO_WORLD_HELP_INFORMATION);

#include <Library/UefiLib.h> 
#include <Library/ShellCEntryLib.h> 
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/SimplePointer.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/DebugLib.h>

#include <Guid/GlobalVariable.h>
#include <Guid/ShellLibHiiGuid.h>

#include <Protocol/Shell.h>
#include <Protocol/ShellParameters.h>
#include <Protocol/DevicePath.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/UnicodeCollation.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/ShellCommandLib.h>
#include <Library/ShellLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/HiiLib.h>
#include <Library/SortLib.h>
#include <Guid/GlobalVariable.h>
#include <Guid/ConsoleInDevice.h>
#include <Guid/ConsoleOutDevice.h>
#include <Guid/ShellLibHiiGuid.h>

#include <IndustryStandard/Pci.h>

#include <Pi/PiFirmwareVolume.h>
#include <Pi/PiFirmwareFile.h>

#include <Library/FileHandleLib.h>
#include <Pi/PiFirmwareFile.h>
#include <Protocol/FirmwareVolume2.h>

#include <Protocol/Shell.h>
#include <Protocol/ShellParameters.h>
#include <Protocol/DevicePath.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/UnicodeCollation.h>
#include <Protocol/DriverDiagnostics2.h>
#include <Protocol/DriverDiagnostics.h>
#include <Protocol/PlatformDriverOverride.h>
#include <Protocol/BusSpecificDriverOverride.h>
#include <Protocol/PlatformToDriverConfiguration.h>
#include <Protocol/DriverSupportedEfiVersion.h>
#include <Protocol/DriverFamilyOverride.h>
#include <Protocol/DriverHealth.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/ShellCommandLib.h>
#include <Library/ShellLib.h>
#include <Library/SortLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/HiiLib.h>
#include <Library/FileHandleLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PrintLib.h>
#include <Library/HandleParsingLib.h>
#include <Library/PeCoffGetEntryPointLib.h>
#include <Library/HandleParsingLib.h>
#include <Guid/TtyTerm.h>
#include <Guid/MdeModuleHii.h>
#include <Guid/FileSystemVolumeLabelInfo.h>
#include <Guid/GlobalVariable.h>
#include <Guid/HiiBootMaintenanceFormset.h>

#include <Protocol/LoadFile.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/SerialIo.h>
#include <Protocol/DevicePathToText.h>
#include <Protocol/FormBrowserEx2.h>

#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/HiiLib.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/UefiBootManagerLib.h>
#include <Library/FileExplorerLib.h>


//#include <Protocol/EfiShell.h>
//#include <guid/FileInfo.h>


EFI_STATUS
EFIAPI
BZero (
  OUT     CHAR16                    *Destination,
  IN      UINTN                     Length
  )
 {
	UINT32 i ;
	CHAR8 * ptr = (CHAR8 * )Destination;
	for (i = 0; i < 2*Length ; i ++){
		ptr [i] = 0;
	}
	return EFI_SUCCESS;
 }

EFI_STATUS OpenShellProtocol( EFI_SHELL_PROTOCOL            **gEfiShellProtocol )
{
    EFI_STATUS                      Status;
    Status = gBS->OpenProtocol(
            gImageHandle,
            &gEfiShellProtocolGuid,
            (VOID **)gEfiShellProtocol,
            gImageHandle,
            NULL,
            EFI_OPEN_PROTOCOL_GET_PROTOCOL
            );
    if (EFI_ERROR(Status)) {
    //
    // Search for the shell protocol
    //
        Status = gBS->LocateProtocol(
                &gEfiShellProtocolGuid,
                NULL,
                (VOID **)gEfiShellProtocol
                );
        if (EFI_ERROR(Status)) {
            gEfiShellProtocol = NULL;
        }
  }
  return Status;
}

VOID
Ascii2UnicodeString (
  CHAR8    *String,
  CHAR16   *UniString
  )
{
  while (*String != '\0') {
    *(UniString++) = (CHAR16) *(String++);
  }
  //
  // End the UniString with a NULL.
  //
  *UniString = '\0';
} 


EFI_STATUS PrintNode(EFI_DEVICE_PATH_PROTOCOL *Node)
{
    Print(L"(%d, %d)/", Node->Type, Node->SubType);
    return 0;
}

EFI_DEVICE_PATH_PROTOCOL *WalkthroughDevicePath(EFI_DEVICE_PATH_PROTOCOL *DevPath,
                                                EFI_STATUS (* CallBack)(EFI_DEVICE_PATH_PROTOCOL*))
{
    EFI_DEVICE_PATH_PROTOCOL *pDevPath = DevPath;

    while(! IsDevicePathEnd(pDevPath))
    {
        CallBack(pDevPath);
        pDevPath = NextDevicePathNode(pDevPath);
    }

    return pDevPath;
}



EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    Print(L"Hello Renqihong jiayou!!\r\n");

    CHAR16 * OldLogFileName = L"test.txt";
    //CHAR16  *LineBuff = NULL;
    CHAR16  NewFileName[128] = L"test2.txt";
    CHAR16 ArrayBuffer[39]  = L"renqihong";
    EFI_STATUS Status ;
    SHELL_FILE_HANDLE FileHandle;
    EFI_SHELL_PROTOCOL  *gEfiShellProtocol;
    //CHAR8 *Ptr = NULL;
    UINTN NumHandles, i;
    EFI_HANDLE *ControllerHandle = NULL;
    EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;

    Status = OpenShellProtocol(&gEfiShellProtocol);

    
    Status = gBS->LocateProtocol (&gEfiDevicePathToTextProtocolGuid, NULL, (VOID **) &DevPathToText);
    if (EFI_ERROR(Status))
    {
        Print(L"LocateProtocol1 error: %x\n", Status);
        return Status;
    }
   
    Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiDiskIoProtocolGuid, NULL, &NumHandles, &ControllerHandle);
    if (EFI_ERROR(Status))
    {
        Print(L"LocateHandleBuffer error: %x\n", Status);
        return Status;
    }

    Print(L"NumHandles: %d\n", NumHandles);
    
    Print(L"Before for\n", Status);
    for (i = 0; i < NumHandles; i++)
    {
        EFI_DEVICE_PATH_PROTOCOL *DiskDevicePath;
        Status = gBS->OpenProtocol(ControllerHandle[i],
                                   &gEfiDevicePathProtocolGuid,
                                   (VOID **)&DiskDevicePath,
                                   gImageHandle, 
                                   NULL,
                                   EFI_OPEN_PROTOCOL_GET_PROTOCOL);
        if (EFI_ERROR(Status))
        {
            Print(L"Status = gBS->OpenProtocol error index %d: %x\n", i, Status);
            return Status;
        }

        CHAR16 *TextDevicePath = 0;
        TextDevicePath = DevPathToText->ConvertDeviceNodeToText(DiskDevicePath, TRUE, TRUE);
        Print(L"%s\n", TextDevicePath);

        if (TextDevicePath) gBS->FreePool(TextDevicePath);

        WalkthroughDevicePath(DiskDevicePath, PrintNode);
        
        Print(L"\n\n");
    }

    /*  
    Status = gEfiShellProtocol->OpenFileByName((CONST CHAR16*)OldLogFileName, &FileHandle, EFI_FILE_MODE_READ); 
    if (EFI_ERROR(Status)){
      Print(L"Please Input Valid Filename!\n");
      //return (-1);
    }
    StrnCpyS(NewFileName,128,OldLogFileName,StrLen(OldLogFileName)-4);
    StrCatS(NewFileName,128,L"_New.txt");
    Print(L"New FileName is %s\n",NewFileName);

    //删除同名的文件
    gEfiShellProtocol->DeleteFileByName(NewFileName);

    //获取文件实际大小
    Status = gEfiShellProtocol->GetFileSize(FileHandle,&FileSize);
    if (EFI_ERROR (Status)) {
    gEfiShellProtocol->CloseFile (FileHandle);
    return Status;
    }
    Print (L"File FileSize is %d!\n",FileSize);

    if (FileSize < 0){
      Print (L"File cotent is empty!\n");
      return (-1);
    }

    FileSize += 1;

    //根据文件大小申请对应大小的内存
    Status = gBS -> AllocatePool (EfiReservedMemoryType, FileSize , &ArrayBuffer);
    Status = gBS -> AllocatePool (EfiReservedMemoryType, FileSize , &LineBuff);

    BZero(ArrayBuffer,FileSize);
    BZero(LineBuff,FileSize);
    Status = gEfiShellProtocol->ReadFile(FileHandle, &FileSize ,ArrayBuffer); 

    if (EFI_ERROR(Status)){
    	Print(L"Read Filename Error!\n");
    	return (-1);
    }
    */

    //创建新的文件句柄
    Status = gEfiShellProtocol->CreateFile((CONST CHAR16*)NewFileName, 0, &FileHandle); 
    if (EFI_ERROR(Status)){
      Print(L"Create Filename %s Fail!:%X \n", NewFileName, Status);
      return (-1);
    }

    //读取的文件内容写入新建文件
    INT8 WbufSize = 10;
    Status = gEfiShellProtocol->WriteFile(FileHandle,&WbufSize,ArrayBuffer);

    //关闭文件句柄
    Status = gEfiShellProtocol->CloseFile(FileHandle);

    return EFI_SUCCESS;
}

