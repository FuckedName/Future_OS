


#include "Network.h"


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

// stub funciton
VOID NopNoify (  IN EFI_EVENT  Event,  IN VOID *Context  )
{
}

VOID  Tcp4SendNotify(IN EFI_EVENT  Event,  IN VOID *Context)
{
     MYTCP4SOCKET *CurSocket = (MYTCP4SOCKET *)(Context);

     //INFO(L"Tcp4SendNotify: stub=%x\n", (int)CurSocket->stub);
     //INFO(L"Tcp4SendNotify: Context=%p\n", Context);
}
VOID  Tcp4RecvNotify(IN EFI_EVENT  Event,  IN VOID *Context)
{
     MYTCP4SOCKET *CurSocket = (MYTCP4SOCKET *)(Context);

     //INFO(L"Tcp4RecvNotify: stub=%x\n", (int)CurSocket->stub);
     //INFO(L"Tcp4RecvNotify: Context=%p\n", Context);
}
EFI_STATUS InitTcp4SocketFd()
{
    EFI_STATUS                           Status;
    MYTCP4SOCKET *CurSocket = TCP4SocketFd;

    // 1 Create Configure data
    CurSocket->m_pTcp4ConfigData = (EFI_TCP4_CONFIG_DATA*)AllocatePool(sizeof(EFI_TCP4_CONFIG_DATA));
	INFO(L"%x\n", CurSocket->m_pTcp4ConfigData);
    if (NULL == CurSocket->m_pTcp4ConfigData)
    {
    	return;
    }
    
    // CurSocket->stub = 0x1212;
    // 2 Create Connect Event
    // CurSocket->ConnectToken.CompletionToken.Status = EFI_ABORTED;
    Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL, TPL_CALLBACK, (EFI_EVENT_NOTIFY)NopNoify , (VOID*)&CurSocket->ConnectToken, &CurSocket->ConnectToken.CompletionToken.Event );
    INFO(L"%d\n", Status);    
    if(EFI_ERROR(Status)) 
    {
        return Status;  
    }
    
    // 3 Create Transmit Event
    Status = gBS->CreateEvent(EVT_NOTIFY_WAIT, TPL_CALLBACK, (EFI_EVENT_NOTIFY)Tcp4SendNotify , (VOID*)CurSocket, &CurSocket->SendToken.CompletionToken.Event);
    INFO(L"%d Init: CurSocket=%p TCP4SocketFd[index]=%p\n", __LINE__, CurSocket, TCP4SocketFd);
    if(EFI_ERROR(Status)) 
    {
        INFO(L"%d Init: Create Send Event fail!\n\r", __LINE__);
        return Status;     
    }
    
    // CurSocket->SendToken.CompletionToken.Status  =EFI_ABORTED; 
    CurSocket->m_TransData = (EFI_TCP4_TRANSMIT_DATA*)AllocatePool(sizeof(EFI_TCP4_TRANSMIT_DATA));
	INFO(L"%x\n", CurSocket->m_TransData);
	
    // 4 Create Recv Event
    Status = gBS->CreateEvent(EVT_NOTIFY_WAIT, TPL_CALLBACK, (EFI_EVENT_NOTIFY)Tcp4RecvNotify , (VOID*)CurSocket, &CurSocket->RecvToken.CompletionToken.Event);
    // CurSocket->RecvToken.CompletionToken.Status  =EFI_ABORTED;
    INFO(L"%d\n", Status);
    
    CurSocket->m_RecvData = (EFI_TCP4_RECEIVE_DATA*) AllocatePool(sizeof(EFI_TCP4_RECEIVE_DATA));;
    INFO(L"%x\n", CurSocket->m_RecvData);
    if(EFI_ERROR(Status)) 
    {
        gST->ConOut->OutputString(gST->ConOut,L"Init: Create Recv Event fail!\n\r");
        return Status;
    }   
    
    // 5 Create Close Event
    // CurSocket->CloseToken.CompletionToken.Status = EFI_ABORTED;
    Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL, TPL_CALLBACK, (EFI_EVENT_NOTIFY)NopNoify , (VOID*)&CurSocket->CloseToken, &CurSocket->CloseToken.CompletionToken.Event );
    INFO(L"%d\n", Status);
    if(EFI_ERROR(Status))
    {
        gST->ConOut->OutputString(gST->ConOut,L"Init: Create Close Event fail!\n\r");
        return Status;
    }
    return Status;
}

UINTN CreateTCP4Socket(VOID)
{
    EFI_STATUS                           Status;
    EFI_SERVICE_BINDING_PROTOCOL*  pTcpServiceBinding;
	INFO(L"\n");
	
    TCP4SocketFd=(MYTCP4SOCKET *) AllocatePool(sizeof(MYTCP4SOCKET));
    INFO(L"CurSocket: %x\n", TCP4SocketFd);
    if (TCP4SocketFd == NULL)
    {
    	INFO(L"CurSocket == NULL\n");
    	return EFI_SUCCESS;
    }
            
    gBS->SetMem((void*)TCP4SocketFd, 0, sizeof(MYTCP4SOCKET));        
    TCP4SocketFd->m_SocketHandle  = NULL;
    Status = gBS->LocateProtocol (&gEfiTcp4ServiceBindingProtocolGuid,
							        NULL,
							        (VOID **)&pTcpServiceBinding);
	INFO(L"%d\n", Status);
    if(EFI_ERROR(Status))
    {
        return Status;
    }
    
    Status = pTcpServiceBinding->CreateChild ( pTcpServiceBinding,
										         &TCP4SocketFd->m_SocketHandle );
    INFO(L"%d\n", Status);
    if(EFI_ERROR(Status))
    {
        return Status;
    }

    Status = gBS->OpenProtocol ( TCP4SocketFd->m_SocketHandle,
							        &gEfiTcp4ProtocolGuid,
							        (VOID **)&TCP4SocketFd->m_pTcp4Protocol,
							        gImageHandle,
							        TCP4SocketFd->m_SocketHandle,
							        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL );
    INFO(L"%d\n", Status);
    if(EFI_ERROR(Status))
    {
        return Status;
    }
    
    InitTcp4SocketFd();
    
	INFO(L"%d\n", Status);
	
    return 0;
}


