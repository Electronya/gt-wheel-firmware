/**
 * Copyright (C) 2023 by Electronya
 *
 * @file      main.c
 * @author    jbacon
 * @date      2023-07-14
 * @brief     DIY GT wheel Firmware
 *
 * This file is the entry point for the Electronya DIY GT Wheel.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "acmDevice.h"
#include "ledCtrl.h"
#include "zephyrUSB.h"

#define MAIN_MODULE_NAME main_module

/* Setting module logging */
LOG_MODULE_REGISTER(MAIN_MODULE_NAME);

void main(void)
{
  int rc;

  LOG_INF("booting gt wheel");

  rc = ledCtrlInit();
  if(rc < 0)
  {
    LOG_ERR("LED control failed to initialize");
    return;
  }

  rc = acmDeviceInit();
  if(rc < 0)
  {
    LOG_ERR("ACM device failed to initialize");
    return;
  }

  rc = zephyrUsbEnable(NULL);
  if(rc < 0)
  {
    LOG_ERR("USB failed to enable");
    return;
  }

  rc = acmDeviceStart();
  if(rc < 0)
  {
    LOG_ERR("ACM device failed to start");
    return;
  }
}
