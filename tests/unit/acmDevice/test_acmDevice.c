/**
 * Copyright (C) 2023 by Electronya
 *
 * @file      test_acmDevice.h
 * @author    jbacon
 * @date      2023-07-19
 * @brief     ACM device Module Test Cases
 *
 *            This file is the test cases of the ACM device module.
 *
 * @ingroup  acmDevice
 *
 * @{
 */

#include <zephyr/ztest.h>
#include <zephyr/fff.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

#include "acmDevice.h"
#include "acmDevice.c"

#include "zephyrACM.h"
#include "zephyrThread.h"

/** @} */
