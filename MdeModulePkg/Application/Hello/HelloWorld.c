#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>

//GLOBAL_REMOVE_IF_UNREFERENCED EFI_STRING_ID mStringHelpTokenId = STRING_TOKEN (STR_HELLO_WORLD_HELP_INFORMATION);

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
#include <Protocol/Diskio.h>
#include <Protocol/Blockio.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
//#include <Library/PcdLib.h>
//#include <Library/ShellCommandLib.h>
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

//refer to ReadFileToBuffer
//refer to GetAllCapsuleOnDisk
//refer to BmGetNextLoadOptionBuffer
EFI_STATUS FileSystem1()
{
    Print(L"%d, FileSystem1 start\n", __LINE__);
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *Fs = NULL;
	EFI_FILE_HANDLE                  RootDir;
	EFI_FILE_HANDLE                  FileDir;
	EFI_STATUS                         Status;
	EFI_HANDLE                       *FsHandle = NULL;

	UINT16                           *TempOptionNumber;

	TempOptionNumber = NULL;
	//*CapsuleNum      = 0;

	Status = GetEfiSysPartitionFromActiveBootOption( 3, &TempOptionNumber, FsHandle );
	if ( EFI_ERROR( Status ) )
	{
		return(Status);
	}

	Status = gBS->HandleProtocol( *FsHandle, &gEfiSimpleFileSystemProtocolGuid, (VOID * *) &Fs );
	if ( EFI_ERROR( Status ) )
	{
		return(Status);
	}

	Status = Fs->OpenVolume( Fs, &RootDir );
	if ( EFI_ERROR( Status ) )
	{
		return(Status);
	}

	Status = RootDir->Open(
		RootDir,
		&FileDir,
		EFI_CAPSULE_FILE_DIRECTORY,
		EFI_FILE_MODE_READ,
		0
		);
	if ( EFI_ERROR( Status ) )
	{
		DEBUG( (DEBUG_ERROR, "CodLibGetAllCapsuleOnDisk fail to open RootDir!\n") );
		RootDir->Close( RootDir );
		return(Status);
	}
	RootDir->Close (RootDir);

}

EFI_STATUS EFIAPI GetString(IN EFI_SYSTEM_TABLE *gST,CHAR16* Str)
{
 UINTN                     Index1,Index2=0;
 EFI_STATUS                Status;
 EFI_INPUT_KEY             key;

 while(EFI_SUCCESS == (Status = gST->BootServices->WaitForEvent(1, &(gST->ConIn->WaitForKey), &Index1)))
 {
  gST->ConIn->ReadKeyStroke(gST->ConIn,&key);

  if(key.UnicodeChar == 13)
  {
   Str[Index2]=0;
   break;
  }

  if(key.UnicodeChar!=8)
  {
   Str[Index2]=key.UnicodeChar;
   ++Index2;
   Print(L"%c",key.UnicodeChar);
  }
  else
  {
   if(Index2>0)
   {
    Str[Index2]=0;
    --Index2;
    Print(L"\b");
   }
  }
 }

 Print(L"\n");

 return Status;
}

EFI_FILE_PROTOCOL* EFIAPI GetFile(IN EFI_SYSTEM_TABLE *gST)
{ 
 EFI_STATUS              Status;         
 INTN               HandleFileCount,HandleFileIndex=0;   
 EFI_SIMPLE_FILE_SYSTEM_PROTOCOL   *Sfs;          
 EFI_HANDLE                        *Files=NULL;         
 EFI_FILE_PROTOCOL         *Root;        
 EFI_FILE_PROTOCOL         *file;      
// UINTN               FileSize=512,i;      
 CHAR16                            FileName[100];    

 Print(L"  please input the file name : \n  ");
 GetString(gST,FileName);

 Status=gST->BootServices->LocateHandleBuffer(
											   ByProtocol,
											   &gEfiSimpleFileSystemProtocolGuid,
											   NULL,
											   &HandleFileCount,
											   &Files);

 if(!EFI_ERROR(Status))
 {
  Print(L"==successed to find %d controllers==\n",HandleFileCount);

  for(HandleFileIndex=HandleFileCount-1; HandleFileIndex > -1; --HandleFileIndex)
  {
    Status = gST->BootServices->HandleProtocol(
										     Files[HandleFileIndex],
										     &gEfiSimpleFileSystemProtocolGuid,
										     (VOID**)&Sfs);


    if(Status == EFI_SUCCESS)
    {
     Status = Sfs->OpenVolume(Sfs, &Root);

     if(Status == EFI_SUCCESS)
     {
      Print(L"==controller %d successed to open the volume==\n",HandleFileIndex);

      Status = Root->Open(Root, &file, FileName, EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 0);

      if(Status == EFI_SUCCESS)
      {
       Print(L"==controller %d successed to open the file : %s==\n",HandleFileIndex,FileName);  

       return file;
      }
      else
      {
       Print(L"!!controller %d failed to open the file : %s!!\n",HandleFileIndex,FileName);       
      }
     }
    }

  }

 }  


 return NULL;
}
//https://blog.csdn.net/youyudexiaowangzi/article/details/42008465


