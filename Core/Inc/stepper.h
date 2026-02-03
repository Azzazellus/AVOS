#ifndef STEPPER_H
#define STEPPER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ===== INIT ===== */
void Stepper_Init(void);
void Stepper_Task(void);

/* ===== TARGETS ===== */
void Stepper_SetTargetZ_mm(float z_mm);
void Stepper_SetTargetX_deg(float x_deg);

/* ===== SPEED ===== */
void Stepper_SetSpeedZ_mm_s(float speed_mm_s);
void Stepper_SetSpeedX_deg_s(float speed_deg_s);
float Stepper_GetSpeedZ_mm_s(void);
float Stepper_GetSpeedX_deg_s(void);

/* ===== ACCEL ===== */
void Stepper_SetAccelZ_mm_s2(float accel_mm_s2);
void Stepper_SetAccelX_deg_s2(float accel_deg_s2);
float Stepper_GetAccelZ_mm_s2(void);
float Stepper_GetAccelX_deg_s2(void);

/* ===== POSITION ===== */
float Stepper_GetPosZ_mm(void);
float Stepper_GetPosX_deg(void);
int32_t Stepper_GetStepsZ(void);
int32_t Stepper_GetStepsX(void);

/* ===== MANUAL POSITION SET ===== */
void Stepper_SetPositionZ_mm(float z_mm);
void Stepper_SetPositionX_deg(float x_deg);

/* ===== STATUS ===== */
bool Stepper_IsBusyZ(void);
bool Stepper_IsBusyX(void);
bool Stepper_IsAnyBusy(void);
bool Stepper_IsHoming(void);

/* ===== HOME ===== */
void Stepper_StartHoming(void);

/* ===== DIRECTION CONTROL ===== */
void Stepper_SetDirZ(uint8_t dir);
void Stepper_SetDirX(uint8_t dir);

/* ===== EMERGENCY ===== */
void Stepper_EmergencyStop(void);

/* ===== ISR ===== */
void Stepper_TIM_ISR(void);

/* ===== DEBUG ===== */
#ifdef STEPPER_DEBUG
void Stepper_DebugPrintState(void);
void Stepper_DebugPrintTargets(void);
#endif

/* ===== EXTERN VARIABLES FOR ISR (опционально, только если нужны извне) ===== */
#ifdef STEPPER_EXTERN_VARS
extern volatile bool z_busy;
extern volatile bool x_busy;
extern volatile bool homing_z;
extern volatile int32_t cur_z_steps;
extern volatile int32_t cur_x_steps;
extern volatile int32_t tgt_z_steps;
extern volatile int32_t tgt_x_steps;

extern static float z_current_speed;
extern static float x_current_speed;
extern static uint32_t z_pulse_interval;
extern static uint32_t x_pulse_interval;
#endif

#ifdef __cplusplus
}
#endif

#endif /* STEPPER_H */