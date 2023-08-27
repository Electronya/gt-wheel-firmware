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
{
  pktBuf->head = pktBuf->buffer;
  pktBuf->tail = pktBuf->buffer;
}

/** @} */
