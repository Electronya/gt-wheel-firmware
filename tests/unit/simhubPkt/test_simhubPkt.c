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
#include "zephyrRingBuffer.h"

DEFINE_FFF_GLOBALS;

/* mocks */

/**
 * @brief The test buffer size.
*/
#define TEST_BUFFER_SIZE          128

/**
 * @brief The test fixture.
*/
struct simhubPkt_suite_fixture
{
  uint8_t buffer[TEST_BUFFER_SIZE];
  SimhubPktBuffer pktBuf;
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

  fixture->pktBuf.buffer = fixture->buffer;
  fixture->pktBuf.size = TEST_BUFFER_SIZE;
  fixture->pktBuf.head = 0;
  fixture->pktBuf.tail = 0;
}

ZTEST_SUITE(simhubPkt_suite, NULL, simhubPktSuiteSetup, simhubPktCaseSetup,
  NULL, simhubPktSuiteTeardown);

#define INIT_PKT_BUFFER_SIZE    8
/**
 * @test  simhubPktInitBuffer must initialize the packet buffer as empty.
*/
ZTEST(simhubPkt_suite, test_simhubPktInitBuffer_InitHeadAndTail)
{
  uint8_t buffer[INIT_PKT_BUFFER_SIZE];
  SimhubPktBuffer testBuffer = {
    .buffer = NULL,
    .size = 0,
    .head = 100,
    .tail = 50,
  };

  simhubPktInitBuffer(&testBuffer, buffer, INIT_PKT_BUFFER_SIZE);

  zassert_equal(buffer, testBuffer.buffer);
  zassert_equal(INIT_PKT_BUFFER_SIZE, testBuffer.size);
  zassert_equal(0, testBuffer.head);
  zassert_equal(0, testBuffer.tail);
}

#define PKT_BUF_EMPTY_TEST_COUNT    4
/**
 * @test  simhubPktIsBufferEmpty must return true if the provided packet
 *        buffer is empty and false otherwise.
*/
ZTEST_F(simhubPkt_suite, test_simhubPktIsBufferEmpty_ReturnVal)
{
  uint32_t heads[PKT_BUF_EMPTY_TEST_COUNT] = {0, 10, 69, TEST_BUFFER_SIZE};
  uint32_t tails[PKT_BUF_EMPTY_TEST_COUNT] = {0, TEST_BUFFER_SIZE, 69, 2};

  for(uint8_t i = 0; i < PKT_BUF_EMPTY_TEST_COUNT; ++i)
  {
    fixture->pktBuf.head = heads[i];
    fixture->pktBuf.tail = tails[i];

    if(i % 2 == 0)
      zassert_true(simhubPktIsBufferEmpty(&fixture->pktBuf));
    else
      zassert_false(simhubPktIsBufferEmpty(&fixture->pktBuf));
  }
}

#define PKT_BUF_SIZE_TEST_COUNT     3
/**
 * @test  simhubPktGetBufferSize must return the packet buffer size.
*/
ZTEST_F(simhubPkt_suite, test_simhubPktGetBufferSize_ReturnSize)
{
  size_t expectedSizes[PKT_BUF_SIZE_TEST_COUNT] = {1, 5, TEST_BUFFER_SIZE};

  for(uint8_t i = 0; i < PKT_BUF_SIZE_TEST_COUNT; ++i)
  {
    fixture->pktBuf.size = expectedSizes[i];

    zassert_equal(expectedSizes[i], simhubPktGetBufferSize(&fixture->pktBuf));
  }
}

/**
 * @test  simhubPktGetBufferFreeSpace must return the number of free bytes
 *        in the packet buffer.
*/
ZTEST_F(simhubPkt_suite, test_simhubPktGetBufferFreeSpace_ReturnFreeSpace)
{
  size_t result;
  uint32_t heads[PKT_BUF_SIZE_TEST_COUNT] =
    {0, TEST_BUFFER_SIZE - TEST_BUFFER_SIZE / 4, TEST_BUFFER_SIZE / 2};
  uint32_t tails[PKT_BUF_SIZE_TEST_COUNT] =
    {TEST_BUFFER_SIZE, TEST_BUFFER_SIZE / 4, TEST_BUFFER_SIZE / 2};
  size_t expectedFreeSpaces[PKT_BUF_SIZE_TEST_COUNT] = {0, 64, 128};

  for(uint8_t i = 0; i < PKT_BUF_SIZE_TEST_COUNT; ++i)
  {
    fixture->pktBuf.head = heads[i];
    fixture->pktBuf.tail = tails[i];

    result = simhubPktGetBufferFreeSpace(&fixture->pktBuf);

    zassert_equal(expectedFreeSpaces[i], result);
  }
}

/**
 * @test  simhubPktGetBufferUsedSpace must return the number of used bytes
 *        in the packet buffer.
*/
ZTEST_F(simhubPkt_suite, test_simhubPktGetBufferUsedSpace_ReturnUsedSpace)
{
  size_t result;
  uint32_t heads[PKT_BUF_SIZE_TEST_COUNT] =
    {0, TEST_BUFFER_SIZE - TEST_BUFFER_SIZE / 4, TEST_BUFFER_SIZE / 2};
  uint32_t tails[PKT_BUF_SIZE_TEST_COUNT] =
    {TEST_BUFFER_SIZE, TEST_BUFFER_SIZE / 4, TEST_BUFFER_SIZE / 2};
  size_t expectedFreeSpaces[PKT_BUF_SIZE_TEST_COUNT] = {128, 64, 0};

  for(uint8_t i = 0; i < PKT_BUF_SIZE_TEST_COUNT; ++i)
  {
    fixture->pktBuf.head = heads[i];
    fixture->pktBuf.tail = tails[i];

    result = simhubPktGetBufferUsedSpace(&fixture->pktBuf);

    zassert_equal(expectedFreeSpaces[i], result);
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
