/**
 * Copyright (C) 2023 by Electronya
 *
 * @file      test_acmDevice.h
 * @author    jbacon
 * @date      2023-07-19
 * @brief     ACM device Module Test Cases
 *
 *            This file is the test cases of the ACM device module.
 *
 * @ingroup  acmDevice
 *
 * @{
 */

#include <zephyr/ztest.h>
#include <zephyr/fff.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

#include "acmDevice.h"
#include "acmDevice.c"

#include "simhubPkt.h"
#include "zephyrACM.h"
#include "zephyrThread.h"

DEFINE_FFF_GLOBALS;

/* mocks */
FAKE_VALUE_FUNC(int, zephyrAcmInit, ZephyrACM*, size_t, size_t);
FAKE_VOID_FUNC(zephyrAcmEnableRxIrq, ZephyrACM*);
FAKE_VOID_FUNC(zephyrAcmEnableTxIrq, ZephyrACM*);
FAKE_VALUE_FUNC(int, zephyrAcmReadRingBuffer, ZephyrACM*, uint8_t*, size_t);
FAKE_VALUE_FUNC(int, zephyrAcmWriteRingBuffer, ZephyrACM*, uint8_t*, size_t);

FAKE_VOID_FUNC(simhubPktInitBuffer);
FAKE_VALUE_FUNC(size_t, simhubPktBufClaimPutting, uint8_t**, size_t);
FAKE_VALUE_FUNC(int, simhubPktBufFinishPutting, size_t);
FAKE_VALUE_FUNC(size_t, simhubPktBufClaimGetting, uint8_t**, size_t);
FAKE_VALUE_FUNC(int, simhubPktBufFinishGetting, size_t);
FAKE_VALUE_FUNC(bool, simhubPktIsPktAvailable, SimhubPktTypes*);
FAKE_VALUE_FUNC(int, simhubPktProcessUnlock);
FAKE_VALUE_FUNC(int, simhubPktProcessProto);
FAKE_VALUE_FUNC(int, simhubPktProcessLedCount);
FAKE_VALUE_FUNC(int, simhubPktProcessLedData);

/**
 * @brief The claimed data area address.
 */
#define TEST_CLAIMED_AREA_ADDR        50000

/**
 * @brief The requested claim size for Rx packet buffer.
*/
#define TEST_RX_CLAIM_SIZE            128

/**
 * @brief The requested claim size for the Tx packet buffer.
*/
#define TEST_TX_CLAIM_SIZE            24

/**
 * @brief The claimed data size.
*/
#define TEST_CLAIMED_DATA_SIZE        7

/**
 * @brief The test fixture.
*/
struct acmDevice_suite_fixture
{
  uint8_t rxBuffer[SIMHUB_RX_PKT_BUF_SIZE];
  uint8_t txBuffer[SIMHUB_TX_PKT_BUF_SIZE];
};

static void *acmDeviceSuiteSetup(void)
{
  struct acmDevice_suite_fixture *fixture =
    k_malloc(sizeof(struct acmDevice_suite_fixture));
  zassume_not_null(fixture, NULL);

  return (void *)fixture;
}

static void acmDeviceSuiteTeardown(void *f)
{
  k_free(f);
}

static void acmDeviceCaseSetup(void *f)
{
  struct acmDevice_suite_fixture *fixture =
    (struct acmDevice_suite_fixture *)f;

  for(uint8_t i = 0; i < SIMHUB_RX_PKT_BUF_SIZE; ++i)
    fixture->rxBuffer[i] = 0;

  for(uint8_t i = 0; i < SIMHUB_TX_PKT_BUF_SIZE; ++i)
    fixture->txBuffer[i] = 0;

  RESET_FAKE(zephyrAcmInit);
  RESET_FAKE(zephyrAcmEnableRxIrq);
  RESET_FAKE(zephyrAcmEnableTxIrq);
  RESET_FAKE(zephyrAcmReadRingBuffer);
  RESET_FAKE(zephyrAcmWriteRingBuffer);
  RESET_FAKE(simhubPktInitBuffer);
  RESET_FAKE(simhubPktBufClaimPutting);
  RESET_FAKE(simhubPktBufFinishPutting);
  RESET_FAKE(simhubPktBufClaimGetting);
  RESET_FAKE(simhubPktBufFinishGetting);
  RESET_FAKE(simhubPktIsPktAvailable);
  RESET_FAKE(simhubPktProcessUnlock);
  RESET_FAKE(simhubPktProcessProto);
  RESET_FAKE(simhubPktProcessLedCount);
  RESET_FAKE(simhubPktProcessLedData);
}

