/** @file
  Implementation of Managed Network Protocol private services.

Copyright (c) 2005 - 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "MnpImpl.h"
#include "MnpVlan.h"


/**
  Allocate a free TX buffer from MnpDeviceData->FreeTxBufList. If there is none
  in the queue, first try to recycle some from SNP, then try to allocate some and add
  them into the queue, then fetch the NET_BUF from the updated FreeTxBufList.

  @param[in, out]  MnpDeviceData        Pointer to the MNP_DEVICE_DATA.

  @return     Pointer to the allocated free NET_BUF structure, if NULL the
              operation is failed.

**/
UINT8 *
MnpAllocTxBuf (
  IN OUT MNP_DEVICE_DATA   *MnpDeviceData
  );

/**
  Try to recycle all the transmitted buffer address from SNP.

  @param[in, out]  MnpDeviceData     Pointer to the mnp device context data.

  @retval EFI_SUCCESS             Successed to recyclethe transmitted buffer address.
  @retval Others                  Failed to recyclethe transmitted buffer address.

**/
EFI_STATUS
MnpRecycleTxBuf (
  IN OUT MNP_DEVICE_DATA   *MnpDeviceData
  );

