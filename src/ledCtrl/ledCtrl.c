/**
 * Copyright (C) 2023 by Electronya
 *
 * @file      ledCtrlHw.h
 * @author    jbacon
 * @date      2023-07-20
 * @brief     Led Control Module
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

/* Setting module logging */
LOG_MODULE_REGISTER(LED_CTRL_MODULE_NAME);

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

/**
 * @brief The RPM chaser pixel count.
*/
#define RPM_CHASER_PIXEL_COUNT        12

#ifndef CONFIG_ZTEST
static ZephyrLedStrip ledStrip = {
  .dev = DEVICE_DT_GET(DT_ALIAS(ledstrip)),
  .pixelCount = DT_PROP(DT_ALIAS(ledstrip), chain_length),
};
#else
static ZephyrLedStrip ledStrip;
#endif

ZephyrRgbLed rpmPixels[RPM_CHASER_PIXEL_COUNT];

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

int ledCtrlSetRpmChaserPixels(uint8_t *pixels, size_t size)
{
  int rc;

  if(size != RPM_CHASER_PIXEL_COUNT * 3)
    return -EINVAL;

  for(uint8_t i = 0; i < RPM_CHASER_PIXEL_COUNT; ++i)
  {
    rpmPixels[i].r = pixels[i + 0];
    rpmPixels[i].g = pixels[i + 1];
    rpmPixels[i].b = pixels[i + 2];
  }

  rc = zephyrLedStripSetPixels(&ledStrip, RPM_CHASER_PIXEL_OFFSET,
    ledStrip.pixelCount, rpmPixels);
  if(rc < 0)
    return rc;

  return zephyrLedStripUpdate(&ledStrip);
}

/** @} */
