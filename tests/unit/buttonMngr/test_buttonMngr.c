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

#include "buttonMngr.h"
#include "buttonMngr.c"

DEFINE_FFF_GLOBALS;

ZTEST_SUITE(buttonMngr_suite, NULL, NULL, NULL, NULL, NULL);

/**
 * @test  buttonMngrInit must return the error code if the initialization
 *        fails.
*/
ZTEST(buttonMngr_suite, test_buttonMngrInit_Fail)
{
  zassert_true(1);
}

/** @} */
