
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




#include <Library/UefiLib.h>
#include <Library/BaseLib.h>

#include <string.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Global/Global.h>



#include <L2_DEVICE_Network.h>


#define IPV4_TO_LONG(a,b,c,d) (a | b<<8 | c << 16 | d <<24)
#define INFO(...)   \
				do {   \
				     if (0)\
					 {\
					 Print(L"[INFO  ]%a %a(Line %d): ", __FILE__,__FUNCTION__,__LINE__);  \
				    Print(__VA_ARGS__); \
				    }\
				}while(0);


VOID  Tcp4SendNotify(EFI_EVENT Event,      VOID *Context)
{
     MYTCP4SOCKET *CurSocket = (MYTCP4SOCKET *)(Context);
     
     //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Tcp4SendNotify \n", __LINE__);

     //INFO(L"Tcp4SendNotify: stub=%x\n", (int)CurSocket->stub);
     //INFO(L"Tcp4SendNotify: Context=%p\n", Context);
}

// stub funciton
VOID NopNoify (EFI_EVENT  Event,    VOID *Context)
{
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d NopNoify \n", __LINE__);
    
}


VOID  Tcp4RecvNotify(EFI_EVENT      Event,  VOID *Context)
{
     MYTCP4SOCKET *CurSocket = (MYTCP4SOCKET *)(Context);

     //INFO(L"Tcp4RecvNotify: stub=%x\n", (int)CurSocket->stub);
     //INFO(L"Tcp4RecvNotify: Context=%p\n", Context);
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Tcp4RecvNotify \n", __LINE__);
}

EFI_STATUS InitTcp4SocketFd()
{
    EFI_STATUS                           Status;
    MYTCP4SOCKET *CurSocket = TCP4SocketFd;

    // 1 Create Configure data
    CurSocket->m_pTcp4ConfigData = L2_MEMORY_Allocate("Network TCP config Buffer", MEMORY_TYPE_NETWORK, sizeof(EFI_TCP4_CONFIG_DATA));
    if (NULL == CurSocket->m_pTcp4ConfigData)
    {
    	return;
    }
    
    // 2 Create Connect Event
    Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL, TPL_CALLBACK, (EFI_EVENT_NOTIFY)NopNoify , (VOID*)&CurSocket->ConnectToken, &CurSocket->ConnectToken.CompletionToken.Event );
    if(EFI_ERROR(Status)) 
    {
        return Status;  
    }
    
    // 3 Create Transmit Event
    Status = gBS->CreateEvent(EVT_NOTIFY_WAIT, TPL_CALLBACK, (EFI_EVENT_NOTIFY)Tcp4SendNotify , (VOID*)CurSocket, &CurSocket->SendToken.CompletionToken.Event);
    if(EFI_ERROR(Status)) 
    {
        return Status;     
    }
    
    CurSocket->m_TransData = L2_MEMORY_Allocate("Network TCP4 Transmit Buffer", MEMORY_TYPE_NETWORK, sizeof(EFI_TCP4_TRANSMIT_DATA));
    
    // 4 Create Recv Event
    Status = gBS->CreateEvent(EVT_NOTIFY_WAIT, TPL_CALLBACK, (EFI_EVENT_NOTIFY)Tcp4RecvNotify , (VOID*)CurSocket, &CurSocket->RecvToken.CompletionToken.Event);
    
    CurSocket->m_RecvData = L2_MEMORY_Allocate("Network Receive Buffer", MEMORY_TYPE_NETWORK, sizeof(EFI_TCP4_RECEIVE_DATA));
    if(EFI_ERROR(Status)) 
    {
        gST->ConOut->OutputString(gST->ConOut,L"Init: Create Recv Event fail!\n\r");
        return Status;
    }   
    
    // 5 Create Close Event
    // CurSocket->CloseToken.CompletionToken.Status = EFI_ABORTED;
    //Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL, TPL_CALLBACK, (EFI_EVENT_NOTIFY)NopNoify , (VOID*)&CurSocket->CloseToken, &CurSocket->CloseToken.CompletionToken.Event );
    //if(EFI_ERROR(Status))
    //{
    //    gST->ConOut->OutputString(gST->ConOut,L"Init: Create Close Event fail!\n\r");
    //    return Status;
    //}
    return Status;
}


