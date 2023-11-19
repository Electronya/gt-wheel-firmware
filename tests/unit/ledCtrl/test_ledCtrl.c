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

#include "ledCtrl.h"
#include "ledCtrl.c"

#include "zephyrLedStrip.h"

DEFINE_FFF_GLOBALS;

/* mocks */
FAKE_VALUE_FUNC(int, zephyrLedStripInit, ZephyrLedStrip*, uint32_t);
FAKE_VALUE_FUNC(uint32_t, zephyrLedStripGetPixelCnt, ZephyrLedStrip*);
FAKE_VALUE_FUNC(int, zephyrLedStripSetPixel, ZephyrLedStrip*, uint32_t,
  const ZephyrRgbLed*);
FAKE_VALUE_FUNC(int, zephyrLedStripSetPixels, ZephyrLedStrip*, uint32_t,
  uint32_t, const ZephyrRgbLed*);
FAKE_VALUE_FUNC(int, zephyrLedStripUpdate, ZephyrLedStrip*);

/**
 * @brief Test RPM chanser pixel data size.
*/
#define TEST_RPM_CHASER_PIXEL_SIZE      (RPM_CHASER_PIXEL_COUNT * 3)

static void ledCtrlCaseSetup(void *f)
{
  RESET_FAKE(zephyrLedStripInit);
  RESET_FAKE(zephyrLedStripGetPixelCnt);
  RESET_FAKE(zephyrLedStripSetPixel);
  RESET_FAKE(zephyrLedStripSetPixels);
  RESET_FAKE(zephyrLedStripUpdate);
}

ZTEST_SUITE(ledCtrl_suite, NULL, NULL, ledCtrlCaseSetup, NULL, NULL);

#define LED_CTRL_IS_DEV_READY_TEST_CNT  3
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
    zephyrLedStripInit_fake.return_val = returnVals[i];

    result = ledCtrlInit();
    zassert_equal(1, zephyrLedStripInit_fake.call_count,
      "ledCtrlInit failed to initialize the LED strip.");
    zassert_equal(&ledStrip, zephyrLedStripInit_fake.arg0_val,
      "ledCtrlInit failed to initialize the right LED strip.");
    zassert_equal(pixelCounts[i], zephyrLedStripInit_fake.arg1_val,
      "ledCtrlInit failed to initialize the LED strip with the right pixel count.");
    zassert_equal(returnVals[i], result,
      "ledCtrlInit failed to return the error code.");

    RESET_FAKE(zephyrLedStripInit);
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
    zephyrLedStripInit_fake.return_val = returnVals[i];

    result = ledCtrlInit();
    zassert_equal(1, zephyrLedStripInit_fake.call_count,
      "ledCtrlInit failed to initialize the LED strip.");
    zassert_equal(&ledStrip, zephyrLedStripInit_fake.arg0_val,
      "ledCtrlInit failed to initialize the right LED strip.");
    zassert_equal(pixelCounts[i], zephyrLedStripInit_fake.arg1_val,
      "ledCtrlInit failed to initialize the LED strip with the right pixel count.");
    zassert_equal(returnVals[i], result,
      "ledCtrlInit failed to return the success code.");

    RESET_FAKE(zephyrLedStripInit);
  }
}

#define LED_CTRL_PIXEL_CNT_TEST_CNT     3
/**
 * @test  ledCtrlGetRpmChaserPxlCnt must return the number of pixel in the
 *        RPM chaser.
*/
ZTEST(ledCtrl_suite, test_ledCtrlGetRpmChaserPxlCnt)
{
  uint32_t expectedPixelCnt[LED_CTRL_PIXEL_CNT_TEST_CNT] = { 1, 10, 500};

  for(uint8_t i = 0; i < LED_CTRL_PIXEL_CNT_TEST_CNT; ++i)
  {
    zephyrLedStripGetPixelCnt_fake.return_val = expectedPixelCnt[i] + 2;

    zassert_equal(expectedPixelCnt[i], ledCtrlGetRpmChaserPxlCnt());
    zassert_equal(i + 1, zephyrLedStripGetPixelCnt_fake.call_count);
    zassert_equal(&ledStrip, zephyrLedStripGetPixelCnt_fake.arg0_val);
  }
}

