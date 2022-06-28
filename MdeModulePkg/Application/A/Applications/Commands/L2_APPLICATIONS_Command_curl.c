
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

#include "L2_APPLICATIONS_Command_curl.h"

#include <Global/Global.h>


typedef struct _EFI_HTTP_PROTOCOL2 EFI_HTTP_PROTOCOL2;

///
/// EFI_HTTP_VERSION
///
typedef enum {
  HttpVersion10,
  HttpVersion11,
  HttpVersionUnsupported
} EFI_HTTP_VERSION2;

///
/// EFI_HTTP_METHOD
///
typedef enum {
  HttpMethodGet,
  HttpMethodPost,
  HttpMethodPatch,
  HttpMethodOptions,
  HttpMethodConnect,
  HttpMethodHead,
  HttpMethodPut,
  HttpMethodDelete,
  HttpMethodTrace,
  HttpMethodMax
} EFI_HTTP_METHOD2;

///
/// EFI_HTTP_STATUS_CODE
///
typedef enum {
  HTTP_STATUS_UNSUPPORTED_STATUS = 0,
  HTTP_STATUS_100_CONTINUE,
  HTTP_STATUS_101_SWITCHING_PROTOCOLS,
  HTTP_STATUS_200_OK,
  HTTP_STATUS_201_CREATED,
  HTTP_STATUS_202_ACCEPTED,
  HTTP_STATUS_203_NON_AUTHORITATIVE_INFORMATION,
  HTTP_STATUS_204_NO_CONTENT,
  HTTP_STATUS_205_RESET_CONTENT,
  HTTP_STATUS_206_PARTIAL_CONTENT,
  HTTP_STATUS_300_MULTIPLE_CHOICES,
  HTTP_STATUS_301_MOVED_PERMANENTLY,
  HTTP_STATUS_302_FOUND,
  HTTP_STATUS_303_SEE_OTHER,
  HTTP_STATUS_304_NOT_MODIFIED,
  HTTP_STATUS_305_USE_PROXY,
  HTTP_STATUS_307_TEMPORARY_REDIRECT,
  HTTP_STATUS_400_BAD_REQUEST,
  HTTP_STATUS_401_UNAUTHORIZED,
  HTTP_STATUS_402_PAYMENT_REQUIRED,
  HTTP_STATUS_403_FORBIDDEN,
  HTTP_STATUS_404_NOT_FOUND,
  HTTP_STATUS_405_METHOD_NOT_ALLOWED,
  HTTP_STATUS_406_NOT_ACCEPTABLE,
  HTTP_STATUS_407_PROXY_AUTHENTICATION_REQUIRED,
  HTTP_STATUS_408_REQUEST_TIME_OUT,
  HTTP_STATUS_409_CONFLICT,
  HTTP_STATUS_410_GONE,
  HTTP_STATUS_411_LENGTH_REQUIRED,
  HTTP_STATUS_412_PRECONDITION_FAILED,
  HTTP_STATUS_413_REQUEST_ENTITY_TOO_LARGE,
  HTTP_STATUS_414_REQUEST_URI_TOO_LARGE,
  HTTP_STATUS_415_UNSUPPORTED_MEDIA_TYPE,
  HTTP_STATUS_416_REQUESTED_RANGE_NOT_SATISFIED,
  HTTP_STATUS_417_EXPECTATION_FAILED,
  HTTP_STATUS_500_INTERNAL_SERVER_ERROR,
  HTTP_STATUS_501_NOT_IMPLEMENTED,
  HTTP_STATUS_502_BAD_GATEWAY,
  HTTP_STATUS_503_SERVICE_UNAVAILABLE,
  HTTP_STATUS_504_GATEWAY_TIME_OUT,
  HTTP_STATUS_505_HTTP_VERSION_NOT_SUPPORTED,
  HTTP_STATUS_308_PERMANENT_REDIRECT
} EFI_HTTP_STATUS_CODE2;

///
/// EFI_HTTPv4_ACCESS_POINT
///
typedef struct {
  ///
  /// Set to TRUE to instruct the EFI HTTP instance to use the default address
  /// information in every TCP connection made by this instance. In addition, when set
  /// to TRUE, LocalAddress and LocalSubnet are ignored.
  ///
  BOOLEAN                       UseDefaultAddress;
  ///
  /// If UseDefaultAddress is set to FALSE, this defines the local IP address to be
  /// used in every TCP connection opened by this instance.
  ///
  EFI_IPv4_ADDRESS              LocalAddress;
  ///
  /// If UseDefaultAddress is set to FALSE, this defines the local subnet to be used
  /// in every TCP connection opened by this instance.
  ///
  EFI_IPv4_ADDRESS              LocalSubnet;
  ///
  /// This defines the local port to be used in
  /// every TCP connection opened by this instance.
  ///
  UINT16                        LocalPort;
} EFI_HTTPv4_ACCESS_POINT2;

///
/// EFI_HTTPv6_ACCESS_POINT
///
typedef struct {
  ///
  /// Local IP address to be used in every TCP connection opened by this instance.
  ///
  EFI_IPv6_ADDRESS              LocalAddress;
  ///
  /// Local port to be used in every TCP connection opened by this instance.
  ///
  UINT16                        LocalPort;
} EFI_HTTPv6_ACCESS_POINT2;

///
/// EFI_HTTP_CONFIG_DATA_ACCESS_POINT
///


typedef struct {
  ///
  /// HTTP version that this instance will support.
  ///
  EFI_HTTP_VERSION2                   HttpVersion;
  ///
  /// Time out (in milliseconds) when blocking for requests.
  ///
  UINT32                             TimeOutMillisec;
  ///
  /// Defines behavior of EFI DNS and TCP protocols consumed by this instance. If
  /// FALSE, this instance will use EFI_DNS4_PROTOCOL and EFI_TCP4_PROTOCOL. If TRUE,
  /// this instance will use EFI_DNS6_PROTOCOL and EFI_TCP6_PROTOCOL.
  ///
  BOOLEAN                            LocalAddressIsIPv6;

  union {
    ///
    /// When LocalAddressIsIPv6 is FALSE, this points to the local address, subnet, and
    /// port used by the underlying TCP protocol.
    ///
    EFI_HTTPv4_ACCESS_POINT2          *IPv4Node;
    ///
    /// When LocalAddressIsIPv6 is TRUE, this points to the local IPv6 address and port
    /// used by the underlying TCP protocol.
    ///
    EFI_HTTPv6_ACCESS_POINT2          *IPv6Node;
  } AccessPoint;
} EFI_HTTP_CONFIG_DATA2;

///
/// EFI_HTTP_REQUEST_DATA
///
typedef struct {
  ///
  /// The HTTP method (e.g. GET, POST) for this HTTP Request.
  ///
  EFI_HTTP_METHOD2               Method;
  ///
  /// The URI of a remote host. From the information in this field, the HTTP instance
  /// will be able to determine whether to use HTTP or HTTPS and will also be able to
  /// determine the port number to use. If no port number is specified, port 80 (HTTP)
  /// is assumed. See RFC 3986 for more details on URI syntax.
  ///
  CHAR16                        *Url;
} EFI_HTTP_REQUEST_DATA2;

///
/// EFI_HTTP_RESPONSE_DATA
///
typedef struct {
  ///
  /// Response status code returned by the remote host.
  ///
  EFI_HTTP_STATUS_CODE2          StatusCode;
} EFI_HTTP_RESPONSE_DATA2;

///
/// EFI_HTTP_HEADER
///
typedef struct {
  ///
  /// Null terminated string which describes a field name. See RFC 2616 Section 14 for
  /// detailed information about field names.
  ///
  CHAR8                         *FieldName;
  ///
  /// Null terminated string which describes the corresponding field value. See RFC 2616
  /// Section 14 for detailed information about field values.
  ///
  CHAR8                         *FieldValue;
} EFI_HTTP_HEADER2;

///
/// EFI_HTTP_MESSAGE
///
typedef struct {
  ///
  /// HTTP message data.
  ///
  union {
    ///
    /// When the token is used to send a HTTP request, Request is a pointer to storage that
    /// contains such data as URL and HTTP method.
    ///
    EFI_HTTP_REQUEST_DATA2       *Request;
    ///
    /// When used to await a response, Response points to storage containing HTTP response
    /// status code.
    ///
    EFI_HTTP_RESPONSE_DATA2      *Response;
  } Data;
  ///
  /// Number of HTTP header structures in Headers list. On request, this count is
  /// provided by the caller. On response, this count is provided by the HTTP driver.
  ///
  UINTN                         HeaderCount;
  ///
  /// Array containing list of HTTP headers. On request, this array is populated by the
  /// caller. On response, this array is allocated and populated by the HTTP driver. It
  /// is the responsibility of the caller to free this memory on both request and
  /// response.
  ///
  EFI_HTTP_HEADER2               *Headers;
  ///
  /// Length in bytes of the HTTP body. This can be zero depending on the HttpMethod type.
  ///
  UINTN                         BodyLength;
  ///
  /// Body associated with the HTTP request or response. This can be NULL depending on
  /// the HttpMethod type.
  ///
  VOID                          *Body;
} EFI_HTTP_MESSAGE2;


///
/// EFI_HTTP_TOKEN
///
typedef struct {
  ///
  /// This Event will be signaled after the Status field is updated by the EFI HTTP
  /// Protocol driver. The type of Event must be EFI_NOTIFY_SIGNAL. The Task Priority
  /// Level (TPL) of Event must be lower than or equal to TPL_CALLBACK.
  ///
  EFI_EVENT                     Event;
  ///
  /// Status will be set to one of the following value if the HTTP request is
  /// successfully sent or if an unexpected error occurs:
  ///   EFI_SUCCESS:      The HTTP request was successfully sent to the remote host.
  ///   EFI_HTTP_ERROR:   The response message was successfully received but contains a
  ///                     HTTP error. The response status code is returned in token.
  ///   EFI_ABORTED:      The HTTP request was cancelled by the caller and removed from
  ///                     the transmit queue.
  ///   EFI_TIMEOUT:      The HTTP request timed out before reaching the remote host.
  ///   EFI_DEVICE_ERROR: An unexpected system or network error occurred.
  ///
  EFI_STATUS                    Status;
  ///
  /// Pointer to storage containing HTTP message data.
  ///
  EFI_HTTP_MESSAGE2              *Message;
} EFI_HTTP_TOKEN2;

typedef
EFI_STATUS
(EFIAPI *EFI_HTTP_GET_MODE_DATA2)(
  IN  EFI_HTTP_PROTOCOL2         *This,
  OUT EFI_HTTP_CONFIG_DATA2      *HttpConfigData
  );


typedef
EFI_STATUS
(EFIAPI *EFI_HTTP_CONFIGURE2)(
  IN  EFI_HTTP_PROTOCOL2         *This,
  IN  EFI_HTTP_CONFIG_DATA2      *HttpConfigData OPTIONAL
  );


typedef
EFI_STATUS
(EFIAPI *EFI_HTTP_REQUEST2) (
  IN  EFI_HTTP_PROTOCOL2         *This,
  IN  EFI_HTTP_TOKEN2            *Token
  );


