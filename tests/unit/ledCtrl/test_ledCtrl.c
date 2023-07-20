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

#include <zephyr/ztest.h>
#include <zephyr/fff.h>

#include "LedCtrl.h"
#include "LedCtrl.c"

#include "zephyrLedStrip.h"

DEFINE_FFF_GLOBALS;
ZTEST_SUITE(ledCtrl_suite, NULL, NULL, NULL, NULL, NULL);

#define LED_CTRL_IS_DEV_READY_TEST_CNT  3
FAKE_VALUE_FUNC(int, zephyrLedStripInit, ZephyrLedStrip*, ZephyrLedStripClrFmt,
  uint32_t);
/**
 * @test  ledCtrlInit must try to initialize the LED strip and return
 *        the error code if the operation fails.
*/
ZTEST(ledCtrl_suite, test_ledCtrlInit_Fail)
{
  int returnVals[LED_CTRL_IS_DEV_READY_TEST_CNT] = {-ENODEV, -ENOSPC, -EINVAL};
  int pixelCounts[LED_CTRL_IS_DEV_READY_TEST_CNT] = {1, 24, 250};
  int result;
  for(uint8_t i = 0; i < LED_CTRL_IS_DEV_READY_TEST_CNT; ++i)
  {
    ledStrip.pixelCount = pixelCounts[i];
    RESET_FAKE(zephyrLedStripInit);
    zephyrLedStripInit_fake.return_val = returnVals[i];
    result = ledCtrlInit();
    zassert_equal(zephyrLedStripInit_fake.call_count, 1,
      "ledCtrlInit failed to initialize the LED strip");
    zassert_equal(zephyrLedStripInit_fake.arg0_val, &ledStrip,
      "ledCtrlInit failed to initialize the right LED strip");
    zassert_equal(zephyrLedStripInit_fake.arg1_val, LED_STRIP_COLOR_RGB,
      "ledCtrlInit failed to initialize the LED strip with the right color format");
    zassert_equal(zephyrLedStripInit_fake.arg2_val, pixelCounts[i],
      "ledCtrlInit failed to initialize the LED strip with the right pixel count");
    zassert_equal(result, returnVals[i],
      "ledCtrlInit failed to return the error code.");
  }
}

/**
 * @test  ledCtrlInit must initialize the LED strip and return
 *        the success code if the operation succeeds.
*/
ZTEST(ledCtrl_suite, test_ledCtrlInit_Success)
{
  int returnVals[LED_CTRL_IS_DEV_READY_TEST_CNT] = {0, 0, 0};
  int pixelCounts[LED_CTRL_IS_DEV_READY_TEST_CNT] = {1, 24, 250};
  int result;
  for(uint8_t i = 0; i < LED_CTRL_IS_DEV_READY_TEST_CNT; ++i)
  {
    ledStrip.pixelCount = pixelCounts[i];
    RESET_FAKE(zephyrLedStripInit);
    zephyrLedStripInit_fake.return_val = returnVals[i];
    result = ledCtrlInit();
    zassert_equal(zephyrLedStripInit_fake.call_count, 1,
      "ledCtrlInit failed to initialize the LED strip");
    zassert_equal(zephyrLedStripInit_fake.arg0_val, &ledStrip,
      "ledCtrlInit failed to initialize the right LED strip");
    zassert_equal(zephyrLedStripInit_fake.arg1_val, LED_STRIP_COLOR_RGB,
      "ledCtrlInit failed to initialize the LED strip with the right color format");
    zassert_equal(zephyrLedStripInit_fake.arg2_val, pixelCounts[i],
      "ledCtrlInit failed to initialize the LED strip with the right pixel count");
    zassert_equal(result, returnVals[i],
      "ledCtrlInit failed to return the success code.");
  }
}

#define LED_CTRL_PIXEL_CNT_TEST_CNT     3
/**
 * @test  ledCtrlGetPixelCount must return the number of pixel in the
 *        LED strip.
*/
ZTEST(ledCtrl_suite, test_ledCtrlGetPixelCount)
{
  uint32_t expectedPixelCnt[LED_CTRL_PIXEL_CNT_TEST_CNT] = { 1, 10, 500};

  for(uint8_t i = 0; i < LED_CTRL_PIXEL_CNT_TEST_CNT; ++i)
  {
    ledStrip.pixelCount = expectedPixelCnt[i];
    zassert_equal(ledCtrlGetPixelCount(), expectedPixelCnt[i]);
  }
}

/** @} */
