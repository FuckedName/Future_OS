
/*************************************************
    .
    File name:      	*.*
    Author：	        	任启红
    ID：					00001
    Date:          		202201
    Description:    	用于给应用提供接口
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
//#include <Library/NetLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PrintLib.h>



#include "L2_APPLICATIONS_Command_ping.h"

#include <Global/Global.h>



#define ARRAY_SIZE2(Array) (sizeof (Array) / sizeof ((Array)[0]))

typedef
EFI_STATUS
(EFIAPI *PING_IP_CHOICE_IP42)(
  IN VOID          *This,
  IN VOID          *Token
  );


typedef
EFI_STATUS
(EFIAPI *PING_IP_CHOICE_IP42)(
  IN VOID          *This,
  IN VOID          *Token
  );

typedef struct _EFI_IP6_PROTOCOL EFI_IP6_PROTOCOL2;

typedef
EFI_STATUS
(EFIAPI *PING_IPX_TRANSMIT2)(
  IN VOID          *This,
  IN VOID          *Token
  );

typedef
EFI_STATUS
(EFIAPI *PING_IPX_RECEIVE2)(
  IN VOID          *This,
  IN VOID          *Token
  );

typedef
EFI_STATUS
(EFIAPI *PING_IPX_CANCEL2)(
  IN VOID          *This,
  IN VOID          *Token OPTIONAL
  );

//
// Function templates to match the IPv4 and IPv6 commands that we use.
//
typedef
EFI_STATUS
(EFIAPI *PING_IPX_POLL2)(
  IN VOID          *This
  );

///
/// A set of pointers to either IPv6 or IPv4 functions.
/// Unknown which one to the ping command.
///
typedef struct {
  PING_IP_CHOICE_IP42             Transmit;
  PING_IP_CHOICE_IP42              Receive;
  PING_IPX_CANCEL2               Cancel;
  PING_IPX_POLL2                 Poll;
}PING_IPX_PROTOCOL2;



typedef union {
  VOID                  *RxData;
  VOID                  *TxData;
} PING_PACKET2;

//
// PING_IPX_COMPLETION_TOKEN2
// structures are used for both transmit and receive operations.
// This version is IP-unaware.
//
typedef struct {
  EFI_EVENT               Event;
  EFI_STATUS              Status;
  PING_PACKET2             Packet;
} PING_IPX_COMPLETION_TOKEN2;


#define PING_IP4_COPY_ADDRESS(Dest, Src) (CopyMem ((Dest), (Src), sizeof (EFI_IPv4_ADDRESS)))


//
// Iterate through the double linked list. This is delete-safe.
// Don't touch NextEntry. Also, don't use this macro if list
// entries other than the Entry may be deleted when processing
// the current Entry.
//
#define NET_LIST_FOR_EACH_SAFE2(Entry, NextEntry, ListHead) \
  for(Entry = (ListHead)->ForwardLink, NextEntry = Entry->ForwardLink; \
      Entry != (ListHead); \
      Entry = NextEntry, NextEntry = Entry->ForwardLink \
     )
	  
	  //
	  // Macro that returns the byte offset of a field in a data structure.
	  //


#define OFFSET_OF2(TYPE, Field) ((UINTN) &(((TYPE *)0)->Field))

  
#define BASE_CR2(Record, TYPE, Field)  ((TYPE *) ((CHAR8 *) (Record) - OFFSET_OF2 (TYPE, Field)))

UINT64          mCurrentTick = 0;


typedef union {
  VOID                  *RxData;
  VOID                  *TxData;
} PING_PACKET;


#pragma pack(1)
typedef struct _ICMPX_ECHO_REQUEST_REPLY {
  UINT8                       Type;
  UINT8                       Code;
  UINT16                      Checksum;
  UINT16                      Identifier;
  UINT16                      SequenceNum;
  UINT32                      TimeStamp;
  UINT8                       Data[1];
} ICMPX_ECHO_REQUEST_REPLY2;
#pragma pack()


#define MEDIA_STATE_DETECT_TIME_INTERVAL2  1000000U

#define DEFAULT_TIMEOUT2       5000
#define MAX_SEND_NUMBER2       10000
#define MAX_BUFFER_SIZE2       32768
#define DEFAULT_TIMER_PERIOD2 358049
#define ONE_SECOND2            10000000
#define PING_IP_CHOICE_IP42    1
#define PING_IP_CHOICE_IP62    2
#define ICMP_V6_ECHO_REQUEST2                     0x80
#define DEFAULT_SEND_COUNT2    10
#define DEFAULT_BUFFER_SIZE2   16
#define ICMP_V4_ECHO_REQUEST2  0x8
#define ICMP_V4_ECHO_REPLY2    0x0
#define STALL_1_MILLI_SECOND2  1000
#define ICMP_V6_ECHO_REPLY2                       0x81

#define PING_PRIVATE_DATA2_SIGNATURE2  SIGNATURE_32 ('P', 'i', 'n', 'g')
typedef UINT32          IP4_ADDR;
#define IP6_IS_MULTICAST2(Ip6)             (((Ip6)->Addr[0]) == 0xFF)
#define IP4_IS_MULTICAST2(Ip)              (((Ip) & 0xF0000000) == 0xE0000000)
#define EFI_IP42(EfiIpAddr)       (*(IP4_ADDR *) ((EfiIpAddr).Addr))
#define EFI_IP6_EQUAL2(Ip1, Ip2)  (CompareMem ((Ip1), (Ip2), sizeof (EFI_IPv6_ADDRESS)) == 0)
#define EFI_IP4_EQUAL2(Ip1, Ip2)  (CompareMem ((Ip1), (Ip2), sizeof (EFI_IPv4_ADDRESS)) == 0)

typedef struct _PING_PRIVATE_DATA2 {
  UINT32                      Signature;
  EFI_HANDLE                  NicHandle;
  EFI_HANDLE                  IpChildHandle;
  EFI_EVENT                   Timer;

  UINT32                      TimerPeriod;
  UINT32                      RttTimerTick;
  EFI_EVENT                   RttTimer;

  EFI_STATUS                  Status;
  LIST_ENTRY                  TxList;
  UINT16                      RxCount;
  UINT16                      TxCount;
  UINT64                      RttSum;
  UINT64                      RttMin;
  UINT64                      RttMax;
  UINT32                      SequenceNum;

  UINT32                      SendNum;
  UINT32                      BufferSize;
  UINT32                      IpChoice;

  PING_IPX_PROTOCOL2           ProtocolPointers;
  VOID                        *IpProtocol;
  UINT8                       SrcAddress[MAX(sizeof(EFI_IPv6_ADDRESS)        , sizeof(EFI_IPv4_ADDRESS)          )];
  UINT8                       DstAddress[MAX(sizeof(EFI_IPv6_ADDRESS)        , sizeof(EFI_IPv4_ADDRESS)          )];
  PING_IPX_COMPLETION_TOKEN2   RxToken;
  UINT16                      FailedCount;
} PING_PRIVATE_DATA2;

typedef struct _PING_ICMP_TX_INFO {
  LIST_ENTRY                Link;
  UINT16                    SequenceNum;
  UINT32                    TimeStamp;
  PING_IPX_COMPLETION_TOKEN2 *Token;
} PING_ICMPX_TX_INFO2;

/**
  Calculate the internet checksum (see RFC 1071).

  @param[in] Packet  Buffer which contains the data to be checksummed.
  @param[in] Length  Length to be checksummed.

  @retval Checksum     Returns the 16 bit ones complement of
                       ones complement sum of 16 bit words
**/
UINT16
NetChecksum (
  IN UINT8   *Buffer,
  IN UINT32  Length
  )
{
  UINT32  Sum;
  UINT8   Odd;
  UINT16  *Packet;

  Packet  = (UINT16 *) Buffer;

  Sum     = 0;
  Odd     = (UINT8) (Length & 1);
  Length >>= 1;
  while ((Length--) != 0) {
    Sum += *Packet++;
  }

  if (Odd != 0) {
    Sum += *(UINT8 *) Packet;
  }

  Sum = (Sum & 0xffff) + (Sum >> 16);

  //
  // in case above carried
  //
  Sum += Sum >> 16;

  return (UINT16) Sum;
}

/**
  Reads and returns the current value of register.
  In IA64, the register is the Interval Timer Vector (ITV).
  In X86(IA32/X64), the register is the Time Stamp Counter (TSC)

  @return The current value of the register.

**/

STATIC CONST SHELL_PARAM_ITEM    PingParamList[] = {
  {
    L"-l",
    TypeValue
  },
  {
    L"-n",
    TypeValue
  },
  {
    L"-s",
    TypeValue
  },
  {
    L"-_s",
    TypeValue
  },
  {
    L"-_ip6",
    TypeFlag
  },
  {
    NULL,
    TypeMax
  },
};

//
// Global Variables in Ping command.
//
STATIC CONST CHAR16      *mDstString;
STATIC CONST CHAR16      *mSrcString;

/**
  RTT timer tick routine.

  @param[in]    Event    A EFI_EVENT type event.
  @param[in]    Context  The pointer to Context.

**/
VOID
EFIAPI
RttTimerTickRoutine2 (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  UINT32     *RttTimerTick;

  RttTimerTick = (UINT32*) Context;
  (*RttTimerTick)++;
}

#define TICKS_PER_MS2            10000U
#define MAX_SEND_NUMBER2       10000
#define STALL_1_MILLI_SECOND2  1000
#define  IP6_ICMP2              58
#define ICMP_V4_ECHO_REQUEST2  0x8

extern EFI_GUID gEfiIp4ServiceBindingProtocolGuid;
extern EFI_GUID gEfiIp4ProtocolGuid;

extern EFI_GUID gEfiIp6ServiceBindingProtocolGuid;
extern EFI_GUID gEfiIp6ProtocolGuid;
extern EFI_GUID gEfiIp4Config2ProtocolGuid;
extern EFI_GUID gEfiIp6ConfigProtocolGuid;


#define IP4_COPY_ADDRESS2(Dest, Src) (CopyMem ((Dest), (Src), sizeof (EFI_IPv4_ADDRESS)))
#define IP6_COPY_ADDRESS2(Dest, Src) (CopyMem ((Dest), (Src), sizeof (EFI_IPv6_ADDRESS)))

typedef struct _EFI_ADAPTER_INFORMATION_PROTOCOL2 EFI_ADAPTER_INFORMATION_PROTOCOL2;

typedef
EFI_STATUS
(EFIAPI *EFI_ADAPTER_INFO_GET_SUPPORTED_TYPES2)(
  IN  EFI_ADAPTER_INFORMATION_PROTOCOL2  *This,
  OUT EFI_GUID                          **InfoTypesBuffer,
  OUT UINTN                             *InfoTypesBufferCount
  );