typedef
EFI_STATUS
(EFIAPI *EFI_HTTP_CANCEL2)(
  IN  EFI_HTTP_PROTOCOL2         *This,
  IN  EFI_HTTP_TOKEN2            *Token
  );

typedef
EFI_STATUS
(EFIAPI *EFI_HTTP_RESPONSE2) (
  IN  EFI_HTTP_PROTOCOL2         *This,
  IN  EFI_HTTP_TOKEN2            *Token
  );

typedef
EFI_STATUS
(EFIAPI *EFI_HTTP_POLL2) (
  IN  EFI_HTTP_PROTOCOL2         *This
  );

struct _EFI_HTTP_PROTOCOL2 {
  EFI_HTTP_GET_MODE_DATA2        GetModeData;
  EFI_HTTP_CONFIGURE2            Configure;
  EFI_HTTP_REQUEST2              Request;
  EFI_HTTP_CANCEL2               Cancel;
  EFI_HTTP_RESPONSE2             Response;
  EFI_HTTP_POLL2                 Poll;
};



typedef struct {
  UINTN                 ContentDownloaded;
  UINTN                 ContentLength;
  UINTN                 LastReportedNbOfBytes;
  UINTN                 BufferSize;
  UINTN                 Status;
  UINTN                 Flags;
  UINT8                 *Buffer;
  CHAR16                *ServerAddrAndProto;
  CHAR16                *Uri;
  EFI_HTTP_TOKEN2        ResponseToken;
  EFI_HTTP_TOKEN2        RequestToken;
  EFI_HTTP_PROTOCOL2     *Http;
  EFI_HTTP_CONFIG_DATA2  HttpConfigData;
} HTTP_DOWNLOAD_CONTEXT2;



typedef struct _EFI_MANAGED_NETWORK_PROTOCOL EFI_MANAGED_NETWORK_PROTOCOL;

typedef struct {
  ///
  /// Timeout value for a UEFI one-shot timer event. A packet that has not been removed
  /// from the MNP receive queue will be dropped if its receive timeout expires.
  ///
  UINT32     ReceivedQueueTimeoutValue;
  ///
  /// Timeout value for a UEFI one-shot timer event. A packet that has not been removed
  /// from the MNP transmit queue will be dropped if its receive timeout expires.
  ///
  UINT32     TransmitQueueTimeoutValue;
  ///
  /// Ethernet type II 16-bit protocol type in host byte order. Valid
  /// values are zero and 1,500 to 65,535.
  ///
  UINT16     ProtocolTypeFilter;
  ///
  /// Set to TRUE to receive packets that are sent to the network
  /// device MAC address. The startup default value is FALSE.
  ///
  BOOLEAN    EnableUnicastReceive;
  ///
  /// Set to TRUE to receive packets that are sent to any of the
  /// active multicast groups. The startup default value is FALSE.
  ///
  BOOLEAN    EnableMulticastReceive;
  ///
  /// Set to TRUE to receive packets that are sent to the network
  /// device broadcast address. The startup default value is FALSE.
  ///
  BOOLEAN    EnableBroadcastReceive;
  ///
  /// Set to TRUE to receive packets that are sent to any MAC address.
  /// The startup default value is FALSE.
  ///
  BOOLEAN    EnablePromiscuousReceive;
  ///
  /// Set to TRUE to drop queued packets when the configuration
  /// is changed. The startup default value is FALSE.
  ///
  BOOLEAN    FlushQueuesOnReset;
  ///
  /// Set to TRUE to timestamp all packets when they are received
  /// by the MNP. Note that timestamps may be unsupported in some
  /// MNP implementations. The startup default value is FALSE.
  ///
  BOOLEAN    EnableReceiveTimestamps;
  ///
  /// Set to TRUE to disable background polling in this MNP
  /// instance. Note that background polling may not be supported in
  /// all MNP implementations. The startup default value is FALSE,
  /// unless background polling is not supported.
  ///
  BOOLEAN    DisableBackgroundPolling;
} EFI_MANAGED_NETWORK_CONFIG_DATA;

typedef struct {
  EFI_TIME      Timestamp;
  EFI_EVENT     RecycleEvent;
  UINT32        PacketLength;
  UINT32        HeaderLength;
  UINT32        AddressLength;
  UINT32        DataLength;
  BOOLEAN       BroadcastFlag;
  BOOLEAN       MulticastFlag;
  BOOLEAN       PromiscuousFlag;
  UINT16        ProtocolType;
  VOID          *DestinationAddress;
  VOID          *SourceAddress;
  VOID          *MediaHeader;
  VOID          *PacketData;
} EFI_MANAGED_NETWORK_RECEIVE_DATA;

typedef struct {
  UINT32        FragmentLength;
  VOID          *FragmentBuffer;
} EFI_MANAGED_NETWORK_FRAGMENT_DATA;

typedef struct {
  EFI_MAC_ADDRESS                   *DestinationAddress; //OPTIONAL
  EFI_MAC_ADDRESS                   *SourceAddress;      //OPTIONAL
  UINT16                            ProtocolType;        //OPTIONAL
  UINT32                            DataLength;
  UINT16                            HeaderLength;        //OPTIONAL
  UINT16                            FragmentCount;
  EFI_MANAGED_NETWORK_FRAGMENT_DATA FragmentTable[1];
} EFI_MANAGED_NETWORK_TRANSMIT_DATA;


typedef struct {
  ///
  /// This Event will be signaled after the Status field is updated
  /// by the MNP. The type of Event must be
  /// EFI_NOTIFY_SIGNAL. The Task Priority Level (TPL) of
  /// Event must be lower than or equal to TPL_CALLBACK.
  ///
  EFI_EVENT                             Event;
  ///
  /// The status that is returned to the caller at the end of the operation
  /// to indicate whether this operation completed successfully.
  ///
  EFI_STATUS                            Status;
  union {
    ///
    /// When this token is used for receiving, RxData is a pointer to the EFI_MANAGED_NETWORK_RECEIVE_DATA.
    ///
    EFI_MANAGED_NETWORK_RECEIVE_DATA    *RxData;
    ///
    /// When this token is used for transmitting, TxData is a pointer to the EFI_MANAGED_NETWORK_TRANSMIT_DATA.
    ///
    EFI_MANAGED_NETWORK_TRANSMIT_DATA   *TxData;
  } Packet;
} EFI_MANAGED_NETWORK_COMPLETION_TOKEN;

#define MAX_MCAST_FILTER_CNT                              16

typedef struct {
  ///
  /// Reports the current state of the network interface.
  ///
  UINT32          State;
  ///
  /// The size, in bytes, of the network interface's HW address.
  ///
  UINT32          HwAddressSize;
  ///
  /// The size, in bytes, of the network interface's media header.
  ///
  UINT32          MediaHeaderSize;
  ///
  /// The maximum size, in bytes, of the packets supported by the network interface.
  ///
  UINT32          MaxPacketSize;
  ///
  /// The size, in bytes, of the NVRAM device attached to the network interface.
  ///
  UINT32          NvRamSize;
  ///
  /// The size that must be used for all NVRAM reads and writes. The
  /// start address for NVRAM read and write operations and the total
  /// length of those operations, must be a multiple of this value. The
  /// legal values for this field are 0, 1, 2, 4, and 8.
  ///
  UINT32          NvRamAccessSize;
  ///
  /// The multicast receive filter settings supported by the network interface.
  ///
  UINT32          ReceiveFilterMask;
  ///
  /// The current multicast receive filter settings.
  ///
  UINT32          ReceiveFilterSetting;
  ///
  /// The maximum number of multicast address receive filters supported by the driver.
  ///
  UINT32          MaxMCastFilterCount;
  ///
  /// The current number of multicast address receive filters.
  ///
  UINT32          MCastFilterCount;
  ///
  /// Array containing the addresses of the current multicast address receive filters.
  ///
  EFI_MAC_ADDRESS MCastFilter[MAX_MCAST_FILTER_CNT];
  ///
  /// The current HW MAC address for the network interface.
  ///
  EFI_MAC_ADDRESS CurrentAddress;
  ///
  /// The current HW MAC address for broadcast packets.
  ///
  EFI_MAC_ADDRESS BroadcastAddress;
  ///
  /// The permanent HW MAC address for the network interface.
  ///
  EFI_MAC_ADDRESS PermanentAddress;
  ///
  /// The interface type of the network interface.
  ///
  UINT8           IfType;
  ///
  /// TRUE if the HW MAC address can be changed.
  ///
  BOOLEAN         MacAddressChangeable;
  ///
  /// TRUE if the network interface can transmit more than one packet at a time.
  ///
  BOOLEAN         MultipleTxSupported;
  ///
  /// TRUE if the presence of media can be determined; otherwise FALSE.
  ///
  BOOLEAN         MediaPresentSupported;
  ///
  /// TRUE if media are connected to the network interface; otherwise FALSE.
  ///
  BOOLEAN         MediaPresent;
} EFI_SIMPLE_NETWORK_MODE;

