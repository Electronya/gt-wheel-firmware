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
FAKE_VALUE_FUNC(int, zephyrLedStripInit, ZephyrLedStrip*, ZephyrLedStripClrFmt,
  uint32_t);
FAKE_VALUE_FUNC(uint32_t, zephyrLedStripGetPixelCnt, ZephyrLedStrip*);
FAKE_VALUE_FUNC(int, zephyrLedStripSetGrbPixel, ZephyrLedStrip*, uint32_t,
  const ZephyrGrbPixel*);
FAKE_VALUE_FUNC(int, zephyrLedStripSetGrbPixels, ZephyrLedStrip*, uint32_t,
  uint32_t, const ZephyrGrbPixel*);
FAKE_VALUE_FUNC(int, zephyrLedStripUpdate, ZephyrLedStrip*);

static void ledCtrlCaseSetup(void *f)
{
  RESET_FAKE(zephyrLedStripInit);
  RESET_FAKE(zephyrLedStripGetPixelCnt);
  RESET_FAKE(zephyrLedStripSetGrbPixel);
  RESET_FAKE(zephyrLedStripSetGrbPixels);
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
    zassert_equal(1, zephyrLedStripInit_fake.call_count);
    zassert_equal(&ledStrip, zephyrLedStripInit_fake.arg0_val);
    zassert_equal(LED_STRIP_COLOR_RGB, zephyrLedStripInit_fake.arg1_val);
    zassert_equal(pixelCounts[i], zephyrLedStripInit_fake.arg2_val);
    zassert_equal(returnVals[i], result);

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
    zassert_equal(1, zephyrLedStripInit_fake.call_count);
    zassert_equal(&ledStrip, zephyrLedStripInit_fake.arg0_val);
    zassert_equal(LED_STRIP_COLOR_RGB, zephyrLedStripInit_fake.arg1_val);
    zassert_equal(pixelCounts[i], zephyrLedStripInit_fake.arg2_val);
    zassert_equal(returnVals[i], result);

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

  zephyrLedStripSetGrbPixel_fake.return_val = failRet;

  zassert_equal(failRet, ledCtrlSetRightEncPixelDefaultMode());
  zassert_equal(1, zephyrLedStripSetGrbPixel_fake.call_count);
  zassert_equal(&ledStrip, zephyrLedStripSetGrbPixel_fake.arg0_val);
  zassert_equal(0, zephyrLedStripSetGrbPixel_fake.arg1_val);
  zassert_equal(&encDefColor, zephyrLedStripSetGrbPixel_fake.arg2_val);
}

