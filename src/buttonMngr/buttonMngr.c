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

#include "buttonMngr.h"
#include "zephyrCommon.h"
#include "zephyrGpio.h"

#define BUTTON_MNGR_MODULE_NAME button_mngr_module

/**
 * @brief The button row count.
*/
#define BUTTON_ROW_COUNT        12

/**
 * @brief The button column count.
*/
#define BUTTON_COL_COUNT        6

/**
 * @brief The shifter button count.
*/
#define BUTTON_SHIFTER_COUNT    2

/**
 * @brief The rocker button count.
*/
#define BUTTON_ROCKER_COUNT     2

/**
 * @brief The encoder button count.
*/
#define BUTTON_ENCODER_COUNT    2

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
 * @brief The button matrix states.
*/
WheelButtonState matrixStates[BUTTON_COL_COUNT][BUTTON_ROW_COUNT];

/**
 * @brief The left shifter (down) state.
*/
WheelButtonState leftShifterState;

/**
 * @brief The right shifter (up) state.
*/
WheelButtonState rightShifterState;

/**
 * @brief The left rocker state.
*/
WheelButtonState leftRockerState;

/**
 * @brief The right rocker state.
*/
WheelButtonState rightRockerState;

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

/** @} */