/**
  Returns the operational parameters for the current MNP child driver.

  @param  This          The pointer to the EFI_MANAGED_NETWORK_PROTOCOL instance.
  @param  MnpConfigData The pointer to storage for MNP operational parameters.
  @param  SnpModeData   The pointer to storage for SNP operational parameters.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_INVALID_PARAMETER This is NULL.
  @retval EFI_UNSUPPORTED       The requested feature is unsupported in this MNP implementation.
  @retval EFI_NOT_STARTED       This MNP child driver instance has not been configured. The default
                                values are returned in MnpConfigData if it is not NULL.
  @retval Other                 The mode data could not be read.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_MANAGED_NETWORK_GET_MODE_DATA)(
  IN  EFI_MANAGED_NETWORK_PROTOCOL     *This,
  OUT EFI_MANAGED_NETWORK_CONFIG_DATA  *MnpConfigData  OPTIONAL,
  OUT EFI_SIMPLE_NETWORK_MODE          *SnpModeData    OPTIONAL
  );

/**
  Sets or clears the operational parameters for the MNP child driver.

  @param  This          The pointer to the EFI_MANAGED_NETWORK_PROTOCOL instance.
  @param  MnpConfigData The pointer to configuration data that will be assigned to the MNP
                        child driver instance. If NULL, the MNP child driver instance is
                        reset to startup defaults and all pending transmit and receive
                        requests are flushed.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_INVALID_PARAMETER One or more parameters are invalid.
  @retval EFI_OUT_OF_RESOURCES  Required system resources (usually memory) could not be
                                allocated.
  @retval EFI_UNSUPPORTED       The requested feature is unsupported in this [MNP]
                                implementation.
  @retval EFI_DEVICE_ERROR      An unexpected network or system error occurred.
  @retval Other                 The MNP child driver instance has been reset to startup defaults.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_MANAGED_NETWORK_CONFIGURE)(
  IN EFI_MANAGED_NETWORK_PROTOCOL     *This,
  IN EFI_MANAGED_NETWORK_CONFIG_DATA  *MnpConfigData  OPTIONAL
  );

/**
  Translates an IP multicast address to a hardware (MAC) multicast address.

  @param  This       The pointer to the EFI_MANAGED_NETWORK_PROTOCOL instance.
  @param  Ipv6Flag   Set to TRUE to if IpAddress is an IPv6 multicast address.
                     Set to FALSE if IpAddress is an IPv4 multicast address.
  @param  IpAddress  The pointer to the multicast IP address (in network byte order) to convert.
  @param  MacAddress The pointer to the resulting multicast MAC address.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_INVALID_PARAMETER One of the following conditions is TRUE:
                                - This is NULL.
                                - IpAddress is NULL.
                                - *IpAddress is not a valid multicast IP address.
                                - MacAddress is NULL.
  @retval EFI_NOT_STARTED       This MNP child driver instance has not been configured.
  @retval EFI_UNSUPPORTED       The requested feature is unsupported in this MNP implementation.
  @retval EFI_DEVICE_ERROR      An unexpected network or system error occurred.
  @retval Other                 The address could not be converted.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_MANAGED_NETWORK_MCAST_IP_TO_MAC)(
  IN  EFI_MANAGED_NETWORK_PROTOCOL  *This,
  IN  BOOLEAN                       Ipv6Flag,
  IN  EFI_IP_ADDRESS                *IpAddress,
  OUT EFI_MAC_ADDRESS               *MacAddress
  );

/**
  Enables and disables receive filters for multicast address.

  @param  This       The pointer to the EFI_MANAGED_NETWORK_PROTOCOL instance.
  @param  JoinFlag   Set to TRUE to join this multicast group.
                     Set to FALSE to leave this multicast group.
  @param  MacAddress The pointer to the multicast MAC group (address) to join or leave.

  @retval EFI_SUCCESS           The requested operation completed successfully.
  @retval EFI_INVALID_PARAMETER One or more of the following conditions is TRUE:
                                - This is NULL.
                                - JoinFlag is TRUE and MacAddress is NULL.
                                - *MacAddress is not a valid multicast MAC address.
  @retval EFI_NOT_STARTED       This MNP child driver instance has not been configured.
  @retval EFI_ALREADY_STARTED   The supplied multicast group is already joined.
  @retval EFI_NOT_FOUND         The supplied multicast group is not joined.
  @retval EFI_DEVICE_ERROR      An unexpected network or system error occurred.
  @retval EFI_UNSUPPORTED       The requested feature is unsupported in this MNP implementation.
  @retval Other                 The requested operation could not be completed.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_MANAGED_NETWORK_GROUPS)(
  IN EFI_MANAGED_NETWORK_PROTOCOL  *This,
  IN BOOLEAN                       JoinFlag,
  IN EFI_MAC_ADDRESS               *MacAddress  OPTIONAL
  );

/**
  Places asynchronous outgoing data packets into the transmit queue.

  @param  This  The pointer to the EFI_MANAGED_NETWORK_PROTOCOL instance.
  @param  Token The pointer to a token associated with the transmit data descriptor.

  @retval EFI_SUCCESS           The transmit completion token was cached.
  @retval EFI_NOT_STARTED       This MNP child driver instance has not been configured.
  @retval EFI_INVALID_PARAMETER One or more parameters are invalid.
  @retval EFI_ACCESS_DENIED     The transmit completion token is already in the transmit queue.
  @retval EFI_OUT_OF_RESOURCES  The transmit data could not be queued due to a lack of system resources
                                (usually memory).
  @retval EFI_DEVICE_ERROR      An unexpected system or network error occurred.
  @retval EFI_NOT_READY         The transmit request could not be queued because the transmit queue is full.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_MANAGED_NETWORK_TRANSMIT)(
  IN EFI_MANAGED_NETWORK_PROTOCOL          *This,
  IN EFI_MANAGED_NETWORK_COMPLETION_TOKEN  *Token
  );

/**
  Places an asynchronous receiving request into the receiving queue.

  @param  This  The pointer to the EFI_MANAGED_NETWORK_PROTOCOL instance.
  @param  Token The pointer to a token associated with the receive data descriptor.

  @retval EFI_SUCCESS           The receive completion token was cached.
  @retval EFI_NOT_STARTED       This MNP child driver instance has not been configured.
  @retval EFI_INVALID_PARAMETER One or more of the following conditions is TRUE:
                                - This is NULL.
                                - Token is NULL.
                                - Token.Event is NULL.
  @retval EFI_OUT_OF_RESOURCES  The transmit data could not be queued due to a lack of system resources
                                (usually memory).
  @retval EFI_DEVICE_ERROR      An unexpected system or network error occurred.
  @retval EFI_ACCESS_DENIED     The receive completion token was already in the receive queue.
  @retval EFI_NOT_READY         The receive request could not be queued because the receive queue is full.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_MANAGED_NETWORK_RECEIVE)(
  IN EFI_MANAGED_NETWORK_PROTOCOL          *This,
  IN EFI_MANAGED_NETWORK_COMPLETION_TOKEN  *Token
  );


/**
  Aborts an asynchronous transmit or receive request.

  @param  This  The pointer to the EFI_MANAGED_NETWORK_PROTOCOL instance.
  @param  Token The pointer to a token that has been issued by
                EFI_MANAGED_NETWORK_PROTOCOL.Transmit() or
                EFI_MANAGED_NETWORK_PROTOCOL.Receive(). If
                NULL, all pending tokens are aborted.

  @retval  EFI_SUCCESS           The asynchronous I/O request was aborted and Token.Event
                                 was signaled. When Token is NULL, all pending requests were
                                 aborted and their events were signaled.
  @retval  EFI_NOT_STARTED       This MNP child driver instance has not been configured.
  @retval  EFI_INVALID_PARAMETER This is NULL.
  @retval  EFI_NOT_FOUND         When Token is not NULL, the asynchronous I/O request was
                                 not found in the transmit or receive queue. It has either completed
                                 or was not issued by Transmit() and Receive().

**/
typedef
EFI_STATUS
(EFIAPI *EFI_MANAGED_NETWORK_CANCEL)(
  IN EFI_MANAGED_NETWORK_PROTOCOL          *This,
  IN EFI_MANAGED_NETWORK_COMPLETION_TOKEN  *Token  OPTIONAL
  );

/**
  Polls for incoming data packets and processes outgoing data packets.

  @param  This The pointer to the EFI_MANAGED_NETWORK_PROTOCOL instance.

  @retval EFI_SUCCESS      Incoming or outgoing data was processed.
  @retval EFI_NOT_STARTED  This MNP child driver instance has not been configured.
  @retval EFI_DEVICE_ERROR An unexpected system or network error occurred.
  @retval EFI_NOT_READY    No incoming or outgoing data was processed. Consider increasing
                           the polling rate.
  @retval EFI_TIMEOUT      Data was dropped out of the transmit and/or receive queue.
                            Consider increasing the polling rate.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_MANAGED_NETWORK_POLL)(
  IN EFI_MANAGED_NETWORK_PROTOCOL    *This
  );

///
/// The MNP is used by network applications (and drivers) to
/// perform raw (unformatted) asynchronous network packet I/O.
///
struct _EFI_MANAGED_NETWORK_PROTOCOL {
  EFI_MANAGED_NETWORK_GET_MODE_DATA       GetModeData;
  EFI_MANAGED_NETWORK_CONFIGURE           Configure;
  EFI_MANAGED_NETWORK_MCAST_IP_TO_MAC     McastIpToMac;
  EFI_MANAGED_NETWORK_GROUPS              Groups;
  EFI_MANAGED_NETWORK_TRANSMIT            Transmit;
  EFI_MANAGED_NETWORK_RECEIVE             Receive;
  EFI_MANAGED_NETWORK_CANCEL              Cancel;
  EFI_MANAGED_NETWORK_POLL                Poll;
};


#define IP4_CONFIG2_INTERFACE_INFO_NAME_LENGTH 32

//
// Constant strings and definitions related to the message
// indicating the amount of progress in the dowloading of a HTTP file.
//

//
// Number of steps in the progression slider.
//
#define HTTP_PROGRESS_SLIDER_STEPS  \
  ((sizeof (HTTP_PROGR_FRAME) / sizeof (CHAR16)) - 3)

//
// Size in number of characters plus one (final zero) of the message to
// indicate the progress of an HTTP download. The format is "[(progress slider:
// 40 characters)] (nb of KBytes downloaded so far: 7 characters) Kb". There
// are thus the number of characters in HTTP_PROGR_FRAME[] plus 11 characters
// (2 // spaces, "Kb" and seven characters for the number of KBytes).
//
#define HTTP_PROGRESS_MESSAGE_SIZE  \
  ((sizeof (HTTP_PROGR_FRAME) / sizeof (CHAR16)) + 12)
  
extern EFI_GUID gEfiHttpServiceBindingProtocolGuid;
extern EFI_GUID gEfiHttpProtocolGuid;

//
// Buffer size. Note that larger buffer does not mean better speed.
//
#define DEFAULT_BUF_SIZE      SIZE_32KB
#define MAX_BUF_SIZE          SIZE_4MB

#define MIN_PARAM_COUNT       2
#define MAX_PARAM_COUNT       4
#define NEED_REDIRECTION(Code) \
  (((Code >= HTTP_STATUS_300_MULTIPLE_CHOICES) \
  && (Code <= HTTP_STATUS_307_TEMPORARY_REDIRECT)) \
  || (Code == HTTP_STATUS_308_PERMANENT_REDIRECT))

#define CLOSE_HTTP_HANDLE(ControllerHandle,HttpChildHandle) \
  do { \
    if (HttpChildHandle) { \
      CloseProtocolAndDestroyServiceChild2 ( \
        ControllerHandle, \
        &gEfiHttpServiceBindingProtocolGuid, \
        &gEfiHttpProtocolGuid, \
        HttpChildHandle \
        ); \
      HttpChildHandle = NULL; \
    } \
  } while (0)

typedef enum {
  HdrHost,
  HdrConn,
  HdrAgent,
  HdrMax
} HDR_TYPE;

#define USER_AGENT_HDR  "Mozilla/5.0 (EDK2; Linux) Gecko/20100101 Firefox/79.0"

#define TIMER_MAX_TIMEOUT_S   10

//
// File name to use when Uri ends with "/".
//
#define DEFAULT_HTML_FILE     L"index.html"
#define DEFAULT_HTTP_PROTO    L"http"

//
// String to delete the HTTP progress message to be able to update it :
// (HTTP_PROGRESS_MESSAGE_SIZE-1) '\b'.
//
#define HTTP_PROGRESS_DEL \
  L"\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\
\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"

#define HTTP_KB              L"\b\b\b\b\b\b\b\b\b\b"
//
// Frame for the progression slider.
//
#define HTTP_PROGR_FRAME     L"[                                        ]"

//
// Improve readability by using these macros.
//
#define PRINT_HII(token,...) \
  ShellPrintHiiEx (\
      -1, -1, NULL, token, mHttpHiiHandle, __VA_ARGS__)

#define PRINT_HII_APP(token,value) \
  //PRINT_HII (token, HTTP_APP_NAME, value)

//
// TimeBaseLib.h constants.
// These will be removed once the library gets fixed.
//

//
// Define EPOCH (1970-JANUARY-01) in the Julian Date representation.
//
#define EPOCH_JULIAN_DATE                               2440588

//
// Seconds per unit.
//
#define SEC_PER_MIN                                     ((UINTN)    60)
#define SEC_PER_HOUR                                    ((UINTN)  3600)
#define SEC_PER_DAY                                     ((UINTN) 86400)
#define  NET_IFTYPE_ETHERNET   0x01

//
// String descriptions for server errors.
//
STATIC CONST CHAR16 *ErrStatusDesc[] =
{
  L"400 Bad Request",
  L"401 Unauthorized",
  L"402 Payment required",
  L"403 Forbidden",
  L"404 Not Found",
  L"405 Method not allowed",
  L"406 Not acceptable",
  L"407 Proxy authentication required",
  L"408 Request time out",
  L"409 Conflict",
  L"410 Gone",
  L"411 Length required",
  L"412 Precondition failed",
  L"413 Request entity too large",
  L"414 Request URI to large",
  L"415 Unsupported media type",
  L"416 Requested range not satisfied",
  L"417 Expectation failed",
  L"500 Internal server error",
  L"501 Not implemented",
  L"502 Bad gateway",
  L"503 Service unavailable",
  L"504 Gateway timeout",
  L"505 HTTP version not supported"
};

typedef enum {
  TypeFlag  = 0,    ///< A flag that is present or not present only (IE "-a").
  TypeValue,        ///< A flag that has some data following it with a space (IE "-a 1").
  TypePosition,     ///< Some data that did not follow a parameter (IE "filename.txt").
  TypeStart,        ///< A flag that has variable value appended to the end (IE "-ad", "-afd", "-adf", etc...).
  TypeDoubleValue,  ///< A flag that has 2 space seperated value data following it (IE "-a 1 2").
  TypeMaxValue,     ///< A flag followed by all the command line data before the next flag.
  TypeTimeValue,    ///< A flag that has a time value following it (IE "-a -5:00").
  TypeMax,
} SHELL_PARAM_TYPE2;

typedef struct {
  CHAR16             *Name;
  SHELL_PARAM_TYPE2   Type;
} SHELL_PARAM_ITEM2;

STATIC CONST SHELL_PARAM_ITEM2 ParamList[] = {
  {L"-i", TypeValue},
  {L"-k", TypeFlag},
  {L"-l", TypeValue},
  {L"-m", TypeFlag},
  {L"-s", TypeValue},
  {L"-t", TypeValue},
  {NULL , TypeMax}
};

//
// Local File Handle.
//
STATIC SHELL_FILE_HANDLE   mFileHandle = NULL;

//
// Path of the local file, Unicode encoded.
//
STATIC CONST CHAR16        *mLocalFilePath;

STATIC BOOLEAN             gRequestCallbackComplete = FALSE;
STATIC BOOLEAN             gResponseCallbackComplete = FALSE;

STATIC BOOLEAN             gHttpError;

EFI_HII_HANDLE             mHttpHiiHandle;

//
// Functions declarations.
//

/**
  Check and convert the UINT16 option values of the 'http' command.

  @param[in]  ValueStr  Value as an Unicode encoded string.
  @param[out] Value     UINT16 value.

  @retval     TRUE      The value was returned.
  @retval     FALSE     A parsing error occured.
**/
STATIC
BOOLEAN
StringToUint162 (
  IN   CONST CHAR16  *ValueStr,
  OUT  UINT16        *Value
  );

