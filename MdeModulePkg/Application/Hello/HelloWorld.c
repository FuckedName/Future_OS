#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>

/* GLOBAL_REMOVE_IF_UNREFERENCED EFI_STRING_ID mStringHelpTokenId = STRING_TOKEN (STR_HELLO_WORLD_HELP_INFORMATION); */

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
#include <Protocol/DiskIo.h>
#include <Protocol/BlockIo.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
/*
 * #include <Library/PcdLib.h>
 * #include <Library/ShellCommandLib.h>
 */
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

VOID PrintA (EFI_EVENT Event, VOID *Context)
{	
    Print( L"A" );
}

VOID PrintB (EFI_EVENT Event, VOID *Context)
{	
    Print( L"B" );
}

VOID PrintC (EFI_EVENT Event, VOID *Context)
{	
    Print( L"C" );
}

VOID PrintD (EFI_EVENT Event, VOID *Context)
{	
    Print( L"D" );
}


EFI_EVENT MultiTaskTriggerEvent1;
EFI_EVENT MultiTaskTriggerEvent2;

VOID EFIAPI TimeSlice1(
	IN EFI_EVENT Event,
	IN VOID           *Context
	)
{    
	 DEBUG ((EFI_D_INFO, "TimeSlice1 :%x %lu \n", Context, *((UINT32 *)Context)));
	 //Print(L"%lu\n", *((UINT32 *)Context));
	 if (*((UINT32 *)Context) % 100000000 == 0)
	 	gBS->SignalEvent (MultiTaskTriggerEvent1);
	 	
	 if (*((UINT32 *)Context) % 200000000 == 0)
	 	gBS->SignalEvent (MultiTaskTriggerEvent2);

	return;
}

EFI_STATUS SystemTimeIntervalInit1()
{
    EFI_STATUS	Status;
	EFI_HANDLE	Timer1	= NULL;
	static const UINTN TimeInterval = 10000000;
	UINT32 *p;

	p = (UINT32 *)AllocateZeroPool(4);
	if (NULL == p)
	{
		DEBUG(( EFI_D_INFO, "%d, NULL == p \r\n", __LINE__));
		return;
	}


	Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL | EVT_TIMER,
                       		TPL_CALLBACK,
                       		TimeSlice1,
                       		(VOID *)p,
                       		&Timer1);

	if ( EFI_ERROR( Status ) )
	{
		DEBUG(( EFI_D_INFO, "Create Event Error! \r\n" ));
		return(1);
	}

	Status = gBS->SetTimer(Timer1,
						  TimerPeriodic,
						  MultU64x32( TimeInterval, 1));

	if ( EFI_ERROR( Status ) )
	{
		DEBUG(( EFI_D_INFO, "Set Timer Error! %x\r\n", p ));
		return(2);
	}

    /**/
    while (1)
    {    	
		 *p = *p + 1;
		 if (*p % 100000000 == 0)
	        DEBUG(( EFI_D_INFO, "while (1) p:%x %lu\r\n", p, *p));
    }
        
	gBS->SetTimer( Timer1, TimerCancel, 0 );
	gBS->CloseEvent( Timer1 );    

	return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
UefiMain(
	IN EFI_HANDLE ImageHandle,
	IN EFI_SYSTEM_TABLE  *SystemTable
	)
{
	EFI_GUID gMultiProcessGuid1  = { 0x0579257E, 0x1843, 0x45FB, { 0x83, 0x9D, 0x6B, 0x79, 0x09, 0x38, 0x29, 0xAA } };
	EFI_GUID gMultiProcessGuid2  = { 0x0579257f, 0x1843, 0x45FB, { 0x83, 0x9D, 0x6B, 0x79, 0x09, 0x38, 0x29, 0xAA } };

    EFI_EVENT_NOTIFY TaskProcesses1[] = {PrintA, PrintB};
    EFI_EVENT_NOTIFY TaskProcesses2[] = {PrintC, PrintD};
    UINT8 TaskPriorityLevel1[] = {TPL_CALLBACK, TPL_CALLBACK};
    UINT8 TaskPriorityLevel2[] = {TPL_NOTIFY, TPL_NOTIFY};
    
    for (int i = 0; i < sizeof(TaskProcesses1) / sizeof(EFI_EVENT_NOTIFY); i++)
    {
        gBS->CreateEventEx(EVT_NOTIFY_SIGNAL,
                          TaskPriorityLevel1[i],
                          TaskProcesses1[i],
                          NULL,
                          &gMultiProcessGuid1,
                          &MultiTaskTriggerEvent1
                          );
    }    

    for (int i = 0; i < sizeof(TaskProcesses2) / sizeof(EFI_EVENT_NOTIFY); i++)
    {
        gBS->CreateEventEx(EVT_NOTIFY_SIGNAL,
                          TaskPriorityLevel2[i],
                          TaskProcesses2[i],
                          NULL,
                          &gMultiProcessGuid2,
                          &MultiTaskTriggerEvent2
                          );
    }    

	 SystemTimeIntervalInit1();

	return(EFI_SUCCESS);
}

