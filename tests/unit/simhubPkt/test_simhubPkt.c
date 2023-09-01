/**
 * Copyright (C) 2023 by Electronya
 *
 * @file      test_simhubPkt.c
 * @author    jbacon
 * @date      2023-07-19
 * @brief     ACM device Module Test Cases
 *
 *            This file is the test cases of the SIMHUB packet module.
 *
 * @ingroup  simhubPkt
 *
 * @{
 */

#include <zephyr/ztest.h>
#include <zephyr/fff.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

#include "simhubPkt.h"
#include "simhubPkt.c"

#include "ledCtrl.h"
#include "zephyrRingBuffer.h"

DEFINE_FFF_GLOBALS;

/* mocks */
FAKE_VOID_FUNC(zephyrRingBufInit, ZephyrRingBuffer*, size_t, uint8_t*);

FAKE_VALUE_FUNC(uint32_t, ledCtrlGetRpmChaserPxlCnt);

/**
 * @brief The test buffer size.
*/
#define TEST_BUFFER_SIZE          128

/**
 * @brief The test LED pixel count.
*/
#define TEST_LED_PIXEL_COUNT      8

/**
 * @brief The LED data payload size.
*/
#define TEST_LED_DATA_PLD_SIZE    (TEST_LED_PIXEL_COUNT * 3)

/**
 * @brief The test fixture.
*/
struct simhubPkt_suite_fixture
{
  uint8_t buffer[TEST_BUFFER_SIZE];
};

static void *simhubPktSuiteSetup(void)
{
  struct simhubPkt_suite_fixture *fixture =
    k_malloc(sizeof(struct simhubPkt_suite_fixture));
  zassume_not_null(fixture, NULL);

  return (void *)fixture;
}

static void simhubPktSuiteTeardown(void *f)
{
  k_free(f);
}

static void simhubPktCaseSetup(void *f)
{
  struct simhubPkt_suite_fixture *fixture =
    (struct simhubPkt_suite_fixture *)f;

  for(uint8_t i = 0; i < TEST_BUFFER_SIZE; ++i)
    fixture->buffer[i] = 0;

  RESET_FAKE(zephyrRingBufInit);
  RESET_FAKE(ledCtrlGetRpmChaserPxlCnt);
}

ZTEST_SUITE(simhubPkt_suite, NULL, simhubPktSuiteSetup, simhubPktCaseSetup,
  NULL, simhubPktSuiteTeardown);

typedef size_t (*SetupPktFunction)(uint8_t *);

/**
 * @brief   Setup an incomplete unlock upload packet.
 *
 * @param pktBuf  The packet buffer to setup.
 */
static size_t setupIncompleteUnlockPkt(uint8_t *pktBuf)
{
  uint8_t pktSize = SIMHUB_PKT_HEADER_SIZE + SIMHUB_UNLOCK_PLD_SIZE;
  uint32_t head = 0;

  while(head < pktSize - 1)
  {
    if(head < SIMHUB_PKT_HEADER_SIZE)
      pktBuf[head] = pktHeader[head];
    else
      pktBuf[head] = unlockPld[head - SIMHUB_PKT_HEADER_SIZE];
    ++head;
  }

  return pktSize;
}

/**
 * @brief   Setup a complete unlock upload packet.
 *
 * @param pktBuf  The packet buffer to setup.
 */
static size_t setupCompleteUnlockPkt(uint8_t *pktBuf)
{
  uint8_t pktSize = SIMHUB_PKT_HEADER_SIZE + SIMHUB_UNLOCK_PLD_SIZE;
  uint32_t head = 0;

  while(head < pktSize)
  {
    if(head < SIMHUB_PKT_HEADER_SIZE)
      pktBuf[head] = pktHeader[head];
    else
      pktBuf[head] = unlockPld[head - SIMHUB_PKT_HEADER_SIZE];
    ++head;
  }

  return pktSize;
}

/**
 * @brief   Setup an incomplete protocol version packet.
 *
 * @param pktBuf  The packet buffer to setup.
 */
static size_t setupIncompleteProtocolPkt(uint8_t *pktBuf)
{
  uint8_t pktSize = SIMHUB_PKT_HEADER_SIZE + SIMHUB_UNLOCK_PLD_SIZE;
  uint32_t head = 0;

  while(head < pktSize - 1)
  {
    if(head < SIMHUB_PKT_HEADER_SIZE)
      pktBuf[head] = pktHeader[head];
    else
      pktBuf[head] = protoPld[head - SIMHUB_PKT_HEADER_SIZE];
    ++head;
  }

  return pktSize;
}

/**
 * @brief   Setup a complete protocol version packet.
 *
 * @param pktBuf  The packet buffer to setup.
 */
