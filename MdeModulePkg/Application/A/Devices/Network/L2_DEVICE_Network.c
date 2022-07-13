
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


#include "Ip6Impl.h"

#include "Ip6Common.h"
#include "Ip6ConfigImpl.h"
#include "Ip6ConfigNv.h"
#include "Ip6Driver.h"
#include "Ip6Icmp.h"
#include "Ip6If.h"
#include "Ip6Input.h"
#include "Ip6Mld.h"
#include "Ip6Nd.h"
#include "Ip6NvData.h"
#include "Ip6Option.h"
#include "Ip6Output.h"
#include "Ip6Route.h"
#include "Ip4Common.h"
#include "Ip4Config2Impl.h"
#include "Ip4Config2Nv.h"
#include "Ip4Driver.h"
#include "Ip4Icmp.h"
#include "Ip4If.h"
#include "Ip4Igmp.h"
//#include "Ip4Impl.h"
#include "Ip4Input.h"
#include "Ip4NvData.h"
#include "Ip4Option.h"
#include "Ip4Output.h"
#include "Ip4Route.h"

#include "Ip4Impl.h"

#include "Ip4Output.h"



/**/


#define IPV4_TO_LONG(a,b,c,d) (a | b<<8 | c << 16 | d <<24)
#define INFO(...)   \
				do {   \
				     if (0)\
					 {\
					 Print(L"[INFO  ]%a %a(Line %d): ", __FILE__,__FUNCTION__,__LINE__);  \
				    Print(__VA_ARGS__); \
				    }\
				}while(0);
				
				

#define IP4_PROTOCOL_SIGNATURE  SIGNATURE_32 ('I', 'P', '4', 'P')
#define IP4_SERVICE_SIGNATURE   SIGNATURE_32 ('I', 'P', '4', 'S')


//extern EFI_IPSEC2_PROTOCOL   *mIpSec;
//extern BOOLEAN               mIpSec2Installed;




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



/**
  Places outgoing data packets into the transmit queue.

  The Transmit() function places a sending request in the transmit queue of this
  EFI IPv4 Protocol instance. Whenever the packet in the token is sent out or some
  errors occur, the event in the token will be signaled and the status is updated.

  @param[in]  This  Pointer to the EFI_IP4_PROTOCOL instance.
  @param[in]  Token Pointer to the transmit token.

  @retval  EFI_SUCCESS           The data has been queued for transmission.
  @retval  EFI_NOT_STARTED       This instance has not been started.
  @retval  EFI_NO_MAPPING        When using the default address, configuration (DHCP, BOOTP,
                                 RARP, etc.) is not finished yet.
  @retval  EFI_INVALID_PARAMETER One or more parameters are invalid.
  @retval  EFI_ACCESS_DENIED     The transmit completion token with the same Token.Event
                                 was already in the transmit queue.
  @retval  EFI_NOT_READY         The completion token could not be queued because the transmit
                                 queue is full.
  @retval  EFI_NOT_FOUND         Not route is found to destination address.
  @retval  EFI_OUT_OF_RESOURCES  Could not queue the transmit data.
  @retval  EFI_BUFFER_TOO_SMALL  Token.Packet.TxData.TotalDataLength is too
                                 short to transmit.
  @retval  EFI_BAD_BUFFER_SIZE   The length of the IPv4 header + option length + total data length is
                                 greater than MTU (or greater than the maximum packet size if
                                 Token.Packet.TxData.OverrideData.
                                 DoNotFragment is TRUE).

**/


