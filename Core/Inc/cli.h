#ifndef __CLI_H
#define __CLI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

    void CLI_Init(void);
    void CLI_Task(void);
    void CLI_PushChar(uint8_t c);

#ifdef __cplusplus
}
#endif

#endif /* __CLI_H */