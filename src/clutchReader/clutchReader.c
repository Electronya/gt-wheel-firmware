/**
 * Copyright (C) 2023 by Electronya
 *
 * @file      clutchReader.c
 * @author    jbacon
 * @date      2023-07-31
 * @brief     Clutch Reader Module
 *
 *            This file is the implementation of the clutch reader module.
 *
 * @ingroup  clutchReader
 *
 * @{
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

#include "clutchReader.h"
#include "zephyrCommon.h"
#include "zephyrAdc.h"
#include "zephyrThread.h"

#define CLUTCH_READER_MODULE_NAME clutch_reader_module

/* Setting module logging */
LOG_MODULE_REGISTER(CLUTCH_READER_MODULE_NAME);

/**
 * @brief The thread stack size.
*/
#define BUTTON_MNGR_STACK_SIZE      256

/**
 * @brief The thread name.
 */
#define BUTTON_MNGR_THREAD_NAME     "clutchReader"

int clutchReaderInit(void)
{
 return 0;
}

/** @} */
