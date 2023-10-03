/**
 * Copyright (C) 2023 by Electronya
 *
 * @file      test_ledCtrl.c
 * @author    jbacon
 * @date      2023-07-19
 * @brief     Led Control Module Test Cases
 *
 *            This file is the test cases of the LED control module.
 *
 * @ingroup  ledCtrl
 *
 * @{
 */

#define FFF_ARG_HISTORY_LEN 84

#include <zephyr/ztest.h>
#include <zephyr/fff.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

#include "buttonMngr.h"
#include "buttonMngr.c"

#include "zephyrGpio.h"
#include "zephyrThread.h"

DEFINE_FFF_GLOBALS;

/* mocks */
FAKE_VALUE_FUNC(int, zephyrGpioInit, ZephyrGpio*, ZephyrGpioDir);
FAKE_VALUE_FUNC(int, zephyrGpioSet, ZephyrGpio*);
FAKE_VALUE_FUNC(int, zephyrGpioClear, ZephyrGpio*);
FAKE_VALUE_FUNC(int, zephyrGpioRead, ZephyrGpio*);
FAKE_VALUE_FUNC(uint32_t, zephyrThreadSleepMs, uint32_t);
FAKE_VOID_FUNC(zephyrThreadCreate, ZephyrThread*, char*, uint32_t,
               ZephyrTimeUnit);

/**
 * @brief The total of row and column GPIOs.
*/
#define TOTAL_ROW_COL_COUNT   (BUTTON_ROW_COUNT + BUTTON_COL_COUNT)

/**
 * @brief The total number of GPIOs.
*/
#define TOTAL_GPIO_COUNT      TOTAL_ROW_COL_COUNT + BUTTON_SHIFTER_COUNT + \
                              BUTTON_ROCKER_COUNT

/**
 * @brief The test fixture.
*/
struct buttonMngr_suite_fixture
{
  int gpioInitRetVals[TOTAL_GPIO_COUNT];                    /**< GPIO init mock return values. */
  int colSetRetVals[BUTTON_COL_COUNT];                      /**< Column set return values. */
  int colClearRetVals[BUTTON_COL_COUNT];                    /**< Column clear return values. */
  int readRetVals[BUTTON_ROW_COUNT * BUTTON_COL_COUNT];     /**< Row read return values. */
  WheelButtonState buttonStates[BUTTON_COUNT];              /**< The button states. */
};

static void *buttonMngrSuiteSetup(void)
{
  struct buttonMngr_suite_fixture *fixture =
    k_malloc(sizeof(struct buttonMngr_suite_fixture));
  zassume_not_null(fixture, NULL);

  return (void *)fixture;
}

static void buttonMngrSuiteTeardown(void *f)
{
  k_free(f);
}

static void buttonMngrCaseSetup(void *f)
{
  int successRet = 0;
  struct buttonMngr_suite_fixture *fixture =
    (struct buttonMngr_suite_fixture *)f;

  for(uint8_t i = 0; i < TOTAL_GPIO_COUNT; ++i)
    fixture->gpioInitRetVals[i] = successRet;

  for(uint8_t i = 0; i < BUTTON_COL_COUNT; ++i)
  {
    fixture->colSetRetVals[i] = successRet;
    fixture->colClearRetVals[i] = successRet;
  }

  for(uint8_t i = 0; i < BUTTON_ROW_COUNT * BUTTON_COL_COUNT; ++i)
  {
    if(i % 2)
      fixture->readRetVals[i] = BUTTON_DEPRESSED;
    else
      fixture->readRetVals[i] = BUTTON_PRESSED;
  }

  for(uint8_t i = 0; i < BUTTON_COUNT; ++i)
  {
    if(i % 2)
      fixture->buttonStates[i] = BUTTON_DEPRESSED;
    else
      fixture->buttonStates[i] = BUTTON_PRESSED;
  }

  for(uint8_t i = 0; i < BUTTON_COUNT; ++i)
  {
    if(i % 2)
      buttonStates[i] = BUTTON_PRESSED;
    else
      buttonStates[i] = BUTTON_DEPRESSED;
  }

  for(uint8_t i = 0; i < RIGHT_ENC_IDX + 1; ++i)
    encModes[i] = ENCODER_MODE_1;

  RESET_FAKE(zephyrGpioInit);
  RESET_FAKE(zephyrGpioSet);
  RESET_FAKE(zephyrGpioClear);
  RESET_FAKE(zephyrGpioRead);
  RESET_FAKE(zephyrThreadSleepMs);
  RESET_FAKE(zephyrThreadCreate);
}

ZTEST_SUITE(buttonMngr_suite, NULL, buttonMngrSuiteSetup, buttonMngrCaseSetup,
  NULL, buttonMngrSuiteTeardown);