typedef
EFI_STATUS
(EFIAPI *EFI_ADAPTER_INFO_SET_INFO2)(
  IN  EFI_ADAPTER_INFORMATION_PROTOCOL2  *This,
  IN  EFI_GUID                          *InformationType,
  IN  VOID                              *InformationBlock,
  IN  UINTN                             InformationBlockSize
  );

typedef
EFI_STATUS
(EFIAPI *EFI_ADAPTER_INFO_GET_INFO2)(
  IN  EFI_ADAPTER_INFORMATION_PROTOCOL2  *This,
  IN  EFI_GUID                          *InformationType,
  OUT VOID                              **InformationBlock,
  OUT UINTN                             *InformationBlockSize
  );


///
/// EFI_ADAPTER_INFORMATION_PROTOCOL
/// The protocol for adapter provides the following services.
/// - Gets device state information from adapter.
/// - Sets device information for adapter.
/// - Gets a list of supported information types for this instance of the protocol.
///
struct _EFI_ADAPTER_INFORMATION_PROTOCOL2 {
  EFI_ADAPTER_INFO_GET_INFO2              GetInformation;
  EFI_ADAPTER_INFO_SET_INFO2              SetInformation;
  EFI_ADAPTER_INFO_GET_SUPPORTED_TYPES2   GetSupportedTypes;
};




///
/// EFI_ADAPTER_INFO_MEDIA_STATE
///
typedef struct {
  ///
  /// Returns the current media state status. MediaState can have any of the following values:
  /// EFI_SUCCESS: There is media attached to the network adapter. EFI_NOT_READY: This detects a bounced state.
  /// There was media attached to the network adapter, but it was removed and reattached. EFI_NO_MEDIA: There is
  /// not any media attached to the network.
  ///
  EFI_STATUS                    MediaState;
} EFI_ADAPTER_INFO_MEDIA_STATE2;




/**
  Get handle with Simple Network Protocol installed on it.

  There should be MNP Service Binding Protocol installed on the input ServiceHandle.
  If Simple Network Protocol is already installed on the ServiceHandle, the
  ServiceHandle will be returned. If SNP is not installed on the ServiceHandle,
  try to find its parent handle with SNP installed.

  @param[in]   ServiceHandle    The handle where network service binding protocols are
                                installed on.
  @param[out]  Snp              The pointer to store the address of the SNP instance.
                                This is an optional parameter that may be NULL.

  @return The SNP handle, or NULL if not found.

**/
EFI_HANDLE
EFIAPI
NetLibGetSnpHandle2 (
  IN   EFI_HANDLE                  ServiceHandle,
  OUT  EFI_SIMPLE_NETWORK_PROTOCOL **Snp  OPTIONAL
  )
{
  EFI_STATUS                   Status;
  EFI_SIMPLE_NETWORK_PROTOCOL  *SnpInstance;
  EFI_DEVICE_PATH_PROTOCOL     *DevicePath;
  EFI_HANDLE                   SnpHandle;

  //
  // Try to open SNP from ServiceHandle
  //
  SnpInstance = NULL;
  Status = gBS->HandleProtocol (ServiceHandle, &gEfiSimpleNetworkProtocolGuid, (VOID **) &SnpInstance);
  if (!EFI_ERROR (Status)) {
    if (Snp != NULL) {
      *Snp = SnpInstance;
    }
    return ServiceHandle;
  }

  //
  // Failed to open SNP, try to get SNP handle by LocateDevicePath()
  //
  DevicePath = DevicePathFromHandle (ServiceHandle);
  if (DevicePath == NULL) {
    return NULL;
  }

  SnpHandle = NULL;
  Status = gBS->LocateDevicePath (&gEfiSimpleNetworkProtocolGuid, &DevicePath, &SnpHandle);
  if (EFI_ERROR (Status)) {
    //
    // Failed to find SNP handle
    //
    return NULL;
  }

  Status = gBS->HandleProtocol (SnpHandle, &gEfiSimpleNetworkProtocolGuid, (VOID **) &SnpInstance);
  if (!EFI_ERROR (Status)) {
    if (Snp != NULL) {
      *Snp = SnpInstance;
    }
    return SnpHandle;
  }

  return NULL;
}



