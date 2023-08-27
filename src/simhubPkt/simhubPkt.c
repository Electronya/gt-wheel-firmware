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

#include <stdlib.h>

/**
 * @brief ACM device module name.
*/
#define SIMHUB_PKT_MODULE_NAME  simhub_packet_module

/* Setting module logging */
LOG_MODULE_DECLARE(SIMHUB_PKT_MODULE_NAME);

void simhubPktInitBuffer(SimhubPktBuffer *pktBuf, uint8_t *buffer, size_t size)
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

size_t simhubPktGetBufferFreeSpace(SimhubPktBuffer *pktBuf)
{
  return pktBuf->size - abs(pktBuf->head - pktBuf->tail);
}

size_t simhubPktGetBufferUsedSpace(SimhubPktBuffer *pktBuf)
{
  return abs(pktBuf->head - pktBuf->tail);
}

/** @} */
