#include "vcp_monitor.h"
#include "status_led.h"

/*#include "usb_device.h"*/
#include "usbd_def.h"
#include "usbd_cdc_if.h"

#include "stm32f4xx_hal.h"
#include <string.h>

extern USBD_HandleTypeDef hUsbDeviceFS;

static uint32_t last_activity_ms  = 0;
static uint32_t last_heartbeat_ms = 0;

void VCP_Init(void)
{
    last_activity_ms  = 0;
    last_heartbeat_ms = 0;

    StatusLED_SetVCPState(VCP_DISCONNECTED);
}

bool VCP_IsConnected(void)
{
    return (hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED);
}

void VCP_OnRxActivity(void)
{
    last_activity_ms = HAL_GetTick();
    StatusLED_NotifyActivity();
}

void VCP_OnTxActivity(void)
{
    last_activity_ms = HAL_GetTick();
    StatusLED_NotifyActivity();
}

void VCP_Task(void)
{
    uint32_t now = HAL_GetTick();

    if (VCP_IsConnected())
    {
        StatusLED_SetVCPState(VCP_CONNECTED);
    }
    else
    {
        StatusLED_SetVCPState(VCP_DISCONNECTED);
        return;
    }

    if (now - last_heartbeat_ms >= 5000)
    {
        const char *msg = "Heartbeat\r\n";
        CDC_Transmit_FS((uint8_t *)msg, strlen(msg));
        last_heartbeat_ms = now;
        VCP_OnTxActivity();
    }
}