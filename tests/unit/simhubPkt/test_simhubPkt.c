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
  uint8_t testBuffer[TEST_BUFFER_SIZE];
  SimhubPktBuffer testPktBuf;
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

  fixture->testPktBuf.buffer = fixture->testBuffer;
  fixture->testPktBuf.size = TEST_BUFFER_SIZE;
  fixture->testPktBuf.head = fixture->testBuffer;
  fixture->testPktBuf.tail = fixture->testBuffer;
}

ZTEST_SUITE(simhubPkt_suite, NULL, simhubPktSuiteSetup, simhubPktCaseSetup,
  NULL, simhubPktSuiteTeardown);

/**
 * @test  simhubPktInitBuffer must initialize the buffer head and tail.
*/
ZTEST_F(simhubPkt_suite, test_simhubPktInitBuffer_InitHeadAndTail)
{
  fixture->testPktBuf.head = NULL;
  fixture->testPktBuf.tail = NULL;

  simhubPktInitBuffer(&fixture->testPktBuf);

  zassert_equal(fixture->testBuffer, fixture->testPktBuf.head);
  zassert_equal(fixture->testBuffer, fixture->testPktBuf.tail);
}

/** @} */
