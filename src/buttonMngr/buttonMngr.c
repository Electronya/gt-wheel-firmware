/**
 * Copyright (C) 2023 by Electronya
 *
 * @file      buttonMngr.c
 * @author    jbacon
 * @date      2023-07-31
 * @brief     Button Manager Module
 *
 *            This file is the implementation of the button manager module.
 *
 * @ingroup  buttonMngr
 *
 * @{
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

#include "buttonMngr.h"
#include "zephyrCommon.h"
#include "zephyrGpio.h"
#include "zephyrThread.h"

#define BUTTON_MNGR_MODULE_NAME button_mngr_module

/* Setting module logging */
LOG_MODULE_REGISTER(BUTTON_MNGR_MODULE_NAME);

/**
 * @brief The thread stack size.
*/
#define BUTTON_MNGR_STACK_SIZE      256

/**
 * @brief The thread name.
 */
#define BUTTON_MNGR_THREAD_NAME     "buttonMngr"

/**
 * @brief The encoder signal count.
*/
#define BUTTON_MNGR_ENC_SIGNAL_CNT  2

#ifndef CONFIG_ZTEST
ZephyrGpio rows[BUTTON_ROW_COUNT] = {
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(row0), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(row1), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(row2), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(row3), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(row4), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(row5), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(row6), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(row7), gpios, {0}) },
};

ZephyrGpio columns[BUTTON_COL_COUNT] = {
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(col0), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(col1), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(col2), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(col3), gpios, {0}) },
};

ZephyrGpio shifters[BUTTON_SHIFTER_COUNT] = {
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(left_shifter), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(right_shifter), gpios, {0}) },
};

ZephyrGpio rockers[BUTTON_ROCKER_COUNT] = {
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(left_rocker), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(right_rocker), gpios, {0}) },
};

ZephyrGpio leftEncoder[BUTTON_MNGR_ENC_SIGNAL_CNT] = {
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(left_enc_a), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(left_enc_b), gpios, {0}) },
};

ZephyrGpio rightEncoder[BUTTON_MNGR_ENC_SIGNAL_CNT] = {
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(right_enc_a), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(right_enc_b), gpios, {0}) },
};

ZephyrGpio tcEncoder[BUTTON_MNGR_ENC_SIGNAL_CNT] = {
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(tc_enc_a), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(tc_enc_b), gpios, {0}) },
};

ZephyrGpio tc1Encoder[BUTTON_MNGR_ENC_SIGNAL_CNT] = {
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(tc1_enc_a), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(tc1_enc_b), gpios, {0}) },
};

ZephyrGpio absEncoder[BUTTON_MNGR_ENC_SIGNAL_CNT] = {
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(abs_enc_a), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(abs_enc_b), gpios, {0}) },
};

ZephyrGpio mapEncoder[BUTTON_MNGR_ENC_SIGNAL_CNT] = {
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(map_enc_a), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(map_enc_b), gpios, {0}) },
};
#else
ZephyrGpio rows[BUTTON_ROW_COUNT];
ZephyrGpio columns[BUTTON_COL_COUNT];
ZephyrGpio shifters[BUTTON_SHIFTER_COUNT];
ZephyrGpio rockers[BUTTON_ROCKER_COUNT];
ZephyrGpio leftEncoder[BUTTON_MNGR_ENC_SIGNAL_CNT];
ZephyrGpio rightEncoder[BUTTON_MNGR_ENC_SIGNAL_CNT];
ZephyrGpio tcEncoder[BUTTON_MNGR_ENC_SIGNAL_CNT];
ZephyrGpio tc1Encoder[BUTTON_MNGR_ENC_SIGNAL_CNT];
ZephyrGpio absEncoder[BUTTON_MNGR_ENC_SIGNAL_CNT];
ZephyrGpio mapEncoder[BUTTON_MNGR_ENC_SIGNAL_CNT];
#endif

K_THREAD_STACK_DEFINE(buttonThreadStack, BUTTON_MNGR_STACK_SIZE);
static ZephyrThread thread = {
  .stack = buttonThreadStack,
  .stackSize = BUTTON_MNGR_STACK_SIZE,
  .priority = 2,
  .options = 0,
};

