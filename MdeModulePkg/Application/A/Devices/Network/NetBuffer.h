/** @file
  Network library functions providing net buffer operation support.

Copyright (c) 2005 - 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>

#include <Library/NetLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>


/**
  Allocate and build up the sketch for a NET_BUF.

  The net buffer allocated has the BlockOpNum's NET_BLOCK_OP, and its associated
  NET_VECTOR has the BlockNum's NET_BLOCK. But all the NET_BLOCK_OP and
  NET_BLOCK remain un-initialized.

  @param[in]  BlockNum       The number of NET_BLOCK in the vector of net buffer
  @param[in]  BlockOpNum     The number of NET_BLOCK_OP in the net buffer

  @return                    Pointer to the allocated NET_BUF, or NULL if the
                             allocation failed due to resource limit.

**/
NET_BUF *
NetbufAllocStruct (
  IN UINT32                 BlockNum,
  IN UINT32                 BlockOpNum
  );


/**
  Allocate a single block NET_BUF. Upon allocation, all the
  free space is in the tail room.

  @param[in]  Len              The length of the block.

  @return                      Pointer to the allocated NET_BUF, or NULL if the
                               allocation failed due to resource limit.

**/
NET_BUF  *
EFIAPI
NetbufAlloc (
  IN UINT32                 Len
  );


/**
  Free the net vector.

  Decrease the reference count of the net vector by one. The real resource free
  operation isn't performed until the reference count of the net vector is
  decreased to 0.

  @param[in]  Vector                Pointer to the NET_VECTOR to be freed.

**/
VOID
NetbufFreeVector (
  IN NET_VECTOR             *Vector
  );

/**
  Free the net buffer and its associated NET_VECTOR.

  Decrease the reference count of the net buffer by one. Free the associated net
  vector and itself if the reference count of the net buffer is decreased to 0.
  The net vector free operation just decrease the reference count of the net
  vector by one and do the real resource free operation when the reference count
  of the net vector is 0.

  @param[in]  Nbuf                  Pointer to the NET_BUF to be freed.

**/
VOID
EFIAPI
NetbufFree (
  IN NET_BUF                *Nbuf
  );


/**
  Create a copy of the net buffer that shares the associated net vector.

  The reference count of the newly created net buffer is set to 1. The reference
  count of the associated net vector is increased by one.

  @param[in]  Nbuf              Pointer to the net buffer to be cloned.

  @return                       Pointer to the cloned net buffer, or NULL if the
                                allocation failed due to resource limit.

**/
NET_BUF *
EFIAPI
NetbufClone (
  IN NET_BUF                *Nbuf
  );


/**
  Create a duplicated copy of the net buffer with data copied and HeadSpace
  bytes of head space reserved.

  The duplicated net buffer will allocate its own memory to hold the data of the
  source net buffer.

  @param[in]       Nbuf         Pointer to the net buffer to be duplicated from.
  @param[in, out]  Duplicate    Pointer to the net buffer to duplicate to, if
                                NULL a new net buffer is allocated.
  @param[in]      HeadSpace     Length of the head space to reserve.

  @return                       Pointer to the duplicated net buffer, or NULL if
                                the allocation failed due to resource limit.

**/
NET_BUF  *
EFIAPI
NetbufDuplicate (
  IN NET_BUF                *Nbuf,
  IN OUT NET_BUF            *Duplicate        OPTIONAL,
  IN UINT32                 HeadSpace
  );



/**
  Free a list of net buffers.

  @param[in, out]  Head              Pointer to the head of linked net buffers.

**/
VOID
EFIAPI
NetbufFreeList (
  IN OUT LIST_ENTRY         *Head
  );


/**
  Get the index of NET_BLOCK_OP that contains the byte at Offset in the net
  buffer.

  This can be used to, for example, retrieve the IP header in the packet. It
  also can be used to get the fragment that contains the byte which is used
  mainly by the library implementation itself.

  @param[in]   Nbuf      Pointer to the net buffer.
  @param[in]   Offset    The offset of the byte.
  @param[out]  Index     Index of the NET_BLOCK_OP that contains the byte at
                         Offset.

  @return       Pointer to the Offset'th byte of data in the net buffer, or NULL
                if there is no such data in the net buffer.

**/
UINT8  *
EFIAPI
NetbufGetByte (
  IN  NET_BUF               *Nbuf,
  IN  UINT32                Offset,
  OUT UINT32                *Index  OPTIONAL
  );



