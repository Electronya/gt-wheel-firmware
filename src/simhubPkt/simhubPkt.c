/**
 * Copyright (C) 2023 by Electronya
 *
 * @file      simhubPkt.c
 * @author    jbacon
 * @date      2023-08-27
 * @brief     SIMHUB Packet Module
 *
 *            This file is the implementation of the SIMHUB packet module.
 *
 * @ingroup  simhubPkt
 *
 * @{
 */

#include <zephyr/logging/log.h>

#include "simhubPkt.h"

#include "zephyrRingBuffer.h"

/**
 * @brief ACM device module name.
*/
#define SIMHUB_PKT_MODULE_NAME  simhub_packet_module

/* Setting module logging */
LOG_MODULE_DECLARE(SIMHUB_PKT_MODULE_NAME);

/**
 * @brief The Rx ring buffer data area.
*/
uint8_t rxBufData[SIMHUB_RX_PKT_BUF_SIZE];

/**
 * @brief The Rx ring buffer.
*/
ZephyrRingBuffer rxBuffer;

/**
 * @brief The Tx ring buffer data area.
*/
uint8_t txBufData[SIMHUB_TX_PKT_BUF_SIZE];

/**
 * @brief The Tx ring buffer.
*/
ZephyrRingBuffer txBuffer;
{
  pktBuf->buffer = buffer;
  pktBuf->size = size;
  pktBuf->head = 0;
  pktBuf->tail = 0;
}

bool simhubPktIsBufferEmpty(SimhubPktBuffer *pktBuf)
{
  return pktBuf->head == pktBuf->tail;
}

size_t simhubPktGetBufferSize(SimhubPktBuffer *pktBuf)
{
  return pktBuf->size;
}

void simhubPktInitBuffer(void)
{
  zephyrRingBufInit(&rxBuffer, SIMHUB_RX_PKT_BUF_SIZE, rxBufData);
  zephyrRingBufInit(&txBuffer, SIMHUB_TX_PKT_BUF_SIZE, txBufData);
}

bool simhubPktIsPktAvailable(SimhubPktTypes *pktType)
{
  return false;
}

/** @} */
