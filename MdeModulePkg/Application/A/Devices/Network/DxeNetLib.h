/** @file
  Network library.

Copyright (c) 2005 - 2018, Intel Corporation. All rights reserved.<BR>
(C) Copyright 2015 Hewlett Packard Enterprise Development LP<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>

#include <IndustryStandard/SmBios.h>

#include <Protocol/DriverBinding.h>
#include <Protocol/ServiceBinding.h>
#include <Protocol/SimpleNetwork.h>
#include <Protocol/AdapterInformation.h>
#include <Protocol/ManagedNetwork.h>
#include <Protocol/Ip4Config2.h>
#include <Protocol/ComponentName.h>
#include <Protocol/ComponentName2.h>

#include <Guid/SmBios.h>

#include <Library/NetLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiLib.h>


/**
  Convert one Null-terminated Unicode string (decimal dotted) to EFI_IPv4_ADDRESS.

  @param[in]      String         The pointer to the Ascii string.
  @param[out]     Ip4Address     The pointer to the converted IPv4 address.

  @retval EFI_SUCCESS            Convert to IPv4 address successfully.
  @retval EFI_INVALID_PARAMETER  The string is malformatted or Ip4Address is NULL.

**/
EFI_STATUS
EFIAPI
NetLibStrToIp4 (
  IN CONST CHAR16                *String,
  OUT      EFI_IPv4_ADDRESS      *Ip4Address
  );

