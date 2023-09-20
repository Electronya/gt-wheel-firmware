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

#include "hidDevice.h"
#include "hidDevice.c"

#include "buttonMngr.h"
#include "zephyrHID.h"
#include "zephyrWorkQueue.h"
#include "zephyrWork.h"

DEFINE_FFF_GLOBALS;

/* mocks */
FAKE_VALUE_FUNC(int, zephyrHidInit, ZephyrHID*, uint8_t*);

FAKE_VOID_FUNC(zephyrWorkQueueInit, ZephyrWorkQueue*);
FAKE_VOID_FUNC(zephyrWorkInit, ZephyrWork*);
FAKE_VALUE_FUNC(int, zephyrWorkSubmitToQueue, ZephyrWorkQueue*, ZephyrWork*);

FAKE_VALUE_FUNC(int, buttonMngrGetAllStates, WheelButtonState*, size_t);

/**
 * @brief The test fixture.
*/
struct hidDevice_suite_fixture
{
  WheelButtonState testStates[BUTTON_COUNT];
};

static void *hidDeviceSuiteSetup(void)
{
  struct hidDevice_suite_fixture *fixture =
    k_malloc(sizeof(struct hidDevice_suite_fixture));
  zassume_not_null(fixture, NULL);

  return (void *)fixture;
}

static void hidDeviceSuiteTeardown(void *f)
{
  k_free(f);
}

static void hidDeviceCaseSetup(void *f)
{
  // int successRet = 0;
  struct hidDevice_suite_fixture *fixture =
    (struct hidDevice_suite_fixture *)f;

  for(uint8_t i = 0; i < BUTTON_COUNT; ++i)
    fixture->testStates[i] = BUTTON_DEPRESSED;

  RESET_FAKE(zephyrHidInit);
  RESET_FAKE(zephyrWorkQueueInit);
  RESET_FAKE(zephyrWorkInit);
  RESET_FAKE(zephyrWorkSubmitToQueue);
  RESET_FAKE(buttonMngrGetAllStates);
}

ZTEST_SUITE(hidDevice_suite, NULL, hidDeviceSuiteSetup, hidDeviceCaseSetup,
  NULL, hidDeviceSuiteTeardown);

/**
 * @test  hidInIntOp must submit the HID work.
*/
ZTEST(hidDevice_suite, test_hidInIntOp_SubmitWork)
{
  int successRet = 1;

  zephyrWorkSubmitToQueue_fake.return_val = successRet;

  hidInIntOp(NULL);
  zassert_equal(1, zephyrWorkSubmitToQueue_fake.call_count);
  zassert_equal(&workQueue, zephyrWorkSubmitToQueue_fake.arg0_val);
  zassert_equal(&work, zephyrWorkSubmitToQueue_fake.arg1_val);
}

/**
 * @test  hidWorker must return do nothing more if the operation to get all
 *        the button states fails.
*/
ZTEST(hidDevice_suite, test_hidWorker_GetButtonStateFail)
{
  int failRet = -EIO;

  buttonMngrGetAllStates_fake.return_val = failRet;

  hidWorker(NULL);
  zassert_equal(1, buttonMngrGetAllStates_fake.call_count);
  zassert_equal((WheelButtonState *)&report + HID_RPT_BTN_STATE_OFFSET,
    buttonMngrGetAllStates_fake.arg0_val);
  zassert_equal(BUTTON_COUNT, buttonMngrGetAllStates_fake.arg1_val);
}

/**
 * @test  hidDeviceInit must return the error code if initializing the HID
 *        device fails.
*/
ZTEST(hidDevice_suite, test_hidDeviceInit_InitFail)
{
  int failRet = -ENODEV;

  zephyrHidInit_fake.return_val = failRet;

  zassert_equal(failRet, hidDeviceInit());
  zassert_equal(1, zephyrHidInit_fake.call_count);
  zassert_equal(&hidDev, zephyrHidInit_fake.arg0_val);
  zassert_equal((uint8_t *)HID_DEVICE_LABEL, zephyrHidInit_fake.arg1_val);
}

/**
 * @test  hidDeviceInit must initialize the HID device, the work queue and the
 *        work, and return the success code.
*/
ZTEST(hidDevice_suite, test_hidDeviceInit_Success)
{
  int successRet = 0;

  zephyrHidInit_fake.return_val = successRet;

  zassert_equal(successRet, hidDeviceInit());
  zassert_equal(hidInIntOp, hidDev.ops.int_in_ready);
  zassert_equal(1, zephyrHidInit_fake.call_count);
  zassert_equal(&hidDev, zephyrHidInit_fake.arg0_val);
  zassert_equal((uint8_t *)HID_DEVICE_LABEL, zephyrHidInit_fake.arg1_val);
  zassert_equal(hidWorker, work.handler);
  zassert_equal(1, zephyrWorkInit_fake.call_count);
  zassert_equal(&work, zephyrWorkInit_fake.arg0_val);
  zassert_equal(1, zephyrWorkQueueInit_fake.call_count);
  zassert_equal(&workQueue, zephyrWorkQueueInit_fake.arg0_val);
}

/** @} */
