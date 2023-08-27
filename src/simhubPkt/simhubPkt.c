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

void simhubPktInitBuffer(SimhubPktBuffer *pktBuf)
{
  pktBuf->head = pktBuf->buffer;
  pktBuf->tail = pktBuf->buffer;
}

/** @} */