/**
  Get the name of the NIC.

  @param[in]   ControllerHandle  The network physical device handle.
  @param[in]   NicNumber         The network physical device number.
  @param[out]  NicName           Address where to store the NIC name.
                                 The memory area has to be at least
                                 IP4_CONFIG2_INTERFACE_INFO_NAME_LENGTH
                                 double byte wide.

  @retval  EFI_SUCCESS  The name of the NIC was returned.
  @retval  Others       The creation of the child for the Managed
                        Network Service failed or the opening of
                        the Managed Network Protocol failed or
                        the operational parameters for the
                        Managed Network Protocol could not be
                        read.
**/
STATIC
EFI_STATUS
GetNicName2 (
  IN   EFI_HANDLE  ControllerHandle,
  IN   UINTN       NicNumber,
  OUT  CHAR16      *NicName
  );

/**
  Create a child for the service identified by its service binding protocol GUID
  and get from the child the interface of the protocol identified by its GUID.

  @param[in]   ControllerHandle            Controller handle.
  @param[in]   ServiceBindingProtocolGuid  Service binding protocol GUID of the
                                           service to be created.
  @param[in]   ProtocolGuid                GUID of the protocol to be open.
  @param[out]  ChildHandle                 Address where the handler of the
                                           created child is returned. NULL is
                                           returned in case of error.
  @param[out]  Interface                   Address where a pointer to the
                                           protocol interface is returned in
                                           case of success.

  @retval  EFI_SUCCESS  The child was created and the protocol opened.
  @retval  Others       Either the creation of the child or the opening
                        of the protocol failed.
**/
STATIC
EFI_STATUS
CreateServiceChildAndOpenProtocol2 (
  IN   EFI_HANDLE  ControllerHandle,
  IN   EFI_GUID    *ServiceBindingProtocolGuid,
  IN   EFI_GUID    *ProtocolGuid,
  OUT  EFI_HANDLE  *ChildHandle,
  OUT  VOID        **Interface
  );

/**
  Close the protocol identified by its GUID on the child handle of the service
  identified by its service binding protocol GUID, then destroy the child
  handle.

  @param[in]  ControllerHandle            Controller handle.
  @param[in]  ServiceBindingProtocolGuid  Service binding protocol GUID of the
                                          service to be destroyed.
  @param[in]  ProtocolGuid                GUID of the protocol to be closed.
  @param[in]  ChildHandle                 Handle of the child to be destroyed.

**/
STATIC
VOID
CloseProtocolAndDestroyServiceChild2 (
  IN  EFI_HANDLE  ControllerHandle,
  IN  EFI_GUID    *ServiceBindingProtocolGuid,
  IN  EFI_GUID    *ProtocolGuid,
  IN  EFI_HANDLE  ChildHandle
  );

/**
  Worker function that download the data of a file from an HTTP server given
  the path of the file and its size.

  @param[in]   Context           A pointer to the download context.

  @retval  EFI_SUCCESS           The file was downloaded.
  @retval  EFI_OUT_OF_RESOURCES  A memory allocation failed.
  @retval  Others                The downloading of the file
                                 from the server failed.
**/
STATIC
EFI_STATUS
DownloadFile2(
  IN   HTTP_DOWNLOAD_CONTEXT2   *Context,
  IN   EFI_HANDLE              ControllerHandle,
  IN   CHAR16                  *NicName
  );

