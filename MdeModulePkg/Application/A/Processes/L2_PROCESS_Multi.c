
/*************************************************
    .
    File name:          *.*
    Author��                ������
    ID��                    00001
    Date:                  202107
    Description:        
    Others:             ��

    History:            ��
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

#include <Applications/L2_APPLICATIONS.h>

#include "L2_INTERFACES.h"


EFI_EVENT MultiTaskTriggerGroupEventSystem;
EFI_EVENT MultiTaskTriggerGroupEventDateTimePrint;
EFI_EVENT MultiTaskTriggerGroupEventTCPHandle;
EFI_EVENT MultiTaskTriggerGroupEventApplicationCall;
EFI_EVENT MultiTaskTriggerGroupEventSystemResponse;
UINT8 *pApplication = NULL;

VOID (*pFunction)();


//ʱ��Ƭ�������������ֲ�ͬ����Ƶ�ʵ�����
UINT32 TimerSliceCount = 0;



/****************************************************************************
*
*  ����:   xxxxx
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
L2_SYSTEM_Start (IN EFI_EVENT Event, IN VOID *Context)
{

}


/****************************************************************************
*
*  ����:   ����ϵͳ��ʱ��Ƭ��������ȫ���������������
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID EFIAPI L2_TIMER_Slice(
    IN EFI_EVENT Event,
    IN VOID           *Context
    )
{
    L2_DEBUG_Print1(0, ScreenHeight - 30 - 5 * 16, "%d: TimeSlice %x %lu \n", __LINE__, Context, *((UINT32 *)Context));

    //ʱ��Ƭ����
    L2_DEBUG_Print1(0, ScreenHeight - 30 - 6 * 16, "%d: TimerSliceCount: %lu \n", __LINE__, TimerSliceCount);
    //Print(L"%lu\n", *((UINT32 *)Context));

    //��һ���¼���������ꡢ���̣�������ҪƵ�ʸ��ߵ�����
    if (TimerSliceCount % 10 == 0)
    {
        gBS->SignalEvent (MultiTaskTriggerGroupEventSystem);
        gBS->SignalEvent (MultiTaskTriggerGroupEventTCPHandle);        
    }

    //�ڶ����¼���������Ļ���½���ʾ���ڡ�ʱ�䡢���ڼ�������Ƶ�ʿ��Ե�Щ�Խ�ԼCPU��Դ
    if (TimerSliceCount % 20 == 0)
       gBS->SignalEvent (MultiTaskTriggerGroupEventDateTimePrint);

    //ϵͳ���������Ϊ�㣬��ʾӦ�ò���ϵͳ���ã��򴥷���Ӧ���¼��顣
    //if (0 != *APPLICATION_CALL_FLAG_ADDRESS)
    if (TimerSliceCount == 100)
    {
        gBS->SignalEvent(MultiTaskTriggerGroupEventApplicationCall);
    }

    if (APPLICATION_CALL_ID_INIT != pApplicationCallData->ID && ApplicationRunFinished)
    {
        gBS->SignalEvent (MultiTaskTriggerGroupEventSystemResponse);
        pApplicationCallData->ID = APPLICATION_CALL_ID_INIT;
    }
    
    ////DEBUG ((EFI_D_INFO, "System time slice Loop ...\n"));
    //gBS->SignalEvent (MultiTaskTriggerEvent);

    TimerSliceCount++;
    return;
}

/****************************************************************************
*
*  ����:   ������飬ÿ�����������ж�����̣���������������ϵͳ�Ľ�����Щ���
*        ��ǰϵͳ�Ķ���̻���Щ���⣬���磺������ָ���Ľ�����ָ�����ڴ��ִַ�С�
*

*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_COMMON_MultiProcessInit ()
{
    UINT16 i;

    // task group for mouse keyboard
    EFI_GUID gMultiProcessGroupGuidSystem  = { 0x0579257E, 0x1843, 0x45FB, { 0x83, 0x9D, 0x6B, 0x79, 0x09, 0x38, 0x29, 0xA9 } };
    
    // task group for display date time
    EFI_GUID gMultiProcessGroupGuidDateTimePrint  = { 0x0579257E, 0x1843, 0x45FB, { 0x83, 0x9D, 0x6B, 0x79, 0x09, 0x38, 0x29, 0xAA } };
    
    EFI_GUID gMultiProcessGroupGuidTCPHandle  = { 0x0579257E, 0x1843, 0x45FB, { 0x83, 0x9D, 0x6B, 0x79, 0x09, 0x38, 0x29, 0xAB } };
    
    EFI_GUID gMultiProcessGroupGuidApplicationCall  = { 0x0579257E, 0x1843, 0x45FB, { 0x83, 0x9D, 0x6B, 0x79, 0x09, 0x38, 0x29, 0xAC } };
    
    EFI_GUID gMultiProcessGroupGuidSystemResponse  = { 0x0579257E, 0x1843, 0x45FB, { 0x83, 0x9D, 0x6B, 0x79, 0x09, 0x38, 0x29, 0xAD } };

    //ϵͳ�¼������̴�����괦��
    EFI_EVENT_NOTIFY       TaskProcessesGroupSystem[] = {L2_KEYBOARD_Event, L2_MOUSE_Event};

    //ϵͳ���ں�ʱ�����������½���ʾ
    EFI_EVENT_NOTIFY       TaskProcessesGroupDateTimePrint[] = {L2_TIMER_Print};

    //TCPͨ�Ŵ�������¼�
    EFI_EVENT_NOTIFY       TaskProcessesGroupTCPHandle[] = {L2_TCP4_HeartBeatNotify, L2_TCP4_ReceiveNotify, L2_TCP4_SendNotify};

    //step 1: run application 
    //EFI_EVENT_NOTIFY       TaskProcessesGroupApplicationCall[] = {L2_ApplicationRun};
    EFI_EVENT_NOTIFY       TaskProcessesGroupApplicationCall[] = {};
    
    //step 2: System respond the system call 
    EFI_EVENT_NOTIFY       TaskProcessesGroupApplicationCall2[] = {L2_INTERFACES_ApplicationCall};
    
    for (i = 0; i < sizeof(TaskProcessesGroupSystem) / sizeof(EFI_EVENT_NOTIFY); i++)
    {
        gBS->CreateEventEx(
                          EVT_NOTIFY_SIGNAL,
                          TPL_NOTIFY,
                          TaskProcessesGroupSystem[i],
                          NULL,
                          &gMultiProcessGroupGuidSystem,
                          &MultiTaskTriggerGroupEventSystem);
    }    

    for (i = 0; i < sizeof(TaskProcessesGroupDateTimePrint) / sizeof(EFI_EVENT_NOTIFY); i++)
    {
        gBS->CreateEventEx(
                          EVT_NOTIFY_SIGNAL,
                          TPL_NOTIFY,
                          TaskProcessesGroupDateTimePrint[i],
                          NULL,
                          &gMultiProcessGroupGuidDateTimePrint,
                          &MultiTaskTriggerGroupEventDateTimePrint);
    }    

    for (i = 0; i < sizeof(TaskProcessesGroupTCPHandle) / sizeof(EFI_EVENT_NOTIFY); i++)
    {
        gBS->CreateEventEx(
                          EVT_NOTIFY_SIGNAL,
                          TPL_NOTIFY,
                          TaskProcessesGroupTCPHandle[i],
                          NULL,
                          &gMultiProcessGroupGuidTCPHandle,
                          &MultiTaskTriggerGroupEventTCPHandle);
    }    

    for (i = 0; i < sizeof(TaskProcessesGroupApplicationCall) / sizeof(EFI_EVENT_NOTIFY); i++)
    {
        gBS->CreateEventEx(EVT_NOTIFY_SIGNAL,
                          TPL_CALLBACK,
                          TaskProcessesGroupApplicationCall[i],
                          NULL,
                          &gMultiProcessGroupGuidApplicationCall,
                          &MultiTaskTriggerGroupEventApplicationCall);
    }    
 
    for (i = 0; i < sizeof(TaskProcessesGroupApplicationCall2) / sizeof(EFI_EVENT_NOTIFY); i++)
    {
        gBS->CreateEventEx(EVT_NOTIFY_SIGNAL,
                          TPL_NOTIFY,
                          TaskProcessesGroupApplicationCall2[i],
                          NULL,
                          &gMultiProcessGroupGuidSystemResponse,
                          &MultiTaskTriggerGroupEventSystemResponse);
    }    

    return EFI_SUCCESS;
}
// https://blog.csdn.net/longsonssss/article/details/80221513

