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

#define FFF_ARG_HISTORY_LEN 84

#include <zephyr/ztest.h>
#include <zephyr/fff.h>
#include <zephyr/kernel.h>

#include "hidDevice.h"
#include "hidDevice.c"

#include "zephyrHID.h"
#include "zephyrWorkQueue.h"
#include "zephyrWork.h"

DEFINE_FFF_GLOBALS;

/* mocks */


/**
 * @test  hidDeviceInit must return the error code if initializing the HID
 *        device fails.
*/
ZTEST(hidDevice_suite, test_hidDeviceInit_InitFail)
{

}

/** @} */
