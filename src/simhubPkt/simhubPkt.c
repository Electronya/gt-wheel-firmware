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
#include "ledCtrl.h"
#include "zephyrRingBuffer.h"

/**
 * @brief ACM device module name.
*/
#define SIMHUB_PKT_MODULE_NAME  simhub_packet_module

/* Setting module logging */
LOG_MODULE_DECLARE(SIMHUB_PKT_MODULE_NAME);

/**
 * @brief The size of the packet header.
*/
#define SIMHUB_PKT_HEADER_SIZE        6

/**
 * @brief The size of the upload unlock payload.
*/
#define SIMHUB_UNLOCK_PLD_SIZE        6

/**
 * @brief The size of the protocol version payload.
*/
#define SIMHUB_PROTO_PLD_SIZE         5

/**
 * @brief The size of the get LED count payload.
*/
#define SIMHUB_LED_CNT_PLD_SIZE       5

/**
 * @brief The size LED data packet footer.
*/
#define SIMHUB_LED_DATA_FOOTER_SIZE   3

/**
 * @brief The packet header.
*/
uint8_t pktHeader[SIMHUB_PKT_HEADER_SIZE] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

/**
 * @brief The unlock payload content.
*/
uint8_t unlockPld[SIMHUB_UNLOCK_PLD_SIZE] = {'u', 'n', 'l', 'o', 'c', 'k'};

/**
 * @brief The protocol payload content.
*/
uint8_t protoPld[SIMHUB_PROTO_PLD_SIZE] = {'p', 'r', 'o', 't', 'o'};

/**
 * @brief The LED count payload content.
*/
uint8_t ledCntPld[SIMHUB_LED_CNT_PLD_SIZE] = {'l', 'e', 'd', 's', 'c'};

/**
 * @brief The LED data packet footer.
*/
uint8_t ledDataFooter[SIMHUB_LED_DATA_FOOTER_SIZE] = {0xff, 0xfe, 0xfd};

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

/**
 * @brief   Check if the packet is of the unlock upload type.
 *
 * @param pktData The packet data.
 * @param size    The size of the packet data.
 *
 * @return  True if the packet is of unlock upload type, false otherwise.
 */
bool IsPacketUnlockType(uint8_t *pktBuf, size_t size)
{
  bool pktIsMatch = true;
  uint8_t head = SIMHUB_PKT_HEADER_SIZE;

  if( size < SIMHUB_PKT_HEADER_SIZE + SIMHUB_UNLOCK_PLD_SIZE)
    return false;

  while(pktIsMatch && head < size)
  {
    if(pktBuf[head] != unlockPld[head - SIMHUB_PKT_HEADER_SIZE])
      pktIsMatch = false;
    ++head;
  }

  return pktIsMatch;
}

/**
 * @brief   Check if the packet is of the protocol version type.
 *
 * @param pktData The packet data.
 * @param size    The size of the packet data.
 *
 * @return  True if the packet is of protocol version type, false otherwise.
 */
bool IsPacketProtoType(uint8_t *pktBuf, size_t size)
{
  bool pktIsMatch = true;
  uint8_t head = SIMHUB_PKT_HEADER_SIZE;

  if( size < SIMHUB_PKT_HEADER_SIZE + SIMHUB_PROTO_PLD_SIZE)
    return false;

  while(pktIsMatch && head < size)
  {
    if(pktBuf[head] != protoPld[head - SIMHUB_PKT_HEADER_SIZE])
      pktIsMatch = false;
    ++head;
  }

  return pktIsMatch;
}

/**
 * @brief   Check if the packet is of the LED count type.
 *
 * @param pktData The packet data.
 * @param size    The size of the packet data.
 *
 * @return  True if the packet is of LED count type, false otherwise.
 */
bool IsPacketLedCountType(uint8_t *pktBuf, size_t size)
{
  bool pktIsMatch = true;
  uint8_t head = SIMHUB_PKT_HEADER_SIZE;

  if( size < SIMHUB_PKT_HEADER_SIZE + SIMHUB_LED_CNT_PLD_SIZE)
    return false;

  while(pktIsMatch && head < size)
  {
    if(pktBuf[head] != ledCntPld[head - SIMHUB_PKT_HEADER_SIZE])
      pktIsMatch = false;
    ++head;
  }

  return pktIsMatch;
}

/**
 * @brief   Check if the packet is of the LED data type.
 *
 * @param pktData The packet data.
 * @param size    The size of the packet data.
 *
 * @return  True if the packet is of LED data type, false otherwise.
 */
bool IsPacketLedDataType(uint8_t *pktBuf, size_t size)
{
  bool pktIsMatch = true;
  uint8_t head;
  uint32_t ledDataPldSize = ledCtrlGetRpmChaserPxlCnt() * 3;
  uint32_t footerHead = SIMHUB_PKT_HEADER_SIZE + ledDataPldSize;
  uint32_t pktSize = footerHead + SIMHUB_LED_DATA_FOOTER_SIZE;

  if(size < pktSize)
    return false;

  head = SIMHUB_PKT_HEADER_SIZE + ledDataPldSize;

  while(pktIsMatch && head < size)
  {
    if(pktBuf[head] != ledDataFooter[head - footerHead])
      pktIsMatch = false;
    ++head;
  }

  return pktIsMatch;
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
