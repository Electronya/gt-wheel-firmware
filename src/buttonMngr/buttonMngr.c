/**
 * Copyright (C) 2023 by Electronya
 *
 * @file      buttonMngr.h
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

#define BUTTON_MNGR_MODULE_NAME button_mngr_module

/* Setting module logging */
LOG_MODULE_REGISTER(BUTTON_MNGR_MODULE_NAME);

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
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(row8), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(row9), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(row10), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(row11), gpios, {0}) },
};

ZephyrGpio columns[BUTTON_COL_COUNT] = {
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(col0), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(col1), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(col2), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(col3), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(col4), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(col5), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(col6), gpios, {0}) },
};

ZephyrGpio shifters[BUTTON_SHIFTER_COUNT] = {
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(shifterleft), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(shifterright), gpios, {0}) },
};

ZephyrGpio rockers[BUTTON_ROCKER_COUNT] = {
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(rockerleft), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(rockerright), gpios, {0}) },
};
#else
ZephyrGpio rows[BUTTON_ROW_COUNT];
ZephyrGpio columns[BUTTON_COL_COUNT];
ZephyrGpio shifters[BUTTON_SHIFTER_COUNT];
ZephyrGpio rockers[BUTTON_ROCKER_COUNT];
#endif

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

  return rc;
}

int buttonMngrGetAllStates(WheelButtonState *states, size_t count)
{
  if(count != BUTTON_COUNT)
    return -EINVAL;

  bytecpy(states, buttonStates, count * sizeof(WheelButtonState));

  return 0;
}

int buttonMngrGetEncoderStates(WheelButtonState *states, size_t count)
{
  if(count != BUTTON_ENCODER_COUNT)
    return -EINVAL;

  states[0] = buttonStates[LEFT_ENC_MODE_IDX];
  states[1] = buttonStates[RIGHT_ENC_MODE_IDX];

  return 0;
}

/** @} */