ZTEST_SUITE(acmDevice_suite, NULL, acmDeviceSuiteSetup, acmDeviceCaseSetup,
  NULL, acmDeviceSuiteTeardown);

/**
 * @brief   Custom SIMHUB packet claim putting.
 *
 * @param data  The data area claimed.
 * @param size  The requested data area size.
 *
 * @return  The actual size of the data area claimed.
 */
size_t customClaimPutting(uint8_t **data, size_t size)
{
  *data = (uint8_t *)TEST_CLAIMED_AREA_ADDR;
  zassert_equal(TEST_RX_CLAIM_SIZE, size);
  return TEST_CLAIMED_DATA_SIZE;
}

/**
 * @brief   Custom SIMHUB packet claim getting.
 *
 * @param data  The data area claimed.
 * @param size  The requested data area size.
 *
 * @return  The actual size of the data area claimed.
 */
size_t customClaimGetting(uint8_t **data, size_t size)
{
  *data = (uint8_t *)(TEST_CLAIMED_AREA_ADDR + 1);
  zassert_equal(TEST_TX_CLAIM_SIZE, size);
  return TEST_CLAIMED_DATA_SIZE;
}

/**
 * @brief   Custom simhubPktIsPktAvailable for unlock upload packet.
 *
 * @param pktType The packet type.
 *
 * @return  true when a new packet is available.
 */
bool customUlockPacketAvailable(SimhubPktTypes *pktType)
{
  *pktType = UNLOCK_TYPE;
  return true;
}

/**
 * @brief   Custom simhubPktIsPktAvailable for protocol version packet.
 *
 * @param pktType The packet type.
 *
 * @return  true when a new packet is available.
 */
bool customProtoPacketAvailable(SimhubPktTypes *pktType)
{
  *pktType = PROTO_TYPE;
  return true;
}

/**
 * @brief   Custom simhubPktIsPktAvailable for LED count packet.
 *
 * @param pktType The packet type.
 *
 * @return  true when a new packet is available.
 */
bool customLedCountPacketAvailable(SimhubPktTypes *pktType)
{
  *pktType = LED_COUNT_TYPE;
  return true;
}

/**
 * @brief   Custom simhubPktIsPktAvailable for LED data packet.
 *
 * @param pktType The packet type.
 *
 * @return  true when a new packet is available.
 */
bool customLedDataPacketAvailable(SimhubPktTypes *pktType)
{
  *pktType = LED_DATA_TYPE;
  return true;
}

/**
 * @test  acmDeviceWorker must do nothing with the received data if it doesn't
 *        contain any complete packet.
*/
ZTEST(acmDevice_suite, test_acmDeviceWorker_IncompletePacket)
{
  int successRet = 0;
  size_t byteRcvd = 6;

  simhubPktBufClaimPutting_fake.custom_fake = customClaimPutting;
  zephyrAcmReadRingBuffer_fake.return_val = byteRcvd;
  simhubPktBufFinishPutting_fake.return_val = successRet;
  simhubPktIsPktAvailable_fake.return_val = false;

  acmDeviceWorker(NULL, NULL, NULL);
  zassert_equal(1, simhubPktBufClaimPutting_fake.call_count);
  zassert_equal(1, zephyrAcmReadRingBuffer_fake.call_count);
  zassert_equal(&acmDev, zephyrAcmReadRingBuffer_fake.arg0_val);
  zassert_equal((uint8_t *)TEST_CLAIMED_AREA_ADDR,
    zephyrAcmReadRingBuffer_fake.arg1_val);
  zassert_equal(TEST_CLAIMED_DATA_SIZE, zephyrAcmReadRingBuffer_fake.arg2_val);
  zassert_equal(1, simhubPktBufFinishPutting_fake.call_count);
  zassert_equal(byteRcvd, simhubPktBufFinishPutting_fake.arg0_val);
  zassert_equal(1, simhubPktIsPktAvailable_fake.call_count);
}

