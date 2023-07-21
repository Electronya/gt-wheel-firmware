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
#include <zephyr/sys/util.h>

#include "LedCtrl.h"
#include "LedCtrl.c"

#include "zephyrLedStrip.h"

DEFINE_FFF_GLOBALS;

#define LED_CTRL_PIXEL_CNT            5

struct ledCtrl_suite_fixture
{
  ZephyrRgbLed pixels[LED_CTRL_PIXEL_CNT];
  ZephyrRgbLed newPixels[LED_CTRL_PIXEL_CNT];
  ZephyrRgbLed expectedPixels[LED_CTRL_PIXEL_CNT];
};

static void *ledCtrlSuiteSetup(void)
{
  struct ledFixture *fixture = k_malloc(sizeof(struct ledCtrl_suite_fixture));
  return fixture;
}

static void ledCtrlSuiteTeardown(void *f)
{
  k_free(f);
}

static void ledCtrlCaseSetup(void *f)
{
  struct ledCtrl_suite_fixture *fixture = (struct ledCtrl_suite_fixture *)f;

  for(uint8_t i = 0; i < LED_CTRL_PIXEL_CNT; ++i)
  {
    fixture->pixels[i].b = 0;
    fixture->pixels[i].g = 0;
    fixture->pixels[i].r = 0;
    fixture->newPixels[i].b = i - 1;
    fixture->newPixels[i].g = i;
    fixture->newPixels[i].r = i + 1;
  }
}

ZTEST_SUITE(ledCtrl_suite, NULL, ledCtrlSuiteSetup, ledCtrlCaseSetup,
  NULL, ledCtrlSuiteTeardown);

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
 * @test  ledCtrlGetRpmChaserPxlCnt must return the number of pixel in the
 *        LED strip.
*/
ZTEST(ledCtrl_suite, test_ledCtrlGetPixelCount)
{
  uint32_t expectedPixelCnt[LED_CTRL_PIXEL_CNT_TEST_CNT] = { 1, 10, 500};

  for(uint8_t i = 0; i < LED_CTRL_PIXEL_CNT_TEST_CNT; ++i)
  {
    ledStrip.pixelCount = expectedPixelCnt[i] + 2;
    zassert_equal(expectedPixelCnt[i], ledCtrlGetRpmChaserPxlCnt(),
      "ledCtrlGetRpmChaserPxlCnt failed to return the pixel count.");
  }
}

/**
 * @test  ledCtrlSetRightEncoderDefaultMode must set the right encoder
 *        pixel to the default mode color (blue).
*/
ZTEST_F(ledCtrl_suite, test_ledCtrlSetRightEncoderDefaultMode_SetPixelColor)
{
  ZephyrRgbLed expectedPixels[LED_CTRL_PIXEL_CNT];

  bytecpy(expectedPixels, fixture->newPixels,
    sizeof(ZephyrRgbLed) * LED_CTRL_PIXEL_CNT);
  expectedPixels[0].b = 0x0f;
  expectedPixels[0].g = 0x00;
  expectedPixels[0].r = 0x00;

  ledStrip.pixelCount = LED_CTRL_PIXEL_CNT;
  ledStrip.rgbPixels = fixture->newPixels;

  ledCtrlSetRightEncoderDefaultMode();

  for(uint8_t i = 0; i < LED_CTRL_PIXEL_CNT; ++i)
  {
    zassert_equal(expectedPixels[i].b, ledStrip.rgbPixels[i].b,
      "ledCtrlSetRightEncoderDefaultMode failed to update the pixel data.");
    zassert_equal(expectedPixels[i].g, ledStrip.rgbPixels[i].g,
      "ledCtrlSetRightEncoderDefaultMode failed to update the pixel data.");
    zassert_equal(expectedPixels[i].r, ledStrip.rgbPixels[i].r,
      "ledCtrlSetRightEncoderDefaultMode failed to update the pixel data.");
  }
}

#define LED_CTRL_SET_PIXEL_TEST_CNT   2
/**
 * @test  ledCtrlSetRpmChaserPixels must return the error code if the new pixel
 *        string is not the same length of the RPM chaser.
*/
ZTEST_F(ledCtrl_suite, test_ledCtrlSetPixels_Fail)
{
  uint32_t pixelCnt[LED_CTRL_SET_PIXEL_TEST_CNT] = { LED_CTRL_PIXEL_CNT - 3,
                                                     LED_CTRL_PIXEL_CNT - 1};

  for(uint8_t i = 0; i < LED_CTRL_SET_PIXEL_TEST_CNT; ++i)
  {
    ledStrip.pixelCount = pixelCnt[i];
    zassert_equal(-EDOM,
      ledCtrlSetRpmChaserPixels(fixture->newPixels, LED_CTRL_PIXEL_CNT),
      "ledCtrlSetRpmChaserPixels failed to return the error code.");
  }
}

/**
 * @test  ledCtrlSetRpmChaserPixels must update the RPM chaser pixel data and
 *        return the success code.
*/
ZTEST_F(ledCtrl_suite, test_ledCtrlSetPixels_UpdateSuccess)
{
  int successRet = 0;
  ZephyrRgbLed expectedPixels[LED_CTRL_PIXEL_CNT];

  ledStrip.rgbPixels = fixture->pixels;
  ledStrip.pixelCount = LED_CTRL_PIXEL_CNT;
  for(uint8_t i = 0; i < LED_CTRL_PIXEL_CNT; ++i)
  {
    if(i < 2)
    {
      expectedPixels[i].b = fixture->pixels[i].b;
      expectedPixels[i].g = fixture->pixels[i].g;
      expectedPixels[i].r = fixture->pixels[i].r;
    }
    else
    {
      expectedPixels[i].b = fixture->newPixels[i].b;
      expectedPixels[i].g = fixture->newPixels[i].g;
      expectedPixels[i].r = fixture->newPixels[i].r;
    }
  }

  zassert_equal(successRet,
    ledCtrlSetRpmChaserPixels(fixture->newPixels + 2, LED_CTRL_PIXEL_CNT - 2),
    "ledCtrlSetRpmChaserPixels failed to return the success code.");

  for(uint8_t i = 0; i < LED_CTRL_PIXEL_CNT; ++i)
  {
    zassert_equal(expectedPixels[i].b, ledStrip.rgbPixels[i].b,
      "ledCtrlSetRpmChaserPixels failed to update the pixel data.");
    zassert_equal(expectedPixels[i].g, ledStrip.rgbPixels[i].g,
      "ledCtrlSetRpmChaserPixels failed to update the pixel data.");
    zassert_equal(expectedPixels[i].r, ledStrip.rgbPixels[i].r,
      "ledCtrlSetRpmChaserPixels failed to update the pixel data.");
  }
}

/** @} */
