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
#include "buttonMngr.h"
#include "zephyrCommon.h"
#include "zephyrHID.h"
#include "zephyrWork.h"
#include "zephyrWorkQueue.h"

#define HID_DEVICE_MODULE_NAME  hid_device_module

/* Setting module logging */
LOG_MODULE_REGISTER(HID_DEVICE_MODULE_NAME);

/* Creating the worker stack */
K_THREAD_STACK_DEFINE(hidWorkerStack, 128);

/**
 * @brief The HID device label.
*/
#define HID_DEVICE_LABEL              "HID_0"

/**
 * @brief The gamepad report.
*/
#define HID_GAMEPAD_REPORT            0x01

/**
 * @brief The button states report offset.
*/
#define HID_RPT_BTN_STATE_OFFSET      2

/**
 * @brief The HID device work queue.
*/
static ZephyrWorkQueue workQueue = {
  .priority = 1,
  .stack = hidWorkerStack,
  .stackSize = K_THREAD_STACK_SIZEOF(hidWorkerStack),
};

/**
 * @brief The HID device work.
*/
static ZephyrWork work;

/**
 * @brief The HID report (report ID + clutch value + button states).
*/
static uint8_t report[HID_RPT_BTN_STATE_OFFSET + BUTTON_COUNT];

/**
 * @brief HID descriptor.
*/
uint8_t hidDescriptor[] = {
  HID_USAGE_PAGE(HID_USAGE_GEN_DESKTOP),
	HID_USAGE(HID_USAGE_GEN_DESKTOP_GAMEPAD),
	HID_COLLECTION(HID_COLLECTION_APPLICATION),
  HID_COLLECTION(HID_COLLECTION_PHYSICAL),
	HID_REPORT_ID(HID_GAMEPAD_REPORT),
  HID_USAGE_PAGE(HID_USAGE_GEN_DESKTOP),
  HID_USAGE(HID_USAGE_GEN_DESKTOP_X),
  HID_LOGICAL_MIN8(0),
	HID_LOGICAL_MAX8(255),
	HID_REPORT_SIZE(8),
	HID_REPORT_COUNT(1),
	HID_INPUT(0x02),
  HID_USAGE_PAGE(HID_USAGE_GEN_BUTTON),
  HID_USAGE_MIN8(1),
  HID_USAGE_MAX8(BUTTON_COUNT),
  HID_LOGICAL_MIN8(0),
  HID_LOGICAL_MAX8(1),
  HID_REPORT_SIZE(8),
  HID_REPORT_COUNT(BUTTON_COUNT),
  HID_INPUT(0x02),
  HID_END_COLLECTION,
	HID_END_COLLECTION,
};

/**
 * @brief   The HID IN interrupt operation.
 *
 * @param dev   The HID device.
 */
static void hidInIntOp(const struct device *dev)
{
  int rc;

  rc = zephyrWorkSubmitToQueue(&workQueue, &work);
  if(rc < 0)
    LOG_ERR("unable to submit the HID IN interrupt work");
  else if(rc == 0)
    LOG_ERR("HID IN interrupt work already submitted");
}

/**
 * @brief The HID device.
*/
static ZephyrHID hidDev = {
  .ops = {
    .int_in_ready = hidInIntOp,
  },
};

/**
 * @brief   The HID device worker.
 *
 * @param item  The work item.
 */
static void hidWorker(struct k_work *item)
{
  int rc;
  uint8_t *repBtnStates = (WheelButtonState *)report + HID_RPT_BTN_STATE_OFFSET;

  report[0] = HID_GAMEPAD_REPORT;

  // TODO: read the clutch value.

  rc = buttonMngrGetAllStates(repBtnStates, BUTTON_COUNT);
  if(rc < 0)
  {
    LOG_ERR("unable to populate button report with button states");
    return;
  }
}

int hidDeviceInit(void)
{
  int rc;

  hidDev.descriptor = hidDescriptor;
  rc = zephyrHidInit(&hidDev, HID_DEVICE_LABEL);
  if(rc < 0)
    return rc;

  work.handler = hidWorker;
  zephyrWorkInit(&work);
  zephyrWorkQueueInit(&workQueue);

  return rc;
}

/** @} */
