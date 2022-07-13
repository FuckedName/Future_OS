/** @file
  Interface function of the Socket.

  Copyright (c) 2009 - 2018, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "SockImpl.h"

/**
  Issue a token with data to the socket to send out.

  @param[in]  Sock             Pointer to the socket to process the token with
                               data.
  @param[in]  Token            The token with data that needs to send out.

  @retval EFI_SUCCESS          The token processed successfully.
  @retval EFI_ACCESS_DENIED    Failed to get the lock to access the socket, or the
                               socket is closed, or the socket is not in a
                               synchronized state , or the token is already in one
                               of this socket's lists.
  @retval EFI_NO_MAPPING       The IP address configuration operation is not
                               finished.
  @retval EFI_NOT_STARTED      The socket is not configured.
  @retval EFI_OUT_OF_RESOURCE  Failed to buffer the token due to memory limits.

**/
EFI_STATUS
SockSend (
  IN SOCKET *Sock,
  IN VOID   *Token
  );

