#include <Library/UefiLib.h>
/*************************************************
    .
    File name:      	A.c
    Author：	        任启红
    ID：		00001
    Version: Date:  	202107
    Description:    	整个模块的主入口函数
    Others:         	无
    History:        	无
	    1.  Date:
		    Author: 
		    ID:
		    Modification:
		    
	    2.  Date:
		    Author: 
		    ID:
		    Modification:
*************************************************/





#include <L2_PROCESS_Multi.h>

//#include <Device/Keyboard/L2_DEVICE_Keyboard.h>
//#include <Device/Mouse/L2_DEVICE_Mouse.h>
//#include <Device/Timer/L2_DEVICE_Timer.h>
#include <Graphics/L2_GRAPHICS.h>
#include <Devices/Keyboard/L2_DEVICE_Keyboard.h>
#include <Devices/Mouse/L2_DEVICE_Mouse.h>
#include <Devices/Timer/L2_DEVICE_Timer.h>



EFI_EVENT MultiTaskTriggerGroup0Event;
EFI_EVENT MultiTaskTriggerGroup1Event;
EFI_EVENT MultiTaskTriggerGroup2Event;


UINT32 TimerSliceCount = 0;

VOID
EFIAPI
L2_SYSTEM_Start (IN EFI_EVENT Event, IN VOID *Context)
{

}

VOID EFIAPI L2_TIMER_Slice(
    IN EFI_EVENT Event,
    IN VOID           *Context
    )
{
    L2_DEBUG_Print1(0, ScreenHeight - 30 - 5 * 16, "%d: TimeSlice %x %lu \n", __LINE__, Context, *((UINT32 *)Context));
    L2_DEBUG_Print1(0, ScreenHeight - 30 - 6 * 16, "%d: TimerSliceCount: %lu \n", __LINE__, TimerSliceCount);
    //Print(L"%lu\n", *((UINT32 *)Context));
    if (TimerSliceCount % 10 == 0)
       gBS->SignalEvent (MultiTaskTriggerGroup1Event);
       
    if (TimerSliceCount % 50 == 0)
       gBS->SignalEvent (MultiTaskTriggerGroup2Event);
    
    ////DEBUG ((EFI_D_INFO, "System time slice Loop ...\n"));
    //gBS->SignalEvent (MultiTaskTriggerEvent);

    TimerSliceCount++;
    return;
}


EFI_STATUS L2_COMMON_MultiProcessInit ()
{
    UINT16 i;

    // task group for mouse keyboard
    EFI_GUID gMultiProcessGroup1Guid  = { 0x0579257E, 0x1843, 0x45FB, { 0x83, 0x9D, 0x6B, 0x79, 0x09, 0x38, 0x29, 0xA9 } };
    
    // task group for display date time
    EFI_GUID gMultiProcessGroup2Guid  = { 0x0579257E, 0x1843, 0x45FB, { 0x83, 0x9D, 0x6B, 0x79, 0x09, 0x38, 0x29, 0xAA } };
    
    //L2_GRAPHICS_ChineseCharDraw(pMouseBuffer, 0, 0, 11 * 94 + 42, Color, 16);
    
    //DrawChineseCharIntoBuffer(pMouseBuffer, 0, 0, 0, Color, 16);
    
    EFI_EVENT_NOTIFY       TaskProcessesGroup1[] = {L2_KEYBOARD_Event, L2_MOUSE_Event, L2_SYSTEM_Start};

    EFI_EVENT_NOTIFY       TaskProcessesGroup2[] = {L2_TIMER_Print};

    for (i = 0; i < sizeof(TaskProcessesGroup1) / sizeof(EFI_EVENT_NOTIFY); i++)
    {
        gBS->CreateEventEx(
                          EVT_NOTIFY_SIGNAL,
                          TPL_NOTIFY,
                          TaskProcessesGroup1[i],
                          NULL,
                          &gMultiProcessGroup1Guid,
                          &MultiTaskTriggerGroup1Event
                          );
    }    

    for (i = 0; i < sizeof(TaskProcessesGroup2) / sizeof(EFI_EVENT_NOTIFY); i++)
    {
        gBS->CreateEventEx(
                          EVT_NOTIFY_SIGNAL,
                          TPL_NOTIFY,
                          TaskProcessesGroup2[i],
                          NULL,
                          &gMultiProcessGroup2Guid,
                          &MultiTaskTriggerGroup2Event
                          );
    }    

    return EFI_SUCCESS;
}
// https://blog.csdn.net/longsonssss/article/details/80221513

