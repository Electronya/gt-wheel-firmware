/**
 * Copyright (C) 2023 by Electronya
 *
 * @file      ledCtrlHw.h
 * @author    jbacon
 * @date      2023-07-20
 * @brief     Led Control Module
 *
 *            This file is the declaration of the LED control module.
 *
 * @defgroup  ledCtrl
 *
 * @{
 */

#ifndef LED_CTRL
#define LED_CTRL

/**
 * @brief   Initialize the module.
 *
 * @return  0 if successful, the error code otherwise.
*/
int ledCtrlInit(void);

/**
 * @brief   Get the pixel count of the strip.
 *
 * @return  The count of pixel in strip.
 */
uint32_t ledCtrlGetPixelCount(void);

#endif    /* LED_CTRL */

/** @} */
