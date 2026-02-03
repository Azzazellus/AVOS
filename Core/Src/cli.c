#include "cli.h"
#include "stepper.h"
#include "eeprom.h"
#include "led.h"
#include "usbd_cdc_if.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <math.h>

#define BUF_SZ 128
static char buf[BUF_SZ];
static uint8_t idx;
static EEPROM_Data_t ee;

/* ================= TIMING ================= */
static uint32_t last_heartbeat_time = 0;
#define HEARTBEAT_INTERVAL_MS 5000  // 5 секунд

/* ================= POSITION HELPERS ================= */

static void read_stepper_positions(float *z_mm, float *x_deg)
{
    extern volatile int32_t cur_z_steps;
    extern volatile int32_t cur_x_steps;

    const float z_steps_per_mm = 80.0f;
    const float x_steps_per_deg = 1600.0f / 360.0f;

    __disable_irq();
    int32_t z = cur_z_steps;
    int32_t x = cur_x_steps;
    __enable_irq();

    if (z_mm) *z_mm = (float)z / z_steps_per_mm;

    if (x_deg) {
        float deg = (float)x / x_steps_per_deg;
        while (deg >= 360.0f) deg -= 360.0f;
        while (deg < 0.0f) deg += 360.0f;
        *x_deg = deg;
    }
}

/* ================= OUTPUT ================= */

static void print(const char *s)
{
    if (strlen(s) > 0) {
        CDC_Transmit_FS((uint8_t *)s, strlen(s));
    }
}

static void printf_line(const char *fmt, ...)
{
    char out[256];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(out, sizeof(out), fmt, ap);
    va_end(ap);
    print(out);
}

/* ================= PARSERS ================= */

static bool parse_float(const char *s, float *value)
{
    if (s == NULL || strlen(s) == 0) return false;

    char *end;
    *value = strtof(s, &end);
    if (end == s) return false;

    while (*end == ' ' || *end == '\t') end++;
    return (*end == '\0');
}

static bool parse_int(const char *s, int *value)
{
    if (s == NULL || strlen(s) == 0) return false;

    char *end;
    long v = strtol(s, &end, 10);
    if (end == s) return false;

    while (*end == ' ' || *end == '\t') end++;
    if (*end != '\0') return false;

    *value = (int)v;
    return true;
}

/* ================= INIT ================= */

void CLI_Init(void)
{
    EEPROM_Init();
    EEPROM_Load(&ee);

    if (ee.spd_z_max <= 0.0f || ee.spd_z_max > 100.0f) ee.spd_z_max = 5.0f;
    if (ee.spd_x_max <= 0.0f || ee.spd_x_max > 100.0f) ee.spd_x_max = 10.0f;
    if (ee.axl_z <= 0.0f || ee.axl_z > 1000.0f) ee.axl_z = 20.0f;
    if (ee.axl_x <= 0.0f || ee.axl_x > 1000.0f) ee.axl_x = 40.0f;

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

    last_heartbeat_time = HAL_GetTick();
}

/* ================= GUI COMMANDS ================= */