/**
 * @test  ledCtrlSetRightEncPixelDefaultMode must return the error code
 *        if updating the LED strip fails.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetRightEncPixelDefaultMode_UpdateStripFail)
{
  int successRet = 0;
  int failRet = -EDOM;

  zephyrLedStripSetGrbPixel_fake.return_val = successRet;
  zephyrLedStripUpdate_fake.return_val = failRet;

  zassert_equal(failRet, ledCtrlSetRightEncPixelDefaultMode());
  zassert_equal(1, zephyrLedStripSetGrbPixel_fake.call_count);
  zassert_equal(&ledStrip, zephyrLedStripSetGrbPixel_fake.arg0_val);
  zassert_equal(0, zephyrLedStripSetGrbPixel_fake.arg1_val);
  zassert_equal(&encDefColor, zephyrLedStripSetGrbPixel_fake.arg2_val);
  zassert_equal(1, zephyrLedStripUpdate_fake.call_count);
  zassert_equal(&ledStrip, zephyrLedStripUpdate_fake.arg0_val);
}

/**
 * @test  ledCtrlSetRightEncPixelDefaultMode must set the right encoder
 *        pixel to the default mode color, update the LED strip and
 *        return the success code.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetRightEncPixelDefaultMode_Success)
{
  int successRet = 0;

  zephyrLedStripSetGrbPixel_fake.return_val = successRet;
  zephyrLedStripUpdate_fake.return_val = successRet;

  zassert_equal(successRet, ledCtrlSetRightEncPixelDefaultMode());
  zassert_equal(1, zephyrLedStripSetGrbPixel_fake.call_count);
  zassert_equal(&ledStrip, zephyrLedStripSetGrbPixel_fake.arg0_val);
  zassert_equal(0, zephyrLedStripSetGrbPixel_fake.arg1_val);
  zassert_equal(&encDefColor, zephyrLedStripSetGrbPixel_fake.arg2_val);
  zassert_equal(1, zephyrLedStripUpdate_fake.call_count);
  zassert_equal(&ledStrip, zephyrLedStripUpdate_fake.arg0_val);
}

/**
 * @test  ledCtrlSetRightEncPixelSecondaryMode must return the error code
 *        if setting the right encoder pixel color fails.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetRightEncPixelSecondaryMode_SetColorFail)
{
  int failRet = -EDOM;

  zephyrLedStripSetGrbPixel_fake.return_val = failRet;

  zassert_equal(failRet, ledCtrlSetRightEncPixelSecondaryMode());
  zassert_equal(1, zephyrLedStripSetGrbPixel_fake.call_count);
  zassert_equal(&ledStrip, zephyrLedStripSetGrbPixel_fake.arg0_val);
  zassert_equal(0, zephyrLedStripSetGrbPixel_fake.arg1_val);
  zassert_equal(&encSecColor, zephyrLedStripSetGrbPixel_fake.arg2_val);
}

/**
 * @test  ledCtrlSetRightEncPixelSecondaryMode must return the error code
 *        if updating the LED strip fails.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetRightEncPixelSecondaryMode_UpdateStripFail)
{
  int successRet = 0;
  int failRet = -EDOM;

  zephyrLedStripSetGrbPixel_fake.return_val = successRet;
  zephyrLedStripUpdate_fake.return_val = failRet;

  zassert_equal(failRet, ledCtrlSetRightEncPixelSecondaryMode());
  zassert_equal(1, zephyrLedStripSetGrbPixel_fake.call_count);
  zassert_equal(&ledStrip, zephyrLedStripSetGrbPixel_fake.arg0_val);
  zassert_equal(0, zephyrLedStripSetGrbPixel_fake.arg1_val);
  zassert_equal(&encSecColor, zephyrLedStripSetGrbPixel_fake.arg2_val);
  zassert_equal(1, zephyrLedStripUpdate_fake.call_count);
  zassert_equal(&ledStrip, zephyrLedStripUpdate_fake.arg0_val);
}

/**
 * @test  ledCtrlSetLeftEncPixelDefaultMode must return the error code
 *        if setting the right encoder pixel color fails.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetLeftEncPixelDefaultMode_SetColorFail)
{
  int failRet = -EDOM;

  zephyrLedStripSetGrbPixel_fake.return_val = failRet;

  zassert_equal(failRet, ledCtrlSetLeftEncPixelDefaultMode());
  zassert_equal(1, zephyrLedStripSetGrbPixel_fake.call_count);
  zassert_equal(&ledStrip, zephyrLedStripSetGrbPixel_fake.arg0_val);
  zassert_equal(1, zephyrLedStripSetGrbPixel_fake.arg1_val);
  zassert_equal(&encDefColor, zephyrLedStripSetGrbPixel_fake.arg2_val);
}

/**
 * @test  ledCtrlSetLeftEncPixelDefaultMode must return the error code
 *        if updating the LED strip fails.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetLeftEncPixelDefaultMode_UpdateStripFail)
{
  int successRet = 0;
  int failRet = -EDOM;

  zephyrLedStripSetGrbPixel_fake.return_val = successRet;
  zephyrLedStripUpdate_fake.return_val = failRet;

  zassert_equal(failRet, ledCtrlSetLeftEncPixelDefaultMode());
  zassert_equal(1, zephyrLedStripSetGrbPixel_fake.call_count);
  zassert_equal(&ledStrip, zephyrLedStripSetGrbPixel_fake.arg0_val);
  zassert_equal(1, zephyrLedStripSetGrbPixel_fake.arg1_val);
  zassert_equal(&encDefColor, zephyrLedStripSetGrbPixel_fake.arg2_val);
  zassert_equal(1, zephyrLedStripUpdate_fake.call_count);
  zassert_equal(&ledStrip, zephyrLedStripUpdate_fake.arg0_val);
}

/**
 * @test  ledCtrlSetLeftEncPixelDefaultMode must set the right encoder
 *        pixel to the default mode color, update the LED strip and
 *        return the success code.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetLeftEncPixelDefaultMode_Success)
{
  int successRet = 0;

  zephyrLedStripSetGrbPixel_fake.return_val = successRet;
  zephyrLedStripUpdate_fake.return_val = successRet;

  zassert_equal(successRet, ledCtrlSetLeftEncPixelDefaultMode());
  zassert_equal(1, zephyrLedStripSetGrbPixel_fake.call_count);
  zassert_equal(&ledStrip, zephyrLedStripSetGrbPixel_fake.arg0_val);
  zassert_equal(1, zephyrLedStripSetGrbPixel_fake.arg1_val);
  zassert_equal(&encDefColor, zephyrLedStripSetGrbPixel_fake.arg2_val);
  zassert_equal(1, zephyrLedStripUpdate_fake.call_count);
  zassert_equal(&ledStrip, zephyrLedStripUpdate_fake.arg0_val);
}

/**
 * @test  ledCtrlSetLeftEncPixelSecondaryMode must return the error code
 *        if setting the right encoder pixel color fails.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetLeftEncPixelSecondaryMode_SetColorFail)
{
  int failRet = -EDOM;

  zephyrLedStripSetGrbPixel_fake.return_val = failRet;

  zassert_equal(failRet, ledCtrlSetLeftEncPixelSecondaryMode());
  zassert_equal(1, zephyrLedStripSetGrbPixel_fake.call_count);
  zassert_equal(&ledStrip, zephyrLedStripSetGrbPixel_fake.arg0_val);
  zassert_equal(1, zephyrLedStripSetGrbPixel_fake.arg1_val);
  zassert_equal(&encSecColor, zephyrLedStripSetGrbPixel_fake.arg2_val);
}

/**
 * @test  ledCtrlSetLeftEncPixelSecondaryMode must return the error code
 *        if updating the LED strip fails.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetLeftEncPixelSecondaryMode_UpdateStripFail)
{
  int successRet = 0;
  int failRet = -EDOM;

  zephyrLedStripSetGrbPixel_fake.return_val = successRet;
  zephyrLedStripUpdate_fake.return_val = failRet;

  zassert_equal(failRet, ledCtrlSetLeftEncPixelSecondaryMode());
  zassert_equal(1, zephyrLedStripSetGrbPixel_fake.call_count);
  zassert_equal(&ledStrip, zephyrLedStripSetGrbPixel_fake.arg0_val);
  zassert_equal(1, zephyrLedStripSetGrbPixel_fake.arg1_val);
  zassert_equal(&encSecColor, zephyrLedStripSetGrbPixel_fake.arg2_val);
  zassert_equal(1, zephyrLedStripUpdate_fake.call_count);
  zassert_equal(&ledStrip, zephyrLedStripUpdate_fake.arg0_val);
}

/**
 * @test  ledCtrlSetLeftEncPixelSecondaryMode must set the right encoder
 *        pixel to the default mode color, update the LED strip and
 *        return the success code.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetLeftEncPixelSecondaryMode_Success)
{
  int successRet = 0;

  zephyrLedStripSetGrbPixel_fake.return_val = successRet;
  zephyrLedStripUpdate_fake.return_val = successRet;

  zassert_equal(successRet, ledCtrlSetLeftEncPixelSecondaryMode());
  zassert_equal(1, zephyrLedStripSetGrbPixel_fake.call_count);
  zassert_equal(&ledStrip, zephyrLedStripSetGrbPixel_fake.arg0_val);
  zassert_equal(1, zephyrLedStripSetGrbPixel_fake.arg1_val);
  zassert_equal(&encSecColor, zephyrLedStripSetGrbPixel_fake.arg2_val);
  zassert_equal(1, zephyrLedStripUpdate_fake.call_count);
  zassert_equal(&ledStrip, zephyrLedStripUpdate_fake.arg0_val);
}

/**
 * @test  ledCtrlSetRpmChaserPixels must return the error code if the size
 *        of the SIMHUB LED data packet is the wrong size.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetRpmChaserPixels_RxPacketSizeFail)
{
  int failRet = -EINVAL;
  uint8_t pixels[RPM_CHASER_PIXEL_COUNT];

  zassert_equal(failRet, ledCtrlSetRpmChaserPixels(pixels,
    RPM_CHASER_PIXEL_COUNT));
}

/**
 * @test  ledCtrlSetRpmChaserPixels must return the error code if setting
 *        the RPM chaser pixels color fails.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetRpmChaserPixels_SetPixelsFail)
{
  int failRet = -EDOM;
  uint8_t pixels[RPM_CHASER_PIXEL_COUNT * 3];

  ledStrip.pixelCount = RPM_CHASER_PIXEL_COUNT;
  zephyrLedStripSetGrbPixels_fake.return_val = failRet;

  zassert_equal(failRet, ledCtrlSetRpmChaserPixels(pixels,
    RPM_CHASER_PIXEL_COUNT * 3));
  zassert_equal(1, zephyrLedStripSetGrbPixels_fake.call_count);
  zassert_equal(&ledStrip, zephyrLedStripSetGrbPixels_fake.arg0_val);
  zassert_equal(2, zephyrLedStripSetGrbPixels_fake.arg1_val);
  zassert_equal(RPM_CHASER_PIXEL_COUNT,
    zephyrLedStripSetGrbPixels_fake.arg2_val);
  zassert_equal(rpmPixels, zephyrLedStripSetGrbPixels_fake.arg3_val);
}

/**
 * @test  ledCtrlSetRpmChaserPixels must return the error code if updating
 *        the LED strip fails.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetRpmChaserPixels_UpdateFail)
{
  int successRet = 0;
  int failRet = -EDOM;
  uint8_t pixels[RPM_CHASER_PIXEL_COUNT * 3];

  ledStrip.pixelCount = RPM_CHASER_PIXEL_COUNT;
  zephyrLedStripSetGrbPixels_fake.return_val = successRet;
  zephyrLedStripUpdate_fake.return_val = failRet;

  zassert_equal(failRet, ledCtrlSetRpmChaserPixels(pixels,
    RPM_CHASER_PIXEL_COUNT * 3));
  zassert_equal(1, zephyrLedStripSetGrbPixels_fake.call_count);
  zassert_equal(&ledStrip, zephyrLedStripSetGrbPixels_fake.arg0_val);
  zassert_equal(2, zephyrLedStripSetGrbPixels_fake.arg1_val);
  zassert_equal(RPM_CHASER_PIXEL_COUNT,
    zephyrLedStripSetGrbPixels_fake.arg2_val);
  zassert_equal(rpmPixels, zephyrLedStripSetGrbPixels_fake.arg3_val);
  zassert_equal(1, zephyrLedStripUpdate_fake.call_count);
  zassert_equal(&ledStrip, zephyrLedStripUpdate_fake.arg0_val);
}

/**
 * @test  ledCtrlSetRpmChaserPixels must set the RPM chaser pixels color,
 *        update the LED strip and return the success code.
*/
ZTEST(ledCtrl_suite, test_ledCtrlSetRpmChaserPixels_Success)
{
  int successRet = 0;
  uint8_t pixels[RPM_CHASER_PIXEL_COUNT * 3];

  for(uint8_t i = 0; i < RPM_CHASER_PIXEL_COUNT * 3; ++i)
    pixels[i] = i;

  ledStrip.pixelCount = RPM_CHASER_PIXEL_COUNT;
  zephyrLedStripSetGrbPixels_fake.return_val = successRet;
  zephyrLedStripUpdate_fake.return_val = successRet;

  zassert_equal(successRet, ledCtrlSetRpmChaserPixels(pixels,
    RPM_CHASER_PIXEL_COUNT * 3));
  zassert_equal(1, zephyrLedStripSetGrbPixels_fake.call_count);
  zassert_equal(&ledStrip, zephyrLedStripSetGrbPixels_fake.arg0_val);
  zassert_equal(2, zephyrLedStripSetGrbPixels_fake.arg1_val);
  zassert_equal(RPM_CHASER_PIXEL_COUNT,
    zephyrLedStripSetGrbPixels_fake.arg2_val);
  zassert_equal(rpmPixels, zephyrLedStripSetGrbPixels_fake.arg3_val);
  zassert_equal(1, zephyrLedStripUpdate_fake.call_count);
  zassert_equal(&ledStrip, zephyrLedStripUpdate_fake.arg0_val);

  for(uint8_t i = 0; i < RPM_CHASER_PIXEL_COUNT; ++i)
  {
    zassert_equal(pixels[i + 0], rpmPixels[i].r);
    zassert_equal(pixels[i + 1], rpmPixels[i].g);
    zassert_equal(pixels[i + 2], rpmPixels[i].b);
  }
}

/** @} */