EFI_STATUS ConfigTCP4Socket(UINT32 Ip32, UINT16 Port)
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
    *(UINTN*)(CurSocket->m_pTcp4ConfigData->AccessPoint.StationAddress.Addr) = MYIPV4(192, 168, 3, 4);

    //配置本地端口
    CurSocket->m_pTcp4ConfigData->AccessPoint.StationPort = 61558;

    //配置远端IP地址，
    *(UINTN*)(CurSocket->m_pTcp4ConfigData->AccessPoint.RemoteAddress.Addr) = Ip32;
    
    //配置远端端口
    CurSocket->m_pTcp4ConfigData->AccessPoint.RemotePort = Port;
    
    *(UINT32*)(CurSocket->m_pTcp4ConfigData->AccessPoint.SubnetMask.Addr) = (255 | 255 << 8 | 255 << 16 | 0 << 24) ;

    /// if UseDefaultAddress is FALSE， config StationAddress 
    CurSocket->m_pTcp4ConfigData->AccessPoint.UseDefaultAddress = FALSE;

    CurSocket->m_pTcp4ConfigData->AccessPoint.ActiveFlag = TRUE;
    CurSocket->m_pTcp4ConfigData->ControlOption = NULL;
    
    Status = CurSocket->m_pTcp4Protocol->Configure(CurSocket->m_pTcp4Protocol, CurSocket->m_pTcp4ConfigData);
    
    return Status;
}

EFI_STATUS SendTCP4Socket(CHAR8* Data, UINTN Lenth)
{
    EFI_STATUS Status = EFI_NOT_FOUND;
    MYTCP4SOCKET *CurSocket = TCP4SocketFd;
    UINTN waitIndex = 0;

    if(CurSocket->m_pTcp4Protocol == NULL) 
    {
        gST->ConOut->OutputString(gST->ConOut,L"Send: m_Tcp4Protocol is NULL\n\r");
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
        gST->ConOut->OutputString(gST->ConOut,L"Send: Transmit fail!\n\r");
        return Status;
    }
        
        
	Status = gBS->WaitForEvent(1, &(CurSocket->SendToken.CompletionToken.Event), &waitIndex);
    INFO(L"%d Send: WaitForEvent, %r\n", __LINE__, Status);
    
    return CurSocket->SendToken.CompletionToken.Status;
}

EFI_STATUS RecvTCP4Socket(IN CHAR8* Buffer, IN UINTN Length, OUT UINTN *recvLength)
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
        gST->ConOut->OutputString(gST->ConOut,L"Recv: Receive fail!\n\r");
        return Status;
    }
    
    Status = gBS->WaitForEvent(1, &(CurSocket->RecvToken.CompletionToken.Event), &waitIndex);
    INFO(L"%d Recv: WaitForEvent, %r\n", __LINE__, Status);
    *recvLength = CurSocket->m_RecvData->DataLength;
   
    return CurSocket->RecvToken.CompletionToken.Status;
}

//连接服务器端
EFI_STATUS ConnectTCP4Socket(UINT32 Ip32, UINT16 Port)
{
    EFI_STATUS Status = EFI_NOT_FOUND;
    MYTCP4SOCKET *CurSocket = TCP4SocketFd;
    UINTN waitIndex=0;

    //参数配置
    ConfigTCP4Socket(Ip32, Port);

    if(CurSocket->m_pTcp4Protocol == NULL) 
    {
        return Status;
    }
    
    Status = CurSocket->m_pTcp4Protocol->Connect(CurSocket->m_pTcp4Protocol, &CurSocket->ConnectToken);
    INFO(L"%r\n", Status);
    //if(EFI_ERROR(Status))    
    //{
    //    return Status;
    //}

    // 就算这里失败，也可以继续往下运行。
    Status = gBS->WaitForEvent(1, &(CurSocket->ConnectToken.CompletionToken.Event), &waitIndex);
    INFO(L"Connect: WaitForEvent, %r\n", Status);
    // if( !EFI_ERROR(Status)){
    //     gST->ConOut->OutputString(gST->ConOut,L"Connect: WaitForEvent fail!\n\r");
    //     Status = CurSocket->ConnectToken.CompletionToken.Status;
    // }
    return Status;
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
	    FreePool(CurSocket->m_pTcp4ConfigData);
    }
    
    if(CurSocket->SendToken.Packet.TxData)
    {
	    FreePool(CurSocket->SendToken.Packet.TxData);
	    CurSocket->SendToken.Packet.TxData = NULL;
    }
    
    if(CurSocket->RecvToken.Packet.RxData)
    {
	    FreePool(CurSocket->RecvToken.Packet.RxData);
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
	FreePool(CurSocket);
	TCP4SocketFd = NULL;

	return Status;
}