UINTN CreateTCP4Socket(VOID)
{
    EFI_STATUS                           Status;
    EFI_SERVICE_BINDING_PROTOCOL*  pTcpServiceBinding;
	
    TCP4SocketFd = L2_MEMORY_Allocate("Network Receive Buffer", MEMORY_TYPE_NETWORK, sizeof(MYTCP4SOCKET));
    if (TCP4SocketFd == NULL)
    {
    	return EFI_SUCCESS;
    }
            
    //gBS->SetMem((void*)TCP4SocketFd, 0, sizeof(MYTCP4SOCKET));
    TCP4SocketFd->m_SocketHandle  = NULL;
    Status = gBS->LocateProtocol(&gEfiTcp4ServiceBindingProtocolGuid,
							        NULL,
							        (VOID **)&pTcpServiceBinding);
    if(EFI_ERROR(Status))
    {
        return Status;
    }
    
    Status = pTcpServiceBinding->CreateChild ( pTcpServiceBinding, &TCP4SocketFd->m_SocketHandle);
    if(EFI_ERROR(Status))
    {
        return Status;
    }

    Status = gBS->OpenProtocol(TCP4SocketFd->m_SocketHandle,
							        &gEfiTcp4ProtocolGuid,
							        (VOID **)&TCP4SocketFd->m_pTcp4Protocol,
							        gImageHandle,
							        TCP4SocketFd->m_SocketHandle,
							        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL );							        
    if(EFI_ERROR(Status))
    {
        return Status;
    }

    //给初始化结构体指针分配内存
    InitTcp4SocketFd();
    	
    return 0;
}


EFI_STATUS ConfigTCP4Socket()
{
    EFI_STATUS Status = EFI_NOT_FOUND;
    MYTCP4SOCKET *CurSocket = TCP4SocketFd;

    if(CurSocket->m_pTcp4ConfigData == NULL)
    {
        return Status;
    }
    
    CurSocket->m_pTcp4ConfigData->TypeOfService = 0;
    CurSocket->m_pTcp4ConfigData->TimeToLive = 16;    
    
    //配置本地IP地址，这个接口是第一次使用，不知道行不行
    *(UINTN*)(CurSocket->m_pTcp4ConfigData->AccessPoint.StationAddress.Addr) = IPV4_TO_LONG(10, 152, 148, 201);

    //配置本地端口
    CurSocket->m_pTcp4ConfigData->AccessPoint.StationPort = 61558;

    //配置远端IP地址，
    *(UINTN*)(CurSocket->m_pTcp4ConfigData->AccessPoint.RemoteAddress.Addr) = IPV4_TO_LONG(10, 152, 148, 200);
    
    //配置远端端口
    CurSocket->m_pTcp4ConfigData->AccessPoint.RemotePort = 8888;
    
    *(UINT32*)(CurSocket->m_pTcp4ConfigData->AccessPoint.SubnetMask.Addr) = (255 | 255 << 8 | 255 << 16 | 0 << 24) ;

    /// if UseDefaultAddress is FALSE， config StationAddress 
    CurSocket->m_pTcp4ConfigData->AccessPoint.UseDefaultAddress = FALSE;

    CurSocket->m_pTcp4ConfigData->AccessPoint.ActiveFlag = TRUE;
    CurSocket->m_pTcp4ConfigData->ControlOption = NULL;
    
    Status = CurSocket->m_pTcp4Protocol->Configure(CurSocket->m_pTcp4Protocol, CurSocket->m_pTcp4ConfigData);
    
    return Status;
}

//连接服务器端
EFI_STATUS ConnectTCP4Socket()
{
    EFI_STATUS Status = EFI_NOT_FOUND;
    MYTCP4SOCKET *CurSocket = TCP4SocketFd;
    UINTN waitIndex=0;


    if(CurSocket->m_pTcp4Protocol == NULL) 
    {
        return Status;
    }
    
    Status = CurSocket->m_pTcp4Protocol->Connect(CurSocket->m_pTcp4Protocol, &CurSocket->ConnectToken);
    //INFO(L"%r\n", Status);
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d :%r \n", __LINE__,  Status);
   
    
    //if(EFI_ERROR(Status))    
    //{
    //    return Status;
    //}

    // 就算这里失败，也可以继续往下运行。
    Status = gBS->WaitForEvent(1, &(CurSocket->ConnectToken.CompletionToken.Event), &waitIndex);
    //INFO(L"Connect: WaitForEvent, %r\n", Status);
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d :%r \n", __LINE__,  Status);
   
    // if( !EFI_ERROR(Status)){
    //     gST->ConOut->OutputString(gST->ConOut,L"Connect: WaitForEvent fail!\n\r");
    //     Status = CurSocket->ConnectToken.CompletionToken.Status;
    // }
    return Status;
}