#define ENC_NO_CHANGE_STATE_TEST_CNT        8
/**
 * @test  processEncoderIrq must read the current encoder gpio state, determine
 *        if the encoder is in no change state and return that state.
*/
ZTEST(buttonMngr_suite, test_processEncoderIrq_NoChangeState)
{
  uint8_t prevStates[ENC_NO_CHANGE_STATE_TEST_CNT] = {0, 3, 1, 2, 1, 2, 0, 3};
  uint8_t states[ENC_NO_CHANGE_STATE_TEST_CNT] = {0, 12, 5, 9, 6, 10, 3, 15};
  int gpioStates[ENC_NO_CHANGE_STATE_TEST_CNT][BUTTON_MNGR_ENC_SIG_CNT] =
    {{GPIO_CLR, GPIO_CLR}, {GPIO_CLR, GPIO_CLR},
     {GPIO_CLR, GPIO_SET}, {GPIO_CLR, GPIO_SET},
     {GPIO_SET, GPIO_CLR}, {GPIO_SET, GPIO_CLR},
     {GPIO_SET, GPIO_SET}, {GPIO_SET, GPIO_SET}};

  for(uint8_t i = 0; i < ENC_NO_CHANGE_STATE_TEST_CNT; ++i)
  {
    SET_RETURN_SEQ(zephyrGpioRead, gpioStates[i], BUTTON_MNGR_ENC_SIG_CNT);

    zassert_equal(ENCODER_NO_CHANGE,
      processEncoderIrq(leftEncoder, prevStates + i));
    zassert_equal(2, zephyrGpioRead_fake.call_count);
    zassert_equal(leftEncoder, zephyrGpioRead_fake.arg0_history[0]);
    zassert_equal(leftEncoder + 1, zephyrGpioRead_fake.arg0_history[1]);
    zassert_equal(states[i], prevStates[i]);

    RESET_FAKE(zephyrGpioRead);
  }
}

#define ENC_CHANGE_STATE_TEST_CNT           4
/**
 * @test  processEncoderIrq must read the current encoder gpio state, determine
 *        if the encoder is in incrementing state and return that state.
*/
ZTEST(buttonMngr_suite, test_processEncoderIrq_IncrementState)
{
  uint8_t prevStates[ENC_CHANGE_STATE_TEST_CNT] = {1, 3, 0, 2};
  uint8_t states[ENC_CHANGE_STATE_TEST_CNT] = {4, 13, 2, 11};
  int gpioStates[ENC_CHANGE_STATE_TEST_CNT][BUTTON_MNGR_ENC_SIG_CNT] =
    {{GPIO_CLR, GPIO_CLR}, {GPIO_CLR, GPIO_SET},
     {GPIO_SET, GPIO_CLR}, {GPIO_SET, GPIO_SET}};

  for(uint8_t i = 0; i < ENC_CHANGE_STATE_TEST_CNT; ++i)
  {
    SET_RETURN_SEQ(zephyrGpioRead, gpioStates[i], BUTTON_MNGR_ENC_SIG_CNT);

    zassert_equal(ENCODER_INCREMENT,
      processEncoderIrq(leftEncoder, prevStates + i));
    zassert_equal(2, zephyrGpioRead_fake.call_count);
    zassert_equal(leftEncoder, zephyrGpioRead_fake.arg0_history[0]);
    zassert_equal(leftEncoder + 1, zephyrGpioRead_fake.arg0_history[1]);
    zassert_equal(states[i], prevStates[i]);

    RESET_FAKE(zephyrGpioRead);
  }
}

/**
 * @test  processEncoderIrq must read the current encoder gpio state, determine
 *        if the encoder is in decrementing state and return that state.
*/
ZTEST(buttonMngr_suite, test_processEncoderIrq_DecrementState)
{
  uint8_t prevStates[ENC_CHANGE_STATE_TEST_CNT] = {2, 0, 3, 1};
  uint8_t states[ENC_CHANGE_STATE_TEST_CNT] = {8, 1, 14, 7};
  int gpioStates[ENC_CHANGE_STATE_TEST_CNT][BUTTON_MNGR_ENC_SIG_CNT] =
    {{GPIO_CLR, GPIO_CLR}, {GPIO_CLR, GPIO_SET},
     {GPIO_SET, GPIO_CLR}, {GPIO_SET, GPIO_SET}};

  for(uint8_t i = 0; i < ENC_CHANGE_STATE_TEST_CNT; ++i)
  {
    SET_RETURN_SEQ(zephyrGpioRead, gpioStates[i], BUTTON_MNGR_ENC_SIG_CNT);

    zassert_equal(ENCODER_DECREMENT,
      processEncoderIrq(leftEncoder, prevStates + i));
    zassert_equal(2, zephyrGpioRead_fake.call_count);
    zassert_equal(leftEncoder, zephyrGpioRead_fake.arg0_history[0]);
    zassert_equal(leftEncoder + 1, zephyrGpioRead_fake.arg0_history[1]);
    zassert_equal(states[i], prevStates[i]);

    RESET_FAKE(zephyrGpioRead);
  }
}