static size_t setupCompleteProtocolPkt(uint8_t *pktBuf)
{
  uint8_t pktSize = SIMHUB_PKT_HEADER_SIZE + SIMHUB_UNLOCK_PLD_SIZE;
  uint32_t head = 0;

  while(head < pktSize)
  {
    if(head < SIMHUB_PKT_HEADER_SIZE)
      pktBuf[head] = pktHeader[head];
    else
      pktBuf[head] = protoPld[head - SIMHUB_PKT_HEADER_SIZE];
    ++head;
  }

  return pktSize;
}

/**
 * @brief   Setup an incomplete led count packet.
 *
 * @param pktBuf  The packet buffer to setup.
 */
static size_t setupIncompleteLedCountPkt(uint8_t *pktBuf)
{
  uint8_t pktSize = SIMHUB_PKT_HEADER_SIZE + SIMHUB_UNLOCK_PLD_SIZE;
  uint32_t head = 0;

  while(head < pktSize - 1)
  {
    if(head < SIMHUB_PKT_HEADER_SIZE)
      pktBuf[head] = pktHeader[head];
    else
      pktBuf[head] = ledCntPld[head - SIMHUB_PKT_HEADER_SIZE];
    ++head;
  }

  return pktSize;
}

/**
 * @brief   Setup a complete led count packet.
 *
 * @param pktBuf  The packet buffer to setup.
 */
static size_t setupCompleteLedCountPkt(uint8_t *pktBuf)
{
  uint8_t pktSize = SIMHUB_PKT_HEADER_SIZE + SIMHUB_UNLOCK_PLD_SIZE;
  uint32_t head = 0;

  while(head < pktSize)
  {
    if(head < SIMHUB_PKT_HEADER_SIZE)
      pktBuf[head] = pktHeader[head];
    else
      pktBuf[head] = ledCntPld[head - SIMHUB_PKT_HEADER_SIZE];
    ++head;
  }

  return pktSize;
}

/**
 * @brief   Setup an incomplete led Data packet.
 *
 * @param pktBuf  The packet buffer to setup.
 */
static size_t setupIncompleteLedDataPkt(uint8_t *pktBuf)
{
  uint8_t footerStart = SIMHUB_PKT_HEADER_SIZE + TEST_LED_DATA_PLD_SIZE;
  uint8_t pktSize = SIMHUB_PKT_HEADER_SIZE + TEST_LED_DATA_PLD_SIZE +
    SIMHUB_LED_DATA_FOOTER_SIZE - 1;
  uint32_t head = 0;

  while(head < pktSize - 1)
  {
    if(head < SIMHUB_PKT_HEADER_SIZE)
      pktBuf[head] = pktHeader[head];
    else if(head < footerStart)
      pktBuf[head] = head;
    else
      pktBuf[head] = ledDataFooter[head - footerStart];
    ++head;
  }

  return pktSize;
}

/**
 * @brief   Setup a complete led Data packet.
 *
 * @param pktBuf  The packet buffer to setup.
 */
static size_t setupCompleteLedDataPkt(uint8_t *pktBuf)
{
  uint8_t footerStart = SIMHUB_PKT_HEADER_SIZE + TEST_LED_DATA_PLD_SIZE;
  uint8_t pktSize = SIMHUB_PKT_HEADER_SIZE + TEST_LED_DATA_PLD_SIZE +
    SIMHUB_LED_DATA_FOOTER_SIZE;
  uint32_t head = 0;

  while(head < pktSize)
  {
    if(head < SIMHUB_PKT_HEADER_SIZE)
      pktBuf[head] = pktHeader[head];
    else if(head < footerStart)
      pktBuf[head] = head;
    else
      pktBuf[head] = ledDataFooter[head - footerStart];
    ++head;
  }

  return pktSize;
}

/**
 * @test  IsPacketUnlockType must return true only when the packet is complete
 *        and of the unlock upload type.
*/
ZTEST_F(simhubPkt_suite, test_IsPacketUnlockType_IsGoodPktType)
{
  bool result;
  size_t pktSize;
  SetupPktFunction setupFunctions[PKT_TYPE_COUNT * 2] =
    {setupIncompleteUnlockPkt, setupCompleteUnlockPkt,
     setupIncompleteProtocolPkt, setupCompleteProtocolPkt,
     setupIncompleteLedCountPkt, setupCompleteLedCountPkt,
     setupIncompleteLedDataPkt, setupCompleteLedDataPkt};
  bool expectedResults[PKT_TYPE_COUNT * 2] = {false, true, false, false,
                                              false, false, false, false};

  for(uint8_t i = 0; i < PKT_TYPE_COUNT * 2; ++i)
  {
    pktSize = setupFunctions[i](fixture->buffer);

    result = IsPacketUnlockType(fixture->buffer, pktSize);
    zassert_equal(expectedResults[i], result);
  }
}