/**
  Set the NET_BLOCK and corresponding NET_BLOCK_OP in the net buffer and
  corresponding net vector according to the bulk pointer and bulk length.

  All the pointers in the Index'th NET_BLOCK and NET_BLOCK_OP are set to the
  bulk's head and tail respectively. So, this function alone can't be used by
  NetbufAlloc.

  @param[in, out]  Nbuf       Pointer to the net buffer.
  @param[in]       Bulk       Pointer to the data.
  @param[in]       Len        Length of the bulk data.
  @param[in]       Index      The data block index in the net buffer the bulk
                              data should belong to.

**/
VOID
NetbufSetBlock (
  IN OUT NET_BUF            *Nbuf,
  IN UINT8                  *Bulk,
  IN UINT32                 Len,
  IN UINT32                 Index
  );



/**
  Set the NET_BLOCK_OP in the net buffer. The corresponding NET_BLOCK
  structure is left untouched.

  Some times, there is no 1:1 relationship between NET_BLOCK and NET_BLOCK_OP.
  For example, that in NetbufGetFragment.

  @param[in, out]  Nbuf       Pointer to the net buffer.
  @param[in]       Bulk       Pointer to the data.
  @param[in]       Len        Length of the bulk data.
  @param[in]       Index      The data block index in the net buffer the bulk
                              data should belong to.

**/
VOID
NetbufSetBlockOp (
  IN OUT NET_BUF            *Nbuf,
  IN UINT8                  *Bulk,
  IN UINT32                 Len,
  IN UINT32                 Index
  );
/**
  Helper function for NetbufGetFragment. NetbufGetFragment may allocate the
  first block to reserve HeadSpace bytes header space. So it needs to create a
  new net vector for the first block and can avoid copy for the remaining data
  by sharing the old net vector.

  @param[in]  Arg                   Point to the old NET_VECTOR.

**/
VOID
EFIAPI
NetbufGetFragmentFree (
  IN VOID                   *Arg
  );

/**
  Create a NET_BUF structure which contains Len byte data of Nbuf starting from
  Offset.

  A new NET_BUF structure will be created but the associated data in NET_VECTOR
  is shared. This function exists to do IP packet fragmentation.

  @param[in]  Nbuf         Pointer to the net buffer to be extracted.
  @param[in]  Offset       Starting point of the data to be included in the new
                           net buffer.
  @param[in]  Len          Bytes of data to be included in the new net buffer.
  @param[in]  HeadSpace    Bytes of head space to reserve for protocol header.

  @return                  Pointer to the cloned net buffer, or NULL if the
                           allocation failed due to resource limit.

**/
NET_BUF  *
EFIAPI
NetbufGetFragment (
  IN NET_BUF                *Nbuf,
  IN UINT32                 Offset,
  IN UINT32                 Len,
  IN UINT32                 HeadSpace
  );


NET_BUF  *
EFIAPI
NetbufFromExt (
  IN NET_FRAGMENT           *ExtFragment,
  IN UINT32                 ExtNum,
  IN UINT32                 HeadSpace,
  IN UINT32                 HeadLen,
  IN NET_VECTOR_EXT_FREE    ExtFree,
  IN VOID                   *Arg          OPTIONAL
  );


EFI_STATUS
EFIAPI
NetbufBuildExt (
  IN NET_BUF                *Nbuf,
  IN OUT NET_FRAGMENT       *ExtFragment,
  IN OUT UINT32             *ExtNum
  );


/**
  Build a net buffer from a list of net buffers.

  All the fragments will be collected from the list of NEW_BUF and then a new
  net buffer will be created through NetbufFromExt.

  @param[in]   BufList    A List of the net buffer.
  @param[in]   HeadSpace  The head space to be reserved.
  @param[in]   HeaderLen  The length of the protocol header, This function
                          will pull that number of data into a linear block.
  @param[in]   ExtFree    Pointer to the caller provided free function.
  @param[in]   Arg        The argument passed to ExtFree when ExtFree is called.

  @return                 Pointer to the net buffer built from the list of net
                          buffers.

**/
NET_BUF  *
EFIAPI
NetbufFromBufList (
  IN LIST_ENTRY             *BufList,
  IN UINT32                 HeadSpace,
  IN UINT32                 HeaderLen,
  IN NET_VECTOR_EXT_FREE    ExtFree,
  IN VOID                   *Arg              OPTIONAL
  );


