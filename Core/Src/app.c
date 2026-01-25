#include "app.h"

#include "encoder.h"
#include "stepper.h"
#include "status_led.h"
#include "cli.h"
#include "vcp_monitor.h"

void App_Init(void)
{
    Encoder_Init();
    Stepper_Init();
    CLI_Init();
    VCP_Init();
    StatusLED_Init();

    Stepper_StartHomeZ();
}

void App_Loop(void)
{
    Encoder_Task();
    Stepper_Task();
    CLI_Task();
    VCP_Task();
    StatusLED_Task();

    /* ===== JOG ===== */
    if (!Stepper_IsHoming())
    {
        int32_t d = Encoder_GetJogDelta();
        if (d != 0)
        {
            Encoder_ResetJog();

            if (Encoder_GetAxis() == AXIS_Z)
            {
                float z = Stepper_GetPosZ_mm();
                Stepper_SetTargetZ_mm(z + d);
            }
            else
            {
                float x = Stepper_GetPosX_deg();
                Stepper_SetTargetX_deg(x + d);
            }
        }
    }
}