/**
 * Copyright (C) 2023 by Electronya
 *
 * @file      clutchReader.c
 * @author    jbacon
 * @date      2023-07-31
 * @brief     Clutch Reader Module
 *
 *            This file is the implementation of the clutch reader module.
 *
 * @ingroup  clutchReader
 *
 * @{
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

#include "clutchReader.h"
#include "zephyrCommon.h"
#include "zephyrAdc.h"
#include "zephyrThread.h"

#define CLUTCH_READER_MODULE_NAME clutch_reader_module

/* Setting module logging */
LOG_MODULE_REGISTER(CLUTCH_READER_MODULE_NAME);

/**
 * @brief The thread stack size.
*/
#define CLUTCH_READER_STACK_SIZE          256

/**
 * @brief The clutch ADC channel count.
*/
#define CLUTCH_READER_CHAN_CNT            2

/**
 * @brief The clutch raw value limit count.
*/
#define CLUTCH_RAW_LIMIT_CNT              2

/**
 * @brief The clutch minimal value.
*/
#define CLUTCH_MIN_VALUE                  0

/**
 * @brief The clutch maximal value.
*/
#define CLUTCH_MAX_VALUE                  255

/**
 * @brief The thread name.
 */
#define CLUTCH_READER_THREAD_NAME         "clutchReader"

K_THREAD_STACK_DEFINE(clutchThreadStack, CLUTCH_READER_STACK_SIZE);
static ZephyrThread thread = {
  .stack = clutchThreadStack,
  .stackSize = CLUTCH_READER_STACK_SIZE,
  .priority = 2,
  .options = 0,
};

/**
 * @brief The clutch raw value limits.
*/
static uint32_t rawLimits[CLUTCH_READER_CHAN_CNT][CLUTCH_RAW_LIMIT_CNT];

/**
 * @brief The friction point value.
*/
uint8_t frictionPoint = 127;

/**
 * @brief The clutch state.
*/
uint8_t clutchState = 0;

/**
 * @brief   Sample the clutch raw values.
 *
 * @param rawValues   The clutch raw values. Since the clutch use 2 ADC channel,
 *                    this must be an array of 2. No more, no less.
 *
 * @return 0 if successful, the error code otherwise.
 */
static int sampleClutchRawValues(uint32_t *rawValues)
{
  int rc = 0;

  for(uint8_t i = 0; i < CLUTCH_READER_CHAN_CNT && rc == 0; ++i)
    // TODO: filtering might be needed to reduce noise.
    rc = zephyrAdcGetSample(i, rawValues + i);

  return rc;
}

/**
 * @brief   Calculate the clutch state base on the 2 raw values.
 *
 * @param rawValues     The clutch raw values. Since the clutch use 2 ADC
 *                      channel, this must be an array of 2. No more, no less.
 * @param frictionPoint The clutch friction point.
 *
 * @return  The clutch state.
 */
static uint8_t calculateClutchState(uint32_t *rawValues, uint8_t frictionPoint)
{
  // TODO: range might be needed to compensate for noise.
  uint8_t state;
  uint8_t rawValIdx = 0;
  int releasedIdx = -1;
  uint8_t unreleasedIdx;
  float positionRatio;

  while(rawValIdx < CLUTCH_READER_CHAN_CNT && releasedIdx < 0)
  {
    if(rawValues[rawValIdx] == rawLimits[rawValIdx][1])
      releasedIdx = rawValIdx;
    ++rawValIdx;
  }

  if(releasedIdx < 0)
    state = CLUTCH_MAX_VALUE;
  else
  {
    unreleasedIdx = releasedIdx == 0 ? 1 : 0;
    positionRatio = ((float)rawValues[unreleasedIdx] - rawLimits[unreleasedIdx][0]) /
      (rawLimits[unreleasedIdx][1] - rawLimits[unreleasedIdx][0]);
    positionRatio = 1 - positionRatio;
    state = (uint8_t)(frictionPoint * positionRatio + 0.5);
  }

  return state;
}

/**
 * @brief   Clutch reader thread entry.
 *
 * @param p1  The user first parameter.
 * @param p2  The user second parameter.
 * @param p3  The user third parameter.
 */
static void clutchReaderThread(void *p1, void *p2, void *p3)
{
  int rc;
  uint32_t rawValues[CLUTCH_READER_CHAN_CNT];

  for(;;)
  {
    rc = sampleClutchRawValues(rawValues);
    if(rc < 0)
      // TODO: fatal error management.
      return;

    clutchState = calculateClutchState(rawValues, frictionPoint);
  }
}

int clutchReaderInit(void)
{
  int rc;
  ZephyrAdcChanConfig configs[CLUTCH_READER_CHAN_CNT];

  for(uint8_t i = 0; i < CLUTCH_READER_CHAN_CNT; ++i)
  {
    configs[i].id = i;
    configs[i].acqTime = ADC_ACQ_56_CYCLES;
    configs[i].gain = ADC_UNIT_GAIN;
    configs[i].ref = ADC_VDD_REF;
  }

  rc = zephyrAdcInit(configs, CLUTCH_READER_CHAN_CNT, ADC_12BITS_RES, 3300);
  if(rc < 0)
    return rc;

  thread.entry = clutchReaderThread;
  thread.p1 = NULL;
  thread.p2 = NULL;
  thread.p3 = NULL;
  zephyrThreadCreate(&thread, CLUTCH_READER_THREAD_NAME, ZEPHYR_TIME_NO_WAIT,
    MILLI_SEC);

  return rc;
}

/** @} */
