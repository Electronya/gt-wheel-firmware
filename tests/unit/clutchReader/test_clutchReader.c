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
FAKE_VALUE_FUNC(int, zephyrAdcGetSample, uint32_t, uint32_t*);
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
 * @brief The zephyrAdcInit custom fake for fails.
*/
int customZephyrAdcInitFail(ZephyrAdcChanConfig *configs, size_t configCount,
                            ZephyrAdcRes res, uint32_t vdd)
{
  zassert_equal(2, configCount);
  zassert_equal(ADC_12BITS_RES, res);
  zassert_equal(3300, vdd);

  for(uint8_t i = 0; i < configCount; ++i)
  {
    zassert_equal(i, configs[i].id);
    zassert_equal(ADC_UNIT_GAIN, configs[i].gain);
    zassert_equal(ADC_ACQ_56_CYCLES, configs[i].acqTime);
    zassert_equal(ADC_VDD_REF, configs[i].ref);
  }

  return -ENODEV;
}

/**
 * @brief The zephyrAdcInit custom fake for success.
*/
int customZephyrAdcInitSuccess(ZephyrAdcChanConfig *configs, size_t configCount,
                               ZephyrAdcRes res, uint32_t vdd)
{
  zassert_equal(2, configCount);
  zassert_equal(ADC_12BITS_RES, res);
  zassert_equal(3300, vdd);

  for(uint8_t i = 0; i < configCount; ++i)
  {
    zassert_equal(i, configs[i].id);
    zassert_equal(ADC_UNIT_GAIN, configs[i].gain);
    zassert_equal(ADC_ACQ_56_CYCLES, configs[i].acqTime);
    zassert_equal(ADC_VDD_REF, configs[i].ref);
  }

  return 0;
}

#define CLUTCH_SAMPLE_TEST_CNT            2
/**
 * @test  sampleClutchRawValues must return the error code if any of the
 *        sampling operation fails.
*/
ZTEST(clutchReader_suite, test_sampleClutchRawValues_SamplingFail)
{
  int successRet = 0;
  int failRet = -EIO;
  uint32_t samples[CLUTCH_READER_CHAN_CNT];
  int retVals[CLUTCH_SAMPLE_TEST_CNT] = {successRet, successRet};

  for(uint8_t i = 0; i < CLUTCH_SAMPLE_TEST_CNT; ++i)
  {
    retVals[i] = failRet;
    if(i > 0)
      retVals[i - 1] = successRet;

    SET_RETURN_SEQ(zephyrAdcGetSample, retVals, CLUTCH_SAMPLE_TEST_CNT);

    zassert_equal(failRet, sampleClutchRawValues(samples));
    zassert_equal(i + 1, zephyrAdcGetSample_fake.call_count);

    RESET_FAKE(zephyrAdcGetSample);
  }
}

#define CLUTCH_CALC_STATE_TEST_CNT        6
/**
 * @test  calculateClutchState must return the calculated clutch state from the
 *        raw values and the friction point.
*/
ZTEST(clutchReader_suite, test_calculateClutchState_CalcClutchState)
{
  uint8_t result;
  uint32_t testRawLimits[CLUTCH_CALC_STATE_TEST_CNT][CLUTCH_RAW_LIMIT_CNT][CLUTCH_READER_CHAN_CNT] =
    {{{1650, 500}, {1600, 400}}, {{1650, 3300}, {1650, 3300}},
     {{1000, 750}, {1000, 750}}, {{2000, 2500}, {2000, 2500}},
     {{2000, 3000}, {2000, 3000}}, {{1650, 2000}, {1650, 2000}}};
  uint32_t rawValues[CLUTCH_CALC_STATE_TEST_CNT][CLUTCH_READER_CHAN_CNT] =
    {{600, 600}, {1650, 3299},
     {750, 1000}, {2500, 2000},
     {3000, 2250}, {2000, 2000}};
  uint8_t frictPoints[CLUTCH_CALC_STATE_TEST_CNT] = {127, 127,
                                                     127, 200,
                                                     127, 127};
  uint8_t expectedStates[CLUTCH_CALC_STATE_TEST_CNT] = {255, 255,
                                                        127, 200,
                                                        95, 0};

  for(uint8_t i = 0; i < CLUTCH_CALC_STATE_TEST_CNT; ++i)
  {
    for(uint8_t j = 0; j < CLUTCH_READER_CHAN_CNT; ++j)
    {
      rawLimits[j][0] = testRawLimits[i][j][0];
      rawLimits[j][1] = testRawLimits[i][j][1];
    }

    result = calculateClutchState(rawValues[i], frictPoints[i]);

    zassert_equal(expectedStates[i], result);
  }
}

/**
 * @test  clutchReaderInit must return the error code when initializing the
 *        clutch ADC and its channels fails.
*/
ZTEST(clutchReader_suite, test_clutchReaderInit_AdcInitFail)
{
  int failRet = -ENODEV;

  zephyrAdcInit_fake.custom_fake = customZephyrAdcInitFail;

  zassert_equal(failRet, clutchReaderInit());
  zassert_equal(1, zephyrAdcInit_fake.call_count);
}

/**
 * @test  clutchReaderInit must return the success code when initializing the
 *        ADC and the reading thread succeeds.
*/
ZTEST(clutchReader_suite, test_clutchReaderInit_AdcInitSuccess)
{
  int successRet = 0;

  zephyrAdcInit_fake.custom_fake = customZephyrAdcInitSuccess;

  zassert_equal(successRet, clutchReaderInit());
  zassert_equal(1, zephyrAdcInit_fake.call_count);
  zassert_equal(1, zephyrThreadCreate_fake.call_count);
  zassert_equal(&thread, zephyrThreadCreate_fake.arg0_val);
  zassert_equal(CLUTCH_READER_THREAD_NAME, zephyrThreadCreate_fake.arg1_val);
  zassert_equal(ZEPHYR_TIME_NO_WAIT, zephyrThreadCreate_fake.arg2_val);
  zassert_equal(MILLI_SEC, zephyrThreadCreate_fake.arg3_val);
  zassert_equal(clutchReaderThread, thread.entry);
}

/** @} */
