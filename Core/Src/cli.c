#include "cli.h"
#include "stepper.h"
#include "eeprom.h"
#include "led.h"
#include "vcp_monitor.h"
#include "usbd_cdc_if.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

#define BUF_SZ 64

static char buf[BUF_SZ];
static uint8_t idx;
static EEPROM_Data_t ee;

/* ================= ВСПОМОГАТЕЛЬНОЕ ================= */

static void print(const char *s)
{
    CDC_Transmit_FS((uint8_t *)s, strlen(s));
    VCP_OnTxActivity();
}

static void printf_line(const char *fmt, ...)
{
    char out[96];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(out, sizeof(out), fmt, ap);
    va_end(ap);

    print(out);
}

/* Парсер яркости LED: 0…100 */
static int parse_led_pwm(const char *s, bool *ok)
{
    char *end;
    long v = strtol(s, &end, 10);

    if (end == s || *end != '\0')
    {
        *ok = false;
        return 0;
    }

    if (v < 0)   v = 0;
    if (v > 100) v = 100;

    *ok = true;
    return (int)v;
}

/* ================= INIT ================= */

void CLI_Init(void)
{
    EEPROM_Init();
    EEPROM_Load(&ee);

    Stepper_SetSpeedZ_mm_s(ee.spd_z_max);
    Stepper_SetSpeedX_deg_s(ee.spd_x_max);
    Stepper_SetAccelZ_mm_s2(ee.axl_z);
    Stepper_SetAccelX_deg_s2(ee.axl_x);

    LED_Init();
    LED_SetR(ee.led_r);
    LED_SetG(ee.led_g);
    LED_SetB(ee.led_b);
    LED_SetUV(ee.led_uv);
    LED_SetIR(ee.led_ir);
}

/* ================= COMMAND EXEC ================= */

static void exec(const char *c)
{
    bool ok;
    int v;

    /* -------- STOP -------- */
    if (!strcmp(c, "STOP"))
    {
        Stepper_EmergencyStop();
        print("OK STOP\r\n");
        return;
    }

    /* -------- STATUS -------- */
    if (!strcmp(c, "?STATUS"))
    {
        if (Stepper_IsHoming())
            print("HOMING\r\n");
        else if (Stepper_IsBusyZ() || Stepper_IsBusyX())
            print("BUSY\r\n");
        else
            print("IDLE\r\n");
        return;
    }

    /* -------- POSITION -------- */
    if (!strcmp(c, "?POS"))
    {
        printf_line("Z=%.3fmm X=%.2fdeg\r\n",
                    Stepper_GetPosZ_mm(),
                    Stepper_GetPosX_deg());
        return;
    }

    /* -------- SPEED -------- */
    if (!strcmp(c, "?SPEED"))
    {
        printf_line("Z=%.2fmm/s X=%.2fdeg/s\r\n",
                    Stepper_GetSpeedZ_mm_s(),
                    Stepper_GetSpeedX_deg_s());
        return;
    }

    /* -------- Z MOVE -------- */
    if (c[0] == 'Z')
    {
        float v_f = strtof(c + 1, NULL);

        if (c[1] == '+' || c[1] == '-')
            Stepper_SetTargetZ_mm(Stepper_GetPosZ_mm() + v_f);
        else
            Stepper_SetTargetZ_mm(v_f);

        print("OK\r\n");
        return;
    }

    /* -------- X MOVE -------- */
    if (c[0] == 'X')
    {
        float v_f = strtof(c + 1, NULL);

        if (c[1] == '+' || c[1] == '-')
            Stepper_SetTargetX_deg(Stepper_GetPosX_deg() + v_f);
        else
            Stepper_SetTargetX_deg(v_f);

        print("OK\r\n");
        return;
    }

    /* -------- LED -------- */
    if (!strncmp(c,"LED_R",5))
    {
        v = parse_led_pwm(c+5, &ok);
        if (ok){ ee.led_r=v; LED_SetR(v); print("OK\r\n"); return; }
    }
    if (!strncmp(c,"LED_G",5))
    {
        v = parse_led_pwm(c+5, &ok);
        if (ok){ ee.led_g=v; LED_SetG(v); print("OK\r\n"); return; }
    }
    if (!strncmp(c,"LED_B",5))
    {
        v = parse_led_pwm(c+5, &ok);
        if (ok){ ee.led_b=v; LED_SetB(v); print("OK\r\n"); return; }
    }
    if (!strncmp(c,"LED_UV",6))
    {
        v = parse_led_pwm(c+6, &ok);
        if (ok){ ee.led_uv=v; LED_SetUV(v); print("OK\r\n"); return; }
    }
    if (!strncmp(c,"LED_IR",6))
    {
        v = parse_led_pwm(c+6, &ok);
        if (ok){ ee.led_ir=v; LED_SetIR(v); print("OK\r\n"); return; }
    }

    if (!strcmp(c,"LED_OFF")){ LED_Off(); print("OK\r\n"); return; }
    if (!strcmp(c,"LED_ON")) { LED_On();  print("OK\r\n"); return; }

    /* -------- SAVE MAX -------- */
    if (!strcmp(c,"SAVE_MAX"))
    {
        ee.z_max_mm = Stepper_GetPosZ_mm();
        EEPROM_Save(&ee);
        print("OK\r\n");
        return;
    }

    print("ERR\r\n");
}

/* ================= INPUT ================= */

void CLI_PushChar(uint8_t c)
{
    VCP_OnRxActivity();

    if (c == '\r' || c == '\n')
    {
        buf[idx] = 0;
        if (idx > 0)
            exec(buf);
        idx = 0;
        return;
    }

    if (idx < BUF_SZ - 1)
        buf[idx++] = c;
}

void CLI_Task(void)
{
}