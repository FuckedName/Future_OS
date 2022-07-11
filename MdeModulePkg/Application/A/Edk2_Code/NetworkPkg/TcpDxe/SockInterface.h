/** @file
  Interface function of the Socket.

  Copyright (c) 2009 - 2018, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "SockImpl.h"

BOOLEAN
SockTokenExistedInList (
  IN LIST_ENTRY     *List,
  IN EFI_EVENT      Event
  );
  
BOOLEAN
SockTokenExisted (
  IN SOCKET    *Sock,
  IN EFI_EVENT Event
  );
  
SOCK_TOKEN *
SockBufferToken (
  IN SOCKET         *Sock,
  IN LIST_ENTRY     *List,
  IN VOID           *Token,
  IN UINT32         DataLen
  );
  
EFI_STATUS
SockDestroyChild (
  IN OUT SOCKET *Sock
  );
  
SOCKET *
SockCreateChild (
  IN SOCK_INIT_DATA *SockInitData
  );
  
EFI_STATUS
SockConfigure (
  IN SOCKET *Sock,
  IN VOID   *ConfigData
  );
  
EFI_STATUS
SockConnect (
  IN SOCKET *Sock,
  IN VOID   *Token
  );
  
EFI_STATUS
SockAccept (
  IN SOCKET *Sock,
  IN VOID   *Token
  );
  
EFI_STATUS
SockSend (
  IN SOCKET *Sock,
  IN VOID   *Token
  );
  
EFI_STATUS
SockRcv (
  IN SOCKET *Sock,
  IN VOID   *Token
  );
  
EFI_STATUS
SockFlush (
  IN OUT SOCKET *Sock
  );
  
EFI_STATUS
SockClose (
  IN OUT SOCKET  *Sock,
  IN     VOID    *Token,
  IN     BOOLEAN OnAbort
  );
  
EFI_STATUS
SockCancel (
  IN OUT SOCKET  *Sock,
  IN     VOID    *Token
  );
  
EFI_STATUS
SockGetMode (
  IN     SOCKET *Sock,
  IN OUT VOID   *Mode
  );

EFI_STATUS
SockRoute (
  IN SOCKET    *Sock,
  IN VOID      *RouteInfo
  );


