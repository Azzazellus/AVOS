#ifndef STEPPER_H
#define STEPPER_H

#include <stdint.h>
#include <stdbool.h>

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

/* ===== JERK ===== */
void Stepper_SetJerkZ_mm_s3(float jerk_mm_s3);
void Stepper_SetJerkX_deg_s3(float jerk_deg_s3);
float Stepper_GetJerkZ_mm_s3(void);
float Stepper_GetJerkX_deg_s3(void);

/* ===== POSITION ===== */
float Stepper_GetPosZ_mm(void);
float Stepper_GetPosX_deg(void);

/* ===== STATUS ===== */
bool Stepper_IsBusyZ(void);
bool Stepper_IsBusyX(void);
bool Stepper_IsHoming(void);

/* ===== HOME ===== */
void Stepper_StartHomeZ(void);

/* ===== EMERGENCY ===== */
void Stepper_EmergencyStop(void);

/* ===== ISR ===== */
void Stepper_TIM_ISR(void);

#endif /* STEPPER_H */