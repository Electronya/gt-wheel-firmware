/**
 * Copyright (C) 2023 by Electronya
 *
 * @file      ledCtrlHw.h
 * @author    jbacon
 * @date      2023-05-06
 * @brief     Led Control Hardware
 *
 *            This file is the implementation of the LED control module.
 *
 * @ingroup  ledCtrl
 *
 * @{
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

#include "LedCtrl.h"
#include "zephyrCommon.h"

#define LED_CTRL_MODULE_NAME led_ctrl_module


/**
 * @brief The right encoder pixel index.
*/
#define RIGHT_ENCODER_PIXEL_IDX       0

/**
 * @brief The left encoder pixel index.
*/
#define LEF_ENCODER_PIXEL_IDX         1

/**
 * @brief The RPM chaser pixel offset.
*/
#define RPM_CHASER_PIXEL_OFFSET       2

/* Setting module logging */
LOG_MODULE_REGISTER(LED_CTRL_MODULE_NAME);

#ifndef CONFIG_ZTEST
static ZephyrLedStrip ledStrip = {
  .dev = DEVICE_DT_GET(DT_ALIAS(ledstrip)),
  .pixelCount = DT_PROP(DT_ALIAS(ledstrip), chain_length),
};
#else
static ZephyrLedStrip ledStrip;
#endif

/**
 * @brief Encoder pixel default color.
*/
ZephyrRgbLed encDefColor = {
  .b = 0x0f,
  .g = 0x00,
  .r = 0x00,
};

/**
 * @brief Encoder pixel secondary color.
*/
ZephyrRgbLed encSecColor = {
  .b = 0x00,
  .g = 0x00,
  .r = 0x0f,
};

int ledCtrlInit(void)
{
  int rc;

  rc = zephyrLedStripInit(&ledStrip, LED_STRIP_COLOR_RGB, ledStrip.pixelCount);
  return rc;
}

uint32_t ledCtrlGetRpmChaserPxlCnt(void)
{
  return ledStrip.pixelCount - RPM_CHASER_PIXEL_OFFSET;
}

void ledCtrlSetRightEncoderDefaultMode(void)
{

}

int ledCtrlSetRpmChaserPixels(ZephyrRgbLed *pixels, size_t pixelCnt)
{
  if(pixelCnt != ledStrip.pixelCount - RPM_CHASER_PIXEL_OFFSET)
    return -EDOM;

  bytecpy(ledStrip.rgbPixels + RPM_CHASER_PIXEL_OFFSET, pixels, 3 * pixelCnt);
  return 0;
}

/** @} */
