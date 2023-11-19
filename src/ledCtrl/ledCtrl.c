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

#include "ledCtrl.h"
#include "zephyrCommon.h"

#define LED_CTRL_MODULE_NAME led_ctrl_module


/**
 * @brief The right encoder pixel index.
*/
#define RIGHT_ENCODER_PIXEL_IDX       0

/**
 * @brief The left encoder pixel index.
*/
#define LEFT_ENCODER_PIXEL_IDX        1

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
  .g = 0x00,
  .r = 0x00,
  .b = 0x0f,
};

/**
 * @brief Encoder pixel secondary color.
*/
ZephyrRgbLed encSecColor = {
  .g = 0x00,
  .r = 0x0f,
  .b = 0x00,
};

int ledCtrlInit(void)
{
  int rc;

  rc = zephyrLedStripInit(&ledStrip, ledStrip.pixelCount);
  return rc;
}

uint32_t ledCtrlGetRpmChaserPxlCnt(void)
{
  return zephyrLedStripGetPixelCnt(&ledStrip) - RPM_CHASER_PIXEL_OFFSET;
}

int ledCtrlSetRightEncPixelDefaultMode(void)
{
  int rc;

  rc = zephyrLedStripSetPixel(&ledStrip, RIGHT_ENCODER_PIXEL_IDX,
    &encDefColor);
  if(rc < 0)
    return rc;

  return zephyrLedStripUpdate(&ledStrip);
}

int ledCtrlSetRightEncPixelSecondaryMode(void)
{
  int rc;

  rc = zephyrLedStripSetPixel(&ledStrip, RIGHT_ENCODER_PIXEL_IDX,
    &encSecColor);
  if(rc < 0)
    return rc;

  return zephyrLedStripUpdate(&ledStrip);
}

int ledCtrlSetLeftEncPixelDefaultMode(void)
{
  int rc;

  rc = zephyrLedStripSetPixel(&ledStrip, LEFT_ENCODER_PIXEL_IDX,
    &encDefColor);
  if(rc < 0)
    return rc;

  return zephyrLedStripUpdate(&ledStrip);
}

int ledCtrlSetLeftEncPixelSecondaryMode(void)
{
  int rc;

  rc = zephyrLedStripSetPixel(&ledStrip, LEFT_ENCODER_PIXEL_IDX,
    &encSecColor);
  if(rc < 0)
    return rc;

  return zephyrLedStripUpdate(&ledStrip);
}

int ledCtrlSetRpmChaserPixels(ZephyrRgbLed *pixels)
{
  int rc;

  rc = zephyrLedStripSetPixels(&ledStrip, RPM_CHASER_PIXEL_OFFSET,
    ledStrip.pixelCount, pixels);
  if(rc < 0)
    return rc;

  return zephyrLedStripUpdate(&ledStrip);
}

/** @} */
