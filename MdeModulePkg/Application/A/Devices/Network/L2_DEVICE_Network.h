
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


typedef struct MyTCP4Socket{
		EFI_HANDLE                     m_SocketHandle;                   
		EFI_TCP4_PROTOCOL*             m_pTcp4Protocol;
		EFI_TCP4_CONFIG_DATA*          m_pTcp4ConfigData;
		EFI_TCP4_TRANSMIT_DATA*        m_TransData;
		EFI_TCP4_RECEIVE_DATA*         m_RecvData; 
		EFI_TCP4_CONNECTION_TOKEN      ConnectToken;
		EFI_TCP4_CLOSE_TOKEN           CloseToken;
		EFI_TCP4_IO_TOKEN              SendToken;
		EFI_TCP4_IO_TOKEN              RecvToken;
        int stub;   //robin add for debug
}MYTCP4SOCKET;

static MYTCP4SOCKET* TCP4SocketFd;

BOOLEAN L2_TCP4_ReceiveFinished();

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


