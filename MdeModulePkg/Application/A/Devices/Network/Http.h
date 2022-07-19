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


// infer from http.h
#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HiiLib.h>
#include <Library/HttpLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/NetLib.h>
#include <Library/PrintLib.h>
#include <Library/ShellLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <Protocol/HiiPackageList.h>
#include <Protocol/HttpUtilities.h>
#include <Protocol/ServiceBinding.h>

#define HTTP_APP_NAME L"http"

#define REQ_OK           0
#define REQ_NEED_REPEAT  1

//
// Download Flags.
//
#define DL_FLAG_TIME     BIT0 // Show elapsed time.
#define DL_FLAG_KEEP_BAD BIT1 // Keep files even if download failed.

extern EFI_HII_HANDLE mHttpHiiHandle;

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
  EFI_HTTP_TOKEN        ResponseToken;
  EFI_HTTP_TOKEN        RequestToken;
  EFI_HTTP_PROTOCOL     *Http;
  EFI_HTTP_CONFIG_DATA  HttpConfigData;
} HTTP_DOWNLOAD_CONTEXT;

/**
  Function for 'http' command.

  @param[in]  ImageHandle     The image handle.
  @param[in]  SystemTable     The system table.

  @retval SHELL_SUCCESS            Command completed successfully.
  @retval SHELL_INVALID_PARAMETER  Command usage error.
  @retval SHELL_ABORTED            The user aborts the operation.
  @retval value                    Unknown error.
**/
SHELL_STATUS
RunHttp (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  );

/**
  Retrieve HII package list from ImageHandle and publish to HII database.

  @param[in] ImageHandle            The image handle of the process.

  @retval HII handle.
**/
EFI_HII_HANDLE
InitializeHiiPackage (
  IN EFI_HANDLE                  ImageHandle
  );

  

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