/**
 * @test  acmDeviceWorker must process any unlock update packet and don't
 *        respond to it.
*/
ZTEST(acmDevice_suite, test_acmDeviceWorker_UnlockPacket)
{
  int successRet = 0;
  size_t byteRcvd = 6;

  simhubPktBufClaimPutting_fake.custom_fake = customClaimPutting;
  zephyrAcmReadRingBuffer_fake.return_val = byteRcvd;
  simhubPktBufFinishPutting_fake.return_val = successRet;
  simhubPktIsPktAvailable_fake.custom_fake = customUlockPacketAvailable;
  simhubPktProcessUnlock_fake.return_val = successRet;

  acmDeviceWorker(NULL, NULL, NULL);
  zassert_equal(1, simhubPktBufClaimPutting_fake.call_count);
  zassert_equal(1, zephyrAcmReadRingBuffer_fake.call_count);
  zassert_equal(&acmDev, zephyrAcmReadRingBuffer_fake.arg0_val);
  zassert_equal((uint8_t *)TEST_CLAIMED_AREA_ADDR,
    zephyrAcmReadRingBuffer_fake.arg1_val);
  zassert_equal(TEST_CLAIMED_DATA_SIZE, zephyrAcmReadRingBuffer_fake.arg2_val);
  zassert_equal(1, simhubPktBufFinishPutting_fake.call_count);
  zassert_equal(byteRcvd, simhubPktBufFinishPutting_fake.arg0_val);
  zassert_equal(1, simhubPktIsPktAvailable_fake.call_count);
  zassert_equal(1, simhubPktProcessUnlock_fake.call_count);
}

/**
 * @test  acmDeviceWorker must process any protocol version packet and respond
 *        to it.
*/
ZTEST(acmDevice_suite, test_acmDeviceWorker_ProtoPacket)
{
  int successRet = 0;
  size_t byteRcvd = 6;

  simhubPktBufClaimPutting_fake.custom_fake = customClaimPutting;
  zephyrAcmReadRingBuffer_fake.return_val = byteRcvd;
  simhubPktBufFinishPutting_fake.return_val = successRet;
  simhubPktIsPktAvailable_fake.custom_fake = customProtoPacketAvailable;
  simhubPktProcessProto_fake.return_val = successRet;
  simhubPktBufClaimGetting_fake.custom_fake = customClaimGetting;
  zephyrAcmWriteRingBuffer_fake.return_val = successRet;

  acmDeviceWorker(NULL, NULL, NULL);
  zassert_equal(1, simhubPktBufClaimPutting_fake.call_count);
  zassert_equal(1, zephyrAcmReadRingBuffer_fake.call_count);
  zassert_equal(&acmDev, zephyrAcmReadRingBuffer_fake.arg0_val);
  zassert_equal((uint8_t *)TEST_CLAIMED_AREA_ADDR,
    zephyrAcmReadRingBuffer_fake.arg1_val);
  zassert_equal(TEST_CLAIMED_DATA_SIZE, zephyrAcmReadRingBuffer_fake.arg2_val);
  zassert_equal(1, simhubPktBufFinishPutting_fake.call_count);
  zassert_equal(byteRcvd, simhubPktBufFinishPutting_fake.arg0_val);
  zassert_equal(1, simhubPktIsPktAvailable_fake.call_count);
  zassert_equal(1, simhubPktProcessProto_fake.call_count);
  zassert_equal(1, simhubPktBufClaimGetting_fake.call_count);
  zassert_equal(1, zephyrAcmWriteRingBuffer_fake.call_count);
  zassert_equal(&acmDev, zephyrAcmWriteRingBuffer_fake.arg0_val);
  zassert_equal((uint8_t *)(TEST_CLAIMED_AREA_ADDR + 1),
    zephyrAcmWriteRingBuffer_fake.arg1_val);
  zassert_equal(TEST_CLAIMED_DATA_SIZE, zephyrAcmWriteRingBuffer_fake.arg2_val);
  zassert_equal(1, zephyrAcmEnableTxIrq_fake.call_count);
  zassert_equal(&acmDev, zephyrAcmEnableTxIrq_fake.arg0_val);
}

