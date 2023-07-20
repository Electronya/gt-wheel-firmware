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

#include "LedCtrl.h"
#include "zephyrCommon.h"
#include "zephyrLedStrip.h"

#define LED_CTRL_MODULE_NAME led_ctrl_module

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

int ledCtrlInit(void)
{
  int rc;

  rc = zephyrLedStripInit(&ledStrip, LED_STRIP_COLOR_RGB, ledStrip.pixelCount);
  return rc;
}

uint32_t ledCtrlGetPixelCount(void)
{
  return ledStrip.pixelCount;
}

/** @} */
