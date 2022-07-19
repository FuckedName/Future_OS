/** @file
  The implementation for the 'http' Shell command.

  Copyright (c) 2015, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved. <BR>
  (C) Copyright 2015 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2020, Broadcom. All rights reserved. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#pragma once

#include <Protocol/Shell.h>

//#include "Http.h"

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
RunHttp (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  );