#define ENC_STATE_BUTTONS_TEST_CNT          3
/**
 * @test  leftEncoderIrq must process the left encoder signals and set the
 *        encoder M1 increment/decrement/ no change buttons states when the
 *        encoder is in mode M1.
*/
ZTEST(buttonMngr_suite, test_leftEncoderIrq_ButtonUpdateStateM1)
{
  uint8_t prevStates[ENC_STATE_BUTTONS_TEST_CNT] = {0, 1, 2};
  int gpioStates[BUTTON_MNGR_ENC_SIG_CNT] = {GPIO_CLR, GPIO_CLR};
  WheelButtonState expectedStates[ENC_STATE_BUTTONS_TEST_CNT][2] =
    {{BUTTON_DEPRESSED, BUTTON_DEPRESSED},
     {BUTTON_PRESSED, BUTTON_DEPRESSED},
     {BUTTON_DEPRESSED, BUTTON_PRESSED}};

  for(uint8_t i = 0; i < ENC_STATE_BUTTONS_TEST_CNT; ++i)
  {
    SET_RETURN_SEQ(zephyrGpioRead, gpioStates, BUTTON_MNGR_ENC_SIG_CNT);

    buttonStates[LEFT_ENC_M1_INC_IDX] = BUTTON_DEPRESSED;
    buttonStates[LEFT_ENC_M1_DEC_IDX] = BUTTON_DEPRESSED;
    encSigStates[LEFT_ENC_IDX] = prevStates[i];

    leftEncoderIrq(NULL, NULL, 0);
    zassert_equal(2, zephyrGpioRead_fake.call_count);
    zassert_equal(leftEncoder, zephyrGpioRead_fake.arg0_history[0]);
    zassert_equal(leftEncoder + 1, zephyrGpioRead_fake.arg0_history[1]);
    zassert_equal(expectedStates[i][0], buttonStates[LEFT_ENC_M1_INC_IDX]);
    zassert_equal(expectedStates[i][1], buttonStates[LEFT_ENC_M1_DEC_IDX]);

    RESET_FAKE(zephyrGpioRead);
  }
}

/**
 * @test  leftEncoderIrq must process the left encoder signals and set the
 *        encoder M2 increment/decrement/ no change buttons states when the
 *        encoder is in mode M2.
*/
ZTEST(buttonMngr_suite, test_leftEncoderIrq_ButtonUpdateStateM2)
{
  uint8_t prevStates[ENC_STATE_BUTTONS_TEST_CNT] = {0, 1, 2};
  int gpioStates[BUTTON_MNGR_ENC_SIG_CNT] = {GPIO_CLR, GPIO_CLR};
  WheelButtonState expectedStates[ENC_STATE_BUTTONS_TEST_CNT][2] =
    {{BUTTON_DEPRESSED, BUTTON_DEPRESSED},
     {BUTTON_PRESSED, BUTTON_DEPRESSED},
     {BUTTON_DEPRESSED, BUTTON_PRESSED}};

  encModes[LEFT_ENC_IDX] = ENCODER_MODE_2;

  for(uint8_t i = 0; i < ENC_STATE_BUTTONS_TEST_CNT; ++i)
  {
    SET_RETURN_SEQ(zephyrGpioRead, gpioStates, BUTTON_MNGR_ENC_SIG_CNT);

    buttonStates[LEFT_ENC_M2_INC_IDX] = BUTTON_DEPRESSED;
    buttonStates[LEFT_ENC_M2_DEC_IDX] = BUTTON_DEPRESSED;
    encSigStates[LEFT_ENC_IDX] = prevStates[i];

    leftEncoderIrq(NULL, NULL, 0);
    zassert_equal(2, zephyrGpioRead_fake.call_count);
    zassert_equal(leftEncoder, zephyrGpioRead_fake.arg0_history[0]);
    zassert_equal(leftEncoder + 1, zephyrGpioRead_fake.arg0_history[1]);
    zassert_equal(expectedStates[i][0], buttonStates[LEFT_ENC_M2_INC_IDX]);
    zassert_equal(expectedStates[i][1], buttonStates[LEFT_ENC_M2_DEC_IDX]);

    RESET_FAKE(zephyrGpioRead);
  }
}

/**
 * @test  rightEncoderIrq must process the right encoder signals and set the
 *        encoder M1 increment/decrement/no change buttons states when the
 *        encoder is in mode M1.
*/
ZTEST(buttonMngr_suite, test_rightEncoderIrq_ButtonUpdateStateM1)
{
  uint8_t prevStates[ENC_STATE_BUTTONS_TEST_CNT] = {0, 1, 2};
  int gpioStates[BUTTON_MNGR_ENC_SIG_CNT] = {GPIO_CLR, GPIO_CLR};
  WheelButtonState expectedStates[ENC_STATE_BUTTONS_TEST_CNT][2] =
    {{BUTTON_DEPRESSED, BUTTON_DEPRESSED},
     {BUTTON_PRESSED, BUTTON_DEPRESSED},
     {BUTTON_DEPRESSED, BUTTON_PRESSED}};

  for(uint8_t i = 0; i < ENC_STATE_BUTTONS_TEST_CNT; ++i)
  {
    SET_RETURN_SEQ(zephyrGpioRead, gpioStates, BUTTON_MNGR_ENC_SIG_CNT);

    buttonStates[BB_INC_IDX] = BUTTON_DEPRESSED;
    buttonStates[BB_DEC_IDX] = BUTTON_DEPRESSED;
    encSigStates[RIGHT_ENC_IDX] = prevStates[i];

    rightEncoderIrq(NULL, NULL, 0);
    zassert_equal(2, zephyrGpioRead_fake.call_count);
    zassert_equal(rightEncoder, zephyrGpioRead_fake.arg0_history[0]);
    zassert_equal(rightEncoder + 1, zephyrGpioRead_fake.arg0_history[1]);
    zassert_equal(expectedStates[i][0], buttonStates[BB_INC_IDX]);
    zassert_equal(expectedStates[i][1], buttonStates[BB_DEC_IDX]);

    RESET_FAKE(zephyrGpioRead);
  }
}