static void exec(const char *cmd)
{
    if (cmd[0] == '\0') return;

    // === LED COMMANDS ===
    if (strncmp(cmd, "LED_", 4) == 0) {
        const char *subcmd = cmd + 4;

        if (strcmp(subcmd, "ON") == 0) {
            LED_On();
            print("OK LED ON\r\n");
            return;
        }

        if (strcmp(subcmd, "OFF") == 0) {
            LED_Off();
            print("OK LED OFF\r\n");
            return;
        }

        if (subcmd[0] == 'R' || subcmd[0] == 'G' || subcmd[0] == 'B') {
            char channel = subcmd[0];
            int value;
            if (!parse_int(subcmd + 1, &value)) {
                print("ERR: Invalid value\r\n");
                return;
            }

            if (value < 0) value = 0;
            if (value > 100) value = 100;

            switch (channel) {
                case 'R': LED_SetR(value); ee.led_r = value; break;
                case 'G': LED_SetG(value); ee.led_g = value; break;
                case 'B': LED_SetB(value); ee.led_b = value; break;
            }

            printf_line("OK LED %c%d\r\n", channel, value);
            return;
        }

        if (strncmp(subcmd, "IR", 2) == 0) {
            int value;
            if (!parse_int(subcmd + 2, &value)) {
                print("ERR: Invalid value\r\n");
                return;
            }

            if (value < 0) value = 0;
            if (value > 100) value = 100;

            LED_SetIR(value);
            ee.led_ir = value;
            printf_line("OK LED IR%d\r\n", value);
            return;
        }

        if (strncmp(subcmd, "UV", 2) == 0) {
            int value;
            if (!parse_int(subcmd + 2, &value)) {
                print("ERR: Invalid value\r\n");
                return;
            }

            if (value < 0) value = 0;
            if (value > 100) value = 100;

            LED_SetUV(value);
            ee.led_uv = value;
            printf_line("OK LED UV%d\r\n", value);
            return;
        }

        print("ERR: Unknown LED command\r\n");
        return;
    }

    // === SIMPLE COMMANDS ===
    if (strcmp(cmd, "STOP") == 0) {
        Stepper_EmergencyStop();
        print("OK STOP\r\n");
        return;
    }

    if (strcmp(cmd, "HOME") == 0) {
        Stepper_StartHoming();
        print("OK\r\n");
        return;
    }

    if (strcmp(cmd, "SAVE") == 0) {
        ee.spd_z_max = Stepper_GetSpeedZ_mm_s();
        ee.spd_x_max = Stepper_GetSpeedX_deg_s();
        ee.axl_z = Stepper_GetAccelZ_mm_s2();
        ee.axl_x = Stepper_GetAccelX_deg_s2();
        EEPROM_Save(&ee);
        print("OK SAVED\r\n");
        return;
    }

    // === STATUS QUERIES ===
    if (strcmp(cmd, "?STATUS") == 0) {
        if (Stepper_IsHoming())
            print("STATUS: HOMING\r\n");
        else if (Stepper_IsBusyZ() || Stepper_IsBusyX())
            print("STATUS: BUSY\r\n");
        else
            print("STATUS: READY\r\n");
        return;
    }

    if (strcmp(cmd, "?POS") == 0) {
        float z_mm, x_deg;
        read_stepper_positions(&z_mm, &x_deg);
        printf_line("Z=%.3fmm X=%.3fdeg\r\n", z_mm, x_deg);
        return;
    }

    if (strcmp(cmd, "?SPEED") == 0) {
        printf_line("Z=%.2f mm/s X=%.2f deg/s\r\n",
                    Stepper_GetSpeedZ_mm_s(),
                    Stepper_GetSpeedX_deg_s());
        return;
    }

    // === MOVEMENT COMMANDS ===
    if (cmd[0] == 'Z' || cmd[0] == 'X') {
        char axis = cmd[0];
        const char *value_str = cmd + 1;

        if (strlen(value_str) == 0) {
            print("ERR: No value\r\n");
            return;
        }

        float value;
        if (!parse_float(value_str, &value)) {
            print("ERR: Invalid number\r\n");
            return;
        }

        if (axis == 'Z') {
            float current = Stepper_GetPosZ_mm();
            float new_target;

            if (value_str[0] == '+' || value_str[0] == '-') {
                new_target = current + value;
            } else {
                new_target = value;
            }

            Stepper_SetTargetZ_mm(new_target);
            print("OK\r\n");
        } else {
            float current = Stepper_GetPosX_deg();
            float new_target;

            if (value_str[0] == '+' || value_str[0] == '-') {
                new_target = current + value;
                while (new_target >= 360.0f) new_target -= 360.0f;
                while (new_target < 0.0f) new_target += 360.0f;
            } else {
                new_target = value;
            }

            Stepper_SetTargetX_deg(new_target);
            print("OK\r\n");
        }
        return;
    }

    print("ERR: Unknown command\r\n");
}

/* ================= RX ================= */

void CLI_PushChar(uint8_t c)
{
    if (c == '\r' || c == '\n')
    {
        if (idx > 0) {
            buf[idx] = 0;
            exec(buf);
        }
        idx = 0;
        memset(buf, 0, BUF_SZ);
        return;
    }

    if (c == 0x08 || c == 0x7F) {
        if (idx > 0) {
            idx--;
            buf[idx] = 0;
        }
        return;
    }

    if (idx < BUF_SZ - 1 && c >= 32 && c <= 126) {
        buf[idx++] = c;
    }
}

/* ================= TASK ================= */

void CLI_Task(void)
{
    uint32_t now = HAL_GetTick();

    if (now - last_heartbeat_time >= HEARTBEAT_INTERVAL_MS) {
        print("HEARTHBEAT\r\n");  // ТОЧНО как ты написал
        last_heartbeat_time = now;
    }
}