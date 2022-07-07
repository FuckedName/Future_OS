
/*************************************************
*   .
*   File name:          *.*
*   Author：	                任启红
*   ID：					00001
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


#include <Devices/Network/L2_DEVICE_Network.h>


#include "L2_APPLICATIONS_Command_ping.h"

#include <Global/Global.h>




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
	EFI_STATUS		Status;
	SHELL_STATUS		ShellStatus;
	EFI_IPv6_ADDRESS	DstAddress;
	EFI_IPv6_ADDRESS	SrcAddress;
	UINT64			BufferSize;
	UINTN			SendNumber;
	LIST_ENTRY		*ParamPackage;
	CONST CHAR16		*ValueStr;
	UINTN			NonOptionCount;

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
	Status = NetLibStrToIp42( L"192.168.3.4", (EFI_IPv4_ADDRESS *) &SrcAddress );
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


	ShellStatus = ShellPing2(2,
							16,
							&SrcAddress,
							&DstAddress,
							PING_IP_CHOICE_IP4_2);
}