/**
 * @test  rightEncoderIrq must process the right encoder signals and set the
 *        encoder M2 increment/decrement/no change buttons states when the
 *        encoder is in mode M2.
*/
ZTEST(buttonMngr_suite, test_rightEncoderIrq_ButtonUpdateStateM2)
{
  uint8_t prevStates[ENC_STATE_BUTTONS_TEST_CNT] = {0, 1, 2};
  int gpioStates[BUTTON_MNGR_ENC_SIG_CNT] = {GPIO_CLR, GPIO_CLR};
  WheelButtonState expectedStates[ENC_STATE_BUTTONS_TEST_CNT][2] =
    {{BUTTON_DEPRESSED, BUTTON_DEPRESSED},
     {BUTTON_PRESSED, BUTTON_DEPRESSED},
     {BUTTON_DEPRESSED, BUTTON_PRESSED}};

  encModes[RIGHT_ENC_IDX] = ENCODER_MODE_2;

  for(uint8_t i = 0; i < ENC_STATE_BUTTONS_TEST_CNT; ++i)
  {
    SET_RETURN_SEQ(zephyrGpioRead, gpioStates, BUTTON_MNGR_ENC_SIG_CNT);

    buttonStates[RIGHT_ENC_M2_INC_IDX] = BUTTON_DEPRESSED;
    buttonStates[RIGHT_ENC_M2_DEC_IDX] = BUTTON_DEPRESSED;
    encSigStates[RIGHT_ENC_IDX] = prevStates[i];

    rightEncoderIrq(NULL, NULL, 0);
    zassert_equal(2, zephyrGpioRead_fake.call_count);
    zassert_equal(rightEncoder, zephyrGpioRead_fake.arg0_history[0]);
    zassert_equal(rightEncoder + 1, zephyrGpioRead_fake.arg0_history[1]);
    zassert_equal(expectedStates[i][0], buttonStates[RIGHT_ENC_M2_INC_IDX]);
    zassert_equal(expectedStates[i][1], buttonStates[RIGHT_ENC_M2_DEC_IDX]);

    RESET_FAKE(zephyrGpioRead);
  }
}

/**
 * @test  tcEncoderIrq must process the let encoder signals and set the
 *        encoder increment/decrement/no change buttons states.
*/
ZTEST(buttonMngr_suite, test_tcEncoderIrq_ButtonUpdateState)
{
  uint8_t prevStates[ENC_STATE_BUTTONS_TEST_CNT] = {0, 1, 2};
  int gpioStates[BUTTON_MNGR_ENC_SIG_CNT] = {GPIO_CLR, GPIO_CLR};
  WheelButtonState expectedStates[ENC_STATE_BUTTONS_TEST_CNT][2] =
    {{BUTTON_DEPRESSED, BUTTON_DEPRESSED},
     {BUTTON_PRESSED, BUTTON_DEPRESSED},
     {BUTTON_DEPRESSED, BUTTON_PRESSED}};

  for(uint8_t i = 0; i < ENC_STATE_BUTTONS_TEST_CNT; ++i)
  {
    SET_RETURN_SEQ(zephyrGpioRead, gpioStates, BUTTON_MNGR_ENC_SIG_CNT);

    buttonStates[TC_INC_IDX] = BUTTON_DEPRESSED;
    buttonStates[TC_DEC_IDX] = BUTTON_DEPRESSED;
    encSigStates[TC_ENC_IDX] = prevStates[i];

    tcEncoderIrq(NULL, NULL, 0);
    zassert_equal(2, zephyrGpioRead_fake.call_count);
    zassert_equal(tcEncoder, zephyrGpioRead_fake.arg0_history[0]);
    zassert_equal(tcEncoder + 1, zephyrGpioRead_fake.arg0_history[1]);
    zassert_equal(expectedStates[i][0], buttonStates[TC_INC_IDX]);
    zassert_equal(expectedStates[i][1], buttonStates[TC_DEC_IDX]);

    RESET_FAKE(zephyrGpioRead);
  }
}

/**
 * @test  tc1EncoderIrq must process the let encoder signals and set the
 *        encoder M1 increment/decrement/no change buttons states.
*/
ZTEST(buttonMngr_suite, test_tc1EncoderIrq_ButtonUpdateState)
{
  uint8_t prevStates[ENC_STATE_BUTTONS_TEST_CNT] = {0, 1, 2};
  int gpioStates[BUTTON_MNGR_ENC_SIG_CNT] = {GPIO_CLR, GPIO_CLR};
  WheelButtonState expectedStates[ENC_STATE_BUTTONS_TEST_CNT][2] =
    {{BUTTON_DEPRESSED, BUTTON_DEPRESSED},
     {BUTTON_PRESSED, BUTTON_DEPRESSED},
     {BUTTON_DEPRESSED, BUTTON_PRESSED}};

  for(uint8_t i = 0; i < ENC_STATE_BUTTONS_TEST_CNT; ++i)
  {
    SET_RETURN_SEQ(zephyrGpioRead, gpioStates, BUTTON_MNGR_ENC_SIG_CNT);

    buttonStates[TC1_INC_IDX] = BUTTON_DEPRESSED;
    buttonStates[TC1_DEC_IDX] = BUTTON_DEPRESSED;
    encSigStates[TC1_ENC_IDX] = prevStates[i];

    tc1EncoderIrq(NULL, NULL, 0);
    zassert_equal(2, zephyrGpioRead_fake.call_count);
    zassert_equal(tc1Encoder, zephyrGpioRead_fake.arg0_history[0]);
    zassert_equal(tc1Encoder + 1, zephyrGpioRead_fake.arg0_history[1]);
    zassert_equal(expectedStates[i][0], buttonStates[TC1_INC_IDX]);
    zassert_equal(expectedStates[i][1], buttonStates[TC1_DEC_IDX]);

    RESET_FAKE(zephyrGpioRead);
  }
}

