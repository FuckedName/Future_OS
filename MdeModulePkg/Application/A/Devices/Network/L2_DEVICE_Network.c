
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


VOID  L2_TCP4_SendNotify(EFI_EVENT Event,      VOID *Context)
{
     MYTCP4SOCKET *CurSocket = (MYTCP4SOCKET *)(Context);
     
     //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Tcp4SendNotify \n", __LINE__);

     //INFO(L"Tcp4SendNotify: stub=%x\n", (int)CurSocket->stub);
     //INFO(L"Tcp4SendNotify: Context=%p\n", Context);
}

// stub funciton
VOID L2_TCP4_HeartBeatNotify (EFI_EVENT  Event,    VOID *Context)
{
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d NopNoify \n", __LINE__);
    
}


VOID  L2_TCP4_ReceiveNotify(EFI_EVENT      Event,  VOID *Context)
{
     MYTCP4SOCKET *CurSocket = (MYTCP4SOCKET *)(Context);

     //INFO(L"Tcp4RecvNotify: stub=%x\n", (int)CurSocket->stub);
     //INFO(L"Tcp4RecvNotify: Context=%p\n", Context);
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Tcp4RecvNotify \n", __LINE__);
}

EFI_STATUS L2_TCP4_SocketInit()
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
    Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL, TPL_CALLBACK, (EFI_EVENT_NOTIFY)L2_TCP4_HeartBeatNotify , (VOID*)&CurSocket->ConnectToken, &CurSocket->ConnectToken.CompletionToken.Event );
    if(EFI_ERROR(Status)) 
    {
        return Status;  
    }
    
    // 3 Create Transmit Event
    Status = gBS->CreateEvent(EVT_NOTIFY_WAIT, TPL_CALLBACK, (EFI_EVENT_NOTIFY)L2_TCP4_SendNotify , (VOID*)CurSocket, &CurSocket->SendToken.CompletionToken.Event);
    if(EFI_ERROR(Status)) 
    {
        return Status;     
    }
    
    CurSocket->m_TransData = L2_MEMORY_Allocate("Network TCP4 Transmit Buffer", MEMORY_TYPE_NETWORK, sizeof(EFI_TCP4_TRANSMIT_DATA));
    
    // 4 Create Recv Event
    Status = gBS->CreateEvent(EVT_NOTIFY_WAIT, TPL_CALLBACK, (EFI_EVENT_NOTIFY)L2_TCP4_ReceiveNotify , (VOID*)CurSocket, &CurSocket->RecvToken.CompletionToken.Event);
    
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


UINTN L2_TCP4_SocketCreate(VOID)
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
    L2_TCP4_SocketInit();
    	
    return 0;
}

int L2_TCP4_SetKeyValue(char **dest, char *source)
{
	int length = L1_STRING_Length(source);
	L1_MEMORY_Copy(*dest, source, length);
	*dest += length;
	
	return length;
}

//\r\n
int L2_TCP4_Set_r_n(char **dest)
{
	int length = L1_STRING_Length("\r\n");
	L1_MEMORY_Copy(*dest, "\r\n", length);
	*dest += length;
	
	return length;
}



EFI_STATUS L2_TCP4_SocketConfig()
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
    *(UINTN*)(CurSocket->m_pTcp4ConfigData->AccessPoint.StationAddress.Addr) = IPV4_TO_LONG(192,168,3,4);

    //配置本地端口
    CurSocket->m_pTcp4ConfigData->AccessPoint.StationPort = 61558;

    //配置远端IP地址，
    *(UINTN*)(CurSocket->m_pTcp4ConfigData->AccessPoint.RemoteAddress.Addr) = IPV4_TO_LONG(192,168,3,3);
    //*(UINTN*)(CurSocket->m_pTcp4ConfigData->AccessPoint.RemoteAddress.Addr) = IPV4_TO_LONG(154,214,4,4);
    
    //配置远端端口
    //CurSocket->m_pTcp4ConfigData->AccessPoint.RemotePort = 80;
    CurSocket->m_pTcp4ConfigData->AccessPoint.RemotePort = 8080;
    
    *(UINT32*)(CurSocket->m_pTcp4ConfigData->AccessPoint.SubnetMask.Addr) = (255 | 255 << 8 | 255 << 16 | 0 << 24) ;

    /// if UseDefaultAddress is FALSE， config StationAddress 
    CurSocket->m_pTcp4ConfigData->AccessPoint.UseDefaultAddress = FALSE;

    CurSocket->m_pTcp4ConfigData->AccessPoint.ActiveFlag = TRUE;
    CurSocket->m_pTcp4ConfigData->ControlOption = NULL;
    
    Status = CurSocket->m_pTcp4Protocol->Configure(CurSocket->m_pTcp4Protocol, CurSocket->m_pTcp4ConfigData);
    
    return Status;
}

