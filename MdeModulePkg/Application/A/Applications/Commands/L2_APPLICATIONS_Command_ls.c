
/*************************************************
    .
    File name:      	*.*
    Author��	        	������
    ID��					00001
    Date:          		202201
    Description:    	���ڸ�Ӧ���ṩ�ӿ�
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





#include <Library/UefiRuntimeLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include "L2_APPLICATIONS_Command_ls.h"

#include <Global/Global.h>


/****************************************************************************
*
*  ����:   ����ϵͳ����Ӧ�ó���
*
*  pELF_Buffer     ELF�ļ������ڴ�洢
*  pReturnCode��    ���ش�ELF�ļ������Ļ�����
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_APPLICATIONS_Command_ls(UINT8 parameters[PARAMETER_COUNT][PARAMETER_LENGTH], UINT8 *pReturnCode)
{
    UINT8 j = 0;
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_APPLICATIONS_Command_ls:%a \n", __LINE__, parameters[1]);
	
	int TerminalWindowMaxLineCount = 0;
	TerminalWindowMaxLineCount = (ScreenHeight - 23) / 2;
	TerminalWindowMaxLineCount /= 16;
	
    for (UINT16 i = 0 ; i < PartitionCount; i++)
    {        
    	//д����̻��浽�ն˴��ڡ�
	    L2_DEBUG_Print3(3, 23 + (++TerminalCurrentLineCount) % TerminalWindowMaxLineCount * 16, WindowLayers.item[GRAPHICS_LAYER_TERMINAL_WINDOW], "%a", device[i].PartitionName);
	   
        //L2_DEBUG_Print3(x + 10, y, WindowLayers.item[GRAPHICS_LAYER_MY_COMPUTER_WINDOW], "%a", type);        
    }
        
}



