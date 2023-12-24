/**
 * Copyright (C) 2023 by Electronya
 *
 * @file      acmDevice.h
 * @author    jbacon
 * @date      2023-08-25
 * @brief     ACM Device Module
 *
 *            This file is the declaration of the ACM device module.
 *
 * @defgroup  acmDevice acmDevice
 *
 * @{
 */

#ifndef ACM_DEVICE
#define ACM_DEVICE

/**
 * @brief   Initialize the ACM device.
 *
 * @return  0 if successful, the error code otherwise.
 */
int acmDeviceInit(void);

/**
 * @brief   Get the ACM DTR line.
 *
 * @return  0 if successful, the error code otherwise.
 */
int acmDeviceStart(void);

#endif    /* ACM_DEVICE */

/** @} */