/**
  Reserve some space in the header room of the net buffer.

  Upon allocation, all the space are in the tail room of the buffer. Call this
  function to move some space to the header room. This function is quite limited
  in that it can only reserve space from the first block of an empty NET_BUF not
  built from the external. But it should be enough for the network stack.

  @param[in, out]  Nbuf     Pointer to the net buffer.
  @param[in]       Len      The length of buffer to be reserved from the header.

**/
VOID
EFIAPI
NetbufReserve (
  IN OUT NET_BUF            *Nbuf,
  IN UINT32                 Len
  );


/**
  Allocate Len bytes of space from the header or tail of the buffer.

  @param[in, out]  Nbuf       Pointer to the net buffer.
  @param[in]       Len        The length of the buffer to be allocated.
  @param[in]       FromHead   The flag to indicate whether reserve the data
                              from head (TRUE) or tail (FALSE).

  @return                     Pointer to the first byte of the allocated buffer,
                              or NULL if there is no sufficient space.

**/
UINT8*
EFIAPI
NetbufAllocSpace (
  IN OUT NET_BUF            *Nbuf,
  IN UINT32                 Len,
  IN BOOLEAN                FromHead
  );


/**
  Trim a single NET_BLOCK by Len bytes from the header or tail.

  @param[in, out]  BlockOp      Pointer to the NET_BLOCK.
  @param[in]       Len          The length of the data to be trimmed.
  @param[in]       FromHead     The flag to indicate whether trim data from head
                                (TRUE) or tail (FALSE).

**/
VOID
NetblockTrim (
  IN OUT NET_BLOCK_OP       *BlockOp,
  IN UINT32                 Len,
  IN BOOLEAN                FromHead
  );


/**
  Trim Len bytes from the header or tail of the net buffer.

  @param[in, out]  Nbuf         Pointer to the net buffer.
  @param[in]       Len          The length of the data to be trimmed.
  @param[in]      FromHead      The flag to indicate whether trim data from head
                                (TRUE) or tail (FALSE).

  @return    Length of the actually trimmed data, which is possible to be less
             than Len because the TotalSize of Nbuf is less than Len.

**/
UINT32
EFIAPI
NetbufTrim (
  IN OUT NET_BUF            *Nbuf,
  IN UINT32                 Len,
  IN BOOLEAN                FromHead
  );


/**
  Copy Len bytes of data from the specific offset of the net buffer to the
  destination memory.

  The Len bytes of data may cross the several fragments of the net buffer.

  @param[in]   Nbuf         Pointer to the net buffer.
  @param[in]   Offset       The sequence number of the first byte to copy.
  @param[in]   Len          Length of the data to copy.
  @param[in]   Dest         The destination of the data to copy to.

  @return           The length of the actual copied data, or 0 if the offset
                    specified exceeds the total size of net buffer.

**/
UINT32
EFIAPI
NetbufCopy (
  IN NET_BUF                *Nbuf,
  IN UINT32                 Offset,
  IN UINT32                 Len,
  IN UINT8                  *Dest
  );


/**
  Initiate the net buffer queue.

  @param[in, out]  NbufQue   Pointer to the net buffer queue to be initialized.

**/
VOID
EFIAPI
NetbufQueInit (
  IN OUT NET_BUF_QUEUE          *NbufQue
  );


/**
  Allocate and initialize a net buffer queue.

  @return         Pointer to the allocated net buffer queue, or NULL if the
                  allocation failed due to resource limit.

**/
NET_BUF_QUEUE  *
EFIAPI
NetbufQueAlloc (
  VOID
  );


/**
  Free a net buffer queue.

  Decrease the reference count of the net buffer queue by one. The real resource
  free operation isn't performed until the reference count of the net buffer
  queue is decreased to 0.

  @param[in]  NbufQue               Pointer to the net buffer queue to be freed.

**/
VOID
EFIAPI
NetbufQueFree (
  IN NET_BUF_QUEUE          *NbufQue
  );


/**
  Append a net buffer to the net buffer queue.

  @param[in, out]  NbufQue            Pointer to the net buffer queue.
  @param[in, out]  Nbuf               Pointer to the net buffer to be appended.

**/
VOID
EFIAPI
NetbufQueAppend (
  IN OUT NET_BUF_QUEUE          *NbufQue,
  IN OUT NET_BUF                *Nbuf
  );

/**
  Remove a net buffer from the head in the specific queue and return it.

  @param[in, out]  NbufQue               Pointer to the net buffer queue.

  @return           Pointer to the net buffer removed from the specific queue,
                    or NULL if there is no net buffer in the specific queue.

**/
NET_BUF  *
EFIAPI
NetbufQueRemove (
  IN OUT NET_BUF_QUEUE          *NbufQue
  );


