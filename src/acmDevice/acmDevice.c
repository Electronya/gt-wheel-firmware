/**
 * Copyright (C) 2023 by Electronya
 *
 * @file      acmDevice.c
 * @author    jbacon
 * @date      2023-08-25
 * @brief     ACM Device Module
 *
 *            This file is the implementation of the ACM device module.
 *
 * @ingroup  acmDevice
 *
 * @{
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>

#include "acmDevice.h"
#include "simhubPkt.h"
#include "zephyrACM.h"
#include "zephyrCommon.h"
#include "zephyrThread.h"

/**
 * @brief ACM device module name.
*/
#define ACM_DEVICE_MODULE_NAME  acm_device_module

/* Setting module logging */
LOG_MODULE_REGISTER(ACM_DEVICE_MODULE_NAME);

/**
 * @brief The ACM device.
*/
ZephyrACM acmDev;

/**
 * @brief   The ACM worker thread.
 *
 * @param p1  The first user parameter.
 * @param p2  The second user parameter.
 * @param p3  The third user parameter.
 */
static void acmDeviceWorker(void *p1, void *p2, void *p3)
{
  int rc;
  bool txResponse = false;
  size_t byteCount;
  SimhubPktTypes pktType;
  uint8_t *data = NULL;

  byteCount = simhubPktBufClaimPutting(&data, SIMHUB_RX_PKT_BUF_SIZE);
  byteCount = zephyrAcmReadRingBuffer(&acmDev, data, byteCount);
  rc = simhubPktBufFinishPutting(byteCount);
  // TODO: fatal error management.
  if(rc < 0)
    LOG_ERR("unable to finish moving received bytes to SIMHUB Rx packet buffer");

  if(simhubPktIsPktAvailable(&pktType))
  {
    switch(pktType)
    {
      case UNLOCK_TYPE:
        rc = simhubPktProcessUnlock();
        if(rc < 0)
          LOG_WRN("unable to process unlock upload packet");
        break;
      case PROTO_TYPE:
        rc = simhubPktProcessProto();
        if(rc < 0)
          LOG_WRN("unable to process protocol version packet");
        else
          txResponse = true;
        break;
      case LED_COUNT_TYPE:
        rc = simhubPktProcessLedCount();
        if(rc < 0)
          LOG_WRN("unable to process LED count packet");
        else
          txResponse = true;
        break;
      case LED_DATA_TYPE:
        rc = simhubPktProcessLedData();
        if(rc < 0)
          LOG_WRN("unable to process LED data packet");
        break;
      default:
        LOG_WRN("unsupported packet type");
        break;
    }

    if(txResponse)
    {
      byteCount = simhubPktBufClaimGetting(&data, SIMHUB_TX_PKT_BUF_SIZE);
      byteCount = zephyrAcmWriteRingBuffer(&acmDev, data, byteCount);
      rc = simhubPktBufFinishGetting(byteCount);
      // TODO: fatal error management.
      if(rc < 0)
        LOG_ERR("unable to finish moving response bytes to SIMHUB Tx packet buffer");
      zephyrAcmEnableTxIrq(&acmDev);
    }
  }
}

/**
 * @brief   The ACM device IRQ callback.
 *
 * @param dev     The device.
 * @param usrData The user data.
 */
static void acmDeviceIrq(const struct device *dev, void *usrData)
{

}

/**
 * @brief   Initialize the ACM device.
 *
 * @return  0 if successful, the error code otherwise.
 */
int acmDeviceInit(void)
{
  int rc;

  acmDev.cb = acmDeviceIrq;
  rc = zephyrAcmInit(&acmDev, SIMHUB_RX_PKT_BUF_SIZE, SIMHUB_TX_PKT_BUF_SIZE);
  if(rc < 0)
  {
    LOG_ERR("unable to initialize the ACM device");
    return rc;
  }

  simhubPktInitBuffer();

  zephyrAcmEnableRxIrq(&acmDev);

  return rc;
}

/** @} */