/**
  Detect media status for specified network device.

  If MediaPresent is NULL, then ASSERT().

  The underlying UNDI driver may or may not support reporting media status from
  GET_STATUS command (PXE_STATFLAGS_GET_STATUS_NO_MEDIA_SUPPORTED). This routine
  will try to invoke Snp->GetStatus() to get the media status: if media already
  present, it return directly; if media not present, it will stop SNP and then
  restart SNP to get the latest media status, this give chance to get the correct
  media status for old UNDI driver which doesn't support reporting media status
  from GET_STATUS command.
  Note: there will be two limitations for current algorithm:
  1) for UNDI with this capability, in case of cable is not attached, there will
     be an redundant Stop/Start() process;
  2) for UNDI without this capability, in case that network cable is attached when
     Snp->Initialize() is invoked while network cable is unattached later,
     NetLibDetectMedia() will report MediaPresent as TRUE, causing upper layer
     apps to wait for timeout time.

  @param[in]   ServiceHandle    The handle where network service binding protocols are
                                installed on.
  @param[out]  MediaPresent     The pointer to store the media status.

  @retval EFI_SUCCESS           Media detection success.
  @retval EFI_INVALID_PARAMETER ServiceHandle is not valid network device handle.
  @retval EFI_UNSUPPORTED       Network device does not support media detection.
  @retval EFI_DEVICE_ERROR      SNP is in unknown state.

**/
EFI_STATUS
EFIAPI
NetLibDetectMedia2 (
  IN  EFI_HANDLE            ServiceHandle,
  OUT BOOLEAN               *MediaPresent
  )
{
  EFI_STATUS                   Status;
  EFI_HANDLE                   SnpHandle;
  EFI_SIMPLE_NETWORK_PROTOCOL  *Snp;
  UINT32                       InterruptStatus;
  UINT32                       OldState;
  EFI_MAC_ADDRESS              *MCastFilter;
  UINT32                       MCastFilterCount;
  UINT32                       EnableFilterBits;
  UINT32                       DisableFilterBits;
  BOOLEAN                      ResetMCastFilters;

  ASSERT (MediaPresent != NULL);

  //
  // Get SNP handle
  //
  Snp = NULL;
  SnpHandle = NetLibGetSnpHandle2 (ServiceHandle, &Snp);
  if (SnpHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check whether SNP support media detection
  //
  if (!Snp->Mode->MediaPresentSupported) {
    return EFI_UNSUPPORTED;
  }

  //
  // Invoke Snp->GetStatus() to refresh MediaPresent field in SNP mode data
  //
  Status = Snp->GetStatus (Snp, &InterruptStatus, NULL);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (Snp->Mode->MediaPresent) {
    //
    // Media is present, return directly
    //
    *MediaPresent = TRUE;
    return EFI_SUCCESS;
  }

  //
  // Till now, GetStatus() report no media; while, in case UNDI not support
  // reporting media status from GetStatus(), this media status may be incorrect.
  // So, we will stop SNP and then restart it to get the correct media status.
  //
  OldState = Snp->Mode->State;
  if (OldState >= EfiSimpleNetworkMaxState) {
    return EFI_DEVICE_ERROR;
  }

  MCastFilter = NULL;

  if (OldState == EfiSimpleNetworkInitialized) {
    //
    // SNP is already in use, need Shutdown/Stop and then Start/Initialize
    //

    //
    // Backup current SNP receive filter settings
    //
    EnableFilterBits  = Snp->Mode->ReceiveFilterSetting;
    DisableFilterBits = Snp->Mode->ReceiveFilterMask ^ EnableFilterBits;

    ResetMCastFilters = TRUE;
    MCastFilterCount  = Snp->Mode->MCastFilterCount;
    if (MCastFilterCount != 0) {
      MCastFilter = AllocateCopyPool (
                      MCastFilterCount * sizeof (EFI_MAC_ADDRESS),
                      Snp->Mode->MCastFilter
                      );
      ASSERT (MCastFilter != NULL);
      if (MCastFilter == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Exit;
      }

      ResetMCastFilters = FALSE;
    }

    //
    // Shutdown/Stop the simple network
    //
    Status = Snp->Shutdown (Snp);
    if (!EFI_ERROR (Status)) {
      Status = Snp->Stop (Snp);
    }
    if (EFI_ERROR (Status)) {
      goto Exit;
    }

    //
    // Start/Initialize the simple network
    //
    Status = Snp->Start (Snp);
    if (!EFI_ERROR (Status)) {
      Status = Snp->Initialize (Snp, 0, 0);
    }
    if (EFI_ERROR (Status)) {
      goto Exit;
    }

    //
    // Here we get the correct media status
    //
    *MediaPresent = Snp->Mode->MediaPresent;

    //
    // Restore SNP receive filter settings
    //
    Status = Snp->ReceiveFilters (
                    Snp,
                    EnableFilterBits,
                    DisableFilterBits,
                    ResetMCastFilters,
                    MCastFilterCount,
                    MCastFilter
                    );

    if (MCastFilter != NULL) {
      FreePool (MCastFilter);
    }

    return Status;
  }

  //
  // SNP is not in use, it's in state of EfiSimpleNetworkStopped or EfiSimpleNetworkStarted
  //
  if (OldState == EfiSimpleNetworkStopped) {
    //
    // SNP not start yet, start it
    //
    Status = Snp->Start (Snp);
    if (EFI_ERROR (Status)) {
      goto Exit;
    }
  }

  //
  // Initialize the simple network
  //
  Status = Snp->Initialize (Snp, 0, 0);
  if (EFI_ERROR (Status)) {
    Status = EFI_DEVICE_ERROR;
    goto Exit;
  }

  //
  // Here we get the correct media status
  //
  *MediaPresent = Snp->Mode->MediaPresent;

  //
  // Shut down the simple network
  //
  Snp->Shutdown (Snp);

Exit:
  if (OldState == EfiSimpleNetworkStopped) {
    //
    // Original SNP sate is Stopped, restore to original state
    //
    Snp->Stop (Snp);
  }

  if (MCastFilter != NULL) {
    FreePool (MCastFilter);
  }

  return Status;
}



/**

  Detect media state for a network device. This routine will wait for a period of time at
  a specified checking interval when a certain network is under connecting until connection
  process finishs or timeout. If Aip protocol is supported by low layer drivers, three kinds
  of media states can be detected: EFI_SUCCESS, EFI_NOT_READY and EFI_NO_MEDIA, represents
  connected state, connecting state and no media state respectively. When function detects
  the current state is EFI_NOT_READY, it will loop to wait for next time's check until state
  turns to be EFI_SUCCESS or EFI_NO_MEDIA. If Aip protocol is not supported, function will
  call NetLibDetectMedia() and return state directly.

  @param[in]   ServiceHandle    The handle where network service binding protocols are
                                installed on.
  @param[in]   Timeout          The maximum number of 100ns units to wait when network
                                is connecting. Zero value means detect once and return
                                immediately.
  @param[out]  MediaState       The pointer to the detected media state.

  @retval EFI_SUCCESS           Media detection success.
  @retval EFI_INVALID_PARAMETER ServiceHandle is not a valid network device handle or
                                MediaState pointer is NULL.
  @retval EFI_DEVICE_ERROR      A device error occurred.
  @retval EFI_TIMEOUT           Network is connecting but timeout.

**/
EFI_STATUS
EFIAPI
NetLibDetectMediaWaitTimeout2 (
  IN  EFI_HANDLE            ServiceHandle,
  IN  UINT64                Timeout,
  OUT EFI_STATUS            *MediaState
  )
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        SnpHandle;
  EFI_SIMPLE_NETWORK_PROTOCOL       *Snp;
  EFI_ADAPTER_INFORMATION_PROTOCOL2  *Aip;
  EFI_ADAPTER_INFO_MEDIA_STATE2      *MediaInfo;
  BOOLEAN                           MediaPresent;
  UINTN                             DataSize;
  EFI_STATUS                        TimerStatus;
  EFI_EVENT                         Timer;
  UINT64                            TimeRemained;

  if (MediaState == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *MediaState = EFI_SUCCESS;
  MediaInfo   = NULL;

  //
  // Get SNP handle
  //
  Snp = NULL;
  SnpHandle = NetLibGetSnpHandle2 (ServiceHandle, &Snp);
  if (SnpHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->HandleProtocol (
                  SnpHandle,
                  &gEfiAdapterInformationProtocolGuid,
                  (VOID *) &Aip
                  );
  if (EFI_ERROR (Status)) {

    MediaPresent = TRUE;
    Status = NetLibDetectMedia2 (ServiceHandle, &MediaPresent);
    if (!EFI_ERROR (Status)) {
      if (MediaPresent) {
        *MediaState = EFI_SUCCESS;
      } else {
        *MediaState = EFI_NO_MEDIA;
      }
    }

    //
    // NetLibDetectMedia doesn't support EFI_NOT_READY status, return now!
    //
    return Status;
  }

  Status = Aip->GetInformation (
                  Aip,
                  &gEfiAdapterInfoMediaStateGuid,
                  (VOID **) &MediaInfo,
                  &DataSize
                  );
  if (!EFI_ERROR (Status)) {

    *MediaState = MediaInfo->MediaState;
    FreePool (MediaInfo);
    if (*MediaState != EFI_NOT_READY || Timeout < MEDIA_STATE_DETECT_TIME_INTERVAL2) {

      return EFI_SUCCESS;
    }
  } else {

    if (MediaInfo != NULL) {
      FreePool (MediaInfo);
    }

    if (Status == EFI_UNSUPPORTED) {

      //
      // If gEfiAdapterInfoMediaStateGuid is not supported, call NetLibDetectMedia to get media state!
      //
      MediaPresent = TRUE;
      Status = NetLibDetectMedia2 (ServiceHandle, &MediaPresent);
      if (!EFI_ERROR (Status)) {
        if (MediaPresent) {
          *MediaState = EFI_SUCCESS;
        } else {
          *MediaState = EFI_NO_MEDIA;
        }
      }
      return Status;
    }

    return Status;
  }

  //
  // Loop to check media state
  //

  Timer        = NULL;
  TimeRemained = Timeout;
  Status = gBS->CreateEvent (EVT_TIMER, TPL_CALLBACK, NULL, NULL, &Timer);
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  do {
    Status = gBS->SetTimer (
                    Timer,
                    TimerRelative,
                    MEDIA_STATE_DETECT_TIME_INTERVAL2
                    );
    if (EFI_ERROR (Status)) {
      gBS->CloseEvent(Timer);
      return EFI_DEVICE_ERROR;
    }

    do {
      TimerStatus = gBS->CheckEvent (Timer);
      if (!EFI_ERROR (TimerStatus)) {

        TimeRemained -= MEDIA_STATE_DETECT_TIME_INTERVAL2;
        Status = Aip->GetInformation (
                        Aip,
                        &gEfiAdapterInfoMediaStateGuid,
                        (VOID **) &MediaInfo,
                        &DataSize
                        );
        if (!EFI_ERROR (Status)) {

          *MediaState = MediaInfo->MediaState;
          FreePool (MediaInfo);
        } else {

          if (MediaInfo != NULL) {
            FreePool (MediaInfo);
          }
          gBS->CloseEvent(Timer);
          return Status;
        }
      }
    } while (TimerStatus == EFI_NOT_READY);
  } while (*MediaState == EFI_NOT_READY && TimeRemained >= MEDIA_STATE_DETECT_TIME_INTERVAL2);

  gBS->CloseEvent(Timer);
  if (*MediaState == EFI_NOT_READY && TimeRemained < MEDIA_STATE_DETECT_TIME_INTERVAL2) {
    return EFI_TIMEOUT;
  } else {
    return EFI_SUCCESS;
  }
}



/**
  Reads a 32-bit value from memory that may be unaligned.

  This function returns the 32-bit value pointed to by Buffer. The function
  guarantees that the read operation does not produce an alignment fault.

  If the Buffer is NULL, then ASSERT().

  @param  Buffer  A pointer to a 32-bit value that may be unaligned.

  @return The 32-bit value read from Buffer.

**/
UINT32
EFIAPI
ReadUnaligned322 (
  IN CONST UINT32              *Buffer
  )
{
  ASSERT (Buffer != NULL);

  return *Buffer;
}



/**
  Determine if a IP4 address is unspecified.

  @param[in] Address  The address to test.

  @retval TRUE      It is.
  @retval FALSE     It is not.
**/
BOOLEAN
PingNetIp4IsUnspecifiedAddr2 (
  IN CONST EFI_IPv4_ADDRESS *Address
  )
{
  return  ((BOOLEAN)((ReadUnaligned322 ((UINT32*)&Address->Addr[0])) == 0x00000000));
}



/**
  Check whether the incoming Ipv6 address is a link-local address.

  ASSERT if Ip6 is NULL.

  @param[in] Ip6   - Ip6 address, in network order.

  @retval TRUE  - Yes, link-local address
  @retval FALSE - No

**/
BOOLEAN
EFIAPI
NetIp6IsLinkLocalAddr2 (
  IN EFI_IPv6_ADDRESS *Ip6
  )
{
  UINT8 Index;

  ASSERT (Ip6 != NULL);

  if (Ip6->Addr[0] != 0xFE) {
    return FALSE;
  }

  if (Ip6->Addr[1] != 0x80) {
    return FALSE;
  }

  for (Index = 2; Index < 8; Index++) {
    if (Ip6->Addr[Index] != 0) {
      return FALSE;
    }
  }

  return TRUE;
}


/**
  Check whether the incoming Ipv6 address is the unspecified address or not.

  ASSERT if Ip6 is NULL.

  @param[in] Ip6   - Ip6 address, in network order.

  @retval TRUE     - Yes, unspecified
  @retval FALSE    - No

**/
BOOLEAN
EFIAPI
NetIp6IsUnspecifiedAddr2 (
  IN EFI_IPv6_ADDRESS       *Ip6
  )
{
  UINT8 Index;

  ASSERT (Ip6 != NULL);

  for (Index = 0; Index < 16; Index++) {
    if (Ip6->Addr[Index] != 0) {
      return FALSE;
    }
  }

  return TRUE;
}



/**
  Get the timer period of the system.

  This function tries to get the system timer period by creating
  an 1ms period timer.

  @return     System timer period in MS, or 0 if operation failed.

**/
UINT32
GetTimerPeriod2(
  VOID
  )
{
  EFI_STATUS                 Status;
  UINT32                     RttTimerTick;
  EFI_EVENT                  TimerEvent;
  UINT32                     StallCounter;
  EFI_TPL                    OldTpl;

  RttTimerTick = 0;
  StallCounter   = 0;

  Status = gBS->CreateEvent (
                  EVT_TIMER | EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  RttTimerTickRoutine2,
                  &RttTimerTick,
                  &TimerEvent
                  );
  if (EFI_ERROR (Status)) {
    return 0;
  }

  OldTpl = gBS->RaiseTPL (TPL_CALLBACK);
  Status = gBS->SetTimer (
                  TimerEvent,
                  TimerPeriodic,
                  TICKS_PER_MS2
                  );
  if (EFI_ERROR (Status)) {
    gBS->CloseEvent (TimerEvent);
    return 0;
  }

  while (RttTimerTick < 10) {
    gBS->Stall (STALL_1_MILLI_SECOND2);
    ++StallCounter;
  }

  gBS->RestoreTPL (OldTpl);

  gBS->SetTimer (TimerEvent, TimerCancel, 0);
  gBS->CloseEvent (TimerEvent);

  return StallCounter / RttTimerTick;
}

/**
  Initialize the timer event for RTT (round trip time).

  @param[in]    Private    The pointer to PING_PRIVATE_DATA2.

  @retval EFI_SUCCESS      RTT timer is started.
  @retval Others           Failed to start the RTT timer.

**/
EFI_STATUS
PingInitRttTimer2 (
  PING_PRIVATE_DATA2      *Private
  )
{
  EFI_STATUS                 Status;

  Private->TimerPeriod = GetTimerPeriod2 ();
  if (Private->TimerPeriod == 0) {
    return EFI_ABORTED;
  }

  Private->RttTimerTick = 0;
  Status = gBS->CreateEvent (
                  EVT_TIMER | EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  RttTimerTickRoutine2,
                  &Private->RttTimerTick,
                  &Private->RttTimer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->SetTimer (
                  Private->RttTimer,
                  TimerPeriodic,
                  TICKS_PER_MS2
                  );
  if (EFI_ERROR (Status)) {
    gBS->CloseEvent (Private->RttTimer);
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Free RTT timer event resource.

  @param[in]    Private    The pointer to PING_PRIVATE_DATA2.

**/
VOID
PingFreeRttTimer2 (
  PING_PRIVATE_DATA2      *Private
  )
{
  if (Private->RttTimer != NULL) {
    gBS->SetTimer (Private->RttTimer, TimerCancel, 0);
    gBS->CloseEvent (Private->RttTimer);
  }
}

/**
  Read the current time.

  @param[in]    Private    The pointer to PING_PRIVATE_DATA2.

  @retval the current tick value.
**/
UINT32
ReadTime2 (
  PING_PRIVATE_DATA2      *Private
  )
{
  return Private->RttTimerTick;
}


/**
  Destroy PING_ICMPX_TX_INFO2, and recollect the memory.

  @param[in]    TxInfo    The pointer to PING_ICMPX_TX_INFO2.
  @param[in]    IpChoice  Whether the token is IPv4 or IPv6
**/
VOID
PingDestroyTxInfo2 (
  IN PING_ICMPX_TX_INFO2    *TxInfo,
  IN UINT32                IpChoice
  )
{
  EFI_IP6_TRANSMIT_DATA    *Ip6TxData;
  EFI_IP4_TRANSMIT_DATA    *Ip4TxData;
  EFI_IP6_FRAGMENT_DATA    *FragData;
  UINTN                    Index;

  if (TxInfo == NULL) {
    return;
  }

  if (TxInfo->Token != NULL) {

    if (TxInfo->Token->Event != NULL) {
      gBS->CloseEvent (TxInfo->Token->Event);
    }

    if (TxInfo->Token->Packet.TxData != NULL) {
      if (IpChoice == PING_IP_CHOICE_IP62) {
        Ip6TxData = TxInfo->Token->Packet.TxData;

        if (Ip6TxData->OverrideData != NULL) {
          FreePool (Ip6TxData->OverrideData);
        }

        if (Ip6TxData->ExtHdrs != NULL) {
          FreePool (Ip6TxData->ExtHdrs);
        }

        for (Index = 0; Index < Ip6TxData->FragmentCount; Index++) {
          FragData = Ip6TxData->FragmentTable[Index].FragmentBuffer;
          if (FragData != NULL) {
            FreePool (FragData);
          }
        }
      } else {
        Ip4TxData = TxInfo->Token->Packet.TxData;

        if (Ip4TxData->OverrideData != NULL) {
          FreePool (Ip4TxData->OverrideData);
        }

        for (Index = 0; Index < Ip4TxData->FragmentCount; Index++) {
          FragData = Ip4TxData->FragmentTable[Index].FragmentBuffer;
          if (FragData != NULL) {
            FreePool (FragData);
          }
        }
      }
    }

    FreePool (TxInfo->Token);
  }

  FreePool (TxInfo);
}

/**
  Match the request, and reply with SequenceNum/TimeStamp.

  @param[in]    Private    The pointer to PING_PRIVATE_DATA2.
  @param[in]    Packet     The pointer to ICMPX_ECHO_REQUEST_REPLY.

  @retval EFI_SUCCESS      The match is successful.
  @retval EFI_NOT_FOUND    The reply can't be matched with any request.

**/
EFI_STATUS
Ping6MatchEchoReply2 (
  IN PING_PRIVATE_DATA2           *Private,
  IN ICMPX_ECHO_REQUEST_REPLY2    *Packet
  )
{
  PING_ICMPX_TX_INFO2     *TxInfo;
  LIST_ENTRY             *Entry;
  LIST_ENTRY             *NextEntry;

  L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Ping6MatchEchoReply2: \n", __LINE__);
  

  NET_LIST_FOR_EACH_SAFE2 (Entry, NextEntry, &Private->TxList) {
    TxInfo = BASE_CR2 (Entry, PING_ICMPX_TX_INFO2, Link);

    if ((TxInfo->SequenceNum == Packet->SequenceNum) && (TxInfo->TimeStamp == Packet->TimeStamp)) {
      Private->RxCount++;
      RemoveEntryList (&TxInfo->Link);
      PingDestroyTxInfo2 (TxInfo, Private->IpChoice);
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}


UINT32
CalculateTick2 (
  PING_PRIVATE_DATA2      *Private,
  IN UINT32              Begin,
  IN UINT32              End
  )
{
  if (End < Begin) {
    return (0);
  }

  return (End - Begin) * Private->TimerPeriod;
}

/**
  The original intention is to send a request.
  Currently, the application retransmits an icmp6 echo request packet
  per second in sendnumber times that is specified by the user.
  Because nothing can be done here, all things move to the timer rountine.

  @param[in]    Event      A EFI_EVENT type event.
  @param[in]    Context    The pointer to Context.

**/
VOID
EFIAPI
Ping6OnEchoRequestSent2 (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
}

/**
  receive reply, match and print reply infomation.

  @param[in]    Event      A EFI_EVENT type event.
  @param[in]    Context    The pointer to context.

**/
VOID
EFIAPI
Ping6OnEchoReplyReceived2 (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS                  Status;
  PING_PRIVATE_DATA2           *Private;
  ICMPX_ECHO_REQUEST_REPLY2    *Reply;
  UINT32                      PayLoad;
  UINT32                      Rtt;

  L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Ping6OnEchoReplyReceived2: \n", __LINE__);
  

  Private = (PING_PRIVATE_DATA2 *) Context;

  if (Private == NULL || Private->Status == EFI_ABORTED || Private->Signature != PING_PRIVATE_DATA2_SIGNATURE2) {
    return;
  }

  if (Private->RxToken.Packet.RxData == NULL) {
    return;
  }

  if (Private->IpChoice == PING_IP_CHOICE_IP62) {
    Reply   = ((EFI_IP6_RECEIVE_DATA*)Private->RxToken.Packet.RxData)->FragmentTable[0].FragmentBuffer;
    PayLoad = ((EFI_IP6_RECEIVE_DATA*)Private->RxToken.Packet.RxData)->DataLength;
    if (((EFI_IP6_RECEIVE_DATA*)Private->RxToken.Packet.RxData)->Header->NextHeader != IP6_ICMP2) {
      goto ON_EXIT;
    }
    if (!IP6_IS_MULTICAST2 ((EFI_IPv6_ADDRESS*)&Private->DstAddress) &&
        !EFI_IP6_EQUAL2 (&((EFI_IP6_RECEIVE_DATA*)Private->RxToken.Packet.RxData)->Header->SourceAddress, (EFI_IPv6_ADDRESS*)&Private->DstAddress)) {
      goto ON_EXIT;
    }

    if ((Reply->Type != ICMP_V6_ECHO_REPLY2) || (Reply->Code != 0)) {
      goto ON_EXIT;
    }
  } else {
    Reply   = ((EFI_IP4_RECEIVE_DATA*)Private->RxToken.Packet.RxData)->FragmentTable[0].FragmentBuffer;
    PayLoad = ((EFI_IP4_RECEIVE_DATA*)Private->RxToken.Packet.RxData)->DataLength;
    if (!IP4_IS_MULTICAST2 (EFI_IP42(*(EFI_IPv4_ADDRESS*)Private->DstAddress)) &&
        !EFI_IP4_EQUAL2 (&((EFI_IP4_RECEIVE_DATA*)Private->RxToken.Packet.RxData)->Header->SourceAddress, (EFI_IPv4_ADDRESS*)&Private->DstAddress)) {
      goto ON_EXIT;
    }

    if ((Reply->Type != ICMP_V4_ECHO_REPLY2) || (Reply->Code != 0)) {
      goto ON_EXIT;
    }
  }


  if (PayLoad != Private->BufferSize) {
    goto ON_EXIT;
  }
  //
  // Check whether the reply matches the sent request before.
  //
  Status = Ping6MatchEchoReply2 (Private, Reply);
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }
  //
  // Display statistics on this icmp6 echo reply packet.
  //
  Rtt  = CalculateTick2 (Private, Reply->TimeStamp, ReadTime2 (Private));

  Private->RttSum += Rtt;
  Private->RttMin  = Private->RttMin > Rtt ? Rtt : Private->RttMin;
  Private->RttMax  = Private->RttMax < Rtt ? Rtt : Private->RttMax;

  L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Ping6OnEchoReplyReceived2: %d %d %d %d %d %d %d %d %d %d\n", __LINE__, Reply->SequenceNum, Rtt, Private->DstAddress[0], Private->DstAddress[1], Private->DstAddress[2], Private->DstAddress[3], Private->SrcAddress[0], Private->SrcAddress[1], Private->SrcAddress[2], Private->SrcAddress[3]);
  

  /*//ShellPrintHiiEx (
    -1,
    -1,
    NULL,
    STRING_TOKEN (STR_PING_REPLY_INFO),
    gShellNetwork1HiiHandle,
    PayLoad,
    mDstString,
    Reply->SequenceNum,
    Private->IpChoice == PING_IP_CHOICE_IP62?((EFI_IP6_RECEIVE_DATA*)Private->RxToken.Packet.RxData)->Header->HopLimit:0,
    Rtt,
    Rtt + Private->TimerPeriod
    );
	*/
ON_EXIT:

  //
  // Recycle the packet before reusing RxToken
  //
  gBS->SignalEvent (Private->IpChoice == PING_IP_CHOICE_IP62?((EFI_IP6_RECEIVE_DATA*)Private->RxToken.Packet.RxData)->RecycleSignal:((EFI_IP4_RECEIVE_DATA*)Private->RxToken.Packet.RxData)->RecycleSignal);

  if (Private->RxCount < Private->SendNum) {
    //
    // Continue to receive icmp echo reply packets.
    //
    Private->RxToken.Status = EFI_ABORTED;

    Status = Private->ProtocolPointers.Receive (Private->IpProtocol, &Private->RxToken);

    if (EFI_ERROR (Status)) {
      //ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_PING_RECEIVE), gShellNetwork1HiiHandle, Status);
      Private->Status = EFI_ABORTED;
    }
  } else {
    //
    // All reply have already been received from the dest host.
    //
    Private->Status = EFI_SUCCESS;
  }
}


/**
  Add two checksums.

  @param[in]   Checksum1             The first checksum to be added.
  @param[in]   Checksum2             The second checksum to be added.

  @return         The new checksum.

**/
UINT16
EFIAPI
NetAddChecksum2 (
  IN UINT16                 Checksum1,
  IN UINT16                 Checksum2
  )
{
  UINT32                    Sum;

  Sum = Checksum1 + Checksum2;

  //
  // two UINT16 can only add up to a carry of 1.
  //
  if ((Sum >> 16) != 0) {
    Sum = (Sum & 0xffff) + 1;

  }

  return (UINT16) Sum;
}


/**
  Create a PING_IPX_COMPLETION_TOKEN2.

  @param[in]    Private        The pointer of PING_PRIVATE_DATA2.
  @param[in]    TimeStamp      The TimeStamp of request.
  @param[in]    SequenceNum    The SequenceNum of request.

  @return The pointer of PING_IPX_COMPLETION_TOKEN2.

**/
PING_IPX_COMPLETION_TOKEN2 *
PingGenerateToken2 (
  IN PING_PRIVATE_DATA2    *Private,
  IN UINT32                TimeStamp,
  IN UINT16                SequenceNum
  )
{
  EFI_STATUS                  Status;
  PING_IPX_COMPLETION_TOKEN2   *Token;
  VOID                        *TxData;
  ICMPX_ECHO_REQUEST_REPLY2    *Request;
  UINT16                        HeadSum;
  UINT16                        TempChecksum;

  Request = AllocateZeroPool (Private->BufferSize);
  if (Request == NULL) {
    return NULL;
  }
  TxData = AllocateZeroPool (Private->IpChoice==PING_IP_CHOICE_IP62?sizeof (EFI_IP6_TRANSMIT_DATA):sizeof (EFI_IP4_TRANSMIT_DATA));
  if (TxData == NULL) {
    FreePool (Request);
    return NULL;
  }
  Token = AllocateZeroPool (sizeof (PING_IPX_COMPLETION_TOKEN2));
  if (Token == NULL) {
    FreePool (Request);
    FreePool (TxData);
    return NULL;
  }

  //
  // Assembly echo request packet.
  //
  Request->Type        = (UINT8)(Private->IpChoice==PING_IP_CHOICE_IP62?ICMP_V6_ECHO_REQUEST2:ICMP_V4_ECHO_REQUEST2);
  Request->Code        = 0;
  Request->SequenceNum = SequenceNum;
  Request->Identifier  = 0;
  Request->Checksum    = 0;

  //
  // Assembly token for transmit.
  //
  if (Private->IpChoice==PING_IP_CHOICE_IP62) {
    Request->TimeStamp   = TimeStamp;
    ((EFI_IP6_TRANSMIT_DATA*)TxData)->ExtHdrsLength                   = 0;
    ((EFI_IP6_TRANSMIT_DATA*)TxData)->ExtHdrs                         = NULL;
    ((EFI_IP6_TRANSMIT_DATA*)TxData)->OverrideData                    = 0;
    ((EFI_IP6_TRANSMIT_DATA*)TxData)->DataLength                      = Private->BufferSize;
    ((EFI_IP6_TRANSMIT_DATA*)TxData)->FragmentCount                   = 1;
    ((EFI_IP6_TRANSMIT_DATA*)TxData)->FragmentTable[0].FragmentBuffer = (VOID *) Request;
    ((EFI_IP6_TRANSMIT_DATA*)TxData)->FragmentTable[0].FragmentLength = Private->BufferSize;
  } else {
    ((EFI_IP4_TRANSMIT_DATA*)TxData)->OptionsLength                   = 0;
    ((EFI_IP4_TRANSMIT_DATA*)TxData)->OptionsBuffer                   = NULL;
    ((EFI_IP4_TRANSMIT_DATA*)TxData)->OverrideData                    = 0;
    ((EFI_IP4_TRANSMIT_DATA*)TxData)->TotalDataLength                 = Private->BufferSize;
    ((EFI_IP4_TRANSMIT_DATA*)TxData)->FragmentCount                   = 1;
    ((EFI_IP4_TRANSMIT_DATA*)TxData)->FragmentTable[0].FragmentBuffer = (VOID *) Request;
    ((EFI_IP4_TRANSMIT_DATA*)TxData)->FragmentTable[0].FragmentLength = Private->BufferSize;
    ((EFI_IP4_TRANSMIT_DATA*)TxData)->DestinationAddress.Addr[0]      = Private->DstAddress[0];
    ((EFI_IP4_TRANSMIT_DATA*)TxData)->DestinationAddress.Addr[1]      = Private->DstAddress[1];
    ((EFI_IP4_TRANSMIT_DATA*)TxData)->DestinationAddress.Addr[2]      = Private->DstAddress[2];
    ((EFI_IP4_TRANSMIT_DATA*)TxData)->DestinationAddress.Addr[3]      = Private->DstAddress[3];

    HeadSum = NetChecksum ((UINT8 *) Request, Private->BufferSize);
    Request->TimeStamp   = TimeStamp;
    TempChecksum = NetChecksum ((UINT8 *) &Request->TimeStamp, sizeof (UINT64));
    Request->Checksum = (UINT16)(~NetAddChecksum2 (HeadSum, TempChecksum));
  }


  Token->Status         = EFI_ABORTED;
  Token->Packet.TxData  = TxData;

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  Ping6OnEchoRequestSent2,
                  Private,
                  &Token->Event
                  );

  if (EFI_ERROR (Status)) {
    FreePool (Request);
    FreePool (TxData);
    FreePool (Token);
    return NULL;
  }

  return Token;
}

/**
  Transmit the PING_IPX_COMPLETION_TOKEN2.

  @param[in]    Private    The pointer of PING_PRIVATE_DATA2.

  @retval EFI_SUCCESS             Transmitted successfully.
  @retval EFI_OUT_OF_RESOURCES    No memory is available on the platform.
  @retval others                  Transmitted unsuccessfully.

**/
EFI_STATUS
PingSendEchoRequest2 (
  IN PING_PRIVATE_DATA2    *Private
  )
{
  EFI_STATUS             Status;
  PING_ICMPX_TX_INFO2     *TxInfo;

  TxInfo = AllocateZeroPool (sizeof (PING_ICMPX_TX_INFO2));

  if (TxInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  TxInfo->TimeStamp   = ReadTime2 (Private);
  TxInfo->SequenceNum = (UINT16) (Private->TxCount + 1);
  TxInfo->Token       = PingGenerateToken2 (
                          Private,
                          TxInfo->TimeStamp,
                          TxInfo->SequenceNum
                          );

  if (TxInfo->Token == NULL) {
    PingDestroyTxInfo2 (TxInfo, Private->IpChoice);
    return EFI_OUT_OF_RESOURCES;
  }

  ASSERT(Private->ProtocolPointers.Transmit != NULL);

  InsertTailList (&Private->TxList, &TxInfo->Link);

  Status = Private->ProtocolPointers.Transmit (Private->IpProtocol, TxInfo->Token);
  
  L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: PingSendEchoRequest2: \n", __LINE__);
  

  if (EFI_ERROR (Status)) {
    RemoveEntryList (&TxInfo->Link);
    PingDestroyTxInfo2 (TxInfo, Private->IpChoice);
    return Status;
  }

  Private->TxCount++;

  return EFI_SUCCESS;
}

/**
  Place a completion token into the receive packet queue to receive the echo reply.

  @param[in]    Private    The pointer of PING_PRIVATE_DATA2.

  @retval EFI_SUCCESS      Put the token into the receive packet queue successfully.
  @retval others           Put the token into the receive packet queue unsuccessfully.

**/
EFI_STATUS
Ping6ReceiveEchoReply2 (
  IN PING_PRIVATE_DATA2    *Private
  )
{
  EFI_STATUS    Status;

  L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Ping6ReceiveEchoReply2: \n", __LINE__);
  

  ZeroMem (&Private->RxToken, sizeof (PING_IPX_COMPLETION_TOKEN2));

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  Ping6OnEchoReplyReceived2,
                  Private,
                  &Private->RxToken.Event
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Private->RxToken.Status = EFI_NOT_READY;

  Status = Private->ProtocolPointers.Receive (Private->IpProtocol, &Private->RxToken);
  if (EFI_ERROR (Status)) {
    //ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_PING_RECEIVE), gShellNetwork1HiiHandle, Status);
  }
  return Status;
}


/**
  Determine if a IP4 address is Link Local.

  169.254.1.0 through 169.254.254.255 is link local.

  @param[in] Address  The address to test.

  @retval TRUE      It is.
  @retval FALSE     It is not.
**/
BOOLEAN
PingNetIp4IsLinkLocalAddr2 (
  IN CONST EFI_IPv4_ADDRESS *Address
  )
{
  return ((BOOLEAN)(Address->Addr[0] == 169 && Address->Addr[1] == 254 && Address->Addr[2] >= 1 && Address->Addr[2] <= 254));
}


/**
  Create a valid IP instance.

  @param[in]    Private    The pointer of PING_PRIVATE_DATA2.

  @retval EFI_SUCCESS              Create a valid IPx instance successfully.
  @retval EFI_ABORTED              Locate handle with ipx service binding protocol unsuccessfully.
  @retval EFI_INVALID_PARAMETER    The source address is unspecified when the destination address is a link-local address.
  @retval EFI_OUT_OF_RESOURCES     No memory is available on the platform.
  @retval EFI_NOT_FOUND            The source address is not found.
**/
EFI_STATUS
PingCreateIpInstance2 (
  IN  PING_PRIVATE_DATA2    *Private
  )
{
  EFI_STATUS                       Status;
  UINTN                            HandleIndex;
  UINTN                            HandleNum;
  EFI_HANDLE                       *HandleBuffer;
  BOOLEAN                          UnspecifiedSrc;
  EFI_STATUS                       MediaStatus;
  EFI_SERVICE_BINDING_PROTOCOL     *EfiSb;
  VOID                             *IpXCfg;
  EFI_IP6_CONFIG_DATA              Ip6Config;
  EFI_IP4_CONFIG_DATA              Ip4Config;
  VOID                             *IpXInterfaceInfo;
  UINTN                            IfInfoSize;
  EFI_IPv6_ADDRESS                 *Addr;
  UINTN                            AddrIndex;

  HandleBuffer      = NULL;
  UnspecifiedSrc    = FALSE;
  MediaStatus       = EFI_SUCCESS;
  EfiSb             = NULL;
  IpXInterfaceInfo  = NULL;
  IfInfoSize        = 0;

  //
  // Locate all the handles with ip6 service binding protocol.
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  Private->IpChoice == PING_IP_CHOICE_IP62?&gEfiIp6ServiceBindingProtocolGuid:&gEfiIp4ServiceBindingProtocolGuid,
                  NULL,
                  &HandleNum,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status) || (HandleNum == 0) || (HandleBuffer == NULL)) {
    return EFI_ABORTED;
  }

  if (Private->IpChoice == PING_IP_CHOICE_IP62 ? NetIp6IsUnspecifiedAddr2 ((EFI_IPv6_ADDRESS*)&Private->SrcAddress) : \
      PingNetIp4IsUnspecifiedAddr2 ((EFI_IPv4_ADDRESS*)&Private->SrcAddress)) {
    //
    // SrcAddress is unspecified. So, both connected and configured interface will be automatic selected.
    //
    UnspecifiedSrc = TRUE;
  }

  //
  // Source address is required when pinging a link-local address.
  //
  if (Private->IpChoice == PING_IP_CHOICE_IP62) {
    if (NetIp6IsLinkLocalAddr2 ((EFI_IPv6_ADDRESS*)&Private->DstAddress) && UnspecifiedSrc) {
      //ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_PING_INVALID_SOURCE), gShellNetwork1HiiHandle);
      Status = EFI_INVALID_PARAMETER;
      goto ON_ERROR;
    }
  } else {
    ASSERT(Private->IpChoice == PING_IP_CHOICE_IP42);
    if (PingNetIp4IsLinkLocalAddr2 ((EFI_IPv4_ADDRESS*)&Private->DstAddress) && UnspecifiedSrc) {
      //ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_PING_INVALID_SOURCE), gShellNetwork1HiiHandle);
      Status = EFI_INVALID_PARAMETER;
      goto ON_ERROR;
    }
  }

  //
  // For each ip6 protocol, check interface addresses list.
  //
  for (HandleIndex = 0; HandleIndex < HandleNum; HandleIndex++) {
    EfiSb             = NULL;
    IpXInterfaceInfo  = NULL;
    IfInfoSize        = 0;

    if (UnspecifiedSrc) {
      //
      // Check media.
      //
      NetLibDetectMediaWaitTimeout2 (HandleBuffer[HandleIndex], 0, &MediaStatus);
      if (MediaStatus != EFI_SUCCESS) {
        //
        // Skip this one.
        //
        continue;
      }
    }

    Status = gBS->HandleProtocol (
                    HandleBuffer[HandleIndex],
                    Private->IpChoice == PING_IP_CHOICE_IP62?&gEfiIp6ServiceBindingProtocolGuid:&gEfiIp4ServiceBindingProtocolGuid,
                    (VOID **) &EfiSb
                    );
    if (EFI_ERROR (Status)) {
      goto ON_ERROR;
    }

    //
    // Ip6config protocol and ip6 service binding protocol are installed
    // on the same handle.
    //
    Status = gBS->HandleProtocol (
                    HandleBuffer[HandleIndex],
                    Private->IpChoice == PING_IP_CHOICE_IP62?&gEfiIp6ConfigProtocolGuid:&gEfiIp4Config2ProtocolGuid,
                    (VOID **) &IpXCfg
                    );

    if (EFI_ERROR (Status)) {
      goto ON_ERROR;
    }
    //
    // Get the interface information size.
    //
    if (Private->IpChoice == PING_IP_CHOICE_IP62) {
      Status = ((EFI_IP6_CONFIG_PROTOCOL*)IpXCfg)->GetData (
                         IpXCfg,
                         Ip6ConfigDataTypeInterfaceInfo,
                         &IfInfoSize,
                         NULL
                         );
    } else {
      Status = ((EFI_IP4_CONFIG2_PROTOCOL*)IpXCfg)->GetData (
                         IpXCfg,
                         Ip4Config2DataTypeInterfaceInfo,
                         &IfInfoSize,
                         NULL
                         );
    }

    //
    // Skip the ones not in current use.
    //
    if (Status == EFI_NOT_STARTED) {
      continue;
    }

    if (Status != EFI_BUFFER_TOO_SMALL) {
      //ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_PING_GETDATA), gShellNetwork1HiiHandle, Status);
      goto ON_ERROR;
    }

    IpXInterfaceInfo = AllocateZeroPool (IfInfoSize);

    if (IpXInterfaceInfo == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ON_ERROR;
    }
    //
    // Get the interface info.
    //
    if (Private->IpChoice == PING_IP_CHOICE_IP62) {
      Status = ((EFI_IP6_CONFIG_PROTOCOL*)IpXCfg)->GetData (
                         IpXCfg,
                         Ip6ConfigDataTypeInterfaceInfo,
                         &IfInfoSize,
                         IpXInterfaceInfo
                         );
    } else {
      Status = ((EFI_IP4_CONFIG2_PROTOCOL*)IpXCfg)->GetData (
                         IpXCfg,
                         Ip4Config2DataTypeInterfaceInfo,
                         &IfInfoSize,
                         IpXInterfaceInfo
                         );
    }

    if (EFI_ERROR (Status)) {
      //ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_PING_GETDATA), gShellNetwork1HiiHandle, Status);
      goto ON_ERROR;
    }
    //
    // Check whether the source address is one of the interface addresses.
    //
    if (Private->IpChoice == PING_IP_CHOICE_IP62) {
      for (AddrIndex = 0; AddrIndex < ((EFI_IP6_CONFIG_INTERFACE_INFO*)IpXInterfaceInfo)->AddressInfoCount; AddrIndex++) {
        Addr = &(((EFI_IP6_CONFIG_INTERFACE_INFO*)IpXInterfaceInfo)->AddressInfo[AddrIndex].Address);

        if (UnspecifiedSrc) {
          if (!NetIp6IsUnspecifiedAddr2 (Addr) && !NetIp6IsLinkLocalAddr2 (Addr)) {
            //
            // Select the interface automatically.
            //
            CopyMem(&Private->SrcAddress, Addr, sizeof(Private->SrcAddress));
            break;
          }
        } else if (EFI_IP6_EQUAL2 (&Private->SrcAddress, Addr)) {
          //
          // Match a certain interface address.
          //
          break;
        }
      }

      if (AddrIndex < ((EFI_IP6_CONFIG_INTERFACE_INFO*)IpXInterfaceInfo)->AddressInfoCount) {
        //
        // Found a nic handle with right interface address.
        //
        break;
      }
    } else {
      if (UnspecifiedSrc) {
        if (!PingNetIp4IsUnspecifiedAddr2 (&((EFI_IP4_CONFIG2_INTERFACE_INFO*)IpXInterfaceInfo)->StationAddress) &&
            !PingNetIp4IsLinkLocalAddr2 (&((EFI_IP4_CONFIG2_INTERFACE_INFO*)IpXInterfaceInfo)->StationAddress)) {
          //
          // Select the interface automatically.
          //
          break;
        }
      } else if (EFI_IP4_EQUAL2 (&Private->SrcAddress, &((EFI_IP4_CONFIG2_INTERFACE_INFO*)IpXInterfaceInfo)->StationAddress)) {
        //
        // Match a certain interface address.
        //
        break;
      }
    }

    FreePool (IpXInterfaceInfo);
    IpXInterfaceInfo = NULL;
  }
  //
  // No exact interface address matched.
  //

  if (HandleIndex == HandleNum) {
    //ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_PING_CONFIGD_NIC_NF), gShellNetwork1HiiHandle, L"ping");
    Status = EFI_NOT_FOUND;
    goto ON_ERROR;
  }

  Private->NicHandle = HandleBuffer[HandleIndex];

  ASSERT (EfiSb != NULL);
  Status = EfiSb->CreateChild (EfiSb, &Private->IpChildHandle);

  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }
  if (Private->IpChoice == PING_IP_CHOICE_IP62) {
    Status = gBS->OpenProtocol (
                    Private->IpChildHandle,
                    &gEfiIp6ProtocolGuid,
                    &Private->IpProtocol,
                    gImageHandle,
                    Private->IpChildHandle,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
    if (EFI_ERROR (Status)) {
      goto ON_ERROR;
    }


    ZeroMem (&Ip6Config, sizeof (EFI_IP6_CONFIG_DATA));

    //
    // Configure the ip6 instance for icmp6 packet exchange.
    //
    Ip6Config.DefaultProtocol   = 58;
    Ip6Config.AcceptAnyProtocol = FALSE;
    Ip6Config.AcceptIcmpErrors  = TRUE;
    Ip6Config.AcceptPromiscuous = FALSE;
    Ip6Config.TrafficClass      = 0;
    Ip6Config.HopLimit          = 128;
    Ip6Config.FlowLabel         = 0;
    Ip6Config.ReceiveTimeout    = 0;
    Ip6Config.TransmitTimeout   = 0;

    IP6_COPY_ADDRESS2 (&Ip6Config.StationAddress,     &Private->SrcAddress);
    IP6_COPY_ADDRESS2 (&Ip6Config.DestinationAddress, &Private->DstAddress);

    Status = ((EFI_IP6_PROTOCOL*)(Private->IpProtocol))->Configure (Private->IpProtocol, &Ip6Config);

    if (EFI_ERROR (Status)) {
      //ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_PING_CONFIG), gShellNetwork1HiiHandle, Status);
      goto ON_ERROR;
    }

    Private->ProtocolPointers.Transmit  = (PING_IPX_TRANSMIT2 )((EFI_IP6_PROTOCOL2*)Private->IpProtocol)->Transmit;
    Private->ProtocolPointers.Receive   = (PING_IPX_RECEIVE2  )((EFI_IP6_PROTOCOL2*)Private->IpProtocol)->Receive;
    Private->ProtocolPointers.Cancel    = (PING_IPX_CANCEL2   )((EFI_IP6_PROTOCOL2*)Private->IpProtocol)->Cancel;
    Private->ProtocolPointers.Poll      = (PING_IPX_POLL2     )((EFI_IP6_PROTOCOL2*)Private->IpProtocol)->Poll;
  } else {
    Status = gBS->OpenProtocol (
                    Private->IpChildHandle,
                    &gEfiIp4ProtocolGuid,
                    &Private->IpProtocol,
                    gImageHandle,
                    Private->IpChildHandle,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
    if (EFI_ERROR (Status)) {
      goto ON_ERROR;
    }


    ZeroMem (&Ip4Config, sizeof (EFI_IP4_CONFIG_DATA));

    //
    // Configure the ip4 instance for icmp4 packet exchange.
    //
    Ip4Config.DefaultProtocol   = 1;
    Ip4Config.AcceptAnyProtocol = FALSE;
    Ip4Config.AcceptBroadcast   = FALSE;
    Ip4Config.AcceptIcmpErrors  = TRUE;
    Ip4Config.AcceptPromiscuous = FALSE;
    Ip4Config.DoNotFragment     = FALSE;
    Ip4Config.RawData           = FALSE;
    Ip4Config.ReceiveTimeout    = 0;
    Ip4Config.TransmitTimeout   = 0;
    Ip4Config.UseDefaultAddress = TRUE;
    Ip4Config.TimeToLive        = 128;
    Ip4Config.TypeOfService     = 0;

    Status = ((EFI_IP4_PROTOCOL*)(Private->IpProtocol))->Configure (Private->IpProtocol, &Ip4Config);

    if (EFI_ERROR (Status)) {
      //ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_PING_CONFIG), gShellNetwork1HiiHandle, Status);
      goto ON_ERROR;
    }

    Private->ProtocolPointers.Transmit  = (PING_IPX_TRANSMIT2 )((EFI_IP4_PROTOCOL*)Private->IpProtocol)->Transmit;
    Private->ProtocolPointers.Receive   = (PING_IPX_TRANSMIT2  )((EFI_IP4_PROTOCOL*)Private->IpProtocol)->Receive;
    Private->ProtocolPointers.Cancel    = (PING_IPX_CANCEL2   )((EFI_IP4_PROTOCOL*)Private->IpProtocol)->Cancel;
    Private->ProtocolPointers.Poll      = (PING_IPX_POLL2     )((EFI_IP4_PROTOCOL*)Private->IpProtocol)->Poll;
  }

  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
  }

  return EFI_SUCCESS;

ON_ERROR:
  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
  }

  if (IpXInterfaceInfo != NULL) {
    FreePool (IpXInterfaceInfo);
  }

  if ((EfiSb != NULL) && (Private->IpChildHandle != NULL)) {
    EfiSb->DestroyChild (EfiSb, Private->IpChildHandle);
  }

  return Status;
}

/**
  Destroy the IP instance.

  @param[in]    Private    The pointer of PING_PRIVATE_DATA2.

**/
VOID
Ping6DestroyIp6Instance2 (
  IN PING_PRIVATE_DATA2    *Private
  )
{
  EFI_STATUS                      Status;
  EFI_SERVICE_BINDING_PROTOCOL    *IpSb;

  gBS->CloseProtocol (
         Private->IpChildHandle,
         Private->IpChoice == PING_IP_CHOICE_IP62?&gEfiIp6ProtocolGuid:&gEfiIp4ProtocolGuid,
         gImageHandle,
         Private->IpChildHandle
         );

  Status = gBS->HandleProtocol (
                  Private->NicHandle,
                  Private->IpChoice == PING_IP_CHOICE_IP62?&gEfiIp6ServiceBindingProtocolGuid:&gEfiIp4ServiceBindingProtocolGuid,
                  (VOID **) &IpSb
                  );

  if (!EFI_ERROR(Status)) {
    IpSb->DestroyChild (IpSb, Private->IpChildHandle);
  }
}

/**
  Function for 'ping' command.

  @param[in] ImageHandle  Handle to the Image (NULL if Internal).
  @param[in] SystemTable  Pointer to the System Table (NULL if Internal).

  @retval SHELL_SUCCESS  The ping processed successfullly.
  @retval others         The ping processed unsuccessfully.

**/
SHELL_STATUS
EFIAPI
ShellCommandRunPing2 (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS          Status;
  SHELL_STATUS        ShellStatus;
  EFI_IPv6_ADDRESS    DstAddress;
  EFI_IPv6_ADDRESS    SrcAddress;
  UINT64              BufferSize;
  UINTN               SendNumber;
  LIST_ENTRY          *ParamPackage;
  CONST CHAR16        *ValueStr;
  UINTN               NonOptionCount;
  UINT32              IpChoice;
  CHAR16              *ProblemParam;

  //
  // we use IPv6 buffers to hold items...
  // make sure this is enough space!
  //
  ASSERT(sizeof(EFI_IPv4_ADDRESS        ) <= sizeof(EFI_IPv6_ADDRESS         ));
  ASSERT(sizeof(EFI_IP4_COMPLETION_TOKEN) <= sizeof(EFI_IP6_COMPLETION_TOKEN ));

  IpChoice = PING_IP_CHOICE_IP42;

  ShellStatus = SHELL_SUCCESS;
  ProblemParam = NULL;

  Status = ShellCommandLineParseEx (PingParamList, &ParamPackage, &ProblemParam, TRUE, FALSE);
  if (EFI_ERROR(Status)) {
    //ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_GEN_PARAM_INV), gShellNetwork1HiiHandle, L"ping", ProblemParam);
    ShellStatus = SHELL_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  if (ShellCommandLineGetFlag (ParamPackage, L"-_ip6")) {
    IpChoice = PING_IP_CHOICE_IP62;
  }

  //
  // Parse the parameter of count number.
  //
  ValueStr = ShellCommandLineGetValue (ParamPackage, L"-n");
  if (ValueStr != NULL) {
    SendNumber = ShellStrToUintn (ValueStr);

    //
    // ShellStrToUintn will return 0 when input is 0 or an invalid input string.
    //
    if ((SendNumber == 0) || (SendNumber > MAX_SEND_NUMBER2)) {
      //ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_GEN_PARAM_INV), gShellNetwork1HiiHandle, L"ping", ValueStr);
      ShellStatus = SHELL_INVALID_PARAMETER;
      goto ON_EXIT;
    }
  } else {
    SendNumber = DEFAULT_SEND_COUNT2;
  }
  //
  // Parse the parameter of buffer size.
  //
  ValueStr = ShellCommandLineGetValue (ParamPackage, L"-l");
  if (ValueStr != NULL) {
    BufferSize = ShellStrToUintn (ValueStr);

    //
    // ShellStrToUintn will return 0 when input is 0 or an invalid input string.
    //
    if ((BufferSize < 16) || (BufferSize > MAX_BUFFER_SIZE2)) {
      //ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_GEN_PARAM_INV), gShellNetwork1HiiHandle, L"ping", ValueStr);
      ShellStatus = SHELL_INVALID_PARAMETER;
      goto ON_EXIT;
    }
  } else {
    BufferSize = DEFAULT_BUFFER_SIZE2;
  }

  ZeroMem (&SrcAddress, sizeof (EFI_IPv6_ADDRESS));
  ZeroMem (&DstAddress, sizeof (EFI_IPv6_ADDRESS));

  //
  // Parse the parameter of source ip address.
  //
  ValueStr = ShellCommandLineGetValue (ParamPackage, L"-s");
  if (ValueStr == NULL) {
    ValueStr = ShellCommandLineGetValue (ParamPackage, L"-_s");
  }

  if (ValueStr != NULL) {
    mSrcString = ValueStr;
    if (IpChoice == PING_IP_CHOICE_IP62) {
      Status = NetLibStrToIp6 (ValueStr, &SrcAddress);
    } else {
      Status = NetLibStrToIp4 (ValueStr, (EFI_IPv4_ADDRESS*)&SrcAddress);
    }
    if (EFI_ERROR (Status)) {
      ////ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_GEN_PARAM_INV), gShellNetwork1HiiHandle, L"ping", ValueStr);
      ShellStatus = SHELL_INVALID_PARAMETER;
      goto ON_EXIT;
    }
  }
  //
  // Parse the parameter of destination ip address.
  //
  NonOptionCount = ShellCommandLineGetCount(ParamPackage);
  if (NonOptionCount < 2) {
    ////ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_GEN_TOO_FEW), gShellNetwork1HiiHandle, L"ping");
    ShellStatus = SHELL_INVALID_PARAMETER;
    goto ON_EXIT;
  }
  if (NonOptionCount > 2) {
    ////ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_GEN_TOO_MANY), gShellNetwork1HiiHandle, L"ping");
    ShellStatus = SHELL_INVALID_PARAMETER;
    goto ON_EXIT;
  }
  ValueStr = ShellCommandLineGetRawValue (ParamPackage, 1);
  if (ValueStr != NULL) {
    mDstString = ValueStr;
    if (IpChoice == PING_IP_CHOICE_IP62) {
      Status = NetLibStrToIp6 (ValueStr, &DstAddress);
    } else {
      Status = NetLibStrToIp4 (ValueStr, (EFI_IPv4_ADDRESS*)&DstAddress);
    }
    if (EFI_ERROR (Status)) {
      ////ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_GEN_PARAM_INV), gShellNetwork1HiiHandle, L"ping", ValueStr);
      ShellStatus = SHELL_INVALID_PARAMETER;
      goto ON_EXIT;
    }
  }

  //
  // Enter into ping process.
  //
  ShellStatus = ShellPing (
             (UINT32)SendNumber,
             (UINT32)BufferSize,
             &SrcAddress,
             &DstAddress,
             IpChoice
             );

ON_EXIT:
  ShellCommandLineFreeVarList (ParamPackage);
  return ShellStatus;
}




BOOLEAN
InternalIsDecimalDigitCharacter2 (
  CHAR16                    Char
  )
{
  return (BOOLEAN) (Char >= L'0' && Char <= L'9');
}


RETURN_STATUS StrToIpv4Address2 (
  IN  CONST CHAR16       *String,
  OUT IPv4_ADDRESS       *Address
  )
{
  RETURN_STATUS          Status;
  UINTN                  AddressIndex;
  UINTN                  Uintn;
  IPv4_ADDRESS           LocalAddress;
  UINT8                  LocalPrefixLength;
  CHAR16                 *Pointer;

  LocalPrefixLength = MAX_UINT8;


  for (Pointer = (CHAR16 *) String, AddressIndex = 0; AddressIndex < ARRAY_SIZE2 (Address->Addr) + 1;) 
  {
    if (!InternalIsDecimalDigitCharacter2 (*Pointer)) {
      //
      // D or P contains invalid characters.
      //
      break;
    }

    //
    // Get D or P.
    //
    Status = StrDecimalToUintnS ((CONST CHAR16 *) Pointer, &Pointer, &Uintn);
    if (RETURN_ERROR (Status)) {
      return 3;
    }
    if (AddressIndex == ARRAY_SIZE (Address->Addr)) {
      //
      // It's P.
      //
      if (Uintn > 32) {
        return 3;
      }
      LocalPrefixLength = (UINT8) Uintn;
    } else {
      //
      // It's D.
      //
      if (Uintn > MAX_UINT8) {
        return 3;
      }
      LocalAddress.Addr[AddressIndex] = (UINT8) Uintn;
      AddressIndex++;
    }

    //
    // Check the '.' or '/', depending on the AddressIndex.
    //
    if (AddressIndex == ARRAY_SIZE (Address->Addr)) {
      if (*Pointer == L'/') {
        //
        // '/P' is in the String.
        // Skip "/" and get P in next loop.
        //
        Pointer++;
      } else {
        //
        // '/P' is not in the String.
        //
        break;
      }
    } else if (AddressIndex < ARRAY_SIZE (Address->Addr)) {
      if (*Pointer == L'.') {
        //
        // D should be followed by '.'
        //
        Pointer++;
      } else {
        return 3;
      }
    }
  }

  if (AddressIndex < ARRAY_SIZE (Address->Addr)) {
    return 3;
  }

  CopyMem (Address, &LocalAddress, sizeof (*Address));

  return RETURN_SUCCESS;
}



EFI_STATUS
EFIAPI
NetLibStrToIp42 (
  IN CONST CHAR16                *String,
  OUT      EFI_IPv4_ADDRESS      *Ip4Address
  )
{
  RETURN_STATUS                  Status;

  Status = StrToIpv4Address2 (String, Ip4Address);
  if (RETURN_ERROR (Status)) 
  {
    return EFI_INVALID_PARAMETER;
  } 
  else 
  {
    return EFI_SUCCESS;
  }
}



/**
  Remove the timeout request from the list.

  @param[in]    Event    A EFI_EVENT type event.
  @param[in]    Context  The pointer to Context.

**/
VOID
EFIAPI
Ping6OnTimerRoutine2 (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS             Status;
  PING_PRIVATE_DATA2      *Private;
  PING_ICMPX_TX_INFO2     *TxInfo;
  LIST_ENTRY             *Entry;
  LIST_ENTRY             *NextEntry;
  UINT64                 Time;

  Private = (PING_PRIVATE_DATA2 *) Context;
  if (Private->Signature != PING_PRIVATE_DATA2_SIGNATURE2) {
    Private->Status = EFI_NOT_FOUND;
    return;
  }

  //
  // Retransmit icmp6 echo request packets per second in sendnumber times.
  //
  if (Private->TxCount < Private->SendNum) {

    Status = PingSendEchoRequest2 (Private);
    if (Private->TxCount != 0){
      if (EFI_ERROR (Status)) {
        ////ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_PING_SEND_REQUEST), gShellNetwork1HiiHandle, Private->TxCount + 1);
      }
    }
  }
  //
  // Check whether any icmp6 echo request in the list timeout.
  //
  NET_LIST_FOR_EACH_SAFE2 (Entry, NextEntry, &Private->TxList) {
    TxInfo = BASE_CR2 (Entry, PING_ICMPX_TX_INFO2, Link);
    Time   = CalculateTick2 (Private, TxInfo->TimeStamp, ReadTime2 (Private));

    //
    // Remove the timeout echo request from txlist.
    //
    if (Time > DEFAULT_TIMEOUT2) {

      if (EFI_ERROR (TxInfo->Token->Status)) {
        Private->ProtocolPointers.Cancel (Private->IpProtocol, TxInfo->Token);
      }
      //
      // Remove the timeout icmp6 echo request from list.
      //
      ////ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_PING_TIMEOUT), gShellNetwork1HiiHandle, TxInfo->SequenceNum);

      RemoveEntryList (&TxInfo->Link);
      PingDestroyTxInfo2(TxInfo, Private->IpChoice);

      Private->RxCount++;
      Private->FailedCount++;

      if (IsListEmpty (&Private->TxList) && (Private->TxCount == Private->SendNum)) {
        //
        // All the left icmp6 echo request in the list timeout.
        //
        Private->Status = 19;
      }
    }
  }
}

#define ONE_SECOND2            10000000


/**
  The Ping Process.

  @param[in]   SendNumber     The send request count.
  @param[in]   BufferSize     The send buffer size.
  @param[in]   SrcAddress     The source address.
  @param[in]   DstAddress     The destination address.
  @param[in]   IpChoice       The choice between IPv4 and IPv6.

  @retval SHELL_SUCCESS  The ping processed successfullly.
  @retval others         The ping processed unsuccessfully.
**/
SHELL_STATUS
ShellPing2 (
  IN UINT32              SendNumber,
  IN UINT32              BufferSize,
  IN EFI_IPv6_ADDRESS    *SrcAddress,
  IN EFI_IPv6_ADDRESS    *DstAddress,
  IN UINT32              IpChoice
  )
{
	
  L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: ShellPing2: \n", __LINE__);
  EFI_STATUS             Status;
  PING_PRIVATE_DATA2      *Private;
  PING_ICMPX_TX_INFO2     *TxInfo;
  LIST_ENTRY             *Entry;
  LIST_ENTRY             *NextEntry;
  SHELL_STATUS           ShellStatus;

  ShellStatus = SHELL_SUCCESS;
  Private     = AllocateZeroPool (sizeof (PING_PRIVATE_DATA2));

  if (Private == NULL) {
    return (SHELL_OUT_OF_RESOURCES);
  }

  Private->IpChoice    = IpChoice;
  Private->Signature   = PING_PRIVATE_DATA2_SIGNATURE2;
  Private->SendNum     = SendNumber;
  Private->BufferSize  = BufferSize;
  Private->RttMin      = ~((UINT64 )(0x0));
  Private->Status      = EFI_NOT_READY;

  CopyMem(&Private->SrcAddress, SrcAddress, sizeof(Private->SrcAddress));
  CopyMem(&Private->DstAddress, DstAddress, sizeof(Private->DstAddress));

  InitializeListHead (&Private->TxList);

  //
  // Open and configure a ip instance for us.
  //
  Status = PingCreateIpInstance2 (Private);

  if (EFI_ERROR (Status)) {
    ShellStatus = SHELL_ACCESS_DENIED;
    goto ON_EXIT;
  }
  //
  // Print the command line itself.
  //
  ////ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_PING_START), gShellNetwork1HiiHandle, mDstString, Private->BufferSize);
  //
  // Create a ipv6 token to receive the first icmp6 echo reply packet.
  //
  Status = Ping6ReceiveEchoReply2 (Private);
  L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: ShellPing2: \n", __LINE__);
  

  if (EFI_ERROR (Status)) {
    ShellStatus = SHELL_ACCESS_DENIED;
    goto ON_EXIT;
  }
  //
  // Create and start timer to send icmp6 echo request packet per second.
  //
  Status = gBS->CreateEvent (
                  EVT_TIMER | EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  Ping6OnTimerRoutine2,
                  Private,
                  &Private->Timer
                  );

  if (EFI_ERROR (Status)) {
    ShellStatus = SHELL_ACCESS_DENIED;
    goto ON_EXIT;
  }

  //
  // Start a timer to calculate the RTT.
  //
  Status = PingInitRttTimer2(Private);
  if (EFI_ERROR (Status)) {
    ShellStatus = SHELL_ACCESS_DENIED;
    goto ON_EXIT;
  }

  L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: ShellPing2: \n", __LINE__);
  


  //
  // Create a ipv6 token to send the first icmp6 echo request packet.
  //
  Status = PingSendEchoRequest2 (Private);
  //
  // EFI_NOT_READY for IPsec is enable and IKE is not established.
  //
  if (EFI_ERROR (Status) && (Status != EFI_NOT_READY)) {
    ShellStatus = SHELL_ACCESS_DENIED;
    if(Status == EFI_NOT_FOUND) {
      ////ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_PING_NOSOURCE_INDO), gShellNetwork1HiiHandle, mDstString);
      
  		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: ShellPing2: \n", __LINE__);
  
    } else if (Status == RETURN_NO_MAPPING) {
    	
  		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: ShellPing2: \n", __LINE__);
  
      ////ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_PING_NOROUTE_FOUND), gShellNetwork1HiiHandle, mDstString, mSrcString);
    } else {
    	
  		L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: ShellPing2: \n", __LINE__);
  
      ////ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_PING_NETWORK_ERROR), gShellNetwork1HiiHandle, L"ping", Status);
    }

    goto ON_EXIT;
  }

  L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: ShellPing2: \n", __LINE__);
  

  Status = gBS->SetTimer (
                  Private->Timer,
                  TimerPeriodic,
                  ONE_SECOND2
                  );

  if (EFI_ERROR (Status)) {
    ShellStatus = SHELL_ACCESS_DENIED;
    goto ON_EXIT;
  }

  return;  

  //
  // Control the ping6 process by two factors:
  // 1. Hot key
  // 2. Private->Status
  //   2.1. success means all icmp6 echo request packets get reply packets.
  //   2.2. timeout means the last icmp6 echo reply request timeout to get reply.
  //   2.3. noready means ping6 process is on-the-go.
  //
  while (Private->Status == EFI_NOT_READY) {
    Status = Private->ProtocolPointers.Poll (Private->IpProtocol);
    if (ShellGetExecutionBreakFlag()) {
      Private->Status = EFI_ABORTED;
      goto ON_STAT;
    }
  }


  L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: ShellPing2: \n", __LINE__);
  

ON_STAT:
  //
  // Display the statistics in all.
  //
  gBS->SetTimer (Private->Timer, TimerCancel, 0);

  if (Private->TxCount != 0) {
    
  }

  if (Private->RxCount > Private->FailedCount) {
    
  }

ON_EXIT:

  if (Private != NULL) {

    NET_LIST_FOR_EACH_SAFE2 (Entry, NextEntry, &Private->TxList) {
      TxInfo = BASE_CR2 (Entry, PING_ICMPX_TX_INFO2, Link);

      if (Private->IpProtocol != NULL && Private->ProtocolPointers.Cancel != NULL) {
        Status = Private->ProtocolPointers.Cancel (Private->IpProtocol, TxInfo->Token);
      }

      RemoveEntryList (&TxInfo->Link);
      PingDestroyTxInfo2 (TxInfo, Private->IpChoice);
    }

    PingFreeRttTimer2 (Private);

    if (Private->Timer != NULL) {
      gBS->CloseEvent (Private->Timer);
    }

    if (Private->IpProtocol != NULL && Private->ProtocolPointers.Cancel != NULL) {
      Status = Private->ProtocolPointers.Cancel (Private->IpProtocol, &Private->RxToken);
    }

    if (Private->RxToken.Event != NULL) {
      gBS->CloseEvent (Private->RxToken.Event);
    }

    if (Private->IpChildHandle != NULL) {
      Ping6DestroyIp6Instance2 (Private);
    }

    FreePool (Private);
  }

  return ShellStatus;
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
EFI_STATUS L2_APPLICATIONS_Command_ping(UINT8 parameters[PARAMETER_COUNT][PARAMETER_LENGTH], UINT8 *pReturnCode)
{
    UINT8 j = 0;
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_APPLICATIONS_Command_ping:%a \n", __LINE__, parameters[1]);
	
	int TerminalWindowMaxLineCount = 0;
	TerminalWindowMaxLineCount = (ScreenHeight - 23) / 2;
	TerminalWindowMaxLineCount /= 16;

	//ShellCommandRunPing
	//ShellPing, SHELL PING源码实现
	 EFI_STATUS          Status;
	  SHELL_STATUS        ShellStatus;
	  EFI_IPv6_ADDRESS    DstAddress;
	  EFI_IPv6_ADDRESS    SrcAddress;
	  UINT64              BufferSize;
	  UINTN               SendNumber;
	  LIST_ENTRY          *ParamPackage;
	  CONST CHAR16        *ValueStr;
	  UINTN               NonOptionCount;

	  ZeroMem (&SrcAddress, sizeof (EFI_IPv6_ADDRESS));
	  ZeroMem (&DstAddress, sizeof (EFI_IPv6_ADDRESS));

	  //
	  // Parse the parameter of source ip address.
	  //
	  ValueStr = ShellCommandLineGetValue (ParamPackage, L"-s");
	  if (ValueStr == NULL) {
	    ValueStr = ShellCommandLineGetValue (ParamPackage, L"-_s");
	  }
	  
      //Status = NetLibStrToIp6 ("192.168.3.3", &DstAddress);
      //Status = NetLibStrToIp6 ("192.168.3.3", &DstAddress);
      Status = NetLibStrToIp42 (L"192.168.3.2", (EFI_IPv4_ADDRESS*)&SrcAddress);
      Status = NetLibStrToIp42 (L"192.168.3.4", (EFI_IPv4_ADDRESS*)&DstAddress);
      //Status = NetLibStrToIp42 (L"180.101.49.11", (EFI_IPv4_ADDRESS*)&DstAddress);
	  
	  L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_APPLICATIONS_Command_ping:%a \n", __LINE__, parameters[1]);

	  ShellStatus = ShellPing2 (
	             10,
	             16,
	             &SrcAddress,
	             &DstAddress,
	             PING_IP_CHOICE_IP42
	             );
	}