/**
  Copy Len bytes of data from the net buffer queue at the specific offset to the
  destination memory.

  The copying operation is the same as NetbufCopy but applies to the net buffer
  queue instead of the net buffer.

  @param[in]   NbufQue         Pointer to the net buffer queue.
  @param[in]   Offset          The sequence number of the first byte to copy.
  @param[in]   Len             Length of the data to copy.
  @param[out]  Dest            The destination of the data to copy to.

  @return       The length of the actual copied data, or 0 if the offset
                specified exceeds the total size of net buffer queue.

**/
UINT32
EFIAPI
NetbufQueCopy (
  IN NET_BUF_QUEUE          *NbufQue,
  IN UINT32                 Offset,
  IN UINT32                 Len,
  OUT UINT8                 *Dest
  );


/**
  Trim Len bytes of data from the buffer queue and free any net buffer
  that is completely trimmed.

  The trimming operation is the same as NetbufTrim but applies to the net buffer
  queue instead of the net buffer.

  @param[in, out]  NbufQue               Pointer to the net buffer queue.
  @param[in]       Len                   Length of the data to trim.

  @return   The actual length of the data trimmed.

**/
UINT32
EFIAPI
NetbufQueTrim (
  IN OUT NET_BUF_QUEUE      *NbufQue,
  IN UINT32                 Len
  );


/**
  Flush the net buffer queue.

  @param[in, out]  NbufQue               Pointer to the queue to be flushed.

**/
VOID
EFIAPI
NetbufQueFlush (
  IN OUT NET_BUF_QUEUE          *NbufQue
  );


/**
  Compute the checksum for a bulk of data.

  @param[in]   Bulk                  Pointer to the data.
  @param[in]   Len                   Length of the data, in bytes.

  @return    The computed checksum.

**/
UINT16
EFIAPI
NetblockChecksum (
  IN UINT8                  *Bulk,
  IN UINT32                 Len
  );


/**
  Add two checksums.

  @param[in]   Checksum1             The first checksum to be added.
  @param[in]   Checksum2             The second checksum to be added.

  @return         The new checksum.

**/
UINT16
EFIAPI
NetAddChecksum (
  IN UINT16                 Checksum1,
  IN UINT16                 Checksum2
  );


/**
  Compute the checksum for a NET_BUF.

  @param[in]   Nbuf                  Pointer to the net buffer.

  @return    The computed checksum.

**/
UINT16
EFIAPI
NetbufChecksum (
  IN NET_BUF                *Nbuf
  );


/**
  Compute the checksum for TCP/UDP pseudo header.

  Src and Dst are in network byte order, and Len is in host byte order.

  @param[in]   Src                   The source address of the packet.
  @param[in]   Dst                   The destination address of the packet.
  @param[in]   Proto                 The protocol type of the packet.
  @param[in]   Len                   The length of the packet.

  @return   The computed checksum.

**/
UINT16
EFIAPI
NetPseudoHeadChecksum (
  IN IP4_ADDR               Src,
  IN IP4_ADDR               Dst,
  IN UINT8                  Proto,
  IN UINT16                 Len
  );
/**
  Compute the checksum for TCP6/UDP6 pseudo header.

  Src and Dst are in network byte order, and Len is in host byte order.

  @param[in]   Src                   The source address of the packet.
  @param[in]   Dst                   The destination address of the packet.
  @param[in]   NextHeader            The protocol type of the packet.
  @param[in]   Len                   The length of the packet.

  @return   The computed checksum.

**/
UINT16
EFIAPI
NetIp6PseudoHeadChecksum (
  IN EFI_IPv6_ADDRESS       *Src,
  IN EFI_IPv6_ADDRESS       *Dst,
  IN UINT8                  NextHeader,
  IN UINT32                 Len
  );

/**
  The function frees the net buffer which allocated by the IP protocol. It releases
  only the net buffer and doesn't call the external free function.

  This function should be called after finishing the process of mIpSec->ProcessExt()
  for outbound traffic. The (EFI_IPSEC2_PROTOCOL)->ProcessExt() allocates a new
  buffer for the ESP, so there needs a function to free the old net buffer.

  @param[in]  Nbuf       The network buffer to be freed.

**/
VOID
NetIpSecNetbufFree (
  NET_BUF   *Nbuf
  );

