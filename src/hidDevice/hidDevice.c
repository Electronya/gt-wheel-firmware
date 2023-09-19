/**
 * Copyright (C) 2023 by Electronya
 *
 * @file      hidDevice.c
 * @author    jbacon
 * @date      2023-08-24
 * @brief     HID Device Module
 *
 *            This file is the implementation of the HID device module.
 *
 * @ingroup  hidDevice
 *
 * @{
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

#include "hidDevice.h"
#include "zephyrCommon.h"
#include "zephyrThread.h"

#define HID_DEVICE_MODULE_NAME  hid_device_module

/* Setting module logging */
LOG_MODULE_REGISTER(HID_DEVICE_MODULE_NAME);

int hidDeviceInit(void)
{
  return 0;
}

/** @} */
