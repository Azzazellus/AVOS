#ifndef CLI_H
#define CLI_H

#include <stdint.h>

void CLI_Init(void);
void CLI_Task(void);

/* Приём символов из VCP */
void CLI_PushChar(uint8_t c);

#endif /* CLI_H */