//连接服务器端
EFI_STATUS L2_TCP4_SocketConnect()
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


EFI_STATUS L2_TCP4_SocketSend(CHAR8* Data, UINTN Lenth)
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

EFI_STATUS L2_TCP4_SocketReceive(CHAR8* Buffer, UINTN Length, UINTN *recvLength)
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

INTN L2_TCP4_SocketDestroy()
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

EFI_STATUS L2_TCP4_SocketClose()
{
    EFI_STATUS Status;
    MYTCP4SOCKET *CurSocket = TCP4SocketFd;
	Status = CurSocket->m_pTcp4Protocol->Close(CurSocket->m_pTcp4Protocol, &CurSocket->CloseToken);
	
	L2_TCP4_SocketDestroy();
	//FreePool(CurSocket);
	TCP4SocketFd = NULL;

	return Status;
}

CHAR8 *ReceiveBuffer = NULL;
CHAR8 *SendBuffer = NULL;

EFI_STATUS L2_TCP4_Init()
{
    EFI_STATUS Status = 0;
    //CHAR8 SendBuffer[] = "Hello, I'm a client of UEFI.";

    //CHAR8 *RecvBuffer = (CHAR8 *) L2_MEMORY_Allocate(1024);
    
    //INFO(L"%d TCP4Test: SendTCP4Socket, %r\n", __LINE__, Status);
            
    L2_TCP4_SocketCreate();

    //参数配置
    L2_TCP4_SocketConfig();   
    
    Status = L2_TCP4_SocketConnect();  

    ReceiveBuffer = L2_MEMORY_Allocate("Network Receive Buffer", MEMORY_TYPE_NETWORK, 1024 * 4);
    SendBuffer = L2_MEMORY_Allocate("Network Send Buffer", MEMORY_TYPE_NETWORK, 1024 * 4);    
    
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
EFI_STATUS L2_TCP4_Receive()
{
    UINTN recvLen = 0;
    EFI_STATUS Status = 0;

    for (UINT32 i = 0; i < 1024; i++)
        ReceiveBuffer[i] = 0;
            
    //从服务器接收数据。
    Status = L2_TCP4_SocketReceive(ReceiveBuffer, 1024, &recvLen);
    if(EFI_ERROR(Status))    
    {
        return Status;
    }
    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d TCP: Receive: %r \n", __LINE__, Status);
   
    //显示接收的数据。
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Receive raw data Length: %d %02X%02X%02X%02X%02X%02X%02X%02X \n", __LINE__, recvLen, 
    																																											ReceiveBuffer[0], 
    																																											ReceiveBuffer[1], 
    																																											ReceiveBuffer[2], 
    																																											ReceiveBuffer[3], 
    																																											ReceiveBuffer[4], 
    																																											ReceiveBuffer[5], 
    																																											ReceiveBuffer[6], 
    																																											ReceiveBuffer[7]);
   
    //ReceiveBuffer[60] = '\0';
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d TCP: Receive data is: %a \n", __LINE__, ReceiveBuffer);

	
    for (int j = 0; j < 256; j++)
    {
        L2_DEBUG_Print1(DISK_READ_BUFFER_X + ScreenWidth * 3 / 4 + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", ReceiveBuffer[j] & 0xff);
    }
       
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
EFI_STATUS L2_TCP4_Send()
{
    EFI_STATUS Status = 0;
    
    //向服务器发送数据。
    Status = L2_TCP4_SocketSend(SendBuffer, AsciiStrLen(SendBuffer));
    
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
EFI_STATUS L2_WirelessMAC()
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