/**
 * @test  absEncoderIrq must process the let encoder signals and set the
 *        encoder M1 increment/decrement/no change buttons states.
*/
ZTEST(buttonMngr_suite, test_absEncoderIrq_ButtonUpdateState)
{
  uint8_t prevStates[ENC_STATE_BUTTONS_TEST_CNT] = {0, 1, 2};
  int gpioStates[BUTTON_MNGR_ENC_SIG_CNT] = {GPIO_CLR, GPIO_CLR};
  WheelButtonState expectedStates[ENC_STATE_BUTTONS_TEST_CNT][2] =
    {{BUTTON_DEPRESSED, BUTTON_DEPRESSED},
     {BUTTON_PRESSED, BUTTON_DEPRESSED},
     {BUTTON_DEPRESSED, BUTTON_PRESSED}};

  for(uint8_t i = 0; i < ENC_STATE_BUTTONS_TEST_CNT; ++i)
  {
    SET_RETURN_SEQ(zephyrGpioRead, gpioStates, BUTTON_MNGR_ENC_SIG_CNT);

    buttonStates[ABS_INC_IDX] = BUTTON_DEPRESSED;
    buttonStates[ABS_DEC_IDX] = BUTTON_DEPRESSED;
    encSigStates[ABS_ENC_IDX] = prevStates[i];

    absEncoderIrq(NULL, NULL, 0);
    zassert_equal(2, zephyrGpioRead_fake.call_count);
    zassert_equal(absEncoder, zephyrGpioRead_fake.arg0_history[0]);
    zassert_equal(absEncoder + 1, zephyrGpioRead_fake.arg0_history[1]);
    zassert_equal(expectedStates[i][0], buttonStates[ABS_INC_IDX]);
    zassert_equal(expectedStates[i][1], buttonStates[ABS_DEC_IDX]);

    RESET_FAKE(zephyrGpioRead);
  }
}

/**
 * @test  mapEncoderIrq must process the let encoder signals and set the
 *        encoder M1 increment/decrement/no change buttons states.
*/
ZTEST(buttonMngr_suite, test_mapEncoderIrq_ButtonUpdateState)
{
  uint8_t prevStates[ENC_STATE_BUTTONS_TEST_CNT] = {0, 1, 2};
  int gpioStates[BUTTON_MNGR_ENC_SIG_CNT] = {GPIO_CLR, GPIO_CLR};
  WheelButtonState expectedStates[ENC_STATE_BUTTONS_TEST_CNT][2] =
    {{BUTTON_DEPRESSED, BUTTON_DEPRESSED},
     {BUTTON_PRESSED, BUTTON_DEPRESSED},
     {BUTTON_DEPRESSED, BUTTON_PRESSED}};

  for(uint8_t i = 0; i < ENC_STATE_BUTTONS_TEST_CNT; ++i)
  {
    SET_RETURN_SEQ(zephyrGpioRead, gpioStates, BUTTON_MNGR_ENC_SIG_CNT);

    buttonStates[MAP_INC_IDX] = BUTTON_DEPRESSED;
    buttonStates[MAP_DEC_IDX] = BUTTON_DEPRESSED;
    encSigStates[MAP_ENC_IDX] = prevStates[i];

    mapEncoderIrq(NULL, NULL, 0);
    zassert_equal(2, zephyrGpioRead_fake.call_count);
    zassert_equal(mapEncoder, zephyrGpioRead_fake.arg0_history[0]);
    zassert_equal(mapEncoder + 1, zephyrGpioRead_fake.arg0_history[1]);
    zassert_equal(expectedStates[i][0], buttonStates[MAP_INC_IDX]);
    zassert_equal(expectedStates[i][1], buttonStates[MAP_DEC_IDX]);

    RESET_FAKE(zephyrGpioRead);
  }
}

/**
 * @test  readButtonMatrix must return the error code if any of the set column
 *        operation fails.
*/
ZTEST_F(buttonMngr_suite, test_readButtonMatrix_SetColumnFail)
{
  int failRet = -EIO;
  int successRet = 0;

  for(uint8_t i = 0; i < BUTTON_COL_COUNT; ++i)
  {
    if(i > 0)
      fixture->colSetRetVals[i - 1] = successRet;
    fixture->colSetRetVals[i] = failRet;
    SET_RETURN_SEQ(zephyrGpioSet, fixture->colSetRetVals, i + 1);

    zassert_equal(failRet, readButtonMatrix());
    zassert_equal(i + 1, zephyrGpioSet_fake.call_count);
    for(uint8_t j = 0; j > i; ++j)
      zassert_equal(columns + j, zephyrGpioSet_fake.arg0_history[j]);
    RESET_FAKE(zephyrGpioSet);
  }
}

