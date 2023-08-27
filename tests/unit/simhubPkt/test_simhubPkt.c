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
  // uint8_t *heads[GET_FREE_SPACE_TEST_CNT] =
  //   {fixture->testBuffer,
  //    fixture->testBuffer + TEST_BUFFER_SIZE - TEST_BUFFER_SIZE / 4,
  //    fixture->testBuffer + TEST_BUFFER_SIZE / 2};
  // uint8_t *tails[GET_FREE_SPACE_TEST_CNT] =
  //   {fixture->testBuffer + TEST_BUFFER_SIZE,
  //    fixture->testBuffer + TEST_BUFFER_SIZE / 4,
  //    fixture->testBuffer + TEST_BUFFER_SIZE / 2};
  // size_t expectedFreeSpaces[GET_FREE_SPACE_TEST_CNT] = {0, 64, 128};

  // for(uint8_t i = 0; i < GET_FREE_SPACE_TEST_CNT; ++i)
  // {
  //   fixture->testPktBuf.head = heads[i];
  //   fixture->testPktBuf.tail = tails[i];

  //   zassert_equal(expectedFreeSpaces[i],
  //     simhubPktGetBufferFreeSpace(&fixture->testPktBuf));
  // }
}

/** @} */
