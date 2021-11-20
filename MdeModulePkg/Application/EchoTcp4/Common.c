/** @file
  Member functions of global protocol and functions for creation,
  code by luobing 2013-3-19 9:51:53.
**/

#include "Common.h"


EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *gSimpleTextInputEx;
EFI_GRAPHICS_OUTPUT_PROTOCOL       *gGraphicsOutput;
EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *gPCIRootBridgeIO;
EFI_PCI_IO_PROTOCOL *gPCIIO;

EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *gSimpleFileSystem;  //操作FAT文件系统的句柄
EFI_FILE_PROTOCOL *gFileRoot;		//操作文件的句柄
EFI_RNG_PROTOCOL *gRNGOut;			//Handle of Generating random number 2019-08-31 11:36:43 robin

//================  Member function =======================================
//Name:InintGloabalProtocols
//Input:
//Output: 0: 所有protocol都被获取
//				非零: 所对应位标志的protocol没有被正确获取
//        
//Descriptor:
UINT64 InintGloabalProtocols(UINT64 flag)
{
	
	INFO(L"flag=%x\n", flag);
	EFI_STATUS                        Status;
	UINT64	retVal=0;
	
	//Locate SimpleTextInputEx protocol
	if((flag&S_TEXT_INPUT_EX) == S_TEXT_INPUT_EX)
	{
		Status = LocateSimpleTextInputEx();
		INFO(L"Status=%x\n", Status);
		if (EFI_ERROR (Status)) 
		{
			retVal|=S_TEXT_INPUT_EX;
		}
	}
	
	//Locate GraphicsOutput protocol
	if((flag&GRAPHICS_OUTPUT) == GRAPHICS_OUTPUT)
	{
		Status = LocateGraphicsOutput();
		INFO(L"Status=%x\n", Status);
		if (EFI_ERROR (Status)) 
		{
			retVal|=GRAPHICS_OUTPUT;
		}
	}
	
	//Locate PCIRootBridgeIO protocol
	if((flag&PCI_ROOTBRIDGE_IO) == PCI_ROOTBRIDGE_IO)
	{
		Status = LocatePCIRootBridgeIO();
		INFO(L"Status=%x\n", Status);
		if (EFI_ERROR (Status)) 
		{
			retVal|=PCI_ROOTBRIDGE_IO;
		}			
	}
	
	//Locate PCIIO protocol
	if((flag&PCI_IO) == PCI_IO)
	{
		Status = LocatePCIIO();
		INFO(L"Status=%x\n", Status);
		
		if (EFI_ERROR (Status)) 
		{
			retVal|=PCI_IO;
		}
	}

	//Locate FileIO protocol
	if((flag&FILE_IO) == FILE_IO)
	{
		Status = LocateFileRoot();
		INFO(L"Status=%x\n", Status);
		
		if (EFI_ERROR (Status)) 
		{
			retVal|=FILE_IO;
		}
	}
	
	//Locate RNGOut protocol
	if((flag&RNG_OUT) == RNG_OUT)
	{
		Status = LocateRNGOut();
		INFO(L"Status=%x\n", Status);
		if (EFI_ERROR (Status)) 
		{
			retVal|=RNG_OUT;
		}			
	}
	return retVal;
}
//Name:LocateSimpleTextInputEx
//Input:
//Output: EFI_STATUS=EFI_SUCCESS, *SimpleTextInputEx
//				EFI_STATUS=
//Descriptor: Get the handle of SimpleTextInputEx
EFI_STATUS LocateSimpleTextInputEx(void)
{
	EFI_STATUS                        Status;
    EFI_HANDLE                        *Handles;
    UINTN                             HandleCount;
    UINTN                             HandleIndex;


    Status = gBS->LocateHandleBuffer(ByProtocol,
					               &gEfiSimpleTextInputExProtocolGuid,
					               NULL,
					               &HandleCount,
					               &Handles);
					               
    INFO(L"Status=%x\n", Status);

    if(EFI_ERROR (Status))
    {
        return Status;
    }

    for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) 
    {
        Status = gBS->HandleProtocol (Handles[HandleIndex], &gEfiSimpleTextInputExProtocolGuid, (VOID **) &gSimpleTextInputEx);
        
        if (EFI_ERROR(Status))	
        {
            continue;
    	}
    	else
		{
			return EFI_SUCCESS;
      	}
    }	 
	return Status;
}
//Name: LocateGraphicsOutput
//Input: 
//Output: *GraphicsOutput
//Descriptor:
EFI_STATUS LocateGraphicsOutput (void)
{
	EFI_STATUS                         Status;
	EFI_HANDLE                         *GraphicsOutputControllerHandles = NULL;
	UINTN                              HandleIndex = 0;
	UINTN                              HandleCount = 0;

	
	//get the handles which supports GraphicsOutputProtocol
	Status = gBS->LocateHandleBuffer(ByProtocol,
									 &gEfiGraphicsOutputProtocolGuid,
									 NULL,
									 &HandleCount,
									 &GraphicsOutputControllerHandles);
	
  	INFO(L"Status=%x\n", Status);
	if (EFI_ERROR(Status))	
	{
		return Status;		//unsupport
	}
	
	for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++)
	{
		Status = gBS->HandleProtocol(GraphicsOutputControllerHandles[HandleIndex],
									 &gEfiGraphicsOutputProtocolGuid,
									 (VOID**)&gGraphicsOutput);
		
  		INFO(L"Status=%x\n", Status);
  
		if (EFI_ERROR(Status))	
		{
			continue;
		}
		else
		{
			return EFI_SUCCESS;
		}
	}
	
	return Status;
}
//Name: LocatePCIRootBridgeIO
//Input: 
//Output: *gPCIRootBridgeIO
//Descriptor:
EFI_STATUS LocatePCIRootBridgeIO(void)
{
	EFI_STATUS                         Status;
	EFI_HANDLE                         *PciHandleBuffer = NULL;
	UINTN                              HandleIndex = 0;
	UINTN                              HandleCount = 0;
	//get the handles which supports 
	Status = gBS->LocateHandleBuffer(ByProtocol,
									 &gEfiPciRootBridgeIoProtocolGuid,
									 NULL,
									 &HandleCount,
									 &PciHandleBuffer);
	
  	INFO(L"Status=%x\n", Status);
	
	if (EFI_ERROR(Status))	
	{
		return Status;		//unsupport
	}
	
	for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++)
	{
		Status = gBS->HandleProtocol(PciHandleBuffer[HandleIndex],
									 &gEfiPciRootBridgeIoProtocolGuid,
									 (VOID**)&gPCIRootBridgeIO);
		
  		INFO(L"Status=%x\n", Status);
			
		if (EFI_ERROR(Status))	
		{
			continue;
		}
		else
		{
			return EFI_SUCCESS;
		}
	}
	return Status;
}
//Name: LocatePCIIO
//Input: 
//Output: *gPCIIO
//Descriptor:
EFI_STATUS LocatePCIIO(void)
{
	EFI_STATUS                         Status;
	EFI_HANDLE                         *PciHandleBuffer = NULL;
	UINTN                              HandleIndex = 0;
	UINTN                              HandleCount = 0;
	
	//get the handles which supports 
	Status = gBS->LocateHandleBuffer(ByProtocol,
									 &gEfiPciIoProtocolGuid,
									 NULL,
									 &HandleCount,
									 &PciHandleBuffer);
	
  	INFO(L"Status=%x\n", Status);
	
	if (EFI_ERROR(Status))	
	{
		return Status;		//unsupport
	}
	
	for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++)
	{
		Status = gBS->HandleProtocol(PciHandleBuffer[HandleIndex],
									 &gEfiPciIoProtocolGuid,
									 (VOID**)&gPCIIO);
		
		if (EFI_ERROR(Status))	
		{
			continue;
		}
		else
		{
			return EFI_SUCCESS;
		}
	}
	return Status;
}