/**
 * @test  readButtonMatrix must return the error code if any of the clear column
 *        operation fails.
*/
ZTEST_F(buttonMngr_suite, test_readButtonMatrix_ClearColumnFail)
{
  int failRet = -EIO;
  int successRet = 0;

  for(uint8_t i = 0; i < BUTTON_COL_COUNT; ++i)
  {
    SET_RETURN_SEQ(zephyrGpioSet, fixture->colSetRetVals, i + 1);

    if(i > 0)
      fixture->colClearRetVals[i - 1] = successRet;
    fixture->colClearRetVals[i] = failRet;
    SET_RETURN_SEQ(zephyrGpioClear, fixture->colClearRetVals, i + 1);

    zassert_equal(failRet, readButtonMatrix());
    zassert_equal(i + 1, zephyrGpioClear_fake.call_count);
    for(uint8_t j = 0; j > i; ++j)
      zassert_equal(columns + j, zephyrGpioClear_fake.arg0_history[j]);
    RESET_FAKE(zephyrGpioClear);
  }
}

/**
 * @test  readButtonMatrix must return the error code and clear the column
 *        if any of the read row operation fails.
*/
ZTEST_F(buttonMngr_suite, test_readButtonMatrix_ReadRowFail)
{
  int failRet = -EIO;
  int successRet = 0;

  for(uint8_t i = 0; i < BUTTON_ROW_COUNT; ++i)
  {
    zephyrGpioSet_fake.return_val = successRet;

    if(i > 0)
      fixture->readRetVals[i - 1] = successRet;
    fixture->readRetVals[i] = failRet;
    SET_RETURN_SEQ(zephyrGpioRead, fixture->readRetVals, i + 1);

    zephyrGpioClear_fake.return_val = successRet;

    zassert_equal(failRet, readButtonMatrix());
    zassert_equal(i + 1, zephyrGpioRead_fake.call_count);
    zassert_equal(1, zephyrGpioClear_fake.call_count);
    zassert_equal(columns, zephyrGpioClear_fake.arg0_val);
    for(uint8_t j = 0; j > i; ++j)
      zassert_equal(rows + j, zephyrGpioRead_fake.arg0_history[j]);
    RESET_FAKE(zephyrGpioRead);
    RESET_FAKE(zephyrGpioClear);
  }
}

/**
 * @test  readButtonMatrix must return the success code and update the button
 *        states when all operations succeeds.
*/
ZTEST_F(buttonMngr_suite, test_readButtonMatrix_Success)
{
  int successRet = 0;

  SET_RETURN_SEQ(zephyrGpioSet, fixture->colSetRetVals, BUTTON_COL_COUNT);
  SET_RETURN_SEQ(zephyrGpioRead, fixture->readRetVals,
    BUTTON_ROW_COUNT * BUTTON_COL_COUNT);
  SET_RETURN_SEQ(zephyrGpioClear, fixture->colClearRetVals, BUTTON_COL_COUNT);

  zassert_equal(successRet, readButtonMatrix());
  zassert_equal(BUTTON_COL_COUNT, zephyrGpioSet_fake.call_count);
  zassert_equal(BUTTON_ROW_COUNT * BUTTON_COL_COUNT,
    zephyrGpioRead_fake.call_count);
  zassert_equal(BUTTON_COL_COUNT, zephyrGpioClear_fake.call_count);
  for(uint8_t i = 0; i < BUTTON_COL_COUNT; ++i)
  {
    zassert_equal(columns + i, zephyrGpioSet_fake.arg0_history[i]);
    zassert_equal(columns + i, zephyrGpioClear_fake.arg0_history[i]);
  }

  for(uint8_t i = 0; i < BUTTON_ROW_COUNT * BUTTON_COL_COUNT; ++i)
  {
    zassert_equal(rows + (i % 8), zephyrGpioRead_fake.arg0_history[i]);
    zassert_equal(fixture->readRetVals[i], buttonStates[i]);
  }
}

/**
 * @test  readButtonShifters must return the error code if any of the read
 *        operation fails.
*/
ZTEST_F(buttonMngr_suite, test_readButtonShifters_ReadFail)
{
  int failRet = -EIO;
  int successRet = 0;

  for(uint8_t i = 0; i < BUTTON_SHIFTER_COUNT; ++i)
  {
    if(i > 0)
      fixture->readRetVals[i - 1] = successRet;
    fixture->readRetVals[i] = failRet;
    SET_RETURN_SEQ(zephyrGpioRead, fixture->readRetVals, i + 1);

    zassert_equal(failRet, readButtonShifters());
    zassert_equal(i + 1, zephyrGpioRead_fake.call_count);
    for(uint8_t j = 0; j > i; ++j)
      zassert_equal(shifters + j, zephyrGpioRead_fake.arg0_history[j]);
    RESET_FAKE(zephyrGpioRead);
  }
}

/**
 * @test  readButtonShifters must return the success code and update the
 *        shifters button states when all operations succeed.
*/
ZTEST_F(buttonMngr_suite, test_readButtonShifters_Success)
{
  int successRet = 0;

  SET_RETURN_SEQ(zephyrGpioRead, fixture->readRetVals, BUTTON_SHIFTER_COUNT);

  zassert_equal(successRet, readButtonShifters());
  zassert_equal(BUTTON_SHIFTER_COUNT, zephyrGpioRead_fake.call_count);
  for(uint8_t i = 0; i < BUTTON_SHIFTER_COUNT; ++i)
  {
    zassert_equal(shifters + i, zephyrGpioRead_fake.arg0_history[i]);
    zassert_equal(fixture->readRetVals[i],
      buttonStates[LEFT_SHIFTER_IDX + i]);
  }
}

