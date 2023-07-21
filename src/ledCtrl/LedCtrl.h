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

#include "zephyrLedStrip.h"

/**
 * @brief   Initialize the module.
 *
 * @return  0 if successful, the error code otherwise.
*/
int ledCtrlInit(void);

/**
 * @brief   Get the pixel count of the RPM chaser.
 *
 * @return  The count of pixel in strip.
 */
uint32_t ledCtrlGetRpmChaserPxlCnt(void);

/**
 * @brief   Set the pixel colors of the RMP chaser.
 *          A call to ledCtrlUpdateStrip must called to push the new colors
 *          to the strip.
 *
 * @param pixels    The pixel colors.
 * @param pixelCnt  The count of pixel.
 * @return          0 if successful, the error code otherwise.
 */
int ledCtrlSetRpmChaserPixels(ZephyrRgbLed *pixels, size_t pixelCnt);

#endif    /* LED_CTRL */

/** @} */