/**
  Cleans off leading and trailing spaces and tabs.

  @param[in]                      String pointer to the string to trim them off.

  @retval EFI_SUCCESS             No errors.
  @retval EFI_INVALID_PARAMETER   String pointer is NULL.
**/
STATIC
EFI_STATUS
TrimSpaces2 (
  IN CHAR16 *String
  )
{
  CHAR16 *Str;
  UINTN  Len;

  ////ASSERT (String != NULL);

  if (String == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Str = String;

  //
  // Remove any whitespace at the beginning of the Str.
  //
  while (*Str == L' ' || *Str == L'\t') {
    Str++;
  }

  //
  // Remove any whitespace at the end of the Str.
  //
  do {
    Len = StrLen (Str);
    if (!Len || (Str[Len - 1] != L' ' && Str[Len - 1] != '\t')) {
      break;
    }

    Str[Len - 1] = CHAR_NULL;
  } while (Len);

  CopyMem (String, Str, StrSize (Str));

  return EFI_SUCCESS;
}

//
// Callbacks for request and response.
// We just acknowledge that operation has completed here.
//

/**
  Callback to set the request completion flag.

  @param[in] Event:   The event.
  @param[in] Context: pointer to Notification Context.
 **/
STATIC
VOID
EFIAPI
RequestCallback2 (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  gRequestCallbackComplete = TRUE;
}

/**
  Callback to set the response completion flag.
  @param[in] Event:   The event.
  @param[in] Context: pointer to Notification Context.
 **/
STATIC
VOID
EFIAPI
ResponseCallback2 (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  gResponseCallbackComplete = TRUE;
}

//
// Set of functions from TimeBaseLib.
// This will be removed once TimeBaseLib is enabled for ShellPkg.
//

/**
  Calculate Epoch days.

  @param[in] Time - a pointer to the EFI_TIME abstraction.

  @retval           Number of days elapsed since EPOCH_JULIAN_DAY.
 **/
STATIC
UINTN
EfiGetEpochDays2 (
  IN  EFI_TIME  *Time
  )
{
  UINTN a;
  UINTN y;
  UINTN m;
  //
  // Absolute Julian Date representation of the supplied Time.
  //
  UINTN JulianDate;
  //
  // Number of days elapsed since EPOCH_JULIAN_DAY.
  //
  UINTN EpochDays;

  a = (14 - Time->Month) / 12 ;
  y = Time->Year + 4800 - a;
  m = Time->Month + (12 * a) - 3;

  JulianDate = Time->Day + ((153 * m + 2) / 5) + (365 * y) + (y / 4) -
               (y / 100) + (y / 400) - 32045;

  //ASSERT (JulianDate >= EPOCH_JULIAN_DATE);
  EpochDays = JulianDate - EPOCH_JULIAN_DATE;

  return EpochDays;
}

/**
  Converts EFI_TIME to Epoch seconds
  (elapsed since 1970 JANUARY 01, 00:00:00 UTC).

  @param[in] Time: a pointer to EFI_TIME abstraction.
 **/
STATIC
UINTN
EFIAPI
EfiTimeToEpoch2 (
  IN  EFI_TIME  *Time
  )
{
  //
  // Number of days elapsed since EPOCH_JULIAN_DAY.
  //
  UINTN EpochDays;
  UINTN EpochSeconds;

  EpochDays = EfiGetEpochDays2 (Time);

  EpochSeconds = (EpochDays * SEC_PER_DAY) +
                 ((UINTN)Time->Hour * SEC_PER_HOUR) +
                 (Time->Minute * SEC_PER_MIN) + Time->Second;

  return EpochSeconds;
}

//
// Download Flags.
//
#define DL_FLAG_TIME     BIT0 // Show elapsed time.
#define DL_FLAG_KEEP_BAD BIT1 // Keep files even if download failed.

/**
  Function for 'http' command.

  @param[in] ImageHandle  Handle to the Image (NULL if Internal).
  @param[in] SystemTable  Pointer to the System Table (NULL if Internal).

  @retval  SHELL_SUCCESS            The 'http' command completed successfully.
  @retval  SHELL_ABORTED            The Shell Library initialization failed.
  @retval  SHELL_INVALID_PARAMETER  At least one of the command's arguments is
                                    not valid.
  @retval  SHELL_OUT_OF_RESOURCES   A memory allocation failed.
  @retval  SHELL_NOT_FOUND          Network Interface Card not found.
  @retval  SHELL_UNSUPPORTED        Command was valid, but the server returned
                                    a status code indicating some error.
                                    Examine the file requested for error body.
**/
SHELL_STATUS
RunHttp2 (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS              Status;
  LIST_ENTRY              *CheckPackage;
  UINTN                   ParamCount;
  UINTN                   HandleCount;
  UINTN                   NicNumber;
  UINTN                   InitialSize;
  UINTN                   ParamOffset;
  UINTN                   StartSize;
  CHAR16                  *ProblemParam;
  CHAR16                  NicName[IP4_CONFIG2_INTERFACE_INFO_NAME_LENGTH];
  CHAR16                  *Walker1;
  CHAR16                  *VStr;
  CONST CHAR16            *UserNicName;
  CONST CHAR16            *ValueStr;
  CONST CHAR16            *RemoteFilePath;
  CONST CHAR16            *Walker;
  EFI_HTTPv4_ACCESS_POINT2 IPv4Node;
  EFI_HANDLE              *Handles;
  EFI_HANDLE              ControllerHandle;
  HTTP_DOWNLOAD_CONTEXT2   Context;
  BOOLEAN                 NicFound;

  ProblemParam        = NULL;
  RemoteFilePath      = NULL;
  NicFound            = FALSE;
  Handles             = NULL;

  //
  // Initialize the Shell library (we must be in non-auto-init...).
  //
  ParamOffset = 0;
  gHttpError  = FALSE;

  Status = ShellInitialize ();
  if (EFI_ERROR (Status)) {
    ////ASSERT_EFI_ERROR (Status);
    return SHELL_ABORTED;
  }

  ZeroMem (&Context, sizeof (Context));

  //
  // Parse the command line.
  //
  Status = ShellCommandLineParse (
             ParamList,
             &CheckPackage,
             &ProblemParam,
             TRUE
             );
  if (EFI_ERROR (Status)) {
    if ((Status == EFI_VOLUME_CORRUPTED)
     && (ProblemParam != NULL))
    {
      //PRINT_HII_APP (STRING_TOKEN (STR_GEN_PROBLEM), ProblemParam);
      ////SHELL_FREE_NON_NULL (ProblemParam);
    } else {
      //ASSERT (FALSE);
    }

    goto Error;
  }

  //
  // Check the number of parameters.
  //
  Status = EFI_INVALID_PARAMETER;

  ParamCount = ShellCommandLineGetCount (CheckPackage);
  if (ParamCount > MAX_PARAM_COUNT) {
    //PRINT_HII_APP (STRING_TOKEN (STR_GEN_TOO_MANY), NULL);
    goto Error;
  }

  if (ParamCount < MIN_PARAM_COUNT) {
    //PRINT_HII_APP (STRING_TOKEN (STR_GEN_TOO_FEW), NULL);
    goto Error;
  }

  ZeroMem (&Context.HttpConfigData, sizeof (Context.HttpConfigData));
  ZeroMem (&IPv4Node, sizeof (IPv4Node));
  IPv4Node.UseDefaultAddress = TRUE;

  Context.HttpConfigData.HttpVersion = HttpVersion11;
  Context.HttpConfigData.AccessPoint.IPv4Node = &IPv4Node;

  //
  // Get the host address (not necessarily IPv4 format).
  //
  ValueStr = ShellCommandLineGetRawValue (CheckPackage, 1);
  if (!ValueStr) {
    //PRINT_HII_APP (STRING_TOKEN (STR_GEN_PARAM_INV), ValueStr);
    goto Error;
  } else {
    StartSize = 0;
    TrimSpaces2 ((CHAR16 *)ValueStr);
    if (!StrStr (ValueStr, L"://")) {
      Context.ServerAddrAndProto = StrnCatGrow (
                                     &Context.ServerAddrAndProto,
                                     &StartSize,
                                     DEFAULT_HTTP_PROTO,
                                     StrLen (DEFAULT_HTTP_PROTO)
                                     );
      Context.ServerAddrAndProto = StrnCatGrow (
                                     &Context.ServerAddrAndProto,
                                     &StartSize,
                                     L"://",
                                     StrLen (L"://")
                                     );
      VStr = (CHAR16 *)ValueStr;
    } else {
      VStr = StrStr (ValueStr, L"://") + StrLen (L"://");
    }

    for (Walker1 = VStr; *Walker1; Walker1++) {
      if (*Walker1 == L'/') {
        break;
      }
    }

    if (*Walker1 == L'/') {
      ParamOffset = 1;
      RemoteFilePath = Walker1;
    }

    Context.ServerAddrAndProto = StrnCatGrow (
                                   &Context.ServerAddrAndProto,
                                   &StartSize,
                                   ValueStr,
                                   StrLen (ValueStr) - StrLen (Walker1)
                                   );
    if (!Context.ServerAddrAndProto) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Error;
    }
  }

  if (!RemoteFilePath) {
    RemoteFilePath = ShellCommandLineGetRawValue (CheckPackage, 2);
    if (!RemoteFilePath) {
      //
      // If no path given, assume just "/".
      //
      RemoteFilePath = L"/";
    }
  }

  TrimSpaces2 ((CHAR16 *)RemoteFilePath);

  if (ParamCount == MAX_PARAM_COUNT - ParamOffset) {
    mLocalFilePath = ShellCommandLineGetRawValue (
                       CheckPackage,
                       MAX_PARAM_COUNT - 1 - ParamOffset
                       );
  } else {
    Walker = RemoteFilePath + StrLen (RemoteFilePath);
    while ((--Walker) >= RemoteFilePath) {
      if ((*Walker == L'\\') ||
          (*Walker == L'/' )    ) {
        break;
      }
    }

    mLocalFilePath = Walker + 1;
  }

  if (!StrLen (mLocalFilePath)) {
    mLocalFilePath = DEFAULT_HTML_FILE;
  }

  InitialSize = 0;
  Context.Uri = StrnCatGrow (
                  &Context.Uri,
                  &InitialSize,
                  RemoteFilePath,
                  StrLen (RemoteFilePath)
                  );
  if (!Context.Uri) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Error;
  }

  //
  // Get the name of the Network Interface Card to be used if any.
  //
  UserNicName = ShellCommandLineGetValue (CheckPackage, L"-i");

  ValueStr = ShellCommandLineGetValue (CheckPackage, L"-l");
  if ((ValueStr != NULL)
   && (!StringToUint162 (
          ValueStr,
          &Context.HttpConfigData.AccessPoint.IPv4Node->LocalPort
          )
      ))
  {
    goto Error;
  }

  Context.BufferSize = DEFAULT_BUF_SIZE;

  ValueStr = ShellCommandLineGetValue (CheckPackage, L"-s");
  if (ValueStr != NULL) {
    Context.BufferSize = ShellStrToUintn (ValueStr);
    if (!Context.BufferSize || Context.BufferSize > MAX_BUF_SIZE) {
      //PRINT_HII_APP (STRING_TOKEN (STR_GEN_PARAM_INV), ValueStr);
      goto Error;
    }
  }

  ValueStr = ShellCommandLineGetValue (CheckPackage, L"-t");
  if (ValueStr != NULL) {
    Context.HttpConfigData.TimeOutMillisec = (UINT32)ShellStrToUintn (ValueStr);
  }

  //
  // Locate all HTTP Service Binding protocols.
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiManagedNetworkServiceBindingProtocolGuid,
                  NULL,
                  &HandleCount,
                  &Handles
                  );
  if (EFI_ERROR (Status) || (HandleCount == 0)) {
    //PRINT_HII (STRING_TOKEN (STR_HTTP_ERR_NO_NIC), NULL);
    if (!EFI_ERROR (Status)) {
      Status = EFI_NOT_FOUND;
    }

    goto Error;
  }

  Status = EFI_NOT_FOUND;

  Context.Flags = 0;
  if (ShellCommandLineGetFlag (CheckPackage, L"-m")) {
    Context.Flags |= DL_FLAG_TIME;
  }

  if (ShellCommandLineGetFlag (CheckPackage, L"-k")) {
    Context.Flags |= DL_FLAG_KEEP_BAD;
  }

  for (NicNumber = 0;
       (NicNumber < HandleCount) && (Status != EFI_SUCCESS);
       NicNumber++)
  {
    ControllerHandle = Handles[NicNumber];

    Status = GetNicName2 (ControllerHandle, NicNumber, NicName);
    if (EFI_ERROR (Status)) {
      //PRINT_HII (STRING_TOKEN (STR_HTTP_ERR_NIC_NAME), NicNumber, Status);
      continue;
    }

    if (UserNicName != NULL) {
      if (StrCmp (NicName, UserNicName) != 0) {
        Status = EFI_NOT_FOUND;
        continue;
      }

      NicFound = TRUE;
    }

    Status = DownloadFile2 (&Context, ControllerHandle, NicName);
    //PRINT_HII (STRING_TOKEN (STR_GEN_CRLF), NULL);

    if (EFI_ERROR (Status)) {
      /*PRINT_HII (
        STRING_TOKEN (STR_HTTP_ERR_DOWNLOAD),
        RemoteFilePath,
        NicName,
        Status
        );*/
      //
      // If a user aborted the operation,
      // do not try another controller.
      //
      if (Status == EFI_ABORTED) {
        goto Error;
      }
    }

    if (gHttpError) {
     //
     // This is not related to connection, so no need to repeat with
     // another interface.
     //
      break;
    }
  }

  if ((UserNicName != NULL) && (!NicFound)) {
    //PRINT_HII (STRING_TOKEN (STR_HTTP_ERR_NIC_NOT_FOUND), UserNicName);
  }

Error:
  ShellCommandLineFreeVarList (CheckPackage);
  //SHELL_FREE_NON_NULL (Handles);
  //SHELL_FREE_NON_NULL (Context.ServerAddrAndProto);
  //SHELL_FREE_NON_NULL (Context.Uri);

  return Status & ~MAX_BIT;
}

/**
  Check and convert the UINT16 option values of the 'http' command

  @param[in]  ValueStr  Value as an Unicode encoded string
  @param[out] Value     UINT16 value

  @retval     TRUE      The value was returned.
  @retval     FALSE     A parsing error occured.
**/
STATIC
BOOLEAN
StringToUint162 (
  IN   CONST CHAR16  *ValueStr,
  OUT  UINT16        *Value
  )
{
  UINTN  Val;

  Val = ShellStrToUintn (ValueStr);
  if (Val > MAX_UINT16) {
    //PRINT_HII_APP (STRING_TOKEN (STR_GEN_PARAM_INV), ValueStr);
    return FALSE;
  }

  *Value = (UINT16)Val;
  return TRUE;
}

