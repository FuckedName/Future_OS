
/*************************************************
    .
    File name:      	*.*
    Author：	        	任启红
    ID：					00001
    Date:          		202107
    Description:    	
    Others:         	无

    History:        	无
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
		EFI_TCP4_IO_TOKEN              SendToken, RecvToken;
        int stub;   //robin add for debug
}MYTCP4SOCKET;

static MYTCP4SOCKET* TCP4SocketFd;

EFI_STATUS TCP4Test();

UINTN CreateTCP4Socket(VOID);

EFI_STATUS InitTcp4SocketFd();

VOID  Tcp4SendNotify(IN EFI_EVENT  Event,  IN VOID *Context);


VOID  Tcp4RecvNotify(IN EFI_EVENT  Event,  IN VOID *Context);


VOID NopNoify(  IN EFI_EVENT  Event, IN VOID *Context);

EFI_STATUS ConfigTCP4Socket(UINT32 Ip32, UINT16 Port);

EFI_STATUS ConnectTCP4Socket();

EFI_STATUS SendTCP4Socket(CHAR8* Data, UINTN Lenth);


EFI_STATUS RecvTCP4Socket(IN CHAR8* Buffer, IN UINTN Length, OUT UINTN *recvLength);

