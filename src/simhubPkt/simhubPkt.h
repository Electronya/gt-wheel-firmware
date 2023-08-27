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
 * @brief The SIMHUB Rx packet buffer size.
*/
#define SIMHUB_RX_PKT_BUF_SIZE        128

/**
 * @brief The SIMHUB Tx packet buffer size.
*/
#define SIMHUB_TX_PKT_BUF_SIZE        24

typedef struct
{
  size_t size;                  /**< The buffer size. */
  uint32_t head;                /**< The buffer head. */
  uint32_t tail;                /**< The buffer tail. */
  uint8_t *buffer;              /**< The actual buffer. */
} SimhubPktBuffer;

/**
 * @brief   Initialize a SIMHUB packet buffer.
 *
 * @param pktBuf  The packet buffer to initialize.
 * @param buffer  The byte buffer.
 * @param size    The size of the byte buffer.
 */
void simhubPktInitBuffer(SimhubPktBuffer *pktBuf, uint8_t *buffer, size_t size);

/**
 * @brief   Get the free space in a packet buffer.
 *
 * @param pktBuf  The packet buffer.
 *
 * @return  The number of free bytes in the packet buffer.
 */
void simhubPktInitBuffer(SimhubPktBuffer *pktBuf);

#endif    /* SIMHUB_PACKET */

/** @} */