/**
 * @brief The button states.
*/
WheelButtonState buttonStates[BUTTON_COUNT];

/**
 * @brief   Read the button matrix.
 *
 * @return  0 if successful, the error code otherwise.
 */
static int readButtonMatrix(void)
{
  int rc;
  int buttonState = 0;
  bool keepReading = true;

  for(uint8_t col = 0; col < BUTTON_COL_COUNT && keepReading; ++col)
  {
    /* set the column to read */
    rc = zephyrGpioSet(columns + col);
    if(rc < 0)
      return rc;

    for(uint8_t row = 0; row < BUTTON_ROW_COUNT && buttonState >= 0; ++row)
    {
      buttonState = zephyrGpioRead(rows + row);
      if(buttonState < 0)
        keepReading = false;
      else
        buttonStates[BUTTON_ROW_COUNT * col + row] =
          (WheelButtonState)buttonState;
    }

    /* clear the column read */
    rc = zephyrGpioClear(columns + col);
    if(rc < 0)
      keepReading = false;
  }

  if(buttonState < 0)
    rc = buttonState;

  return rc;
}

/**
 * @brief   Read the shifter buttons.
 *
 * @return  0 if successful, the error code otherwise.
 */
static int readButtonShifters(void)
{
  int rc = 0;

  for(uint8_t i = 0; i < BUTTON_SHIFTER_COUNT && rc >= 0; ++i)
  {
    rc = zephyrGpioRead(shifters + i);
    if(rc >= 0)
      buttonStates[LEFT_SHIFTER_IDX + i] = (WheelButtonState)rc;
  }

   return rc;
}

/**
 * @brief   Read the rocker buttons.
 *
 * @return  0 if successful, the error code otherwise.
 */
static int readButtonRockers(void)
{
  int rc = 0;

  for(uint8_t i = 0; i < BUTTON_SHIFTER_COUNT && rc >= 0; ++i)
  {
    rc = zephyrGpioRead(rockers + i);
    if(rc >= 0)
      buttonStates[LEFT_ROCKER_IDX + i] = (WheelButtonState)rc;
  }

   return rc;
}

/**
 * @brief   The button manager thread implementation.
 *
 * @param p1  The first parameter.
 * @param p2  The second parameter.
 * @param p3  The third parameter.
 */
static void buttonMngrThread(void *p1, void *p2, void *p3)
{
  int rc;

  for(;;)
  {
    rc = readButtonMatrix();
    if(rc < 0)
      LOG_ERR("unable to read button matrix");

    rc = readButtonShifters();
    if(rc < 0)
      LOG_ERR("unable to read shifters");

    rc = readButtonRockers();
    if(rc < 0)
      LOG_ERR("unable to read rockers");

    zephyrThreadSleepMs(100);
  }
}

int buttonMngrInit(void)
{
  int rc = 0;

  for(uint8_t i = 0; i < BUTTON_ROW_COUNT && rc == 0; ++i)
    rc = zephyrGpioInit(rows + i, GPIO_IN);

  for(uint8_t i = 0; i < BUTTON_COL_COUNT && rc == 0; ++i)
    rc = zephyrGpioInit(columns + i, GPIO_OUT_CLR);

  for(uint8_t i = 0; i < BUTTON_SHIFTER_COUNT && rc == 0; ++i)
    rc = zephyrGpioInit(shifters + i, GPIO_IN);

  for(uint8_t i = 0; i < BUTTON_ROCKER_COUNT && rc == 0; ++i)
    rc = zephyrGpioInit(rockers + i, GPIO_IN);

  if(rc == 0)
  {
    thread.entry = buttonMngrThread;
    thread.p1 = NULL;
    thread.p2 = NULL;
    thread.p3 = NULL;
    zephyrThreadCreate(&thread, BUTTON_MNGR_THREAD_NAME,
      ZEPHYR_TIME_NO_WAIT, MILLI_SEC);
  }

  return rc;
}

int buttonMngrGetAllStates(WheelButtonState *states, size_t count)
{
  if(count != BUTTON_COUNT)
    return -EINVAL;

  bytecpy(states, buttonStates, count * sizeof(WheelButtonState));

  return 0;
}

/** @} */
