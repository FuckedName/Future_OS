
/*************************************************
    .
    File name:      	*.*
    Author：	        	任启红
    ID：					00001
    Date:          		202107
    Description:    	
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




#include <Library/UefiLib.h>


#include <L2_PROCESS_Multi.h>

//#include <Device/Keyboard/L2_DEVICE_Keyboard.h>
//#include <Device/Mouse/L2_DEVICE_Mouse.h>
//#include <Device/Timer/L2_DEVICE_Timer.h>
#include <Graphics/L2_GRAPHICS.h>
#include <Devices/Keyboard/L2_DEVICE_Keyboard.h>
#include <Devices/Mouse/L2_DEVICE_Mouse.h>
#include <Devices/Timer/L2_DEVICE_Timer.h>
#include <Devices/Network/L2_DEVICE_Network.h>

#include "L2_INTERFACES.h"


EFI_EVENT MultiTaskTriggerGroup1Event;
EFI_EVENT MultiTaskTriggerGroup2Event;
EFI_EVENT MultiTaskTriggerGroup3Event;
EFI_EVENT MultiTaskTriggerGroup4Event;


UINT32 TimerSliceCount = 0;



/****************************************************************************
*
*  描述:   xxxxx
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
L2_SYSTEM_Start (IN EFI_EVENT Event, IN VOID *Context)
{

}


/****************************************************************************
*
*  描述:   整个系统的时间片，多任务全靠这个函数驱动。
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID EFIAPI L2_TIMER_Slice(
    IN EFI_EVENT Event,
    IN VOID           *Context
    )
{
    L2_DEBUG_Print1(0, ScreenHeight - 30 - 5 * 16, "%d: TimeSlice %x %lu \n", __LINE__, Context, *((UINT32 *)Context));

	//时间片计数
    L2_DEBUG_Print1(0, ScreenHeight - 30 - 6 * 16, "%d: TimerSliceCount: %lu \n", __LINE__, TimerSliceCount);
    //Print(L"%lu\n", *((UINT32 *)Context));

	//第一个事件分组是鼠标、键盘，所以需要频率更高的运行
    if (TimerSliceCount % 10 == 0)
    {
        gBS->SignalEvent (MultiTaskTriggerGroup1Event);
        gBS->SignalEvent (MultiTaskTriggerGroup3Event);        
    }

	//第二个事件分组是屏幕右下角显示日期、时间、星期几，所以频率可以低些以节约CPU资源
    if (TimerSliceCount % 20 == 0)
       gBS->SignalEvent (MultiTaskTriggerGroup2Event);

    //系统调用如果不为零，表示应用层有系统调用，则触发对应的事件组。
    //if (0 != *APPLICATION_CALL_FLAG_ADDRESS)
    if (TimerSliceCount == 700)
    {
        gBS->SignalEvent (MultiTaskTriggerGroup4Event);
        *APPLICATION_CALL_FLAG_ADDRESS = 0;
    }
    
    ////DEBUG ((EFI_D_INFO, "System time slice Loop ...\n"));
    //gBS->SignalEvent (MultiTaskTriggerEvent);

    TimerSliceCount++;
    return;
}



/****************************************************************************
*
*  描述:   多进程组，每个进程组里有多个进程，不过跟真正操作系统的进程有些差别。
*        当前系统的多进程还有些问题，比如：不能让指定的进程在指定的内存地址执行。
*

*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L2_COMMON_MultiProcessInit ()
{
    UINT16 i;

    // task group for mouse keyboard
    EFI_GUID gMultiProcessGroup1Guid  = { 0x0579257E, 0x1843, 0x45FB, { 0x83, 0x9D, 0x6B, 0x79, 0x09, 0x38, 0x29, 0xA9 } };
    
    // task group for display date time
    EFI_GUID gMultiProcessGroup2Guid  = { 0x0579257E, 0x1843, 0x45FB, { 0x83, 0x9D, 0x6B, 0x79, 0x09, 0x38, 0x29, 0xAA } };
    
    EFI_GUID gMultiProcessGroup3Guid  = { 0x0579257E, 0x1843, 0x45FB, { 0x83, 0x9D, 0x6B, 0x79, 0x09, 0x38, 0x29, 0xAB } };
    
    EFI_GUID gMultiProcessGroup4Guid  = { 0x0579257E, 0x1843, 0x45FB, { 0x83, 0x9D, 0x6B, 0x79, 0x09, 0x38, 0x29, 0xAC } };
    //L2_GRAPHICS_ChineseCharDraw(pMouseBuffer, 0, 0, 11 * 94 + 42, Color, 16);
    
    //DrawChineseCharIntoBuffer(pMouseBuffer, 0, 0, 0, Color, 16);
    
    EFI_EVENT_NOTIFY       TaskProcessesGroupSystem[] = {L2_KEYBOARD_Event, L2_MOUSE_Event, L2_SYSTEM_Start};

    EFI_EVENT_NOTIFY       TaskProcessesGroupDateTimePrint[] = {L2_TIMER_Print};
    
    //EFI_EVENT_NOTIFY       TaskProcessesGroupTCPHandle[] = {};
    EFI_EVENT_NOTIFY       TaskProcessesGroupTCPHandle[] = {L2_TCP4_HeartBeatNotify, L2_TCP4_ReceiveNotify, L2_TCP4_SendNotify};
    
    EFI_EVENT_NOTIFY       TaskProcessesGroupApplicationCall[] = {L2_ApplicationCall, L2_ApplicationShutdown};

    for (i = 0; i < sizeof(TaskProcessesGroupSystem) / sizeof(EFI_EVENT_NOTIFY); i++)
    {
        gBS->CreateEventEx(
                          EVT_NOTIFY_SIGNAL,
                          TPL_NOTIFY,
                          TaskProcessesGroupSystem[i],
                          NULL,
                          &gMultiProcessGroup1Guid,
                          &MultiTaskTriggerGroup1Event);
    }    

    for (i = 0; i < sizeof(TaskProcessesGroupDateTimePrint) / sizeof(EFI_EVENT_NOTIFY); i++)
    {
        gBS->CreateEventEx(
                          EVT_NOTIFY_SIGNAL,
                          TPL_NOTIFY,
                          TaskProcessesGroupDateTimePrint[i],
                          NULL,
                          &gMultiProcessGroup2Guid,
                          &MultiTaskTriggerGroup2Event);
    }    

    for (i = 0; i < sizeof(TaskProcessesGroupTCPHandle) / sizeof(EFI_EVENT_NOTIFY); i++)
    {
        gBS->CreateEventEx(
                          EVT_NOTIFY_SIGNAL,
                          TPL_NOTIFY,
                          TaskProcessesGroupTCPHandle[i],
                          NULL,
                          &gMultiProcessGroup3Guid,
                          &MultiTaskTriggerGroup3Event);
    }    

    for (i = 0; i < sizeof(TaskProcessesGroupApplicationCall) / sizeof(EFI_EVENT_NOTIFY); i++)
    {
        gBS->CreateEventEx(
                          EVT_NOTIFY_SIGNAL,
                          TPL_NOTIFY,
                          TaskProcessesGroupApplicationCall[i],
                          NULL,
                          &gMultiProcessGroup4Guid,
                          &MultiTaskTriggerGroup4Event);
    }    

    

    return EFI_SUCCESS;
}
// https://blog.csdn.net/longsonssss/article/details/80221513

