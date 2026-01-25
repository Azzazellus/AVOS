#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

typedef enum
{
    AXIS_X = 0,
    AXIS_Z = 1
} JogAxis_t;

void Encoder_Init(void);
void Encoder_Task(void);

JogAxis_t Encoder_GetAxis(void);
int32_t   Encoder_GetJogDelta(void);
void      Encoder_ResetJog(void);

#endif /* ENCODER_H */