/**
 * @test  readButtonRockers must return the error code if any of the read
 *        operation fails.
*/
ZTEST_F(buttonMngr_suite, test_readButtonRockers_ReadFail)
{
  int failRet = -EIO;
  int successRet = 0;

  for(uint8_t i = 0; i < BUTTON_ROCKER_COUNT; ++i)
  {
    if(i > 0)
      fixture->readRetVals[i - 1] = successRet;
    fixture->readRetVals[i] = failRet;
    SET_RETURN_SEQ(zephyrGpioRead, fixture->readRetVals, i + 1);

    zassert_equal(failRet, readButtonRockers());
    zassert_equal(i + 1, zephyrGpioRead_fake.call_count);
    for(uint8_t j = 0; j > i; ++j)
      zassert_equal(rockers + j, zephyrGpioRead_fake.arg0_history[j]);
    RESET_FAKE(zephyrGpioRead);
  }
}

/**
 * @test  readButtonRockers must return the success code and update the
 *        rockers button states when all operations succeed.
*/
ZTEST_F(buttonMngr_suite, test_readButtonRockers_Success)
{
  int successRet = 0;

  SET_RETURN_SEQ(zephyrGpioRead, fixture->readRetVals, BUTTON_ROCKER_COUNT);

  zassert_equal(successRet, readButtonRockers());
  zassert_equal(BUTTON_ROCKER_COUNT, zephyrGpioRead_fake.call_count);
  for(uint8_t i = 0; i < BUTTON_ROCKER_COUNT; ++i)
  {
    zassert_equal(rockers + i, zephyrGpioRead_fake.arg0_history[i]);
    zassert_equal(fixture->readRetVals[i],
      buttonStates[LEFT_ROCKER_IDX + i]);
  }
}

/**
 * @test  buttonMngrInit must return the error code as soon as the
 *        initialization of one row fails.
*/
ZTEST_F(buttonMngr_suite, test_buttonMngrInit_RowInitFail)
{
  int failRet = -EIO;
  int successRet = 0;

  for(uint8_t i = 0; i < BUTTON_ROW_COUNT; ++i)
  {
    if(i > 0)
      fixture->gpioInitRetVals[i - 1] = successRet;

    fixture->gpioInitRetVals[i] = failRet;
    SET_RETURN_SEQ(zephyrGpioInit, fixture->gpioInitRetVals, i + 1);

    zassert_equal(failRet, buttonMngrInit());
    zassert_equal(i + 1, zephyrGpioInit_fake.call_count);
    for(uint8_t j = 0; j > i; ++j)
    {
      zassert_equal(rows + j, zephyrGpioInit_fake.arg0_history[j]);
      zassert_equal(GPIO_IN, zephyrGpioInit_fake.arg1_history[j]);
    }
    zassert_equal(0, zephyrThreadCreate_fake.call_count);
    RESET_FAKE(zephyrGpioInit);
  }
}

/**
 * @test  buttonMngrInit must return the error code as soon as the
 *        initialization of one column fails.
*/
ZTEST_F(buttonMngr_suite, test_buttonMngrInit_ColInitFail)
{
  int failRet = -EIO;
  int successRet = 0;

  for(uint8_t i = 0; i < BUTTON_COL_COUNT; ++i)
  {
    if(i > 0)
      fixture->gpioInitRetVals[BUTTON_ROW_COUNT + i - 1] = successRet;

    fixture->gpioInitRetVals[BUTTON_ROW_COUNT + i] = failRet;
    SET_RETURN_SEQ(zephyrGpioInit, fixture->gpioInitRetVals,
      BUTTON_ROW_COUNT + i + 1);

    zassert_equal(failRet, buttonMngrInit());
    zassert_equal(BUTTON_ROW_COUNT + i + 1, zephyrGpioInit_fake.call_count);
    for(uint8_t j = 0; j > i; ++j)
    {
      zassert_equal(columns + j,
        zephyrGpioInit_fake.arg0_history[BUTTON_ROW_COUNT + j]);
      zassert_equal(GPIO_OUT_CLR,
        zephyrGpioInit_fake.arg1_history[BUTTON_ROW_COUNT + j]);
    }
    zassert_equal(0, zephyrThreadCreate_fake.call_count);
    RESET_FAKE(zephyrGpioInit);
  }
}

/**
 * @test  buttonMngrInit must return the error code as soon as
 *        the initialization of the one of the shifter fails.
*/
ZTEST_F(buttonMngr_suite, test_ButtonMngrInit_ShifterFail)
{
  int failRet = -EIO;
  int successRet = 0;

  for(uint8_t i = 0; i < BUTTON_SHIFTER_COUNT; ++i)
  {
    if(i > 0)
      fixture->gpioInitRetVals[TOTAL_ROW_COL_COUNT + i - 1] = successRet;

    fixture->gpioInitRetVals[TOTAL_ROW_COL_COUNT + i] = failRet;
    SET_RETURN_SEQ(zephyrGpioInit, fixture->gpioInitRetVals,
      TOTAL_ROW_COL_COUNT + i + 1);

    zassert_equal(failRet, buttonMngrInit());
    zassert_equal(TOTAL_ROW_COL_COUNT + i + 1, zephyrGpioInit_fake.call_count);
    for(uint8_t j = 0; j < i; ++j)
    {
      zassert_equal(shifters + j,
        zephyrGpioInit_fake.arg0_history[TOTAL_ROW_COL_COUNT + j]);
      zassert_equal(GPIO_IN,
        zephyrGpioInit_fake.arg1_history[TOTAL_ROW_COL_COUNT + j]);
    }
    zassert_equal(0, zephyrThreadCreate_fake.call_count);
    RESET_FAKE(zephyrGpioInit);
  }
}

