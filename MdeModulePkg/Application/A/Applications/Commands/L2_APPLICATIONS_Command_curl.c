
/*************************************************
*   .
*   File name:          *.*
*   Author��	                ������
*   ID��					00001
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
#include <Library/ShellCommandLib.h>
#include <Library/ShellLib.h>

#include <Uefi.h>

#include <Guid/ShellLibHiiGuid.h>

#include <Protocol/Cpu.h>
#include <Protocol/ServiceBinding.h>
#include <Protocol/Ip6.h>
#include <Protocol/Ip6Config.h>
#include <Protocol/Ip4.h>
#include <Protocol/Ip4Config2.h>
#include <Protocol/Arp.h>

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
/* #include <Library/NetLib.h> */
#include <Library/DevicePathLib.h>
#include <Library/PrintLib.h>


#include <Devices/Network/L2_DEVICE_Network.h>

#include "L2_APPLICATIONS_Command_curl.h"

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
EFI_STATUS L2_APPLICATIONS_Command_curl( UINT8 parameters[PARAMETER_COUNT][PARAMETER_LENGTH], UINT8 *pReturnCode )
{
	UINT8 j = 0;
	L2_DEBUG_Print3( DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_APPLICATIONS_Command_curl:%a \n", __LINE__, parameters[1] );


	/* д����̻��浽�ն˴��ڡ� */
	//L2_DEBUG_Print3( 3, 23 + (++TerminalCurrentLineCount) % TerminalWindowMaxLineCount * 16, WindowLayers.item[GRAPHICS_LAYER_TERMINAL_WINDOW], "%a", "Curl" );

	/*
	 * HttpCreateService;
	 * HttpDxeStart
	 * RunHttp2
	 * DownloadFile
	 */

	/* infer from: RedfishRestExDriverBindingStart */
	EFI_HANDLE	ControllerHandle;
	EFI_STATUS	Status;
	EFI_HANDLE	Handle;
	EFI_HANDLE	HttpChildHandle;

	CHAR8 *p = SendBuffer;

	L1_MEMORY_Memset(SendBuffer, 0, 4 * 1024);
	
	L2_TCP4_SetKeyValue(&p, "GET / HTTP/1.1");
	L2_TCP4_Set_r_n(&p);
    
	//Host: 192.168.0.106:8080\r\n
	L2_TCP4_SetKeyValue(&p, "Host: 192.168.3.3:8080");
    L2_TCP4_Set_r_n(&p);



	L2_TCP4_SetKeyValue(&p, "Connection: keep-alive");
    L2_TCP4_Set_r_n(&p);


	L2_TCP4_SetKeyValue(&p, "Cache-Control: max-age=0");
    L2_TCP4_Set_r_n(&p);

	L2_TCP4_SetKeyValue(&p, "Upgrade-Insecure-Requests: 1");
    L2_TCP4_Set_r_n(&p);

	L2_TCP4_SetKeyValue(&p, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9");
    L2_TCP4_Set_r_n(&p);


	L2_TCP4_SetKeyValue(&p, "Accept-Encoding: gzip, deflate, br");
    L2_TCP4_Set_r_n(&p);

	L2_TCP4_SetKeyValue(&p, "sec-ch-ua: \"Chromium\";v=\"88\", \"Google Chrome\";v=\"88\", \";Not A Brand\";v=\"99\"");
    L2_TCP4_Set_r_n(&p);
    
	L2_TCP4_SetKeyValue(&p, "sec-ch-ua-mobile: ?0");
    L2_TCP4_Set_r_n(&p);
    
	L2_TCP4_SetKeyValue(&p, "Sec-Fetch-Site: none");
    L2_TCP4_Set_r_n(&p);
    
	L2_TCP4_SetKeyValue(&p, "Sec-Fetch-Mode: navigate");
    L2_TCP4_Set_r_n(&p);
    
	L2_TCP4_SetKeyValue(&p, "Sec-Fetch-User: ?1");
    L2_TCP4_Set_r_n(&p);
    
	L2_TCP4_SetKeyValue(&p, "Sec-Fetch-Dest: document");
    L2_TCP4_Set_r_n(&p);

	L2_TCP4_SetKeyValue(&p, "Accept-Language: zh-CN,zh;q=0.9");
    L2_TCP4_Set_r_n(&p);

	L2_TCP4_SetKeyValue(&p, "If-None-Match: W/\"3-1616330370359\"");
    L2_TCP4_Set_r_n(&p);

	L2_TCP4_SetKeyValue(&p, "If-Modified-Since: Sun, 21 Mar 2021 12:39:30 GMT");
    L2_TCP4_Set_r_n(&p);

    
	//User-Agent: Mozilla/4.0 (compatible)\r\n\r\n
	L2_TCP4_SetKeyValue(&p, "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/88.0.4324.182 Safari/537.36");
	L2_TCP4_Set_r_n(&p);
	L2_TCP4_Set_r_n(&p);
	

    *p = '\0';    
    
	int i = 0;
	while(i++ < 3)
	{
		L2_TCP4_Send();
		for (int j= 0; j < 6000000;j++);
		L2_TCP4_Receive();
		for (int j= 0; j < 6000000;j++);
	}

	//ShellCurl(2, 1024, "", "", PING_IP_CHOICE_IP4_2);
}


