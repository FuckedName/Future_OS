
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

#include "L1_APPLICATIONS.h"

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
EFI_STATUS L2_ApplicationAnalysisELF(UINT8 *pELF_Buffer, UINT8 *pReturnCode)
{
    UINT8 j = 0;
    for(UINT16 i = 64; i < 64 + 0x1f; i++)
        pReturnCode[j++] = pELF_Buffer[i];
        
}