/**
 * @test  IsPacketProtoType must return true only when the packet is complete
 *        and of the protocol version type.
*/
ZTEST_F(simhubPkt_suite, test_IsPacketProtoType_IsGoodPktType)
{
  bool result;
  size_t pktSize;
  SetupPktFunction setupFunctions[PKT_TYPE_COUNT * 2] =
    {setupIncompleteUnlockPkt, setupCompleteUnlockPkt,
     setupIncompleteProtocolPkt, setupCompleteProtocolPkt,
     setupIncompleteLedCountPkt, setupCompleteLedCountPkt,
     setupIncompleteLedDataPkt, setupCompleteLedDataPkt};
  bool expectedResults[PKT_TYPE_COUNT * 2] = {false, false, false, true,
                                              false, false, false, false};

  for(uint8_t i = 0; i < PKT_TYPE_COUNT * 2; ++i)
  {
    pktSize = setupFunctions[i](fixture->buffer);

    result = IsPacketProtoType(fixture->buffer, pktSize);
    zassert_equal(expectedResults[i], result);
  }
}

/**
 * @test  IsPacketLedCountType must return true only when the packet is complete
 *        and of the LED count type.
*/
ZTEST_F(simhubPkt_suite, test_IsPacketLedCountType_IsGoodPktType)
{
  bool result;
  size_t pktSize;
  SetupPktFunction setupFunctions[PKT_TYPE_COUNT * 2] =
    {setupIncompleteUnlockPkt, setupCompleteUnlockPkt,
     setupIncompleteProtocolPkt, setupCompleteProtocolPkt,
     setupIncompleteLedCountPkt, setupCompleteLedCountPkt,
     setupIncompleteLedDataPkt, setupCompleteLedDataPkt};
  bool expectedResults[PKT_TYPE_COUNT * 2] = {false, false, false, false,
                                              false, true, false, false};

  for(uint8_t i = 0; i < PKT_TYPE_COUNT * 2; ++i)
  {
    pktSize = setupFunctions[i](fixture->buffer);

    result = IsPacketLedCountType(fixture->buffer, pktSize);
    zassert_equal(expectedResults[i], result);
  }
}

/**
 * @test  IsPacketLedDataType must return true only when the packet is complete
 *        and of the LED data type.
*/
ZTEST_F(simhubPkt_suite, test_IsPacketLedDataType_IsGoodPktType)
{
  bool result;
  size_t pktSize;
  SetupPktFunction setupFunctions[PKT_TYPE_COUNT * 2] =
    {setupIncompleteUnlockPkt, setupCompleteUnlockPkt,
     setupIncompleteProtocolPkt, setupCompleteProtocolPkt,
     setupIncompleteLedCountPkt, setupCompleteLedCountPkt,
     setupIncompleteLedDataPkt, setupCompleteLedDataPkt};
  bool expectedResults[PKT_TYPE_COUNT * 2] = {false, false, false, false,
                                              false, false, false, true};

  for(uint8_t i = 0; i < PKT_TYPE_COUNT * 2; ++i)
  {
    ledCtrlGetRpmChaserPxlCnt_fake.return_val = TEST_LED_PIXEL_COUNT;
    pktSize = setupFunctions[i](fixture->buffer);

    result = IsPacketLedDataType(fixture->buffer, pktSize);
    zassert_equal(expectedResults[i], result);
    RESET_FAKE(ledCtrlGetRpmChaserPxlCnt);
  }
}

/**
 * @test  simhubPktInitBuffer must initialize the Rx and Tx ring
 *        internal buffer.
*/
ZTEST(simhubPkt_suite, test_simhubPktInitBuffer_InitRingBuffers)
{
  simhubPktInitBuffer();

  zassert_equal(2, zephyrRingBufInit_fake.call_count);
  zassert_equal(&rxBuffer, zephyrRingBufInit_fake.arg0_history[0]);
  zassert_equal(128, zephyrRingBufInit_fake.arg1_history[0]);
  zassert_equal(rxBufData, zephyrRingBufInit_fake.arg2_history[0]);
  zassert_equal(&txBuffer, zephyrRingBufInit_fake.arg0_history[1]);
  zassert_equal(24, zephyrRingBufInit_fake.arg1_history[1]);
  zassert_equal(txBufData, zephyrRingBufInit_fake.arg2_history[1]);
}

#define NO_AVAIL_PKT_TEST_CNT   2
/**
 * @test  simhubPktIsPktAvailable must return false if there is no new
 *        available packet in the buffer.
*/
ZTEST_F(simhubPkt_suite, test_simhubPktIsPktAvailable_NoAvailable)
{
  // for(uint8_t i = 0; i < NO_AVAIL_PKT_TEST_CNT; ++i)
  // {
  //   if(i > 0)
  // }
}

/** @} */