/**
  Get the name of the NIC.

  @param[in]   ControllerHandle  The network physical device handle.
  @param[in]   NicNumber         The network physical device number.
  @param[out]  NicName           Address where to store the NIC name.
                                 The memory area has to be at least
                                 IP4_CONFIG2_INTERFACE_INFO_NAME_LENGTH
                                 double byte wide.

  @retval  EFI_SUCCESS  The name of the NIC was returned.
  @retval  Others       The creation of the child for the Managed
                        Network Service failed or the opening of
                        the Managed Network Protocol failed or
                        the operational parameters for the
                        Managed Network Protocol could not be
                        read.
**/
STATIC
EFI_STATUS
GetNicName2 (
  IN   EFI_HANDLE  ControllerHandle,
  IN   UINTN       NicNumber,
  OUT  CHAR16      *NicName
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    MnpHandle;
  EFI_MANAGED_NETWORK_PROTOCOL  *Mnp;
  EFI_SIMPLE_NETWORK_MODE       SnpMode;

  Status = CreateServiceChildAndOpenProtocol2 (
             ControllerHandle,
             &gEfiManagedNetworkServiceBindingProtocolGuid,
             &gEfiManagedNetworkProtocolGuid,
             &MnpHandle,
             (VOID**)&Mnp
             );
  if (EFI_ERROR (Status)) {
    goto Error;
  }

  Status = Mnp->GetModeData (Mnp, NULL, &SnpMode);
  if (EFI_ERROR (Status) && (Status != EFI_NOT_STARTED)) {
    goto Error;
  }

  UnicodeSPrint (
    NicName,
    IP4_CONFIG2_INTERFACE_INFO_NAME_LENGTH,
    SnpMode.IfType == NET_IFTYPE_ETHERNET ? L"eth%d" : L"unk%d",
    NicNumber
    );

  Status = EFI_SUCCESS;

Error:

  if (MnpHandle != NULL) {
    CloseProtocolAndDestroyServiceChild2 (
      ControllerHandle,
      &gEfiManagedNetworkServiceBindingProtocolGuid,
      &gEfiManagedNetworkProtocolGuid,
      MnpHandle
      );
  }

  return Status;
}

/**
  Create a child for the service identified by its service binding protocol GUID
  and get from the child the interface of the protocol identified by its GUID.

  @param[in]   ControllerHandle            Controller handle.
  @param[in]   ServiceBindingProtocolGuid  Service binding protocol GUID of the
                                           service to be created.
  @param[in]   ProtocolGuid                GUID of the protocol to be open.
  @param[out]  ChildHandle                 Address where the handler of the
                                           created child is returned. NULL is
                                           returned in case of error.
  @param[out]  Interface                   Address where a pointer to the
                                           protocol interface is returned in
                                           case of success.

  @retval  EFI_SUCCESS  The child was created and the protocol opened.
  @retval  Others       Either the creation of the child or the opening
                        of the protocol failed.
**/
STATIC
EFI_STATUS
CreateServiceChildAndOpenProtocol2 (
  IN   EFI_HANDLE  ControllerHandle,
  IN   EFI_GUID    *ServiceBindingProtocolGuid,
  IN   EFI_GUID    *ProtocolGuid,
  OUT  EFI_HANDLE  *ChildHandle,
  OUT  VOID        **Interface
  )
{
  EFI_STATUS  Status;

  *ChildHandle = NULL;
  Status = NetLibCreateServiceChild (
             ControllerHandle,
             gImageHandle,
             ServiceBindingProtocolGuid,
             ChildHandle
             );
  if (!EFI_ERROR (Status)) {
    Status = gBS->OpenProtocol (
                    *ChildHandle,
                    ProtocolGuid,
                    Interface,
                    gImageHandle,
                    ControllerHandle,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
    if (EFI_ERROR (Status)) {
      /*NetLibDestroyServiceChild (
        ControllerHandle,
        gImageHandle,
        ServiceBindingProtocolGuid,
        *ChildHandle
        );*/
      *ChildHandle = NULL;
    }
  }

  return Status;
}

/**
  Close the protocol identified by its GUID on the child handle of the service
  identified by its service binding protocol GUID, then destroy the child
  handle.

  @param[in]  ControllerHandle            Controller handle.
  @param[in]  ServiceBindingProtocolGuid  Service binding protocol GUID of the
                                          service to be destroyed.
  @param[in]  ProtocolGuid                GUID of the protocol to be closed.
  @param[in]  ChildHandle                 Handle of the child to be destroyed.
**/
STATIC
VOID
CloseProtocolAndDestroyServiceChild2 (
  IN  EFI_HANDLE  ControllerHandle,
  IN  EFI_GUID    *ServiceBindingProtocolGuid,
  IN  EFI_GUID    *ProtocolGuid,
  IN  EFI_HANDLE  ChildHandle
  )
{
  gBS->CloseProtocol (
         ChildHandle,
         ProtocolGuid,
         gImageHandle,
         ControllerHandle
         );
	/*
  NetLibDestroyServiceChild (
    ControllerHandle,
    gImageHandle,
    ServiceBindingProtocolGuid,
    ChildHandle
    );*/
}

/**
  Wait until operation completes. Completion is indicated by
  setting of an appropriate variable.

  @param[in]      Context             A pointer to the HTTP download context.
  @param[in, out]  CallBackComplete   A pointer to the callback completion
                                      variable set by the callback.

  @retval  EFI_SUCCESS                Callback signalled completion.
  @retval  EFI_TIMEOUT                Timed out waiting for completion.
  @retval  Others                     Error waiting for completion.
**/
STATIC
EFI_STATUS
WaitForCompletion2 (
  IN HTTP_DOWNLOAD_CONTEXT2  *Context,
  IN OUT BOOLEAN            *CallBackComplete
  )
{
  EFI_STATUS                Status;
  EFI_EVENT                 WaitEvt;

  Status = EFI_SUCCESS;

  //
  // Use a timer to measure timeout. Cannot use Stall here!
  //
  Status = gBS->CreateEvent (
                  EVT_TIMER,
                  TPL_CALLBACK,
                  NULL,
                  NULL,
                  &WaitEvt
                  );
   //ASSERT_EFI_ERROR (Status);

   if (!EFI_ERROR (Status)) {
     Status = gBS->SetTimer (
                     WaitEvt,
                     TimerRelative,
                     EFI_TIMER_PERIOD_SECONDS (TIMER_MAX_TIMEOUT_S)
                     );

     //ASSERT_EFI_ERROR (Status);
   }

  while (! *CallBackComplete
      && (!EFI_ERROR (Status))
      && EFI_ERROR (gBS->CheckEvent (WaitEvt)))
  {
    Status = Context->Http->Poll (Context->Http);
    if (!Context->ContentDownloaded
     && CallBackComplete == &gResponseCallbackComplete)
    {
      //
      // An HTTP server may just send a response redirection header.
      // In this case, don't wait for the event as
      // it might never happen and we waste 10s waiting.
      // Note that at this point Response may not has been populated,
      // so it needs to be checked first.
      //
      if (Context->ResponseToken.Message
       && Context->ResponseToken.Message->Data.Response
       && (NEED_REDIRECTION (
            Context->ResponseToken.Message->Data.Response->StatusCode
            )
         ))
      {
        break;
      }
    }
  }

  gBS->SetTimer (WaitEvt, TimerCancel, 0);
  gBS->CloseEvent (WaitEvt);

  if (*CallBackComplete) {
    return EFI_SUCCESS;
  }

  if (!EFI_ERROR (Status)) {
    Status = EFI_TIMEOUT;
  }

  return Status;
}

/**
  Generate and send a request to the http server.

  @param[in]   Context           HTTP download context.
  @param[in]   DownloadUrl       Fully qualified URL to be downloaded.

  @retval EFI_SUCCESS            Request has been sent successfully.
  @retval EFI_INVALID_PARAMETER  Invalid URL.
  @retval EFI_OUT_OF_RESOURCES   Out of memory.
  @retval EFI_DEVICE_ERROR       If HTTPS is used, this probably
                                 means that TLS support either was not
                                 installed or not configured.
  @retval Others                 Error sending the request.
**/
STATIC
EFI_STATUS
SendRequest2 (
  IN HTTP_DOWNLOAD_CONTEXT2  *Context,
  IN CHAR16                 *DownloadUrl
  )
{
  EFI_HTTP_REQUEST_DATA2       RequestData;
  EFI_HTTP_HEADER2             RequestHeader[HdrMax];
  EFI_HTTP_MESSAGE2            RequestMessage;
  EFI_STATUS                  Status;
  CHAR16                      *Host;
  UINTN                       StringSize;

  ZeroMem (&RequestData, sizeof (RequestData));
  ZeroMem (&RequestHeader, sizeof (RequestHeader));
  ZeroMem (&RequestMessage, sizeof (RequestMessage));
  ZeroMem (&Context->RequestToken, sizeof (Context->RequestToken));

  RequestHeader[HdrHost].FieldName = "Host";
  RequestHeader[HdrConn].FieldName = "Connection";
  RequestHeader[HdrAgent].FieldName = "User-Agent";

  Host = (CHAR16 *)Context->ServerAddrAndProto;
  while (*Host != CHAR_NULL && *Host != L'/') {
    Host++;
  }

  if (*Host == CHAR_NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get the next slash.
  //
  Host++;
  //
  // And now the host name.
  //
  Host++;

  StringSize = StrLen (Host) + 1;
  RequestHeader[HdrHost].FieldValue = AllocatePool (StringSize);
  if (!RequestHeader[HdrHost].FieldValue) {
    return EFI_OUT_OF_RESOURCES;
  }

  UnicodeStrToAsciiStrS (
    Host,
    RequestHeader[HdrHost].FieldValue,
    StringSize
    );

  RequestHeader[HdrConn].FieldValue = "close";
  RequestHeader[HdrAgent].FieldValue = USER_AGENT_HDR;
  RequestMessage.HeaderCount = HdrMax;

  RequestData.Method = HttpMethodGet;
  RequestData.Url = DownloadUrl;

  RequestMessage.Data.Request = &RequestData;
  RequestMessage.Headers = RequestHeader;
  RequestMessage.BodyLength = 0;
  RequestMessage.Body = NULL;
  Context->RequestToken.Event = NULL;

  //
  // Completion callback event to be set when Request completes.
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  RequestCallback2,
                  Context,
                  &Context->RequestToken.Event
                  );
  //ASSERT_EFI_ERROR (Status);

  Context->RequestToken.Status = EFI_SUCCESS;
  Context->RequestToken.Message = &RequestMessage;
  gRequestCallbackComplete = FALSE;
  Status = Context->Http->Request (Context->Http, &Context->RequestToken);
  if (EFI_ERROR (Status)) {
    goto Error;
  }

  Status = WaitForCompletion2 (Context, &gRequestCallbackComplete);
  if (EFI_ERROR (Status)) {
    Context->Http->Cancel (Context->Http, &Context->RequestToken);
  }

Error:
  //SHELL_FREE_NON_NULL (RequestHeader[HdrHost].FieldValue);
  if (Context->RequestToken.Event) {
    gBS->CloseEvent (Context->RequestToken.Event);
    ZeroMem (&Context->RequestToken, sizeof (Context->RequestToken));
  }

  return Status;
}

/**
  Update the progress of a file download
  This procedure is called each time a new HTTP body portion is received.

  @param[in]  Context      HTTP download context.
  @param[in]  DownloadLen  Portion size, in bytes.
  @param[in]  Buffer       The pointer to the parsed buffer.

  @retval  EFI_SUCCESS     Portion saved.
  @retval  Other           Error saving the portion.
**/
STATIC
EFI_STATUS
EFIAPI
SavePortion2 (
  IN HTTP_DOWNLOAD_CONTEXT2  *Context,
  IN UINTN                  DownloadLen,
  IN CHAR8                  *Buffer
  )
{
  CHAR16            Progress[HTTP_PROGRESS_MESSAGE_SIZE];
  UINTN             NbOfKb;
  UINTN             Index;
  UINTN             LastStep;
  UINTN             Step;
  EFI_STATUS        Status;

  LastStep = 0;
  Step = 0;

  ShellSetFilePosition (mFileHandle, Context->LastReportedNbOfBytes);
  Status = ShellWriteFile (mFileHandle, &DownloadLen, Buffer);
  if (EFI_ERROR (Status)) {
    if (Context->ContentDownloaded > 0) {
      //PRINT_HII (STRING_TOKEN (STR_GEN_CRLF), NULL);
    }

    //PRINT_HII (STRING_TOKEN (STR_HTTP_ERR_WRITE), mLocalFilePath, Status);
    return Status;
  }

  if (Context->ContentDownloaded == 0) {
    ShellPrintEx (-1, -1, L"%s       0 Kb", HTTP_PROGR_FRAME);
  }

  Context->ContentDownloaded += DownloadLen;
  NbOfKb = Context->ContentDownloaded >> 10;

  Progress[0] = L'\0';
  if (Context->ContentLength) {
    LastStep  = (Context->LastReportedNbOfBytes * HTTP_PROGRESS_SLIDER_STEPS) /
                 Context->ContentLength;
    Step      = (Context->ContentDownloaded * HTTP_PROGRESS_SLIDER_STEPS) /
                 Context->ContentLength;
  }

  Context->LastReportedNbOfBytes = Context->ContentDownloaded;

  if (Step <= LastStep) {
    if (!Context->ContentLength) {
      //
      // Update downloaded size, there is no length info available.
      //
      ShellPrintEx (-1, -1, L"%s", HTTP_KB);
      ShellPrintEx (-1, -1, L"%7d Kb", NbOfKb);
    }

    return EFI_SUCCESS;
  }

  ShellPrintEx (-1, -1, L"%s", HTTP_PROGRESS_DEL);

  Status = StrCpyS (Progress, HTTP_PROGRESS_MESSAGE_SIZE, HTTP_PROGR_FRAME);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Index = 1; Index < Step; Index++) {
    Progress[Index] = L'=';
  }

  if (Step) {
    Progress[Step] = L'>';
  }

  UnicodeSPrint (
    Progress + (sizeof (HTTP_PROGR_FRAME) / sizeof (CHAR16)) - 1,
    sizeof (Progress) - sizeof (HTTP_PROGR_FRAME),
    L" %7d Kb",
    NbOfKb
    );


  ShellPrintEx (-1, -1, L"%s", Progress);

  return EFI_SUCCESS;
}

