
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
#include "L2_APPLICATIONS_Command_ping.h"
#include "Ping.h"
#include "DxeNetLib.h"

#include <Global/Global.h>

UINTN
EFIAPI
AsciiAtoi(
    CHAR8  *Str
    )
{
    UINTN    RetVal;
    CHAR8    TempChar;
    UINTN    MaxVal;
    UINTN    ResteVal;

    ASSERT( Str != NULL );

    MaxVal        = (UINTN) -1 / 10;
    ResteVal    = (UINTN) -1 % 10;
    /*
     *
     * skip preceeding white space
     *
     */
    while ( *Str != '\0' && *Str == ' ' )
    {
        Str += 1;
    }
    /*
     *
     * convert digits
     *
     */
    RetVal        = 0;
    TempChar    = *(Str++);
    while ( TempChar != '\0' )
    {
        if ( TempChar >= '0' && TempChar <= '9' )
        {
            if ( RetVal > MaxVal || (RetVal == MaxVal && TempChar - '0' > (INTN) ResteVal) )
            {
                return( (UINTN) -1);
            }

            RetVal = (RetVal * 10) + TempChar - '0';
        }         else         {
            break;
        }

        TempChar = *(Str++);
    }

    return(RetVal);
}


/* ipv4地址转换 */
int ipv4_to_i( const char *ip, EFI_IPv6_ADDRESS *DstAddress )
{
    char        str_ip_index[4] = { '\0' };
    unsigned int    ip_int;
    unsigned int    j = 0, i = 0, k = 0;;


    for ( i = 0; i < 16; i++ )
    {
        if ( ip[i] == '\0' || ip[i] == '.' )
        {
            ip_int = AsciiAtoi( str_ip_index );
            /* printf("%d\n", ip_int); */
            DstAddress->Addr[k++] = ip_int;
            if ( ip_int > 255 )
                return(0);

            /* memset(str_ip_index, 0, sizeof(str_ip_index)); */
            L1_MEMORY_SetValue( str_ip_index, 0, 4 );

            j = 0;
            continue;
        }

        str_ip_index[j] = ip[i];
        j++;
    }

    return(1);
}


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
EFI_STATUS L2_APPLICATIONS_Command_ping( UINT8 parameters[PARAMETER_COUNT][PARAMETER_LENGTH], UINT8 *pReturnCode )
{
    UINT8 j = 0;
    L2_DEBUG_Print3( DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_APPLICATIONS_Command_ping:%a \n", __LINE__, parameters[1] );


    /*
     * ShellCommandRunPing
     * ShellPing, SHELL PING源码实现
     */
    EFI_STATUS        Status;
    SHELL_STATUS        ShellStatus;
    EFI_IPv6_ADDRESS    DstAddress;
    EFI_IPv6_ADDRESS    SrcAddress;
    UINT64            BufferSize;
    UINTN            SendNumber;
    LIST_ENTRY        *ParamPackage;
    CONST CHAR16        *ValueStr;
    UINTN            NonOptionCount;

    ZeroMem( &SrcAddress, sizeof(EFI_IPv6_ADDRESS) );
    ZeroMem( &DstAddress, sizeof(EFI_IPv6_ADDRESS) );

    /*
     *
     * Parse the parameter of source ip address.
     *
     */
    ValueStr = ShellCommandLineGetValue( ParamPackage, L"-s" );
    if ( ValueStr == NULL )
    {
        ValueStr = ShellCommandLineGetValue( ParamPackage, L"-_s" );
    }

    /*
     * Status = NetLibStrToIp6 (L"192.168.3.3", &DstAddress);
     * Status = NetLibStrToIp6 (L"192.168.3.3", &DstAddress);
     */
    Status = NetLibStrToIp4( L"192.168.3.4", (EFI_IPv4_ADDRESS *) &SrcAddress );
    CHAR16 DestIP[16] = { L"0" };
    L1_STRING_AsciiStringToWchar( parameters[1], DestIP, 16 );

    unsigned char ArrayIP[4] = { 0 };
    ipv4_to_i( parameters[1], &DstAddress );


    L2_DEBUG_Print3( DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_APPLICATIONS_Command_ping:%a \n", __LINE__, parameters[1] );

    L2_DEBUG_Print3( DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", __LINE__,
             DstAddress.Addr[0],
             DstAddress.Addr[1],
             DstAddress.Addr[2],
             DstAddress.Addr[3],
             DstAddress.Addr[4],
             DstAddress.Addr[5],
             DstAddress.Addr[6],
             DstAddress.Addr[7],
             DstAddress.Addr[8],
             DstAddress.Addr[9],
             DstAddress.Addr[10],
             DstAddress.Addr[11],
             DstAddress.Addr[12],
             DstAddress.Addr[13],
             DstAddress.Addr[14],
             DstAddress.Addr[15] );

    ShellStatus = ShellPing(2,
                            16,
                            &SrcAddress,
                            &DstAddress,
                            PING_IP_CHOICE_IP4);
}


