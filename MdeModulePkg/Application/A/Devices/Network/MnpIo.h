/** @file
  Implementation of Managed Network Protocol I/O functions.

Copyright (c) 2005 - 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "MnpImpl.h"
#include "MnpVlan.h"

/**
  Validates the Mnp transmit token.

  @param[in]  Instance            Pointer to the Mnp instance context data.
  @param[in]  Token               Pointer to the transmit token to check.

  @return The Token is valid or not.

**/
BOOLEAN
MnpIsValidTxToken (
  IN MNP_INSTANCE_DATA                       *Instance,
  IN EFI_MANAGED_NETWORK_COMPLETION_TOKEN    *Token
  );

/**
  Build the packet to transmit from the TxData passed in.

  @param[in]   MnpServiceData      Pointer to the mnp service context data.
  @param[in]   TxData              Pointer to the transmit data containing the information
                                   to build the packet.
  @param[out]  PktBuf              Pointer to record the address of the packet.
  @param[out]  PktLen              Pointer to a UINT32 variable used to record the packet's
                                   length.

  @retval EFI_SUCCESS           TxPackage is built.
  @retval EFI_OUT_OF_RESOURCES  The deliver fails due to lack of memory resource.

**/
EFI_STATUS
MnpBuildTxPacket (
  IN     MNP_SERVICE_DATA                    *MnpServiceData,
  IN     EFI_MANAGED_NETWORK_TRANSMIT_DATA   *TxData,
     OUT UINT8                               **PktBuf,
     OUT UINT32                              *PktLen
  );

/**
  Synchronously send out the packet.

  This function places the packet buffer to SNP driver's tansmit queue. The packet
  can be considered successfully sent out once SNP accept the packet, while the
  packet buffer recycle is deferred for better performance.

  @param[in]       MnpServiceData      Pointer to the mnp service context data.
  @param[in]       Packet              Pointer to the packet buffer.
  @param[in]       Length              The length of the packet.
  @param[in, out]  Token               Pointer to the token the packet generated from.

  @retval EFI_SUCCESS                  The packet is sent out.
  @retval EFI_TIMEOUT                  Time out occurs, the packet isn't sent.
  @retval EFI_DEVICE_ERROR             An unexpected network error occurs.

**/
EFI_STATUS
MnpSyncSendPacket (
  IN     MNP_SERVICE_DATA                        *MnpServiceData,
  IN     UINT8                                   *Packet,
  IN     UINT32                                  Length,
  IN OUT EFI_MANAGED_NETWORK_COMPLETION_TOKEN    *Token
  );