EFI_STATUS SendTCP4Socket(CHAR8* Data, UINTN Lenth)
{
    EFI_STATUS Status = EFI_NOT_FOUND;
    MYTCP4SOCKET *CurSocket = TCP4SocketFd;
    UINTN waitIndex = 0;

    if(CurSocket->m_pTcp4Protocol == NULL) 
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Send: m_Tcp4Protocol is NULL \n", __LINE__);
   
        return Status;  
    }
    
    CurSocket->m_TransData->Push = TRUE;
    CurSocket->m_TransData->Urgent = TRUE;
    CurSocket->m_TransData->DataLength = (UINT32)Lenth;
    CurSocket->m_TransData->FragmentCount = 1;
    CurSocket->m_TransData->FragmentTable[0].FragmentLength =CurSocket->m_TransData->DataLength;
    CurSocket->m_TransData->FragmentTable[0].FragmentBuffer =Data;
    CurSocket->SendToken.Packet.TxData=  CurSocket->m_TransData;

    //Queues outgoing data into the transmit queue.
    Status = CurSocket->m_pTcp4Protocol->Transmit(CurSocket->m_pTcp4Protocol, &CurSocket->SendToken);
    if(EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Send: Transmit fail! \n", __LINE__);
   
        return Status;
    }
                
	Status = gBS->WaitForEvent(1, &(CurSocket->SendToken.CompletionToken.Event), &waitIndex);
    return CurSocket->SendToken.CompletionToken.Status;
}

EFI_STATUS RecvTCP4Socket(CHAR8* Buffer, UINTN Length, UINTN *recvLength)
{
    EFI_STATUS Status = EFI_NOT_FOUND;
    MYTCP4SOCKET *CurSocket = TCP4SocketFd;
    UINTN waitIndex = 0;

    if(CurSocket->m_pTcp4Protocol == NULL) 
    {
        return Status;
    }

    CurSocket->m_RecvData->UrgentFlag = TRUE;
    CurSocket->m_RecvData->DataLength = (UINT32)Length;
    CurSocket->m_RecvData->FragmentCount = 1;
    CurSocket->m_RecvData->FragmentTable[0].FragmentLength = CurSocket->m_RecvData->DataLength ;
    CurSocket->m_RecvData->FragmentTable[0].FragmentBuffer = (void*)Buffer;
    
    CurSocket->RecvToken.Packet.RxData=  CurSocket->m_RecvData;

    //Places an asynchronous receive request into the receiving queue.
    Status = CurSocket->m_pTcp4Protocol->Receive(CurSocket->m_pTcp4Protocol, &CurSocket->RecvToken);
    
    if(EFI_ERROR(Status))
    {
        //gST->ConOut->OutputString(gST->ConOut,L"Recv: Receive fail!\n\r");
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Recv: Receive fail! \n", __LINE__);
   
        return Status;
    }
    
    Status = gBS->WaitForEvent(1, &(CurSocket->RecvToken.CompletionToken.Event), &waitIndex);
    //INFO(L"%d Recv: WaitForEvent, %r\n", __LINE__, Status);
    *recvLength = CurSocket->m_RecvData->DataLength;
   
    return CurSocket->RecvToken.CompletionToken.Status;
}

INTN DestroyTCP4Socket()
{
    EFI_STATUS Status;
    MYTCP4SOCKET *CurSocket = TCP4SocketFd;
    
    if(CurSocket->m_SocketHandle)
    {
        EFI_SERVICE_BINDING_PROTOCOL*  pTcpServiceBinding;
        Status = gBS->LocateProtocol(&gEfiTcp4ServiceBindingProtocolGuid, NULL, (VOID **)&pTcpServiceBinding );
                
        Status = pTcpServiceBinding->DestroyChild ( pTcpServiceBinding, CurSocket->m_SocketHandle);
    }
    
    if(CurSocket->ConnectToken.CompletionToken.Event)
    {
        gBS->CloseEvent(CurSocket->ConnectToken.CompletionToken.Event);    
    }
    
    if(CurSocket->SendToken.CompletionToken.Event)
    {
        gBS->CloseEvent(CurSocket->SendToken.CompletionToken.Event);    
    }
    
    if(CurSocket->RecvToken.CompletionToken.Event)
    {
        gBS->CloseEvent(CurSocket->RecvToken.CompletionToken.Event);
    }    
    
    if(CurSocket->m_pTcp4ConfigData)
    {
	    //FreePool(CurSocket->m_pTcp4ConfigData);
    }
    
    if(CurSocket->SendToken.Packet.TxData)
    {
	    //FreePool(CurSocket->SendToken.Packet.TxData);
	    CurSocket->SendToken.Packet.TxData = NULL;
    }
    
    if(CurSocket->RecvToken.Packet.RxData)
    {
	    //FreePool(CurSocket->RecvToken.Packet.RxData);
	    CurSocket->RecvToken.Packet.RxData = NULL;
    }
    return 0;
}