/**
 * @test  buttonMngrInit must return the error code as soon as
 *        the initialization of the one of the rocker fails.
*/
ZTEST_F(buttonMngr_suite, test_ButtonMngrInit_RockerFail)
{
  int failRet = -EIO;
  int successRet = 0;
  uint8_t rockerOffset = TOTAL_ROW_COL_COUNT + BUTTON_SHIFTER_COUNT;

  for(uint8_t i = 0; i < BUTTON_ROCKER_COUNT; ++i)
  {
    if(i > 0)
      fixture->gpioInitRetVals[rockerOffset + i - 1] = successRet;

    fixture->gpioInitRetVals[rockerOffset + i] = failRet;
    SET_RETURN_SEQ(zephyrGpioInit, fixture->gpioInitRetVals,
      rockerOffset + i + 1);

    zassert_equal(failRet, buttonMngrInit());
    zassert_equal(rockerOffset + i + 1, zephyrGpioInit_fake.call_count);
    for(uint8_t j = 0; j < i; ++j)
    {
      zassert_equal(rockers + j,
        zephyrGpioInit_fake.arg0_history[rockerOffset + j]);
      zassert_equal(GPIO_IN,
        zephyrGpioInit_fake.arg1_history[rockerOffset + j]);
    }
    zassert_equal(0, zephyrThreadCreate_fake.call_count);
    RESET_FAKE(zephyrGpioInit);
  }
}

/**
 * @test  buttonMngrInit must return the success code when the initialization
 *        succeeds and create the thread.
*/
ZTEST_F(buttonMngr_suite, test_buttonMngrInit_Success)
{
  int successRet = 0;
  ZephyrGpio *expectedGpio;
  ZephyrGpioDir expectedDir;

  SET_RETURN_SEQ(zephyrGpioInit, fixture->gpioInitRetVals, TOTAL_GPIO_COUNT);

  zassert_equal(successRet, buttonMngrInit());
  zassert_equal(TOTAL_GPIO_COUNT, zephyrGpioInit_fake.call_count);
  for(uint8_t i = 0; i < TOTAL_GPIO_COUNT; ++i)
  {
    if(i < BUTTON_ROW_COUNT)
    {
      expectedGpio = rows + i;
      expectedDir = GPIO_IN;
    }
    else if(i < TOTAL_ROW_COL_COUNT)
    {
      expectedGpio = columns + (i - BUTTON_ROW_COUNT);
      expectedDir = GPIO_OUT_CLR;
    }
    else if(i < TOTAL_ROW_COL_COUNT + BUTTON_SHIFTER_COUNT)
    {
      expectedGpio = shifters + (i - TOTAL_ROW_COL_COUNT);
      expectedDir = GPIO_IN;
    }
    else
    {
      expectedGpio = rockers + (i - TOTAL_ROW_COL_COUNT - BUTTON_SHIFTER_COUNT);
      expectedDir = GPIO_IN;
    }
    zassert_equal(expectedGpio, zephyrGpioInit_fake.arg0_history[i]);
    zassert_equal(expectedDir, zephyrGpioInit_fake.arg1_history[i]);
  }
  zassert_equal(1, zephyrThreadCreate_fake.call_count);
  zassert_equal(&thread, zephyrThreadCreate_fake.arg0_val);
  zassert_equal(BUTTON_MNGR_THREAD_NAME, zephyrThreadCreate_fake.arg1_val);
  zassert_equal(ZEPHYR_TIME_NO_WAIT, zephyrThreadCreate_fake.arg2_val);
  zassert_equal(MILLI_SEC, zephyrThreadCreate_fake.arg3_val);
}

#define GET_STATE_FAIL_TEST_CNT     2
/**
 * @test  buttonMngrGetAllStates must return the error code when requested
 *        button state count does not correspond to the actual count.
*/
ZTEST_F(buttonMngr_suite, test_buttonMngrGetAllStates_BadCount)
{
  int failRet = -EINVAL;
  size_t badCounts[GET_STATE_FAIL_TEST_CNT] = {BUTTON_COUNT - 1,
                                               BUTTON_COUNT + 1};

  for(uint8_t i = 0; i < GET_STATE_FAIL_TEST_CNT; ++i)
    zassert_equal(failRet, buttonMngrGetAllStates(fixture->buttonStates,
      badCounts[i]));
}

/**
 * @test  buttonMngrGetAllStates must return the success code and copy the
 *        current button states to the provided buffer.
*/
ZTEST_F(buttonMngr_suite, test_buttonMngrGetAllStates_Success)
{
  int successRet = 0;

  zassert_equal(successRet, buttonMngrGetAllStates(fixture->buttonStates,
    BUTTON_COUNT));

  for(uint8_t i = 0; i < BUTTON_COUNT; ++i)
    zassert_equal(buttonStates[i], fixture->buttonStates[i]);
}

/** @} */
