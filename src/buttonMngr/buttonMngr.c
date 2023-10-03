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
#define BUTTON_MNGR_ENC_SIG_CNT     2

/**
 * @brief The encoder indexes
*/
enum
{
  LEFT_ENC_IDX = 0,                         /**< The left encoder index. */
  RIGHT_ENC_IDX,                            /**< The right encoder index. */
  TC_ENC_IDX,                               /**< The TC encoder index. */
  TC1_ENC_IDX,                              /**< The TC1 encoder index. */
  ABS_ENC_IDX,                              /**< The ABS encoder index. */
  MAP_ENC_IDX,                              /**< The MAP encoder index. */
  ENCODER_COUNT,                            /**< The total encoder count. */
};

/**
 * @brief The wheel encoder state.
 */
typedef enum
{
  ENCODER_NO_CHANGE,                        /**< The encoder no change state. */
  ENCODER_INCREMENT,                        /**< The encoder increment state. */
  ENCODER_DECREMENT,                        /**< The encoder decrement state. */
} WheelEncoderState;

/**
 * @brief The wheel encoder modes.
*/
typedef enum
{
  ENCODER_MODE_1,                           /**< The encoder mode 1. */
  ENCODER_MODE_2,                           /**< The encoder mode 2. */
} WheelEncoderMode;

#ifndef CONFIG_ZTEST
/**
 * @brief The button matrix rows.
*/
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

/**
 * @brief The button matrix columns.
*/
ZephyrGpio columns[BUTTON_COL_COUNT] = {
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(col0), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(col1), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(col2), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(col3), gpios, {0}) },
};

/**
 * @brief The shifter buttons.
*/
ZephyrGpio shifters[BUTTON_SHIFTER_COUNT] = {
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(left_shifter), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(right_shifter), gpios, {0}) },
};

/**
 * @brief The rocker buttons.
*/
ZephyrGpio rockers[BUTTON_ROCKER_COUNT] = {
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(left_rocker), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(right_rocker), gpios, {0}) },
};

/**
 * @brief The left encoder signals.
*/
ZephyrGpio leftEncoder[BUTTON_MNGR_ENC_SIG_CNT] = {
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(left_enc_a), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(left_enc_b), gpios, {0}) },
};

/**
 * @brief The right encoder signals.
*/
ZephyrGpio rightEncoder[BUTTON_MNGR_ENC_SIG_CNT] = {
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(right_enc_a), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(right_enc_b), gpios, {0}) },
};

/**
 * @brief The TC encoder signals.
*/
ZephyrGpio tcEncoder[BUTTON_MNGR_ENC_SIG_CNT] = {
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(tc_enc_a), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(tc_enc_b), gpios, {0}) },
};

/**
 * @brief The TC1 encoder signals.
*/
ZephyrGpio tc1Encoder[BUTTON_MNGR_ENC_SIG_CNT] = {
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(tc1_enc_a), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(tc1_enc_b), gpios, {0}) },
};

/**
 * @brief The ABS encoder signals.
*/
ZephyrGpio absEncoder[BUTTON_MNGR_ENC_SIG_CNT] = {
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(abs_enc_a), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(abs_enc_b), gpios, {0}) },
};

/**
 * @brief The MAP encoder signals.
*/
ZephyrGpio mapEncoder[BUTTON_MNGR_ENC_SIG_CNT] = {
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(map_enc_a), gpios, {0}) },
  { .dev = GPIO_DT_SPEC_GET_OR(DT_ALIAS(map_enc_b), gpios, {0}) },
};
#else
ZephyrGpio rows[BUTTON_ROW_COUNT];
ZephyrGpio columns[BUTTON_COL_COUNT];
ZephyrGpio shifters[BUTTON_SHIFTER_COUNT];
ZephyrGpio rockers[BUTTON_ROCKER_COUNT];
ZephyrGpio leftEncoder[BUTTON_MNGR_ENC_SIG_CNT];
ZephyrGpio rightEncoder[BUTTON_MNGR_ENC_SIG_CNT];
ZephyrGpio tcEncoder[BUTTON_MNGR_ENC_SIG_CNT];
ZephyrGpio tc1Encoder[BUTTON_MNGR_ENC_SIG_CNT];
ZephyrGpio absEncoder[BUTTON_MNGR_ENC_SIG_CNT];
ZephyrGpio mapEncoder[BUTTON_MNGR_ENC_SIG_CNT];
#endif