EFI_STATUS CloseTCP4Socket()
{
    EFI_STATUS Status;
    MYTCP4SOCKET *CurSocket = TCP4SocketFd;
	Status = CurSocket->m_pTcp4Protocol->Close(CurSocket->m_pTcp4Protocol, &CurSocket->CloseToken);
	
	DestroyTCP4Socket();
	//FreePool(CurSocket);
	TCP4SocketFd = NULL;

	return Status;
}

CHAR8 *ReceiveBuffer = NULL;
CHAR8 *SendBuffer = NULL;

EFI_STATUS TCP_Init()
{
    EFI_STATUS Status = 0;
    //CHAR8 SendBuffer[] = "Hello, I'm a client of UEFI.";

    //CHAR8 *RecvBuffer = (CHAR8 *) L2_MEMORY_Allocate(1024);
    
    //INFO(L"%d TCP4Test: SendTCP4Socket, %r\n", __LINE__, Status);
            
    CreateTCP4Socket();

    //参数配置
    ConfigTCP4Socket();   
    
    Status = ConnectTCP4Socket();  

    ReceiveBuffer = L2_MEMORY_Allocate("Network Receive Buffer", MEMORY_TYPE_NETWORK, 1024);
    SendBuffer = L2_MEMORY_Allocate("Network Send Buffer", MEMORY_TYPE_NETWORK, 10);    
    
    SendBuffer[0] = 'U';
    SendBuffer[1] = 'E';
    SendBuffer[2] = 'F';
    SendBuffer[3] = 'I';
    SendBuffer[4] = '\0';    
}

/****************************************************************************
*
*  描述:   有线网卡TCP通信测试（有很多计算机是不支持TCP协议的，需要注意）
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS TCP_Receive()
{
    UINTN recvLen = 0;
    EFI_STATUS Status = 0;

    for (UINT32 i = 0; i < 1024; i++)
        ReceiveBuffer[i] = 0;
            
    //从服务器接收数据。
    Status = RecvTCP4Socket(ReceiveBuffer, 1024, &recvLen);
    if(EFI_ERROR(Status))    
    {
        return Status;
    }
    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d TCP: Receive: %r \n", __LINE__, Status);
   
    //显示接收的数据。
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Receive raw data Length: %d %c%c%c%c\n", __LINE__, recvLen, ReceiveBuffer[0], ReceiveBuffer[1], ReceiveBuffer[2], ReceiveBuffer[3]);
   
    ReceiveBuffer[20] = '\0';
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d TCP: Receive data is: %a \n", __LINE__, ReceiveBuffer);
       
    return EFI_SUCCESS;
}


/****************************************************************************
*
*  描述:   有线网卡TCP通信测试（有很多计算机是不支持TCP协议的，需要注意）
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS TCP_Send()
{
    EFI_STATUS Status = 0;
    
    //向服务器发送数据。
    Status = SendTCP4Socket(SendBuffer, AsciiStrLen(SendBuffer));
    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d TCP: Send: %r \n", __LINE__, Status);
    if(EFI_ERROR(Status))    
    {
        return Status;
    }
    
    return EFI_SUCCESS;
}


/****************************************************************************
*
*  描述:   无线网卡测试
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS WirelessMAC()
{
    EFI_STATUS                             Status;
    EFI_WIRELESS_MAC_CONNECTION_PROTOCOL*  pWirelessMACConnection;
	
    Status = gBS->LocateProtocol (&gEfiWiFiProtocolGuid,
							        NULL,
							        (VOID **)&pWirelessMACConnection);
							        
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d  Status: %d %r \n", __LINE__, Status, Status);
    if(EFI_ERROR(Status))
    {
        return Status;
    }
    
}
