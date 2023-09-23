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
 * @brief   Clutch reader thread entry.
 *
 * @param p1  The user first parameter.
 * @param p2  The user second parameter.
 * @param p3  The user third parameter.
 */
static void clutchReaderThread(void *p1, void *p2, void *p3)
{

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
