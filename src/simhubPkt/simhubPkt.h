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
 * @brief   Check if a new packet is available.
 *
 * @param pktType The type of the next available packet.
 *
 * @return  True if a new packet is available, false otherwise.
 */
bool simhubPktIsPktAvailable(SimhubPktTypes *pktType);

#endif    /* SIMHUB_PACKET */

/** @} */