#define BYTES   512   
#define EXBYTE   4  

VOID EFIAPI ShowHex(UINT8* Buffer,UINTN len)
{
 UINTN               i=0;
 for(i = 0; i < 52; ++i)
 {
  if(i%26 == 0)
   Print(L"\r\n");

  Print(L"%02x ",Buffer[i]);
 }
}

VOID EFIAPI DecToChar(UINT8* CharBuff,UINT8 I)
{
 CharBuff[0]=((I/16) > 9)?('A'+(I/16)-10):('0'+(I/16));
 CharBuff[1]=((I%16) > 9)?('A'+(I%16)-10):('0'+(I%16));
}

VOID EFIAPI DecToCharBuffer(UINT8* Buffin,UINTN len,UINT8* Buffout)
{
 UINTN i=0;

 for(i=0;i<len;++i)
 {
  DecToChar(Buffout+i*4,Buffin[i]);
  if((i+1)%16==0)
  {
   *(Buffout+ i*4 +2)='\r';
   *(Buffout+ i*4 +3)='\n';
  }
  else 
  {
   *(Buffout+ i*4 +2)=' ';
   *(Buffout+ i*4 +3)=' ';
  }
 }
}


//https://blog.csdn.net/youyudexiaowangzi/article/details/42008465



EFI_STATUS
EFIAPI
UefiMain(
	IN EFI_HANDLE ImageHandle,
	IN EFI_SYSTEM_TABLE  *SystemTable
	)
{
	UINTN MaxRetry = 3;
	Print( L"Hello Renqihong jiayou!!111\r\n" );
	/*
	 * FileSystem1();
	 * CoDRelocateCapsule(MaxRetry);
	 */
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *Fs;


	CHAR16 * OldLogFileName = L"test.txt";
	/* CHAR16  *LineBuff = NULL; */
	CHAR16			NewFileName[128]	= L"test2.txt";
	CHAR16			ArrayBuffer[39]		= L"renqihong";
	EFI_STATUS		Status;
	SHELL_FILE_HANDLE	FileHandle;
	EFI_SHELL_PROTOCOL	*gEfiShellProtocol;
	/* CHAR8 *Ptr = NULL; */
	UINTN					NumHandles, i;
	EFI_HANDLE				*ControllerHandle = NULL;
	EFI_DEVICE_PATH_TO_TEXT_PROTOCOL	*DevPathToText;
	EFI_FILE_HANDLE				FileDir;

	EFI_FILE_HANDLE RootDir;                          
	INTN                                          HandleCount,HandleIndex;           
	EFI_HANDLE                                    *Controllers=NULL;                
	EFI_DISK_IO_PROTOCOL                          *DiskIo;                           
	EFI_BLOCK_IO_PROTOCOL                         *BlockIo;                         
	EFI_DEVICE_PATH_PROTOCOL                      *DevicePath;                      
	EFI_DEVICE_PATH_TO_TEXT_PROTOCOL              *DevicePathToText;                 
	EFI_BLOCK_IO_MEDIA                            *Media;                          
	CHAR16                                        *TextOfDevicePath;              

	EFI_BOOT_SERVICES                             *gBS=SystemTable->BootServices;    
	
	UINT8                                         Buffer[BYTES];                    
	
	UINT8                                         Bufferout[BYTES*EXBYTE];         
	
	UINTN            WriteSize;                          
	EFI_FILE_PROTOCOL         *file=GetFile(SystemTable); 
    

	Status = gBS->LocateProtocol( &gEfiDevicePathToTextProtocolGuid, NULL, (VOID * *) &DevPathToText );
	if ( EFI_ERROR( Status ) )
	{
		Print( L"%d, LocateProtocol gEfiDevicePathToTextProtocolGuid error: %x\n", __LINE__, Status );
		return(Status);
	}

	Status = gBS->LocateHandleBuffer( ByProtocol, &gEfiDiskIoProtocolGuid, NULL, &NumHandles, &ControllerHandle );
	if ( EFI_ERROR( Status ) )
	{
		Print( L"%d, LocateHandleBuffer gEfiDiskIoProtocolGuid error: %x\n", __LINE__, Status );
		return(Status);
	}

	Print( L"%d, NumHandles: %d\n", __LINE__, NumHandles );

	Print( L"Before for\n", Status );
	for ( i = 0; i < NumHandles; i++ )
	{
		Status = gBS->HandleProtocol( ControllerHandle[i],
					      &gEfiDiskIoProtocolGuid,
					      (VOID * *) &DiskIo );


		if ( Status == EFI_SUCCESS )
		{
			Status = gBS->HandleProtocol( Controllers[HandleIndex],
						      &gEfiDevicePathProtocolGuid,
						      (VOID * *) &DevicePath );

			if ( EFI_SUCCESS == Status )
			{
				Status = gBS->LocateProtocol(
					&gEfiDevicePathToTextProtocolGuid,
					NULL,
					(VOID * *) &DevicePathToText );

				if ( EFI_SUCCESS == Status )
				{
					TextOfDevicePath = DevicePathToText->ConvertDevicePathToText( DevicePath, TRUE, TRUE );

					Print( L"== %d path== \n%s \n", HandleIndex, TextOfDevicePath );
					
					WriteSize = StrLen( TextOfDevicePath ) * 2;
					file->Write( file, &WriteSize, TextOfDevicePath );
					WriteSize = 2;
					file->Write( file, &WriteSize, "\r\n" );

					Status = gBS->HandleProtocol(
													Controllers[HandleIndex],
													&gEfiBlockIoProtocolGuid,
													(VOID * *) &BlockIo );
						
					Media = BlockIo->Media;

					if ( EFI_SUCCESS == Status )
					{
						Print( L"==read something==\n" );

						Status = DiskIo->ReadDisk( DiskIo, Media->MediaId, 0, BYTES, Buffer );
						
						DecToCharBuffer( Buffer, BYTES, Bufferout );
						
						WriteSize = BYTES * EXBYTE;


						Status = file->Write( file, &WriteSize, Bufferout );

						if ( EFI_SUCCESS == Status )
						{
							Print( L"==successed to write %d BYTES==\n", WriteSize );
						}else  {
							Print( L"!!failed to write %d BYTES!!\n", WriteSize );
						}


						ShowHex( Buffer, 52 );
					}else  {
						Print( L"!!failed to read disk!!\n" );
					}
				}
			}
		}

		file->Close( file ); 

		EFI_DEVICE_PATH_PROTOCOL *DiskDevicePath;
		Status = gBS->OpenProtocol( ControllerHandle[i],
					    &gEfiDevicePathProtocolGuid,
					    (VOID * *) &DiskDevicePath,
					    gImageHandle,
					    NULL,
					    EFI_OPEN_PROTOCOL_GET_PROTOCOL );
		if ( EFI_ERROR( Status ) )
		{
			Print( L"Status = gBS->OpenProtocol error index %d: %x\n", i, Status );
			return(Status);
		}
		Print( L"%d\n", __LINE__ );

		CHAR16 *TextDevicePath = 0;
		/* TextDevicePath = DevPathToText->ConvertDeviceNodeToText(DiskDevicePath, TRUE, TRUE); */

		/* Print(L"%d %s\n",__LINE__, TextDevicePath); */

		TextDevicePath = DevPathToText->ConvertDevicePathToText( DiskDevicePath, TRUE, TRUE );
		Print( L"%d %s\n", __LINE__, TextDevicePath );

		if ( TextDevicePath )
			gBS->FreePool( TextDevicePath );

		WalkthroughDevicePath( DiskDevicePath, PrintNode );

		Print( L"\n\n" );
	}

	return(EFI_SUCCESS);
}


