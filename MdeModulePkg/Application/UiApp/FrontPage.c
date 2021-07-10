/** @file
  FrontPage routines to handle the callbacks and browser calls

Copyright (c) 2004 - 2017, Intel Corporation. All rights reserved.<BR>
(C) Copyright 2018 Hewlett Packard Enterprise Development LP<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "FrontPage.h"
#include "FrontPageCustomizedUi.h"



EFI_STATUS DrawPoint(IN EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput,
    IN UINTN x,
    IN UINTN y,
    IN UINTN Width,
    IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL PixelColor)
{
    EFI_STATUS Status;

    Status = GraphicsOutput->Blt(GraphicsOutput,
                				 &PixelColor,
                				 EfiBltVideoFill,
                				 0, 0,
                				 x, y,
                				 Width, Width,
                				 0);        
                
    return Status;
}

INT32 abs(INT32 v)
{
	if (v < 0)
		return -v;

	return v;
}

EFI_STATUS DrawLine(IN EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput,
        IN UINTN x0, UINTN y0, UINTN x1, UINTN y1, 
        IN UINTN BorderWidth,
        IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL BorderColor)
{

    INT32 dx  = abs((int)(x1 - x0));
    INT32 sx  = x0 < x1 ? 1 : -1;
    INT32 dy  = abs((int)(y1-y0)), sy = y0 < y1 ? 1 : -1;
    INT32 err = ( dx > dy ? dx : -dy) / 2, e2;
    
    for(;;)
    {    
        DrawPoint(GraphicsOutput, x0, y0, BorderWidth, BorderColor);
    
        if (x0==x1 && y0==y1) break;
    
        e2 = err;
    
        if (e2 > -dx) { err -= dy; x0 += sx; }
        if (e2 <  dy) { err += dx; y0 += sy; }
    }
    return EFI_SUCCESS;
}


//Name:  GetKeyEx
//Input:
//Output:
//Descriptor:

EFI_STATUS GetKeyEx(UINT16 *ScanCode, UINT16 *UniChar, UINT32 *ShiftState, EFI_KEY_TOGGLE_STATE * ToggleState)

{

	EFI_STATUS                        Status;
	EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *SimpleEx;
  EFI_KEY_DATA                      KeyData;
  EFI_HANDLE                        *Handles;
  UINTN                             HandleCount;
  UINTN                             HandleIndex;
  UINTN								Index;

  

  Status = gBS->LocateHandleBuffer (
              ByProtocol,
              &gEfiSimpleTextInputExProtocolGuid,
              NULL,
              &HandleCount,
              &Handles
              );

  if(EFI_ERROR (Status))
  	return Status;

  for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) 
  {
    Status = gBS->HandleProtocol (Handles[HandleIndex], &gEfiSimpleTextInputExProtocolGuid, (VOID **) &SimpleEx);
    
    if (EFI_ERROR(Status))	
    	continue;
	else
	{
		gBS->WaitForEvent(1,&(SimpleEx->WaitForKeyEx),&Index);
    	Status = SimpleEx->ReadKeyStrokeEx(SimpleEx,&KeyData);
    	if(!EFI_ERROR(Status))
    	{

    		*ScanCode=KeyData.Key.ScanCode;
  			*UniChar=KeyData.Key.UnicodeChar;
  			*ShiftState=KeyData.KeyState.KeyShiftState;
  			*ToggleState=KeyData.KeyState.KeyToggleState;
  			return EFI_SUCCESS;
  		}
    }

  }	 

  return Status;

}



/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the image goes into a library that calls this
  function.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
InitializeUserInterface (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_HII_HANDLE                     HiiHandle;
  EFI_STATUS                         Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL       *GraphicsOutput;
  UINTN ScreenWidth, ScreenHeight;  
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;


	UINT16 scanCode=0;

	UINT16 uniChar=0;

	UINT32 shiftState;

	EFI_KEY_TOGGLE_STATE toggleState;

	UINT32 count=0;


  gBS->SetWatchdogTimer (0x0000, 0x0000, 0x0000, NULL);
  gST->ConOut->ClearScreen (gST->ConOut);

  //
  // Install customized fonts needed by Front Page
  //
  HiiHandle = ExportFonts ();
  ASSERT (HiiHandle != NULL);


  Status = gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (VOID **) &GraphicsOutput);
        
  if (EFI_ERROR (Status)) 
  	{
        return EFI_UNSUPPORTED;
  }
  
  ScreenWidth  = GraphicsOutput->Mode->Info->HorizontalResolution;
  ScreenHeight = GraphicsOutput->Mode->Info->VerticalResolution;
  
  
  Color.Red  = 0xFF;
  Color.Green = 0xFF;
  Color.Blue	= 0xFF;

  Print(L"ScreenWidth:%d, ScreenHeight:%d\n\n", ScreenWidth, ScreenHeight);
  
  Status = GraphicsOutput->Blt(GraphicsOutput,
					&Color,
					EfiBltVideoFill,
					0, 0,
					0, 0,
					ScreenWidth, ScreenHeight,
					0);

  Color.Red  = 0xFF;
  Color.Green = 0x00;
  Color.Blue	= 0xFF;

  DrawLine(GraphicsOutput, 0, 0, 100, 100, 2, Color);


	gST->ConOut->OutputString(gST->ConOut,L"please input key(ESC to exit):\n\r");
	while(scanCode!=0x17)	//ESC
	{
		Status=GetKeyEx(&scanCode,&uniChar,&shiftState,&toggleState);
		if (EFI_ERROR (Status)) 
		{
			Print(L"Call GetKeyEx() Error!\n");

			break;
		}
		else
		{

			Print(L"NO.%08d\n",count);
			++count;
			Print(L"  ScanCode=%04x",scanCode);
			Print(L"  UnicodeChar=%04x",uniChar);
			Print(L"  ShiftState=%08x",shiftState);
			Print(L"  ToggleState=%02x",toggleState);
			Print(L"\n");
		}

	}

   return EFI_SUCCESS;
}