/**
 * @brief The thread stack.
*/
K_THREAD_STACK_DEFINE(buttonThreadStack, BUTTON_MNGR_STACK_SIZE);

/**
 * @brief The thread.
*/
static ZephyrThread thread = {
  .stack = buttonThreadStack,
  .stackSize = BUTTON_MNGR_STACK_SIZE,
  .priority = 2,
  .options = 0,
};

/**
 * @brief The button states.
*/
static WheelButtonState buttonStates[BUTTON_COUNT];

/**
 * @brief The encoder modes.
*/
static WheelEncoderMode encModes[RIGHT_ENC_IDX + 1] = {ENCODER_MODE_1,
                                                       ENCODER_MODE_1};

/**
 * @brief The encoder signal states.
*/
static uint8_t encSigStates[ENCODER_COUNT] = {0, 0, 0, 0, 0, 0};

/**
 * @brief   Process encoder signals to get its state.
 *
 * @param gpios       The encoder GPIO signals.
 * @param states      The encoder signal states.
 *
 * @return  The processed encoder state.
 */
static WheelEncoderState processEncoderIrq(ZephyrGpio *gpios, uint8_t *states)
{
  ZephyrGpioState gpioState;
  WheelEncoderState encState;
  int8_t lookup[] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};

  *states = (*states << 2) & 0x0f;
  for(uint8_t i = 0; i < BUTTON_MNGR_ENC_SIG_CNT; ++i)
  {
    gpioState = zephyrGpioRead(gpios + i);
    if(gpioState < 0)
      // TODO: fatal error management.
      LOG_ERR("unable to read encoder GPIO %s", gpios[i].dev.port->name);
    if(i == 0)
      *states |= ((uint8_t)gpioState << 1);
    else
      *states |= (uint8_t)gpioState;
  }

  if(lookup[*states] > 0)
    encState = ENCODER_INCREMENT;
  else if(lookup[*states] < 0)
    encState = ENCODER_DECREMENT;
  else
    encState = ENCODER_NO_CHANGE;

  return encState;
}

/**
 * @brief   Left encoder GPIO IRQ.
 *
 * @param dev         The device structure of the GPIO causing the IRQ.
 * @param cb          The IRQ callback structure.
 * @param pin         The pin number of the GPIO that triggered the interrupt.
 */
static void leftEncoderIrq(const struct device *dev, struct gpio_callback *cb,
                           uint32_t pin)
{
  WheelEncoderState state;

  state = processEncoderIrq(leftEncoder, encSigStates + LEFT_ENC_IDX);

  if(state == ENCODER_INCREMENT)
    buttonStates[LEFT_ENC_M1_INC_IDX + encModes[LEFT_ENC_IDX]] = BUTTON_PRESSED;
  else if(state == ENCODER_DECREMENT)
    buttonStates[LEFT_ENC_M1_DEC_IDX + encModes[LEFT_ENC_IDX]] = BUTTON_PRESSED;
}

/**
 * @brief   Right encoder GPIO IRQ.
 *
 * @param dev         The device structure of the GPIO causing the IRQ.
 * @param cb          The IRQ callback structure.
 * @param pin         The pin number of the GPIO that triggered the interrupt.
 */
static void rightEncoderIrq(const struct device *dev, struct gpio_callback *cb,
                            uint32_t pin)
{
  WheelEncoderState state;

  state = processEncoderIrq(rightEncoder, encSigStates + RIGHT_ENC_IDX);

  if(state == ENCODER_INCREMENT)
    buttonStates[BB_INC_IDX + encModes[RIGHT_ENC_IDX]] = BUTTON_PRESSED;
  else if(state == ENCODER_DECREMENT)
    buttonStates[BB_DEC_IDX + encModes[RIGHT_ENC_IDX]] = BUTTON_PRESSED;
}

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
