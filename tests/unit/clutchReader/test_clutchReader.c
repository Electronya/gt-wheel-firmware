/**
 * Copyright (C) 2023 by Electronya
 *
 * @file      test_clutchReader.c
 * @author    jbacon
 * @date      2023-09-22
 * @brief     Clutch Reader Module Test Cases
 *
 *            This file is the test cases of the clutch reader module.
 *
 * @ingroup  clutchReader
 *
 * @{
 */


#include <zephyr/ztest.h>
#include <zephyr/fff.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

#include "clutchReader.h"
#include "clutchReader.c"

#include "zephyrAdc.h"
#include "zephyrCommon.h"
#include "zephyrThread.h"

DEFINE_FFF_GLOBALS;

/* mocks */
FAKE_VALUE_FUNC(int, zephyrAdcInit, ZephyrAdcChanConfig*, size_t, ZephyrAdcRes,
  uint32_t);
FAKE_VALUE_FUNC(int, zephyrAdcGetSample, ZephyrAdcChanId, uint32_t*);
FAKE_VALUE_FUNC(uint32_t, zephyrThreadSleepMs, uint32_t);
FAKE_VOID_FUNC(zephyrThreadCreate, ZephyrThread*, char*, uint32_t,
               ZephyrTimeUnit);

/**
 * @brief   Clutch reader test cases setup.
 *
 * @param f   The test fixture if one exists.
 */
static void clutchReaderCaseSetup(void *f)
{
  RESET_FAKE(zephyrAdcInit);
  RESET_FAKE(zephyrAdcGetSample);
  RESET_FAKE(zephyrThreadSleepMs);
  RESET_FAKE(zephyrThreadCreate);
}

ZTEST_SUITE(clutchReader_suite, NULL, NULL, clutchReaderCaseSetup, NULL, NULL);

/**
 * @test  clutchReaderInit must return the error code when initializing the
 *        clutch ADC and its channels fails.
*/
ZTEST(clutchReader_suite, test_clutchReaderInit_AdcInitFail)
{

}

/** @} */
