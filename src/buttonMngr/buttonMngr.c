/**
 * Copyright (C) 2023 by Electronya
 *
 * @file      buttonMngr.h
 * @author    jbacon
 * @date      2023-07-31
 * @brief     Button Manager Module
 *
 *            This file is the implementation of the button manager module.
 *
 * @ingroup  buttonMngr
 *
 * @{
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "buttonMngr.h"
#include "zephyrCommon.h"
#include "zephyrGpio.h"

#define BUTTON_MNGR_MODULE_NAME button_mngr_module

/* Setting module logging */
LOG_MODULE_REGISTER(BUTTON_MNGR_MODULE_NAME);

#ifndef CONFIG_ZTEST

#else

#endif


/** @} */