/**
  Replace the original Host and Uri with Host and Uri returned by the
  HTTP server in 'Location' header (redirection).

  @param[in]   Location           A pointer to the 'Location' string
                                  provided by HTTP server.
  @param[in]   Context            A pointer to HTTP download context.
  @param[in]   DownloadUrl        Fully qualified HTTP URL.

  @retval  EFI_SUCCESS            Host and Uri were successfully set.
  @retval  EFI_OUT_OF_RESOURCES   Error setting Host or Uri.
**/
STATIC
EFI_STATUS
SetHostURI2 (
  IN CHAR8                 *Location,
  IN HTTP_DOWNLOAD_CONTEXT2 *Context,
  IN CHAR16                *DownloadUrl
  )
{
  EFI_STATUS    Status;
  UINTN         StringSize;
  UINTN         FirstStep;
  UINTN         Idx;
  UINTN         Step;
  CHAR8         *Walker;
  CHAR16        *Temp;
  CHAR8         *Tmp;
  CHAR16        *Url;
  BOOLEAN       IsAbEmptyUrl;

  Tmp = NULL;
  Url = NULL;
  IsAbEmptyUrl = FALSE;
  FirstStep = 0;

  StringSize = (AsciiStrSize (Location) * sizeof (CHAR16));
  Url = AllocateZeroPool (StringSize);
  if (!Url) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = AsciiStrToUnicodeStrS (
             (CONST CHAR8 *)Location,
             Url,
             StringSize
             );

  if (EFI_ERROR (Status)) {
        goto Error;
  }

  //
  // If an HTTP server redirects to the same location more than once,
  // then stop attempts and tell it is not reachable.
  //
  if (!StrCmp (Url, DownloadUrl)) {
    Status = EFI_NO_MAPPING;
    goto Error;
  }

  if (AsciiStrLen (Location) > 2) {
    //
    // Some servers return 'Location: //server/resource'
    //
    IsAbEmptyUrl = (Location[0] == '/') && (Location[1] == '/');
    if (IsAbEmptyUrl) {
      //
      // Skip first "//"
      //
      Location += 2;
      FirstStep = 1;
    }
  }

  if (AsciiStrStr (Location, "://") || IsAbEmptyUrl) {
    Idx = 0;
    Walker = Location;

    for (Step = FirstStep; Step < 2; Step++) {
      for (; *Walker != '/' && *Walker != '\0'; Walker++) {
        Idx++;
      }

      if (!Step) {
        //
        // Skip "//"
        //
        Idx += 2;
        Walker += 2;
      }
    }

    Tmp = AllocateZeroPool (Idx + 1);
    if (!Tmp) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Error;
    }

    CopyMem (Tmp, Location, Idx);

    //
    // Location now points to Uri
    //
    Location += Idx;
    StringSize = (Idx + 1) * sizeof (CHAR16);

    //SHELL_FREE_NON_NULL (Context->ServerAddrAndProto);

    Temp = AllocateZeroPool (StringSize);
    if (!Temp) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Error;
    }

    Status = AsciiStrToUnicodeStrS (
               (CONST CHAR8 *)Tmp,
               Temp,
               StringSize
               );
    if (EFI_ERROR (Status)) {
      //SHELL_FREE_NON_NULL (Temp);
      goto Error;
    }

    Idx = 0;
    if (IsAbEmptyUrl) {
      Context->ServerAddrAndProto = StrnCatGrow (
                                      &Context->ServerAddrAndProto,
                                      &Idx,
                                      L"http://",
                                      StrLen (L"http://")
                                      );
    }

    Context->ServerAddrAndProto = StrnCatGrow (
                                    &Context->ServerAddrAndProto,
                                    &Idx,
                                    Temp,
                                    StrLen (Temp)
                                    );
    //SHELL_FREE_NON_NULL (Temp);
    if (!Context->ServerAddrAndProto) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Error;
    }
  }

  //SHELL_FREE_NON_NULL (Context->Uri);

  StringSize = AsciiStrSize (Location) * sizeof (CHAR16);
  Context->Uri = AllocateZeroPool (StringSize);
  if (!Context->Uri) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Error;
  }

  //
  // Now make changes to the Uri part.
  //
  Status = AsciiStrToUnicodeStrS (
             (CONST CHAR8 *)Location,
             Context->Uri,
             StringSize
             );
Error:
  //SHELL_FREE_NON_NULL (Tmp);
  //SHELL_FREE_NON_NULL (Url);

  return Status;
}

//
// HTTP body parser interface.
//

typedef enum {
  //
  // Part of entity data.
  // Length of entity body in Data.
  //
  BodyParseEventOnData,
  //
  // End of message body.
  // Length is 0 and Data points to next byte after the end of the message.
  //
  BodyParseEventOnComplete
} HTTP_BODY_PARSE_EVENT;

/**
  Message parser callback.
  Save a portion of HTTP body.

  @param[in]   EventType       Type of event. Can be either
                               OnComplete or OnData.
  @param[in]   Data            A pointer to the buffer with data.
  @param[in]   Length          Data length of this portion.
  @param[in]   Context         A pointer to the HTTP download context.

  @retval      EFI_SUCCESS    The portion was processed successfully.
  @retval      Other          Error returned by SavePortion2.
**/
STATIC
EFI_STATUS
EFIAPI
ParseMsg2 (
  IN HTTP_BODY_PARSE_EVENT      EventType,
  IN CHAR8                      *Data,
  IN UINTN                      Length,
  IN VOID                       *Context
  )
{
  if ((Data == NULL)
   || (EventType == BodyParseEventOnComplete)
   || (Context == NULL))
  {
    return EFI_SUCCESS;
  }

  return SavePortion2 (Context, Length, Data);
}

#define REQ_OK           0
#define REQ_NEED_REPEAT  1


