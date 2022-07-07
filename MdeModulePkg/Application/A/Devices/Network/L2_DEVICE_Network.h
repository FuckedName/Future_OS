
/*************************************************
    .
    File name:      	*.*
    Author£∫	        	»Œ∆Ù∫Ï
    ID£∫					00001
    Date:          		202107
    Description:    	
    Others:         	Œﬁ

    History:        	Œﬁ
	    1.  Date:
		    Author: 
		    ID:
		    Modification:
		    
	    2.  Date:
		    Author: 
		    ID:
		    Modification:
*************************************************/




#pragma once
#include <Library/UefiLib.h>


#include <Protocol/Tcp4.h>
#include <Protocol/WiFi.h>
#include <Protocol/ServiceBinding.h>

#include <L1_DEVICE_Network.h>

#define MEDIA_STATE_DETECT_TIME_INTERVAL2 1000000U

#define DEFAULT_TIMEOUT2	5000
#define MAX_SEND_NUMBER2	10000
#define MAX_BUFFER_SIZE2	32768
#define DEFAULT_TIMER_PERIOD2	358049
#define ONE_SECOND2		10000000
#define PING_IP_CHOICE_IP4_2	1
#define PING_IP_CHOICE_IP6_2	2
#define ICMP_V6_ECHO_REQUEST2	0x80
#define DEFAULT_SEND_COUNT2	10
#define DEFAULT_BUFFER_SIZE2	16
#define ICMP_V4_ECHO_REQUEST2	0x8
#define ICMP_V4_ECHO_REPLY2	0x0
#define STALL_1_MILLI_SECOND2	1000
#define ICMP_V6_ECHO_REPLY2	0x81

typedef struct MyTCP4Socket{
		EFI_HANDLE                     m_SocketHandle;                   
		EFI_TCP4_PROTOCOL*             m_pTcp4Protocol;
		EFI_TCP4_CONFIG_DATA*          m_pTcp4ConfigData;
		EFI_TCP4_TRANSMIT_DATA*        m_TransData;
		EFI_TCP4_RECEIVE_DATA*         m_RecvData; 
		EFI_TCP4_CONNECTION_TOKEN      ConnectToken;
		EFI_TCP4_CLOSE_TOKEN           CloseToken;
		EFI_TCP4_IO_TOKEN              SendToken, RecvToken;
        int stub;   //robin add for debug
}MYTCP4SOCKET;

static MYTCP4SOCKET* TCP4SocketFd;

EFI_STATUS L2_TCP4_Send();

EFI_STATUS L2_TCP4_Receive();

UINTN L2_TCP4_SocketCreate(VOID);

EFI_STATUS L2_TCP4_SocketInit();

VOID  L2_TCP4_SendNotify(EFI_EVENT Event,      VOID *Context);

VOID  L2_TCP4_ReceiveNotify(EFI_EVENT      Event,  VOID *Context);

VOID L2_TCP4_HeartBeatNotify (EFI_EVENT  Event,    VOID *Context);

EFI_STATUS L2_TCP4_SocketConfig();

EFI_STATUS L2_TCP4_SocketConnect();

EFI_STATUS L2_TCP4_SocketSend(CHAR8* Data, UINTN Lenth);

EFI_STATUS L2_TCP4_SocketReceive(CHAR8* Buffer, UINTN Length, UINTN *recvLength);

EFI_STATUS WirelessMAC();

BOOLEAN
EFIAPI
NetIp6IsUnspecifiedAddr2(
	IN EFI_IPv6_ADDRESS       *Ip6
	);

