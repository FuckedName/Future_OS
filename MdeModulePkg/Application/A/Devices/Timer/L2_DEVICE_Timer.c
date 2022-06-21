
/*************************************************
    .
    File name:      	*.*
    Author��	        	������
    ID��					00001
    Date:          		202107
    Description:    	
    Others:         	��

    History:        	��
	    1.  Date:
		    Author: 
		    ID:
		    Modification:
		    
	    2.  Date:
		    Author: 
		    ID:
		    Modification:
*************************************************/





#include <Library/MemoryAllocationLib.h>


#include <Library/UefiRuntimeLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <Processes/L2_PROCESS_Multi.h>
#include <Devices/Screen/L2_DEVICE_Screen.h>
#include <Devices/System/L2_DEVICE_System.h>

#include <Graphics/L2_GRAPHICS.h>
#include "L2_DEVICE_Timer.h"


EFI_HANDLE  TimerOne    = NULL;


/****************************************************************************
*
*  ����:   ��ʱ����ʼ�����������д�Ĳ��ã�����̫���ˣ��л����Ż��¡�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_TIMER_IntervalInit()
{
    EFI_STATUS  Status;
    static const UINTN TimeInterval = 20000;
    
    UINT32 *TimerCount;

    // initial with 0
    TimerCount = (UINT32 *)AllocateZeroPool(4);
    if (NULL == TimerCount)
    {
        //DEBUG(( EFI_D_INFO, "%d, NULL == TimerCount \r\n", __LINE__));
        return -1;
    }

	//����ʱ��Ƭ�ص�����L2_TIMER_Slice
    Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL | EVT_TIMER,
                            TPL_CALLBACK,
                            L2_TIMER_Slice,
                            (VOID *)TimerCount,
                            &TimerOne);

    if ( EFI_ERROR( Status ) )
    {
        //DEBUG(( EFI_D_INFO, "Create Event Error! \r\n" ));
        return(1);
    }

    Status = gBS->SetTimer(TimerOne,
                          TimerPeriodic,
                          MultU64x32( TimeInterval, 1)); // will multi 100, ns

    if ( EFI_ERROR( Status ) )
    {
        //DEBUG(( EFI_D_INFO, "Set Timer Error! \r\n" ));
        return(2);
    }
    UINT32 QuitTimerCount = 0;
    UINT32 SystemQuitCount = 0;
    while (1)
    {
        *TimerCount = *TimerCount + 1;
        //L2_DEBUG_Print1(DISPLAY_X, DISPLAY_Y, "%d: SystemTimeIntervalInit while\n", __LINE__);
        //if (*TimerCount % 1000000 == 0)
        L2_DEBUG_Print1(0, ScreenHeight - 30 - 4 * 16, "%d: L2_TIMER_IntervalInit p:%x %lu %llu\n", __LINE__, TimerCount, *TimerCount, QuitTimerCount);

		//�ػ�����
        if (TRUE == SystemQuitFlag)
        {   
            if (QuitTimerCount == 0)
            {   
                QuitTimerCount = *TimerCount;
                L2_GRAPHICS_SayGoodBye();
            }
        }

        SystemQuitCount = *TimerCount - QuitTimerCount;

		//�ػ�����
        if ((TRUE == SystemQuitFlag) && ( SystemQuitCount % 100 == 0))
        {
            UINT16 count = SystemQuitCount / 100;
            
            L2_DEBUG_Print1(0, ScreenHeight - 30 -  5 * 16, "%d: L2_TIMER_IntervalInit p:%x %lu \n", __LINE__, TimerCount, *TimerCount);
            
            for (UINT16 j = ScreenHeight / 4 ; j < ScreenHeight / 3; j++)
            {
                for (UINT16 i = count * 100; i < (count + 1) * 100; i++)
                {
                    pDeskBuffer[(j * ScreenWidth + i) * 4]     = 0xff;
                    pDeskBuffer[(j * ScreenWidth + i) * 4 + 1] = 0xff;
                    pDeskBuffer[(j * ScreenWidth + i) * 4 + 2] = 0;
                }
            }
			
			L2_SCREEN_Draw(pDeskBuffer, 0, 0, 0, 0, ScreenWidth, ScreenHeight);	   
        }

		//�ػ�������ɺ�Ĺػ�����
        if ((TRUE == SystemQuitFlag) && ( SystemQuitCount >= ScreenWidth))
        {
            L2_DEBUG_Print1(0, ScreenHeight - 30 - 6 * 16, "%d: L2_TIMER_IntervalInit p:%x %lu \n", __LINE__, TimerCount, *TimerCount);
            
            gBS->SetTimer( TimerOne, TimerCancel, 0 );
            gBS->CloseEvent( TimerOne );  
            L2_System_Shutdown();  
        }
                   
    }
    
    return EFI_SUCCESS;
}



