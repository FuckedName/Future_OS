
/*************************************************
*   .
*   File name:          *.*
*   Author��                    ������
*   ID��                    00001
*   Date:                       202201
*   Description:        ���ڸ�Ӧ���ṩ�ӿ�
*   Others:             ��
*
*   History:            ��
*    1.  Date:
*     Author:
*     ID:
*     Modification:
*
*    2.  Date:
*     Author:
*     ID:
*     Modification:
*************************************************/


#include <Library/UefiRuntimeLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include "L2_APPLICATIONS_Command_curl.h"

#include <Graphics/L2_GRAPHICS.h>
#include <Global/Global.h>

#include "Http.h"



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
EFI_STATUS L2_APPLICATIONS_Command_http( UINT8 parameters[PARAMETER_COUNT][PARAMETER_LENGTH], UINT8 *pReturnCode )
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d %a \n", __LINE__, __FUNCTION__);
    
    UINT8 j = 0;
    UINT32 waitIndex = 0;

	RunHttp(gSystemHandle, gSystemTable);
        
}

