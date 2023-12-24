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
#include "zephyrWork.h"
#include "zephyrWorkQueue.h"

/**
 * @brief ACM device module name.
*/
#define ACM_DEVICE_MODULE_NAME  acm_device_module

/* Setting module logging */
LOG_MODULE_REGISTER(ACM_DEVICE_MODULE_NAME);

/* Creating the worker stack */
K_THREAD_STACK_DEFINE(acmWorkerStack, 128);

/**
 * @brief The ACM device.
*/
ZephyrACM acmDev;

/**
 * @brief The work queue.
*/
ZephyrWorkQueue workQueue = {
  .priority = 3,
  .stack = acmWorkerStack,
  .stackSize = K_THREAD_STACK_SIZEOF(acmWorkerStack),
};

/**
 * @brief The work.
*/
ZephyrWork work;

/**
 * @brief   The ACM worker thread.
 *
 * @param item  The work item.
 */
static void acmDeviceWorker(struct k_work *item)
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
  int rc;

  if(zephyrAcmIrqUpdate(&acmDev) && zephyrAcmIsIrqPending(&acmDev))
  {
    if(zephyrAcmIsRxIrqReady(&acmDev))
    {
      rc = zephyrAcmReadFifo(&acmDev);
      if(rc < 0)
        LOG_ERR("unable to read ACM device FIFO");

      rc =zephyrWorkSubmitToQueue(&workQueue, &work);
      if(rc < 0)
        LOG_ERR("unable to submit work for processing");
      if(rc == 0)
        LOG_WRN("previous processing is still queued");
    }

    if(zephyrAcmIsTxIrqReady(&acmDev))
    {
      rc = zephyrAcmWriteFifo(&acmDev);
      if(rc < 0)
        LOG_ERR("unable to write to ACM device FIFO");
      if(rc == 0)
        zephyrAcmDisableTxIrq(&acmDev);
    }
  }
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

  work.handler = acmDeviceWorker;
  zephyrWorkInit(&work);
  zephyrWorkQueueInit(&workQueue);

  return rc;
}

int acmDeviceStart(void)
{
  return zephyrAcmStart(&acmDev);
}

/** @} */