/**
  Get HTTP server response and collect the whole body as a file.
  Set appropriate status in Context (REQ_OK, REQ_REPEAT, REQ_ERROR).
  Note that even if HTTP server returns an error code, it might send
  the body as well. This body will be collected in the resultant file.

  @param[in]   Context         A pointer to the HTTP download context.
  @param[in]   DownloadUrl     A pointer to the fully qualified URL to download.

  @retval  EFI_SUCCESS         Valid file. Body successfully collected.
  @retval  EFI_HTTP_ERROR      Response is a valid HTTP response, but the
                               HTTP server
                               indicated an error (HTTP code >= 400).
                               Response body MAY contain full
                               HTTP server response.
  @retval Others               Error getting the reponse from the HTTP server.
                               Response body is not collected.
**/
STATIC
EFI_STATUS
GetResponse2 (
  IN HTTP_DOWNLOAD_CONTEXT2    *Context,
  IN CHAR16                   *DownloadUrl
  )
{
  EFI_HTTP_RESPONSE_DATA2      ResponseData;
  EFI_HTTP_MESSAGE2            ResponseMessage;
  EFI_HTTP_HEADER2             *Header;
  EFI_STATUS                  Status;
  VOID                        *MsgParser;
  EFI_TIME                    StartTime;
  EFI_TIME                    EndTime;
  CONST CHAR16                *Desc;
  UINTN                       ElapsedSeconds;
  BOOLEAN                     IsTrunked;
  BOOLEAN                     CanMeasureTime;

  ZeroMem (&ResponseData, sizeof (ResponseData));
  ZeroMem (&ResponseMessage, sizeof (ResponseMessage));
  ZeroMem (&Context->ResponseToken, sizeof (Context->ResponseToken));
  IsTrunked = FALSE;

  ResponseMessage.Body = Context->Buffer;
  Context->ResponseToken.Status = EFI_SUCCESS;
  Context->ResponseToken.Message = &ResponseMessage;
  Context->ContentLength = 0;
  Context->Status = REQ_OK;
  Status = EFI_SUCCESS;
  MsgParser = NULL;
  ResponseData.StatusCode = HTTP_STATUS_UNSUPPORTED_STATUS;
  ResponseMessage.Data.Response = &ResponseData;
  Context->ResponseToken.Event = NULL;
  CanMeasureTime = FALSE;
  if (Context->Flags & DL_FLAG_TIME) {
    ZeroMem (&StartTime, sizeof (StartTime));
    CanMeasureTime = !EFI_ERROR (gRT->GetTime (&StartTime, NULL));
  }

  do {
    //SHELL_FREE_NON_NULL (ResponseMessage.Headers);
    ResponseMessage.HeaderCount = 0;
    gResponseCallbackComplete = FALSE;
    ResponseMessage.BodyLength = Context->BufferSize;

    if (ShellGetExecutionBreakFlag ()) {
      Status = EFI_ABORTED;
      break;
    }

    if (!Context->ContentDownloaded && !Context->ResponseToken.Event) {
      Status = gBS->CreateEvent (
                      EVT_NOTIFY_SIGNAL,
                      TPL_CALLBACK,
                      ResponseCallback2,
                      Context,
                      &Context->ResponseToken.Event
                      );
      //ASSERT_EFI_ERROR (Status);
    } else {
      ResponseMessage.Data.Response = NULL;
    }

    if (EFI_ERROR (Status)) {
      break;
    }

    Status = Context->Http->Response (Context->Http, &Context->ResponseToken);
    if (EFI_ERROR (Status)) {
      break;
    }

    Status = WaitForCompletion2 (Context, &gResponseCallbackComplete);
    if (EFI_ERROR (Status) && ResponseMessage.HeaderCount) {
      Status = EFI_SUCCESS;
    }

    if (EFI_ERROR (Status)) {
      Context->Http->Cancel (Context->Http, &Context->ResponseToken);
      break;
    }

    if (!Context->ContentDownloaded) {
      if (NEED_REDIRECTION (ResponseData.StatusCode)) {
        //
        // Need to repeat the request with new Location (server redirected).
        //
        Context->Status = REQ_NEED_REPEAT;

        Header = HttpFindHeader (
                   ResponseMessage.HeaderCount,
                   ResponseMessage.Headers,
                   "Location"
                   );
        if (Header) {
          Status = SetHostURI2 (Header->FieldValue, Context, DownloadUrl);
          if (Status == EFI_NO_MAPPING) {
            /*PRINT_HII (
              STRING_TOKEN (STR_HTTP_ERR_STATUSCODE),
              Context->ServerAddrAndProto,
              L"Recursive HTTP server relocation",
              Context->Uri
              );*/
          }
        } else {
          //
          // Bad reply from the server. Server must specify the location.
          // Indicate that resource was not found, and no body collected.
          //
          Status = EFI_NOT_FOUND;
        }

        Context->Http->Cancel (Context->Http, &Context->ResponseToken);
        break;
      }

      //
      // Init message-body parser by header information.
      //
      if (!MsgParser) {
        Status = HttpInitMsgParser (
                   ResponseMessage.Data.Request->Method,
                   ResponseData.StatusCode,
                   ResponseMessage.HeaderCount,
                   ResponseMessage.Headers,
                   ParseMsg2,
                   Context,
                   &MsgParser
                   );
        if (EFI_ERROR (Status)) {
          break;
        }
      }

      //
      // If it is a trunked message, rely on the parser.
      //
      Header = HttpFindHeader (
                 ResponseMessage.HeaderCount,
                 ResponseMessage.Headers,
                 "Transfer-Encoding"
                 );
      IsTrunked = (Header && !AsciiStrCmp (Header->FieldValue, "chunked"));

      HttpGetEntityLength (MsgParser, &Context->ContentLength);

      if (ResponseData.StatusCode >= HTTP_STATUS_400_BAD_REQUEST
       && (ResponseData.StatusCode != HTTP_STATUS_308_PERMANENT_REDIRECT))
      {
        //
        // Server reported an error via Response code.
        // Collect the body if any.
        //
        if (!gHttpError) {
          gHttpError = TRUE;

          Desc = ErrStatusDesc[ResponseData.StatusCode -
                               HTTP_STATUS_400_BAD_REQUEST];
          /*PRINT_HII (
            STRING_TOKEN (STR_HTTP_ERR_STATUSCODE),
            Context->ServerAddrAndProto,
            Desc,
            Context->Uri
            );*/

          //
          // This gives an RFC HTTP error.
          //
          Context->Status = ShellStrToUintn (Desc);
          Status = ENCODE_ERROR (Context->Status);
        }
      }
    }

    //
    // Do NOT try to parse an empty body.
    //
    if (ResponseMessage.BodyLength || IsTrunked) {
      Status = HttpParseMessageBody (
                 MsgParser,
                 ResponseMessage.BodyLength,
                 ResponseMessage.Body
                 );
    }
  } while (!HttpIsMessageComplete (MsgParser)
        && !EFI_ERROR (Status)
        && ResponseMessage.BodyLength);

  if (Context->Status != REQ_NEED_REPEAT
   && Status == EFI_SUCCESS
   && CanMeasureTime)
  {
    if (!EFI_ERROR (gRT->GetTime (&EndTime, NULL))) {
      ElapsedSeconds = EfiTimeToEpoch2 (&EndTime) - EfiTimeToEpoch2 (&StartTime);
      Print (
        L",%a%Lus\n",
        ElapsedSeconds ? " " : " < ",
        ElapsedSeconds > 1 ? (UINT64)ElapsedSeconds : 1
        );
    }
  }

  //SHELL_FREE_NON_NULL (MsgParser);
  if (Context->ResponseToken.Event) {
    gBS->CloseEvent (Context->ResponseToken.Event);
    ZeroMem (&Context->ResponseToken, sizeof (Context->ResponseToken));
  }

  return Status;
}

/**
  Worker function that downloads the data of a file from an HTTP server given
  the path of the file and its size.

  @param[in]   Context           A pointer to the HTTP download context.
  @param[in]   ControllerHandle  The handle of the network interface controller
  @param[in]   NicName           NIC name

  @retval  EFI_SUCCESS           The file was downloaded.
  @retval  EFI_OUT_OF_RESOURCES  A memory allocation failed.
  #return  EFI_HTTP_ERROR        The server returned a valid HTTP error.
                                 Examine the mLocalFilePath file
                                 to get error body.
  @retval  Others                The downloading of the file from the server
                                 failed.
**/
STATIC
EFI_STATUS
DownloadFile2 (
  IN HTTP_DOWNLOAD_CONTEXT2   *Context,
  IN EFI_HANDLE              ControllerHandle,
  IN CHAR16                  *NicName
  )
{
  EFI_STATUS                 Status;
  CHAR16                     *DownloadUrl;
  UINTN                      UrlSize;
  EFI_HANDLE                 HttpChildHandle;

  //ASSERT (Context);
  if (Context == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DownloadUrl = NULL;
  HttpChildHandle = NULL;

  Context->Buffer = AllocatePool (Context->BufferSize);
  if (Context->Buffer == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  //
  // Open the file.
  //
  if (!EFI_ERROR (ShellFileExists (mLocalFilePath))) {
    ShellDeleteFileByName (mLocalFilePath);
  }

  Status = ShellOpenFileByName (
             mLocalFilePath,
             &mFileHandle,
             EFI_FILE_MODE_CREATE |
             EFI_FILE_MODE_WRITE  |
             EFI_FILE_MODE_READ,
             0
             );
  if (EFI_ERROR (Status)) {
    //PRINT_HII_APP (STRING_TOKEN (STR_GEN_FILE_OPEN_FAIL), mLocalFilePath);
    goto ON_EXIT;
  }

  do {
    //SHELL_FREE_NON_NULL (DownloadUrl);

    CLOSE_HTTP_HANDLE (ControllerHandle, HttpChildHandle);

    Status = CreateServiceChildAndOpenProtocol2 (
               ControllerHandle,
               &gEfiHttpServiceBindingProtocolGuid,
               &gEfiHttpProtocolGuid,
               &HttpChildHandle,
               (VOID**)&Context->Http
               );

    if (EFI_ERROR (Status)) {
      //PRINT_HII (STRING_TOKEN (STR_HTTP_ERR_OPEN_PROTOCOL), NicName, Status);
      goto ON_EXIT;
    }

    Status = Context->Http->Configure (Context->Http, &Context->HttpConfigData);
    if (EFI_ERROR (Status)) {
      //PRINT_HII (STRING_TOKEN (STR_HTTP_ERR_CONFIGURE), NicName, Status);
      goto ON_EXIT;
    }

    UrlSize = 0;
    DownloadUrl = StrnCatGrow (
                    &DownloadUrl,
                    &UrlSize,
                    Context->ServerAddrAndProto,
                    StrLen (Context->ServerAddrAndProto)
                    );
    if (Context->Uri[0] != L'/') {
      DownloadUrl = StrnCatGrow (
                      &DownloadUrl,
                      &UrlSize,
                      L"/",
                      StrLen (Context->ServerAddrAndProto)
                      );
    }

    DownloadUrl = StrnCatGrow (
                    &DownloadUrl,
                    &UrlSize,
                    Context->Uri,
                    StrLen (Context->Uri));

    //PRINT_HII (STRING_TOKEN (STR_HTTP_DOWNLOADING), DownloadUrl);

    Status = SendRequest2 (Context, DownloadUrl);
    if (Status) {
      goto ON_EXIT;
    }

    Status = GetResponse2 (Context, DownloadUrl);

    if (Status) {
      goto ON_EXIT;
    }

  } while (Context->Status == REQ_NEED_REPEAT);

  if (Context->Status) {
    Status = ENCODE_ERROR (Context->Status);
  }

ON_EXIT:
  //
  // Close the file.
  //
  if (mFileHandle != NULL) {
    if (EFI_ERROR (Status) && !(Context->Flags & DL_FLAG_KEEP_BAD)) {
      ShellDeleteFile (&mFileHandle);
    } else {
      ShellCloseFile (&mFileHandle);
    }
  }

  //SHELL_FREE_NON_NULL (DownloadUrl);
  //SHELL_FREE_NON_NULL (Context->Buffer);

  CLOSE_HTTP_HANDLE (ControllerHandle, HttpChildHandle);

  return Status;
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
EFI_STATUS L2_APPLICATIONS_Command_curl(UINT8 parameters[PARAMETER_COUNT][PARAMETER_LENGTH], UINT8 *pReturnCode)
{
    UINT8 j = 0;
	L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_APPLICATIONS_Command_curl:%a \n", __LINE__, parameters[1]);
	
	int TerminalWindowMaxLineCount = 0;
	TerminalWindowMaxLineCount = (ScreenHeight - 23) / 2;
	TerminalWindowMaxLineCount /= 16;
	
    //写入键盘缓存到终端窗口。
	L2_DEBUG_Print3(3, 23 + (++TerminalCurrentLineCount) % TerminalWindowMaxLineCount * 16, WindowLayers.item[GRAPHICS_LAYER_TERMINAL_WINDOW], "%a", "Curl");

	//HttpCreateService;
	//HttpDxeStart
	//RunHttp2
	//DownloadFile

	//infer from: RedfishRestExDriverBindingStart
	EFI_HANDLE                   ControllerHandle;
	EFI_STATUS Status;
	EFI_HANDLE Handle;
	EFI_HANDLE HttpChildHandle;
		
	RunHttp2(NULL, NULL);

	/*DownloadFile();

	EFI_HTTP_PROTOCOL  mEfiHttpTemplate = {
		EfiHttpGetModeData,
		EfiHttpConfigure,
		EfiHttpRequest,
		EfiHttpCancel,
		EfiHttpResponse,
		EfiHttpPoll
	};*/
}



