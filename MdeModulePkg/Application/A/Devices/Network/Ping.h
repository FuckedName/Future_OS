/** @file
  The implementation for Ping shell command.

  (C) Copyright 2015 Hewlett-Packard Development Company, L.P.<BR>
  Copyright (c) 2009 - 2018, Intel Corporation. All rights reserved.<BR>
  (C) Copyright 2016 Hewlett Packard Enterprise Development LP<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "UefiShellNetwork1CommandsLib.h"

#define PING_IP_CHOICE_IP4    1
#define PING_IP_CHOICE_IP6    2

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
ShellPing (
  IN UINT32              SendNumber,
  IN UINT32              BufferSize,
  IN EFI_IPv6_ADDRESS    *SrcAddress,
  IN EFI_IPv6_ADDRESS    *DstAddress,
  IN UINT32              IpChoice
  );
