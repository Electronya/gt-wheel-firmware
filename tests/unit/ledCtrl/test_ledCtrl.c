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
    zassert_equal(1, zephyrLedStripInit_fake.call_count,
      "ledCtrlInit failed to initialize the LED strip.");
    zassert_equal(&ledStrip, zephyrLedStripInit_fake.arg0_val,
      "ledCtrlInit failed to initialize the right LED strip.");
    zassert_equal(LED_STRIP_COLOR_RGB, zephyrLedStripInit_fake.arg1_val,
      "ledCtrlInit failed to initialize the LED strip with the right color format.");
    zassert_equal(pixelCounts[i], zephyrLedStripInit_fake.arg2_val,
      "ledCtrlInit failed to initialize the LED strip with the right pixel count.");
    zassert_equal(returnVals[i], result,
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
    zassert_equal(1, zephyrLedStripInit_fake.call_count,
      "ledCtrlInit failed to initialize the LED strip.");
    zassert_equal(&ledStrip, zephyrLedStripInit_fake.arg0_val,
      "ledCtrlInit failed to initialize the right LED strip.");
    zassert_equal(LED_STRIP_COLOR_RGB, zephyrLedStripInit_fake.arg1_val,
      "ledCtrlInit failed to initialize the LED strip with the right color format.");
    zassert_equal(pixelCounts[i], zephyrLedStripInit_fake.arg2_val,
      "ledCtrlInit failed to initialize the LED strip with the right pixel count.");
    zassert_equal(returnVals[i], result,
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
    zassert_equal(expectedPixelCnt[i], ledCtrlGetPixelCount(),
      "ledCtrlGetPixelCount failed to return the pixel count.");
  }
}

#define LED_CTRL_PIXEL_CNT            5
#define LED_CTRL_SET_PIXEL_TEST_CNT   2
/**
 * @test  ledCtrlSetPixels must return the error code if the new pixel
 *        string is not the same length of the LED strip.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetPixels_Fail)
{
  ZephyrRgbLed pixels[LED_CTRL_PIXEL_CNT];
  uint32_t pixelCnt[LED_CTRL_SET_PIXEL_TEST_CNT] = { LED_CTRL_PIXEL_CNT - 1,
                                                     LED_CTRL_PIXEL_CNT + 1};

  for(uint8_t i = 0; i < LED_CTRL_SET_PIXEL_TEST_CNT; ++i)
  {
    ledStrip.pixelCount = pixelCnt[i];
    zassert_equal(-EDOM, ledCtrlSetPixels(pixels, LED_CTRL_PIXEL_CNT),
      "ledCtrlSetPixels failed to return the error code.");
  }
}

/**
 * @test  ledCtrlSetPixels must update the led strip pixel data and
 *        return the success code.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetPixels_UpdateSuccess)
{
  int successRet = 0;
  ZephyrRgbLed pixels[LED_CTRL_PIXEL_CNT];
  ZephyrRgbLed expectedPixels[LED_CTRL_PIXEL_CNT];

  ledStrip.rgbPixels = pixels;
  ledStrip.pixelCount = LED_CTRL_PIXEL_CNT;
  for(uint8_t i = 0; i < LED_CTRL_PIXEL_CNT; ++i)
  {
    pixels[i].b = 0;
    pixels[i].g = 0;
    pixels[i].r = 0;
    expectedPixels[i].b = i - 1;
    expectedPixels[i].g = i;
    expectedPixels[i].r = i + 1;
  }

  zassert_equal(successRet,
    ledCtrlSetPixels(expectedPixels, LED_CTRL_PIXEL_CNT),
    "ledCtrlSetPixels failed to return the success code.");

  for(uint8_t i = 0; i < LED_CTRL_PIXEL_CNT; ++i)
  {
    zassert_equal(expectedPixels[i].b, ledStrip.rgbPixels[i].b,
      "ledCtrlSetPixels failed to update the pixel data.");
    zassert_equal(expectedPixels[i].g, ledStrip.rgbPixels[i].g,
      "ledCtrlSetPixels failed to update the pixel data.");
    zassert_equal(expectedPixels[i].r, ledStrip.rgbPixels[i].r,
      "ledCtrlSetPixels failed to update the pixel data.");
  }
}

/** @} */
