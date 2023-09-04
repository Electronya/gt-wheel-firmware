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
FAKE_VALUE_FUNC(size_t, zephyrRingBufGet, ZephyrRingBuffer*, uint8_t*, size_t);
FAKE_VALUE_FUNC(size_t, zephyrRingBufClaimGetting, ZephyrRingBuffer*, uint8_t**,
  size_t);
FAKE_VALUE_FUNC(int, zephyrRingBufFinishGetting, ZephyrRingBuffer*, size_t);
FAKE_VALUE_FUNC(size_t, zephyrRingBufClaimPutting, ZephyrRingBuffer*, uint8_t**,
  size_t);
FAKE_VALUE_FUNC(int, zephyrRingBufFinishPutting, ZephyrRingBuffer*, size_t);
FAKE_VALUE_FUNC(size_t, zephyrRingBufPeek, ZephyrRingBuffer*, uint8_t*, size_t);

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

  for(uint8_t i = 0; i < SIMHUB_RX_PKT_BUF_SIZE; ++i)
    rxBufData[i] = 0;

  for(uint8_t i = 0; i < SIMHUB_TX_PKT_BUF_SIZE; ++i)
    txBufData[i] = 0;

  RESET_FAKE(zephyrRingBufInit);
  RESET_FAKE(zephyrRingBufGet);
  RESET_FAKE(zephyrRingBufClaimGetting);
  RESET_FAKE(zephyrRingBufFinishGetting);
  RESET_FAKE(zephyrRingBufClaimPutting);
  RESET_FAKE(zephyrRingBufFinishPutting);
  RESET_FAKE(zephyrRingBufPeek);
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
  uint8_t pktSize = SIMHUB_PKT_HEADER_SIZE + SIMHUB_PROTO_PLD_SIZE;
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
  uint8_t pktSize = SIMHUB_PKT_HEADER_SIZE + SIMHUB_PROTO_PLD_SIZE;
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
  uint8_t pktSize = SIMHUB_PKT_HEADER_SIZE + SIMHUB_LED_CNT_PLD_SIZE;
  uint32_t head = 0;

  while(head < pktSize - 1)
  {
    if(head < SIMHUB_PKT_HEADER_SIZE)
      pktBuf[head] = pktHeader[head];
    else
      pktBuf[head] = ledCntPld[head - SIMHUB_PKT_HEADER_SIZE];
    printk("byte: %d\n", pktBuf[head]);
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
  uint8_t pktSize = SIMHUB_PKT_HEADER_SIZE + SIMHUB_LED_CNT_PLD_SIZE;
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
  uint8_t pktSize = footerStart + SIMHUB_LED_DATA_FOOTER_SIZE - 1;
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
 * @brief   Custom stub to get from the Rx packet buffer.
 *
 * @param buffer  The zephyr ring buffer.
 * @param data    The output buffer.
 * @param size    The requested data size.
 *
 * @return  The real data size.
 */
size_t customRxGet(ZephyrRingBuffer *buffer, uint8_t *data, size_t size)
{
  bytecpy(data, rxBufData, size);
  return size;
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
    for(uint8_t i = 0; i < TEST_BUFFER_SIZE; ++i)
      fixture->buffer[i] = 0;

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
    for(uint8_t i = 0; i < TEST_BUFFER_SIZE; ++i)
      fixture->buffer[i] = 0;

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
    for(uint8_t i = 0; i < TEST_BUFFER_SIZE; ++i)
      fixture->buffer[i] = 0;

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
    for(uint8_t i = 0; i < TEST_BUFFER_SIZE; ++i)
      fixture->buffer[i] = 0;

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

/**
 * @test  simhubPktBufClaimPutting must claim the Rx buffer for putting data
 *        and return the available data area and its size.
*/
ZTEST(simhubPkt_suite, simhubPktBufClaimPutting_ClaimPutting)
{
  size_t reqSize = 8;
  size_t expectedSize = 3;
  uint8_t *data = NULL;

  zephyrRingBufClaimPutting_fake.return_val = expectedSize;

  zassert_equal(expectedSize, simhubPktBufClaimPutting(&data, reqSize));
  zassert_equal(1, zephyrRingBufClaimPutting_fake.call_count);
  zassert_equal(&rxBuffer, zephyrRingBufClaimPutting_fake.arg0_val);
  zassert_equal(&data, zephyrRingBufClaimPutting_fake.arg1_val);
  zassert_equal(reqSize, zephyrRingBufClaimPutting_fake.arg2_val);
}

/**
 * @test  simhubPktBufFinishPutting must return the error code when finishing
 *        the putting operation fails.
*/
ZTEST_F(simhubPkt_suite, test_simhubPktBufFinishPutting_Fail)
{
  int failRet = -EINVAL;
  size_t size = 8;

  zephyrRingBufFinishPutting_fake.return_val = failRet;

  zassert_equal(failRet, simhubPktBufFinishPutting(size));
  zassert_equal(1, zephyrRingBufFinishPutting_fake.call_count);
  zassert_equal(&rxBuffer, zephyrRingBufFinishPutting_fake.arg0_val);
  zassert_equal(size, zephyrRingBufFinishPutting_fake.arg1_val);
}

/**
 * @test  simhubPktBufFinishPutting must return the success code when finishing
 *        the putting operation succeeds.
*/
ZTEST_F(simhubPkt_suite, test_simhubPktBufFinishPutting_Success)
{
  int successRet = 0;
  size_t size = 8;

  zephyrRingBufFinishPutting_fake.return_val = successRet;

  zassert_equal(successRet, simhubPktBufFinishPutting(size));
  zassert_equal(1, zephyrRingBufFinishPutting_fake.call_count);
  zassert_equal(&rxBuffer, zephyrRingBufFinishPutting_fake.arg0_val);
  zassert_equal(size, zephyrRingBufFinishPutting_fake.arg1_val);
}

/**
 * @test  simhubPktBufClaimGetting must claim the Tx buffer for getting data
 *        and return the available data area and its size.
*/
ZTEST(simhubPkt_suite, test_simhubPxtBufClaimGetting_ClaimGetting)
{
  size_t reqSize = 8;
  size_t expectedSize = 3;
  uint8_t *data = NULL;

  zephyrRingBufClaimGetting_fake.return_val = expectedSize;

  zassert_equal(expectedSize, simhubPktBufClaimGetting(&data, reqSize));
  zassert_equal(1, zephyrRingBufClaimGetting_fake.call_count);
  zassert_equal(&txBuffer, zephyrRingBufClaimGetting_fake.arg0_val);
  zassert_equal(&data, zephyrRingBufClaimGetting_fake.arg1_val);
  zassert_equal(reqSize, zephyrRingBufClaimGetting_fake.arg2_val);
}

/**
 * @test  simhubPktBufFinishGetting must return the error code when finishing
 *        the getting operation fails.
*/
ZTEST_F(simhubPkt_suite, test_simhubPktBufFinishGetting_Fail)
{
  int failRet = -EINVAL;
  size_t size = 8;

  zephyrRingBufFinishGetting_fake.return_val = failRet;

  zassert_equal(failRet, simhubPktBufFinishGetting(size));
  zassert_equal(1, zephyrRingBufFinishGetting_fake.call_count);
  zassert_equal(&txBuffer, zephyrRingBufFinishGetting_fake.arg0_val);
  zassert_equal(size, zephyrRingBufFinishGetting_fake.arg1_val);
}

/**
 * @test  simhubPktBufFinishGetting must return the success code when finishing
 *        the getting operation succeeds.
*/
ZTEST_F(simhubPkt_suite, test_simhubPktBufFinishGetting_Success)
{
  int successRet = 0;
  size_t size = 8;

  zephyrRingBufFinishGetting_fake.return_val = successRet;

  zassert_equal(successRet, simhubPktBufFinishGetting(size));
  zassert_equal(1, zephyrRingBufFinishGetting_fake.call_count);
  zassert_equal(&txBuffer, zephyrRingBufFinishGetting_fake.arg0_val);
  zassert_equal(size, zephyrRingBufFinishGetting_fake.arg1_val);
}

/**
 * @test  simhubPktIsPktAvailable must return false and set the packet type
 *        to the packet type count if there is no new available packet in
 *        the buffer.
*/
ZTEST(simhubPkt_suite, test_simhubPktIsPktAvailable_NoAvailable)
{
  size_t pktSize;
  SimhubPktTypes packetType;
  SetupPktFunction setupFunctions[PKT_TYPE_COUNT] =
    {setupIncompleteUnlockPkt, setupIncompleteProtocolPkt,
     setupIncompleteLedCountPkt, setupIncompleteLedDataPkt};

  for(uint8_t i = 0; i < PKT_TYPE_COUNT; ++i)
  {
    printk("test ID: %d\n", i);
    for(uint8_t i = 0; i < SIMHUB_RX_PKT_BUF_SIZE; ++i)
      rxBufData[i] = 0;

    zephyrRingBufPeek_fake.custom_fake = customRxGet;
    if(i == PKT_TYPE_COUNT - 1)
      ledCtrlGetRpmChaserPxlCnt_fake.return_val = TEST_LED_PIXEL_COUNT;
    pktSize = setupFunctions[i](rxBufData);

    zassert_false(simhubPktIsPktAvailable(&packetType));
    zassert_equal(PKT_TYPE_COUNT, packetType);
    zassert_equal(1, zephyrRingBufPeek_fake.call_count);
    zassert_equal(&rxBuffer, zephyrRingBufPeek_fake.arg0_val);
    zassert_equal(SIMHUB_RX_PKT_BUF_SIZE, zephyrRingBufPeek_fake.arg2_val);
    RESET_FAKE(zephyrRingBufPeek);
  }
}

/**
 * @test  simhubPktIsPktAvailable must return true and set the packet type
 *        to the appropriate one if there is a new available packet in
 *        the buffer.
*/
ZTEST(simhubPkt_suite, test_simhubPktIsPktAvailable_Available)
{
  size_t pktSize;
  SimhubPktTypes packetType;
  SimhubPktTypes expectedPktTypes[PKT_TYPE_COUNT] = {UNLOCK_TYPE,
                                                     PROTO_TYPE,
                                                     LED_COUNT_TYPE,
                                                     LED_DATA_TYPE};
  SetupPktFunction setupFunctions[PKT_TYPE_COUNT] =
    {setupCompleteUnlockPkt, setupCompleteProtocolPkt,
     setupCompleteLedCountPkt, setupCompleteLedDataPkt};

  for(uint8_t i = 0; i < PKT_TYPE_COUNT; ++i)
  {
    for(uint8_t i = 0; i < SIMHUB_RX_PKT_BUF_SIZE; ++i)
      rxBufData[i] = 0;

    zephyrRingBufPeek_fake.custom_fake = customRxGet;
    if(i == PKT_TYPE_COUNT - 1)
      ledCtrlGetRpmChaserPxlCnt_fake.return_val = TEST_LED_PIXEL_COUNT;
    pktSize = setupFunctions[i](rxBufData);

    zassert_true(simhubPktIsPktAvailable(&packetType));
    zassert_equal(expectedPktTypes[i], packetType);
    zassert_equal(1, zephyrRingBufPeek_fake.call_count);
    zassert_equal(&rxBuffer, zephyrRingBufPeek_fake.arg0_val);
    zassert_equal(SIMHUB_RX_PKT_BUF_SIZE, zephyrRingBufPeek_fake.arg2_val);
    RESET_FAKE(zephyrRingBufPeek);
  }
}

/** @} */
