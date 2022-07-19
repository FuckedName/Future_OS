
/*************************************************
*   .
*   File name:          *.*
*   Author：                    任启红
*   ID：                    00001
*   Date:                       202201
*   Description:        用于给应用提供接口
*   Others:             无
*
*   History:            无
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

#include <Global/Global.h>


typedef struct _EFI_HTTP_PROTOCOL2 EFI_HTTP_PROTOCOL2;

extern EFI_GUID gEfiHttpServiceBindingProtocolGuid;
extern EFI_GUID gEfiHttpProtocolGuid;



/****************************************************************************
 *
 *  描述:   操作系统运行应用程序
 *
 *  pELF_Buffer     ELF文件缓冲内存存储
 *  pReturnCode：    返回从ELF文件解析的机器码
 *  参数n： xxxxx
 *
 *  返回值： 成功：XXXX，失败：XXXXX
 *
 *****************************************************************************/
EFI_STATUS L2_APPLICATIONS_Command_curl( UINT8 parameters[PARAMETER_COUNT][PARAMETER_LENGTH], UINT8 *pReturnCode )
{
    UINT8 j = 0;
    UINT32 waitIndex = 0;
    
	EFI_STATUS Status = L2_TCP4_CheckSendStatus();
    L2_DEBUG_Print3( DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_APPLICATIONS_Command_curl:%a Send Status: %a\n", __LINE__, parameters[1], Status);

    
    //显示接收的数据。
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d %a: %02X %02X %02X %02X %02X %02X %02X %02X \n", 
                                                                                                                                 __LINE__,  
                                                                                                                                 __FUNCTION__, 
                                                                                                                                ReceiveBuffer[0], 
                                                                                                                                ReceiveBuffer[1], 
                                                                                                                                ReceiveBuffer[2], 
                                                                                                                                ReceiveBuffer[3], 
                                                                                                                                ReceiveBuffer[4], 
                                                                                                                                ReceiveBuffer[5], 
                                                                                                                                ReceiveBuffer[6], 
                                                                                                                                ReceiveBuffer[7]);
        

    /* 写入键盘缓存到终端窗口。 */
    //L2_DEBUG_Print3( 3, 23 + (++TerminalCurrentLineCount) % TerminalWindowMaxLineCount * 16, WindowLayers.item[GRAPHICS_LAYER_TERMINAL_WINDOW], "%a", "Curl" );

    /*
     * HttpCreateService;
     * HttpDxeStart
     * RunHttp2
     * DownloadFile
     */

    /* infer from: RedfishRestExDriverBindingStart */
    EFI_HANDLE    ControllerHandle;
    EFI_HANDLE    Handle;
    EFI_HANDLE    HttpChildHandle;

    CHAR8 *p = SendBuffer;

    

    L1_MEMORY_Memset(SendBuffer, 0, SendBufferLength);

    SendBufferLength = 0;
    
    SendBufferLength += L2_TCP4_SetKeyValue(&p, "GET / HTTP/1.1");
    L2_TCP4_Set_r_n(&p);
    
    //Host: 192.168.0.106:8080\r\n
    SendBufferLength += L2_TCP4_SetKeyValue(&p, "Host: 192.168.3.6:8080");
    SendBufferLength += L2_TCP4_Set_r_n(&p);



    SendBufferLength += L2_TCP4_SetKeyValue(&p, "Connection: keep-alive");
    SendBufferLength += L2_TCP4_Set_r_n(&p);


    SendBufferLength += L2_TCP4_SetKeyValue(&p, "Cache-Control: max-age=0");
    SendBufferLength += L2_TCP4_Set_r_n(&p);

    SendBufferLength += L2_TCP4_SetKeyValue(&p, "Upgrade-Insecure-Requests: 1");
    SendBufferLength += L2_TCP4_Set_r_n(&p);

    SendBufferLength += L2_TCP4_SetKeyValue(&p, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9");
    SendBufferLength += L2_TCP4_Set_r_n(&p);


    SendBufferLength += L2_TCP4_SetKeyValue(&p, "Accept-Encoding: gzip, deflate, br");
    SendBufferLength += L2_TCP4_Set_r_n(&p);

    SendBufferLength += L2_TCP4_SetKeyValue(&p, "sec-ch-ua: \"Chromium\";v=\"88\", \"Google Chrome\";v=\"88\", \";Not A Brand\";v=\"99\"");
    SendBufferLength += L2_TCP4_Set_r_n(&p);
    
    SendBufferLength += L2_TCP4_SetKeyValue(&p, "sec-ch-ua-mobile: ?0");
    SendBufferLength += L2_TCP4_Set_r_n(&p);
    
    SendBufferLength += L2_TCP4_SetKeyValue(&p, "Sec-Fetch-Site: none");
    SendBufferLength += L2_TCP4_Set_r_n(&p);
    
    SendBufferLength += L2_TCP4_SetKeyValue(&p, "Sec-Fetch-Mode: navigate");
    SendBufferLength += L2_TCP4_Set_r_n(&p);
    
    SendBufferLength += L2_TCP4_SetKeyValue(&p, "Sec-Fetch-User: ?1");
    SendBufferLength += L2_TCP4_Set_r_n(&p);
    
    SendBufferLength += L2_TCP4_SetKeyValue(&p, "Sec-Fetch-Dest: document");
    SendBufferLength += L2_TCP4_Set_r_n(&p);

    SendBufferLength += L2_TCP4_SetKeyValue(&p, "Accept-Language: zh-CN,zh;q=0.9");
    SendBufferLength += L2_TCP4_Set_r_n(&p);
    
    SendBufferLength += L2_TCP4_SetKeyValue(&p, "If-None-Match: W/\"3-1616330370359\"");
    SendBufferLength += L2_TCP4_Set_r_n(&p);

    SendBufferLength += L2_TCP4_SetKeyValue(&p, "If-Modified-Since: Sun, 21 Mar 2021 12:39:30 GMT");
    SendBufferLength += L2_TCP4_Set_r_n(&p);

    
    //User-Agent: Mozilla/4.0 (compatible)\r\n\r\n
    SendBufferLength += L2_TCP4_SetKeyValue(&p, "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/88.0.4324.182 Safari/537.36");
    SendBufferLength += L2_TCP4_Set_r_n(&p);
    SendBufferLength += L2_TCP4_Set_r_n(&p);
    

    *p = '\0';  

    SendBufferLength += 1;
    
    SendBufferLength = 746;
    
    UINT32 i = 0;
    
    L2_TCP4_Send();

    L2_TCP4_Receive();
    
}