EFI_STATUS
EFIAPI
EfiIp4Transmit2 (
  IN EFI_IP4_PROTOCOL         *This,
  IN EFI_IP4_COMPLETION_TOKEN *Token
  )
{
  L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d %a \n", __LINE__, __FUNCTION__);

  IP4_SERVICE               *IpSb;
  IP4_PROTOCOL              *IpInstance;
  IP4_INTERFACE             *IpIf;
  IP4_TXTOKEN_WRAP          *Wrap;
  EFI_IP4_TRANSMIT_DATA     *TxData;
  EFI_IP4_CONFIG_DATA       *Config;
  EFI_IP4_OVERRIDE_DATA     *Override;
  IP4_HEAD                  Head;
  IP4_ADDR                  GateWay;
  EFI_STATUS                Status;
  EFI_TPL                   OldTpl;
  BOOLEAN                   DontFragment;
  UINT32                    HeadLen;
  UINT8                     RawHdrLen;
  UINT32                    OptionsLength;
  UINT8                     *OptionsBuffer;
  VOID                      *FirstFragment;


  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  IpInstance = IP4_INSTANCE_FROM_PROTOCOL (This);

  if (IpInstance->State != IP4_STATE_CONFIGED) 
  {
    return EFI_NOT_STARTED;
  }

  return;

  OldTpl  = gBS->RaiseTPL (TPL_CALLBACK);

  IpSb    = IpInstance->Service;
  IpIf    = IpInstance->Interface;
  Config  = &IpInstance->ConfigData;

  if (Config->UseDefaultAddress && IP4_NO_MAPPING (IpInstance)) {
    Status = EFI_NO_MAPPING;
    goto ON_EXIT;
  }
  

  //
  // make sure that token is properly formatted
  //
  Status = Ip4TxTokenValid (Token, IpIf, Config->RawData);

  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

  //
  // Check whether the token or signal already existed.
  //
  if (EFI_ERROR (NetMapIterate (&IpInstance->TxTokens, Ip4TokenExist, Token))) {
    Status = EFI_ACCESS_DENIED;
    goto ON_EXIT;
  }

  //
  // Build the IP header, need to fill in the Tos, TotalLen, Id,
  // fragment, Ttl, protocol, Src, and Dst.
  //
  TxData = Token->Packet.TxData;

  FirstFragment = NULL;

  if (Config->RawData) {
    //
    // When RawData is TRUE, first buffer in FragmentTable points to a raw
    // IPv4 fragment including IPv4 header and options.
    //
    FirstFragment = TxData->FragmentTable[0].FragmentBuffer;
    CopyMem (&RawHdrLen, FirstFragment, sizeof (UINT8));

    RawHdrLen = (UINT8) (RawHdrLen & 0x0f);
    if (RawHdrLen < 5) {
      Status = EFI_INVALID_PARAMETER;
      goto ON_EXIT;
    }

    RawHdrLen = (UINT8) (RawHdrLen << 2);

    CopyMem (&Head, FirstFragment, IP4_MIN_HEADLEN);

    Ip4NtohHead (&Head);
    HeadLen      = 0;
    DontFragment = IP4_DO_NOT_FRAGMENT (Head.Fragment);

    if (!DontFragment) {
      Status = EFI_INVALID_PARAMETER;
      goto ON_EXIT;
    }

    GateWay = IP4_ALLZERO_ADDRESS;

    //
    // Get IPv4 options from first fragment.
    //
    if (RawHdrLen == IP4_MIN_HEADLEN) {
      OptionsLength = 0;
      OptionsBuffer = NULL;
    } else {
      OptionsLength = RawHdrLen - IP4_MIN_HEADLEN;
      OptionsBuffer = (UINT8 *) FirstFragment + IP4_MIN_HEADLEN;
    }

    //
    // Trim off IPv4 header and options from first fragment.
    //
    TxData->FragmentTable[0].FragmentBuffer = (UINT8 *) FirstFragment + RawHdrLen;
    TxData->FragmentTable[0].FragmentLength = TxData->FragmentTable[0].FragmentLength - RawHdrLen;
  } else {
    CopyMem (&Head.Dst, &TxData->DestinationAddress, sizeof (IP4_ADDR));
    Head.Dst = NTOHL (Head.Dst);

    if (TxData->OverrideData != NULL) {
      Override      = TxData->OverrideData;
      Head.Protocol = Override->Protocol;
      Head.Tos      = Override->TypeOfService;
      Head.Ttl      = Override->TimeToLive;
      DontFragment  = Override->DoNotFragment;

      CopyMem (&Head.Src, &Override->SourceAddress, sizeof (IP4_ADDR));
      CopyMem (&GateWay, &Override->GatewayAddress, sizeof (IP4_ADDR));

      Head.Src = NTOHL (Head.Src);
      GateWay  = NTOHL (GateWay);
    } else {
      Head.Src      = IpIf->Ip;
      GateWay       = IP4_ALLZERO_ADDRESS;
      Head.Protocol = Config->DefaultProtocol;
      Head.Tos      = Config->TypeOfService;
      Head.Ttl      = Config->TimeToLive;
      DontFragment  = Config->DoNotFragment;
    }

    Head.Fragment = IP4_HEAD_FRAGMENT_FIELD (DontFragment, FALSE, 0);
    HeadLen       = (TxData->OptionsLength + 3) & (~0x03);

    OptionsLength = TxData->OptionsLength;
    OptionsBuffer = (UINT8 *) (TxData->OptionsBuffer);
  }

  //
  // If don't fragment and fragment needed, return error
  //
  if (DontFragment && (TxData->TotalDataLength + HeadLen > IpSb->MaxPacketSize)) {
    Status = EFI_BAD_BUFFER_SIZE;
    goto ON_EXIT;
  }

  //
  // OK, it survives all the validation check. Wrap the token in
  // a IP4_TXTOKEN_WRAP and the data in a netbuf
  //
  Status = EFI_OUT_OF_RESOURCES;
  Wrap   = AllocateZeroPool (sizeof (IP4_TXTOKEN_WRAP));
  if (Wrap == NULL) {
    goto ON_EXIT;
  }

  Wrap->IpInstance  = IpInstance;
  Wrap->Token       = Token;
  Wrap->Sent        = FALSE;
  Wrap->Life        = IP4_US_TO_SEC (Config->TransmitTimeout);
  Wrap->Packet      = NetbufFromExt (
                        (NET_FRAGMENT *) TxData->FragmentTable,
                        TxData->FragmentCount,
                        IP4_MAX_HEADLEN,
                        0,
                        Ip4FreeTxToken,
                        Wrap
                        );

  if (Wrap->Packet == NULL) {
    FreePool (Wrap);
    goto ON_EXIT;
  }

  Token->Status = EFI_NOT_READY;

  if (EFI_ERROR (NetMapInsertTail (&IpInstance->TxTokens, Token, Wrap))) {
    //
    // NetbufFree will call Ip4FreeTxToken, which in turn will
    // free the IP4_TXTOKEN_WRAP. Now, the token wrap hasn't been
    // enqueued.
    //
    if (Config->RawData) {
      //
      // Restore pointer of first fragment in RawData mode.
      //
      TxData->FragmentTable[0].FragmentBuffer = (UINT8 *) FirstFragment;
    }

    NetbufFree (Wrap->Packet);
    goto ON_EXIT;
  }

  //
  // Mark the packet sent before output it. Mark it not sent again if the
  // returned status is not EFI_SUCCESS;
  //
  Wrap->Sent = TRUE;

  Status = Ip4Output (
             IpSb,
             IpInstance,
             Wrap->Packet,
             &Head,
             OptionsBuffer,
             OptionsLength,
             GateWay,
             Ip4OnPacketSent,
             Wrap
             );

  if (EFI_ERROR (Status)) {
    Wrap->Sent = FALSE;

    if (Config->RawData) {
      //
      // Restore pointer of first fragment in RawData mode.
      //
      TxData->FragmentTable[0].FragmentBuffer = (UINT8 *) FirstFragment;
    }

    NetbufFree (Wrap->Packet);
  }

  if (Config->RawData) {
    //
    // Restore pointer of first fragment in RawData mode.
    //
    TxData->FragmentTable[0].FragmentBuffer = (UINT8 *) FirstFragment;
  }

ON_EXIT:
  gBS->RestoreTPL (OldTpl);
  return Status;
}



EFI_STATUS L2_TCP4_SocketSend(CHAR8* Data, UINTN Lenth)
{
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d %a \n", __LINE__, __FUNCTION__);
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
    Status = Tcp4Transmit(CurSocket->m_pTcp4Protocol, &CurSocket->SendToken);
    //Status = CurSocket->m_pTcp4Protocol->Transmit(CurSocket->m_pTcp4Protocol, &CurSocket->SendToken);
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
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d L2_TCP4_SocketReceive function \n", __LINE__);
   
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
    //Status = EfiIp4Receive(CurSocket->m_pTcp4Protocol, &CurSocket->RecvToken);
    
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

    for (UINT32 i = 0; i < 1024 * 4; i++)
        ReceiveBuffer[i] = 0;
            
    //从服务器接收数据。
    Status = L2_TCP4_SocketReceive(ReceiveBuffer, 1024 * 4, &recvLen);
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
    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d %a %r\n", __LINE__, __FUNCTION__, Status);

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
