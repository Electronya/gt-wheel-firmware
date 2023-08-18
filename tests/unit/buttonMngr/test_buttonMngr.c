/**
 * Copyright (C) 2023 by Electronya
 *
 * @file      test_ledCtrl.h
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

DEFINE_FFF_GLOBALS;

/* mocks */
FAKE_VALUE_FUNC(int, zephyrGpioInit, ZephyrGpio*, ZephyrGpioDir);
FAKE_VALUE_FUNC(int, zephyrGpioSet, ZephyrGpio*);
FAKE_VALUE_FUNC(int, zephyrGpioClear, ZephyrGpio*);
FAKE_VALUE_FUNC(int, zephyrGpioRead, ZephyrGpio*);

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
  int readRetVals[BUTTON_ROW_COUNT * BUTTON_COL_COUNT];  /**< Row read return values. */
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
      buttonStates[i] = BUTTON_PRESSED;
    else
      buttonStates[i] = BUTTON_DEPRESSED;
  }

  RESET_FAKE(zephyrGpioInit);
  RESET_FAKE(zephyrGpioSet);
  RESET_FAKE(zephyrGpioClear);
  RESET_FAKE(zephyrGpioRead);
}

ZTEST_SUITE(buttonMngr_suite, NULL, buttonMngrSuiteSetup, buttonMngrCaseSetup,
  NULL, buttonMngrSuiteTeardown);

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
    zassert_equal(rows + (i % 12), zephyrGpioRead_fake.arg0_history[i]);
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
  }
}

/**
 * @test  buttonMngrInit must return the success code when the initialization
 *        succeeds.
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
}

/** @} */
