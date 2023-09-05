/**
 * Copyright (C) 2023 by Electronya
 *
 * @file      simhubPkt.h
 * @author    jbacon
 * @date      2023-08-27
 * @brief     SIMHUB Packet Module
 *
 *            This file is the declaration of the SIMHUB packet module.
 *
 * @defgroup  simhubPkt simhubPkt
 *
 * @{
 */

#ifndef SIMHUB_PACKET
#define SIMHUB_PACKET

#include <zephyr/kernel.h>

/**
 * @brief The Rx packet buffer size.
*/
#define SIMHUB_RX_PKT_BUF_SIZE        128

/**
 * @brief The Tx packet buffer size.
*/
#define SIMHUB_TX_PKT_BUF_SIZE        24

/**
 * @brief The SIMHUB packet types.
*/
typedef enum
{
  UNLOCK_TYPE,                  /**< The unlock upload packet type. */
  PROTO_TYPE,                   /**< The protocol version packet type. */
  LED_COUNT_TYPE,               /**< The get LED count packet type. */
  LED_DATA_TYPE,                /**< The LED data update packet type. */
  PKT_TYPE_COUNT,               /**< The number of packet type. */
} SimhubPktTypes;

/**
 * @brief   Initialize a SIMHUB packet buffer.
 */
void simhubPktInitBuffer(void);

/**
 * @brief   Claim the packet buffer for putting data.
 *
 * @param data  The data area of the packet buffer claimed.
 * @param size  The size of the data claimed.
 *
 * @return  The size that was really claimed from the packet buffer. Which may
 *          be smaller than the requested size.
 */
size_t simhubPktBufClaimPutting(uint8_t **data, size_t size);

/**
 * @brief   Finish putting in packet packet buffer after claiming it.
 *
 * @param size  The size of data put inside the packet buffer.
 *
 * @return  0 if successful, the error code otherwise.
 */
int simhubPktBufFinishPutting(size_t size);

/**
 * @brief   Claim th packet buffer for getting data.
 *
 * @param data  The data area packet packet buffer for getting data.
 * @param size  The size of the area claimed.
 *
 * @return  The size that was really claimed from the packet buffer. Which may
 *          be smaller than the requested size.
 */
size_t simhubPktBufClaimGetting(uint8_t **data, size_t size);

/**
 * @brief   Finish getting from the packet buffer after claiming it.
 *
 * @param size  The size of that data got from the packet buffer.
 * @return int
 */
int simhubPktBufFinishGetting(size_t size);

/**
 * @brief   Check if a new packet is available.
 *
 * @param pktType The type of the next available packet.
 *
 * @return  True if a new packet is available, false otherwise.
 */
bool simhubPktIsPktAvailable(SimhubPktTypes *pktType);

/**
 * @brief   Process the unlock update packet.
 *
 * @return  0 if successful, the error code otherwise.
 */
int simhubPktProcessUnlock(void);

/**
 * @brief   Process the protocol packet.
 *
 * @return  0 if successful, the error code otherwise.
 */
int simhubPktProcessProto(void);

#endif    /* SIMHUB_PACKET */

/** @} */