//Name: LocateFileIO
//Input: 
//Output: *gFileRoot
//Descriptor:
EFI_STATUS LocateFileRoot(void)
{
	EFI_STATUS  Status = 0;
	
	Status = gBS->LocateProtocol(&gEfiSimpleFileSystemProtocolGuid,
					              NULL,
					              (VOID**)&gSimpleFileSystem);
	 
    if (EFI_ERROR(Status)) 
	{
     	//未找到EFI_SIMPLE_FILE_SYSTEM_PROTOCOL
        return Status;
    }
	
    Status = gSimpleFileSystem->OpenVolume(gSimpleFileSystem, &gFileRoot);

	return Status;
}

//Name: LocateRNGOut
//Input: 
//Output: *gRNGOut
//Descriptor:
EFI_STATUS LocateRNGOut(void)
{
	EFI_STATUS                         Status;
	EFI_HANDLE                         *RngHandleBuffer = NULL;
	UINTN                              HandleIndex = 0;
	UINTN                              HandleCount = 0;
	//get the handles which supports 
	Status = gBS->LocateHandleBuffer(ByProtocol,
									 &gEfiRngProtocolGuid,
									 NULL,
									 &HandleCount,
									 &RngHandleBuffer);
	
	if (EFI_ERROR(Status))	
	{
		return Status;		//unsupport
	}
	
	for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++)
	{
		Status = gBS->HandleProtocol(RngHandleBuffer[HandleIndex],
									 &gEfiRngProtocolGuid,
									 (VOID**)&gRNGOut);
		
		if (EFI_ERROR(Status))	
		{
			continue;
		}
		else
		{
			return EFI_SUCCESS;
		}
	}
	return Status;
}