/**
 * @test  acmDeviceWorker must process any LED count packet and respond
 *        to it.
*/
ZTEST(acmDevice_suite, test_acmDeviceWorker_LedCountPacket)
{
  int successRet = 0;
  size_t byteRcvd = 6;

  simhubPktBufClaimPutting_fake.custom_fake = customClaimPutting;
  zephyrAcmReadRingBuffer_fake.return_val = byteRcvd;
  simhubPktBufFinishPutting_fake.return_val = successRet;
  simhubPktIsPktAvailable_fake.custom_fake = customLedCountPacketAvailable;
  simhubPktProcessLedCount_fake.return_val = successRet;
  simhubPktBufClaimGetting_fake.custom_fake = customClaimGetting;
  zephyrAcmWriteRingBuffer_fake.return_val = successRet;

  acmDeviceWorker(NULL, NULL, NULL);
  zassert_equal(1, simhubPktBufClaimPutting_fake.call_count);
  zassert_equal(1, zephyrAcmReadRingBuffer_fake.call_count);
  zassert_equal(&acmDev, zephyrAcmReadRingBuffer_fake.arg0_val);
  zassert_equal((uint8_t *)TEST_CLAIMED_AREA_ADDR,
    zephyrAcmReadRingBuffer_fake.arg1_val);
  zassert_equal(TEST_CLAIMED_DATA_SIZE, zephyrAcmReadRingBuffer_fake.arg2_val);
  zassert_equal(1, simhubPktBufFinishPutting_fake.call_count);
  zassert_equal(byteRcvd, simhubPktBufFinishPutting_fake.arg0_val);
  zassert_equal(1, simhubPktIsPktAvailable_fake.call_count);
  zassert_equal(1, simhubPktProcessLedCount_fake.call_count);
  zassert_equal(1, simhubPktBufClaimGetting_fake.call_count);
  zassert_equal(1, zephyrAcmWriteRingBuffer_fake.call_count);
  zassert_equal(&acmDev, zephyrAcmWriteRingBuffer_fake.arg0_val);
  zassert_equal((uint8_t *)(TEST_CLAIMED_AREA_ADDR + 1),
    zephyrAcmWriteRingBuffer_fake.arg1_val);
  zassert_equal(TEST_CLAIMED_DATA_SIZE, zephyrAcmWriteRingBuffer_fake.arg2_val);
  zassert_equal(1, zephyrAcmEnableTxIrq_fake.call_count);
  zassert_equal(&acmDev, zephyrAcmEnableTxIrq_fake.arg0_val);
}

/**
 * @test  acmDeviceWorker must process any LED data packet, update and don't
 *        respond to it.
*/
ZTEST(acmDevice_suite, test_acmDeviceWorker_LedDataPacket)
{
  int successRet = 0;
  size_t byteRcvd = 6;

  simhubPktBufClaimPutting_fake.custom_fake = customClaimPutting;
  zephyrAcmReadRingBuffer_fake.return_val = byteRcvd;
  simhubPktBufFinishPutting_fake.return_val = successRet;
  simhubPktIsPktAvailable_fake.custom_fake = customLedDataPacketAvailable;
  simhubPktProcessLedData_fake.return_val = successRet;

  acmDeviceWorker(NULL, NULL, NULL);
  zassert_equal(1, simhubPktBufClaimPutting_fake.call_count);
  zassert_equal(1, zephyrAcmReadRingBuffer_fake.call_count);
  zassert_equal(&acmDev, zephyrAcmReadRingBuffer_fake.arg0_val);
  zassert_equal((uint8_t *)TEST_CLAIMED_AREA_ADDR,
    zephyrAcmReadRingBuffer_fake.arg1_val);
  zassert_equal(TEST_CLAIMED_DATA_SIZE, zephyrAcmReadRingBuffer_fake.arg2_val);
  zassert_equal(1, simhubPktBufFinishPutting_fake.call_count);
  zassert_equal(byteRcvd, simhubPktBufFinishPutting_fake.arg0_val);
  zassert_equal(1, simhubPktIsPktAvailable_fake.call_count);
  zassert_equal(1, simhubPktProcessLedData_fake.call_count);
}

/**
 * @test  acmDeviceInit must return the error code when initializing the
 *        ACM fails.
*/
ZTEST(acmDevice_suite, test_acmDeviceInit_AcmInitFail)
{
  int failRet = -ENOSPC;

  zephyrAcmInit_fake.return_val = failRet;

  zassert_equal(failRet, acmDeviceInit());
  zassert_equal(1, zephyrAcmInit_fake.call_count);
  zassert_equal(&acmDev, zephyrAcmInit_fake.arg0_val);
  zassert_equal(128, zephyrAcmInit_fake.arg1_val);
  zassert_equal(24, zephyrAcmInit_fake.arg2_val);
}

/**
 * @test  acmDeviceInit must return the success code when initializing the ACM
 *        device, initializing the SIMHUB packet buffers and enabling the ACM
 *        Rx interrupt succeed.
*/
ZTEST(acmDevice_suite, test_acmDeviceInit_Success)
{
  int successRet = 0;

  zephyrAcmInit_fake.return_val = successRet;

  zassert_equal(successRet, acmDeviceInit());
  zassert_equal(1, zephyrAcmInit_fake.call_count);
  zassert_equal(&acmDev, zephyrAcmInit_fake.arg0_val);
  zassert_equal(128, zephyrAcmInit_fake.arg1_val);
  zassert_equal(24, zephyrAcmInit_fake.arg2_val);
  zassert_equal(1, simhubPktInitBuffer_fake.call_count);
  zassert_equal(1, zephyrAcmEnableRxIrq_fake.call_count);
}

/** @} */