/**
 * @test  ledCtrlSetRightEncPixelDefaultMode must return the error code
 *        if setting the right encoder pixel color fails.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetRightEncPixelDefaultMode_SetColorFail)
{
  int failRet = -EDOM;

  zephyrLedStripSetPixel_fake.return_val = failRet;

  zassert_equal(failRet, ledCtrlSetRightEncPixelDefaultMode(),
    "ledCtrlSetRightEncPixelDefaultMode failed to return the error code.");
  zassert_equal(1, zephyrLedStripSetPixel_fake.call_count,
    "ledCtrlSetRightEncPixelDefaultMode failed to set the pixel color.");
  zassert_equal(&ledStrip, zephyrLedStripSetPixel_fake.arg0_val,
    "ledCtrlSetRightEncPixelDefaultMode failed to set the pixel color.");
  zassert_equal(0, zephyrLedStripSetPixel_fake.arg1_val,
    "ledCtrlSetRightEncPixelDefaultMode failed to set the pixel color.");
  zassert_equal(&encDefColor, zephyrLedStripSetPixel_fake.arg2_val,
    "ledCtrlSetRightEncPixelDefaultMode failed to set the pixel color.");
}

/**
 * @test  ledCtrlSetRightEncPixelDefaultMode must return the error code
 *        if updating the LED strip fails.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetRightEncPixelDefaultMode_UpdateStripFail)
{
  int successRet = 0;
  int failRet = -EDOM;

  zephyrLedStripSetPixel_fake.return_val = successRet;
  zephyrLedStripUpdate_fake.return_val = failRet;

  zassert_equal(failRet, ledCtrlSetRightEncPixelDefaultMode(),
    "ledCtrlSetRightEncPixelDefaultMode failed to return the error code.");
  zassert_equal(1, zephyrLedStripSetPixel_fake.call_count,
    "ledCtrlSetRightEncPixelDefaultMode failed to set the pixel color.");
  zassert_equal(&ledStrip, zephyrLedStripSetPixel_fake.arg0_val,
    "ledCtrlSetRightEncPixelDefaultMode failed to set the pixel color.");
  zassert_equal(0, zephyrLedStripSetPixel_fake.arg1_val,
    "ledCtrlSetRightEncPixelDefaultMode failed to set the pixel color.");
  zassert_equal(&encDefColor, zephyrLedStripSetPixel_fake.arg2_val,
    "ledCtrlSetRightEncPixelDefaultMode failed to set the pixel color.");
  zassert_equal(1, zephyrLedStripUpdate_fake.call_count,
    "ledCtrlSetRightEncPixelDefaultMode failed to update the LED strip.");
  zassert_equal(&ledStrip, zephyrLedStripUpdate_fake.arg0_val,
    "ledCtrlSetRightEncPixelDefaultMode failed to update the LED strip.");
}

/**
 * @test  ledCtrlSetRightEncPixelDefaultMode must set the right encoder
 *        pixel to the default mode color, update the LED strip and
 *        return the success code.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetRightEncPixelDefaultMode_Success)
{
  int successRet = 0;

  zephyrLedStripSetPixel_fake.return_val = successRet;
  zephyrLedStripUpdate_fake.return_val = successRet;

  zassert_equal(successRet, ledCtrlSetRightEncPixelDefaultMode(),
    "ledCtrlSetRightEncPixelDefaultMode failed to return the success code.");
  zassert_equal(1, zephyrLedStripSetPixel_fake.call_count,
    "ledCtrlSetRightEncPixelDefaultMode failed to set the pixel color.");
  zassert_equal(&ledStrip, zephyrLedStripSetPixel_fake.arg0_val,
    "ledCtrlSetRightEncPixelDefaultMode failed to set the pixel color.");
  zassert_equal(0, zephyrLedStripSetPixel_fake.arg1_val,
    "ledCtrlSetRightEncPixelDefaultMode failed to set the pixel color.");
  zassert_equal(&encDefColor, zephyrLedStripSetPixel_fake.arg2_val,
    "ledCtrlSetRightEncPixelDefaultMode failed to set the pixel color.");
  zassert_equal(1, zephyrLedStripUpdate_fake.call_count,
    "ledCtrlSetRightEncPixelDefaultMode failed to update the LED strip.");
  zassert_equal(&ledStrip, zephyrLedStripUpdate_fake.arg0_val,
    "ledCtrlSetRightEncPixelDefaultMode failed to update the LED strip.");
}

/**
 * @test  ledCtrlSetRightEncPixelSecondaryMode must return the error code
 *        if setting the right encoder pixel color fails.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetRightEncPixelSecondaryMode_SetColorFail)
{
  int failRet = -EDOM;

  zephyrLedStripSetPixel_fake.return_val = failRet;

  zassert_equal(failRet, ledCtrlSetRightEncPixelSecondaryMode(),
    "ledCtrlSetRightEncPixelSecondaryMode failed to return the error code.");
  zassert_equal(1, zephyrLedStripSetPixel_fake.call_count,
    "ledCtrlSetRightEncPixelSecondaryMode failed to set the pixel color.");
  zassert_equal(&ledStrip, zephyrLedStripSetPixel_fake.arg0_val,
    "ledCtrlSetRightEncPixelSecondaryMode failed to set the pixel color.");
  zassert_equal(0, zephyrLedStripSetPixel_fake.arg1_val,
    "ledCtrlSetRightEncPixelSecondaryMode failed to set the pixel color.");
  zassert_equal(&encSecColor, zephyrLedStripSetPixel_fake.arg2_val,
    "ledCtrlSetRightEncPixelSecondaryMode failed to set the pixel color.");
}

/**
 * @test  ledCtrlSetRightEncPixelSecondaryMode must return the error code
 *        if updating the LED strip fails.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetRightEncPixelSecondaryMode_UpdateStripFail)
{
  int successRet = 0;
  int failRet = -EDOM;

  zephyrLedStripSetPixel_fake.return_val = successRet;
  zephyrLedStripUpdate_fake.return_val = failRet;

  zassert_equal(failRet, ledCtrlSetRightEncPixelSecondaryMode(),
    "ledCtrlSetRightEncPixelSecondaryMode failed to return the error code.");
  zassert_equal(1, zephyrLedStripSetPixel_fake.call_count,
    "ledCtrlSetRightEncPixelSecondaryMode failed to set the pixel color.");
  zassert_equal(&ledStrip, zephyrLedStripSetPixel_fake.arg0_val,
    "ledCtrlSetRightEncPixelSecondaryMode failed to set the pixel color.");
  zassert_equal(0, zephyrLedStripSetPixel_fake.arg1_val,
    "ledCtrlSetRightEncPixelSecondaryMode failed to set the pixel color.");
  zassert_equal(&encSecColor, zephyrLedStripSetPixel_fake.arg2_val,
    "ledCtrlSetRightEncPixelSecondaryMode failed to set the pixel color.");
  zassert_equal(1, zephyrLedStripUpdate_fake.call_count,
    "ledCtrlSetRightEncPixelSecondaryMode failed to update the LED strip.");
  zassert_equal(&ledStrip, zephyrLedStripUpdate_fake.arg0_val,
    "ledCtrlSetRightEncPixelSecondaryMode failed to update the LED strip.");
}

/**
 * @test  ledCtrlSetLeftEncPixelDefaultMode must return the error code
 *        if setting the right encoder pixel color fails.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetLeftEncPixelDefaultMode_SetColorFail)
{
  int failRet = -EDOM;

  zephyrLedStripSetPixel_fake.return_val = failRet;

  zassert_equal(failRet, ledCtrlSetLeftEncPixelDefaultMode(),
    "ledCtrlSetLeftEncPixelDefaultMode failed to return the error code.");
  zassert_equal(1, zephyrLedStripSetPixel_fake.call_count,
    "ledCtrlSetLeftEncPixelDefaultMode failed to set the pixel color.");
  zassert_equal(&ledStrip, zephyrLedStripSetPixel_fake.arg0_val,
    "ledCtrlSetLeftEncPixelDefaultMode failed to set the pixel color.");
  zassert_equal(1, zephyrLedStripSetPixel_fake.arg1_val,
    "ledCtrlSetLeftEncPixelDefaultMode failed to set the pixel color.");
  zassert_equal(&encDefColor, zephyrLedStripSetPixel_fake.arg2_val,
    "ledCtrlSetLeftEncPixelDefaultMode failed to set the pixel color.");
}

/**
 * @test  ledCtrlSetLeftEncPixelDefaultMode must return the error code
 *        if updating the LED strip fails.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetLeftEncPixelDefaultMode_UpdateStripFail)
{
  int successRet = 0;
  int failRet = -EDOM;

  zephyrLedStripSetPixel_fake.return_val = successRet;
  zephyrLedStripUpdate_fake.return_val = failRet;

  zassert_equal(failRet, ledCtrlSetLeftEncPixelDefaultMode(),
    "ledCtrlSetLeftEncPixelDefaultMode failed to return the error code.");
  zassert_equal(1, zephyrLedStripSetPixel_fake.call_count,
    "ledCtrlSetLeftEncPixelDefaultMode failed to set the pixel color.");
  zassert_equal(&ledStrip, zephyrLedStripSetPixel_fake.arg0_val,
    "ledCtrlSetLeftEncPixelDefaultMode failed to set the pixel color.");
  zassert_equal(1, zephyrLedStripSetPixel_fake.arg1_val,
    "ledCtrlSetLeftEncPixelDefaultMode failed to set the pixel color.");
  zassert_equal(&encDefColor, zephyrLedStripSetPixel_fake.arg2_val,
    "ledCtrlSetLeftEncPixelDefaultMode failed to set the pixel color.");
  zassert_equal(1, zephyrLedStripUpdate_fake.call_count,
    "ledCtrlSetLeftEncPixelDefaultMode failed to update the LED strip.");
  zassert_equal(&ledStrip, zephyrLedStripUpdate_fake.arg0_val,
    "ledCtrlSetLeftEncPixelDefaultMode failed to update the LED strip.");
}

/**
 * @test  ledCtrlSetLeftEncPixelDefaultMode must set the right encoder
 *        pixel to the default mode color, update the LED strip and
 *        return the success code.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetLeftEncPixelDefaultMode_Success)
{
  int successRet = 0;

  zephyrLedStripSetPixel_fake.return_val = successRet;
  zephyrLedStripUpdate_fake.return_val = successRet;

  zassert_equal(successRet, ledCtrlSetLeftEncPixelDefaultMode(),
    "ledCtrlSetLeftEncPixelDefaultMode failed to return the success code.");
  zassert_equal(1, zephyrLedStripSetPixel_fake.call_count,
    "ledCtrlSetLeftEncPixelDefaultMode failed to set the pixel color.");
  zassert_equal(&ledStrip, zephyrLedStripSetPixel_fake.arg0_val,
    "ledCtrlSetLeftEncPixelDefaultMode failed to set the pixel color.");
  zassert_equal(1, zephyrLedStripSetPixel_fake.arg1_val,
    "ledCtrlSetLeftEncPixelDefaultMode failed to set the pixel color.");
  zassert_equal(&encDefColor, zephyrLedStripSetPixel_fake.arg2_val,
    "ledCtrlSetLeftEncPixelDefaultMode failed to set the pixel color.");
  zassert_equal(1, zephyrLedStripUpdate_fake.call_count,
    "ledCtrlSetLeftEncPixelDefaultMode failed to update the LED strip.");
  zassert_equal(&ledStrip, zephyrLedStripUpdate_fake.arg0_val,
    "ledCtrlSetLeftEncPixelDefaultMode failed to update the LED strip.");
}

/**
 * @test  ledCtrlSetLeftEncPixelSecondaryMode must return the error code
 *        if setting the right encoder pixel color fails.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetLeftEncPixelSecondaryMode_SetColorFail)
{
  int failRet = -EDOM;

  zephyrLedStripSetPixel_fake.return_val = failRet;

  zassert_equal(failRet, ledCtrlSetLeftEncPixelSecondaryMode(),
    "ledCtrlSetLeftEncPixelSecondaryMode failed to return the error code.");
  zassert_equal(1, zephyrLedStripSetPixel_fake.call_count,
    "ledCtrlSetLeftEncPixelSecondaryMode failed to set the pixel color.");
  zassert_equal(&ledStrip, zephyrLedStripSetPixel_fake.arg0_val,
    "ledCtrlSetLeftEncPixelSecondaryMode failed to set the pixel color.");
  zassert_equal(1, zephyrLedStripSetPixel_fake.arg1_val,
    "ledCtrlSetLeftEncPixelSecondaryMode failed to set the pixel color.");
  zassert_equal(&encSecColor, zephyrLedStripSetPixel_fake.arg2_val,
    "ledCtrlSetLeftEncPixelSecondaryMode failed to set the pixel color.");
}

/**
 * @test  ledCtrlSetLeftEncPixelSecondaryMode must return the error code
 *        if updating the LED strip fails.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetLeftEncPixelSecondaryMode_UpdateStripFail)
{
  int successRet = 0;
  int failRet = -EDOM;

  zephyrLedStripSetPixel_fake.return_val = successRet;
  zephyrLedStripUpdate_fake.return_val = failRet;

  zassert_equal(failRet, ledCtrlSetLeftEncPixelSecondaryMode(),
    "ledCtrlSetLeftEncPixelSecondaryMode failed to return the error code.");
  zassert_equal(1, zephyrLedStripSetPixel_fake.call_count,
    "ledCtrlSetLeftEncPixelSecondaryMode failed to set the pixel color.");
  zassert_equal(&ledStrip, zephyrLedStripSetPixel_fake.arg0_val,
    "ledCtrlSetLeftEncPixelSecondaryMode failed to set the pixel color.");
  zassert_equal(1, zephyrLedStripSetPixel_fake.arg1_val,
    "ledCtrlSetLeftEncPixelSecondaryMode failed to set the pixel color.");
  zassert_equal(&encSecColor, zephyrLedStripSetPixel_fake.arg2_val,
    "ledCtrlSetLeftEncPixelSecondaryMode failed to set the pixel color.");
  zassert_equal(1, zephyrLedStripUpdate_fake.call_count,
    "ledCtrlSetLeftEncPixelSecondaryMode failed to update the LED strip.");
  zassert_equal(&ledStrip, zephyrLedStripUpdate_fake.arg0_val,
    "ledCtrlSetLeftEncPixelSecondaryMode failed to update the LED strip.");
}

/**
 * @test  ledCtrlSetLeftEncPixelSecondaryMode must set the right encoder
 *        pixel to the default mode color, update the LED strip and
 *        return the success code.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetLeftEncPixelSecondaryMode_Success)
{
  int successRet = 0;

  zephyrLedStripSetPixel_fake.return_val = successRet;
  zephyrLedStripUpdate_fake.return_val = successRet;

  zassert_equal(successRet, ledCtrlSetLeftEncPixelSecondaryMode(),
    "ledCtrlSetLeftEncPixelSecondaryMode failed to return the success code.");
  zassert_equal(1, zephyrLedStripSetPixel_fake.call_count,
    "ledCtrlSetLeftEncPixelSecondaryMode failed to set the pixel color.");
  zassert_equal(&ledStrip, zephyrLedStripSetPixel_fake.arg0_val,
    "ledCtrlSetLeftEncPixelSecondaryMode failed to set the pixel color.");
  zassert_equal(1, zephyrLedStripSetPixel_fake.arg1_val,
    "ledCtrlSetLeftEncPixelSecondaryMode failed to set the pixel color.");
  zassert_equal(&encSecColor, zephyrLedStripSetPixel_fake.arg2_val,
    "ledCtrlSetLeftEncPixelSecondaryMode failed to set the pixel color.");
  zassert_equal(1, zephyrLedStripUpdate_fake.call_count,
    "ledCtrlSetLeftEncPixelSecondaryMode failed to update the LED strip.");
  zassert_equal(&ledStrip, zephyrLedStripUpdate_fake.arg0_val,
    "ledCtrlSetLeftEncPixelSecondaryMode failed to update the LED strip.");
}

#define BAD_PIXEL_DATA_SIZE_TEST_COUNT    2
/**
 * @test  ledCtrlSetRpmChaserPixels must return the error code if the pixel
 *        data size is not the necessary size.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetRpmChaserPixels_BadSize)
{
  int failRet = -EINVAL;
  uint8_t pixels[TEST_RPM_CHASER_PIXEL_SIZE];
  size_t sizes[BAD_PIXEL_DATA_SIZE_TEST_COUNT] =
    {TEST_RPM_CHASER_PIXEL_SIZE + 2, TEST_RPM_CHASER_PIXEL_SIZE + 5};

  for(uint8_t i = 0; i < BAD_PIXEL_DATA_SIZE_TEST_COUNT; ++i)
  {
    zassert_equal(failRet, ledCtrlSetRpmChaserPixels(pixels, sizes[i]),
      "ledCtrlSetRpmChaserPixels failed to return the error code.");
    zassert_equal(0, zephyrLedStripSetPixels_fake.call_count,
      "ledCtrlSetRpmChaserPixels failed to return the error code.");
  }
}

/**
 * @test  ledCtrlSetRpmChaserPixels must return the error code if setting
 *        the RPM chaser pixels color fails.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetRpmChaserPixels_SetPixelsFail)
{
  int failRet = -EDOM;
  uint8_t pixels[TEST_RPM_CHASER_PIXEL_SIZE];

  ledStrip.pixelCount = TEST_RPM_CHASER_PIXEL_SIZE;
  zephyrLedStripSetPixels_fake.return_val = failRet;

  zassert_equal(failRet, ledCtrlSetRpmChaserPixels(pixels,
    TEST_RPM_CHASER_PIXEL_SIZE),
    "ledCtrlSetRpmChaserPixels failed to return the error code.");
  zassert_equal(1, zephyrLedStripSetPixels_fake.call_count,
    "ledCtrlSetLeftEncPixelSecondaryMode failed to set the pixel color.");
  zassert_equal(&ledStrip, zephyrLedStripSetPixels_fake.arg0_val,
    "ledCtrlSetLeftEncPixelSecondaryMode failed to set the pixel color.");
  zassert_equal(2, zephyrLedStripSetPixels_fake.arg1_val,
    "ledCtrlSetLeftEncPixelSecondaryMode failed to set the pixel color.");
  zassert_equal(TEST_RPM_CHASER_PIXEL_SIZE,
    zephyrLedStripSetPixels_fake.arg2_val,
    "ledCtrlSetLeftEncPixelSecondaryMode failed to set the pixel color.");
  zassert_equal(rpmPixels, zephyrLedStripSetPixels_fake.arg3_val,
    "ledCtrlSetLeftEncPixelSecondaryMode failed to set the pixel color.");
}

/**
 * @test  ledCtrlSetRpmChaserPixels must return the error code if updating
 *        the LED strip fails.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetRpmChaserPixels_UpdateFail)
{
  int successRet = 0;
  int failRet = -EDOM;
  uint8_t pixels[TEST_RPM_CHASER_PIXEL_SIZE];

  ledStrip.pixelCount = TEST_RPM_CHASER_PIXEL_SIZE;
  zephyrLedStripSetPixels_fake.return_val = successRet;
  zephyrLedStripUpdate_fake.return_val = failRet;

  zassert_equal(failRet, ledCtrlSetRpmChaserPixels(pixels,
    TEST_RPM_CHASER_PIXEL_SIZE),
    "ledCtrlSetRpmChaserPixels failed to return the error code.");
  zassert_equal(1, zephyrLedStripSetPixels_fake.call_count,
    "ledCtrlSetRpmChaserPixels failed to set the pixel color.");
  zassert_equal(&ledStrip, zephyrLedStripSetPixels_fake.arg0_val,
    "ledCtrlSetRpmChaserPixels failed to set the pixel color.");
  zassert_equal(2, zephyrLedStripSetPixels_fake.arg1_val,
    "ledCtrlSetRpmChaserPixels failed to set the pixel color.");
  zassert_equal(TEST_RPM_CHASER_PIXEL_SIZE,
    zephyrLedStripSetPixels_fake.arg2_val,
    "ledCtrlSetRpmChaserPixels failed to set the pixel color.");
  zassert_equal(rpmPixels, zephyrLedStripSetPixels_fake.arg3_val,
    "ledCtrlSetLeftEncPixelSecondaryMode failed to set the pixel color.");
  zassert_equal(1, zephyrLedStripUpdate_fake.call_count,
    "ledCtrlSetRpmChaserPixels failed to update the LED strip.");
  zassert_equal(&ledStrip, zephyrLedStripUpdate_fake.arg0_val,
    "ledCtrlSetRpmChaserPixels failed to update the LED strip.");
}

/**
 * @test  ledCtrlSetRpmChaserPixels must set the RPM chaser pixels color,
 *        update the LED strip and return the success code.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetRpmChaserPixels_Success)
{
  int successRet = 0;
  uint8_t pixels[TEST_RPM_CHASER_PIXEL_SIZE];

  ledStrip.pixelCount = TEST_RPM_CHASER_PIXEL_SIZE;
  zephyrLedStripSetPixels_fake.return_val = successRet;
  zephyrLedStripUpdate_fake.return_val = successRet;

  for(uint8_t i = 0; i < TEST_RPM_CHASER_PIXEL_SIZE; ++i)
    pixels[i] = i;

  zassert_equal(successRet, ledCtrlSetRpmChaserPixels(pixels,
    TEST_RPM_CHASER_PIXEL_SIZE),
    "ledCtrlSetRpmChaserPixels failed to return the error code.");
  zassert_equal(1, zephyrLedStripSetPixels_fake.call_count,
    "ledCtrlSetRpmChaserPixels failed to set the pixel color.");
  zassert_equal(&ledStrip, zephyrLedStripSetPixels_fake.arg0_val,
    "ledCtrlSetRpmChaserPixels failed to set the pixel color.");
  zassert_equal(2, zephyrLedStripSetPixels_fake.arg1_val,
    "ledCtrlSetRpmChaserPixels failed to set the pixel color.");
  zassert_equal(TEST_RPM_CHASER_PIXEL_SIZE,
    zephyrLedStripSetPixels_fake.arg2_val,
    "ledCtrlSetRpmChaserPixels failed to set the pixel color.");
  zassert_equal(rpmPixels, zephyrLedStripSetPixels_fake.arg3_val,
    "ledCtrlSetLeftEncPixelSecondaryMode failed to set the pixel color.");
  zassert_equal(1, zephyrLedStripUpdate_fake.call_count,
    "ledCtrlSetRpmChaserPixels failed to update the LED strip.");
  zassert_equal(&ledStrip, zephyrLedStripUpdate_fake.arg0_val,
    "ledCtrlSetRpmChaserPixels failed to update the LED strip.");

  for(uint8_t i = 0; i < RPM_CHASER_PIXEL_COUNT; ++i)
  {
    zassert_equal(pixels[i], rpmPixels[i].r);
    zassert_equal(pixels[i + 1], rpmPixels[i].g);
    zassert_equal(pixels[i + 2], rpmPixels[i].b);
  }
}

/** @} */
