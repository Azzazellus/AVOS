#ifndef VCP_MONITOR_H
#define VCP_MONITOR_H

#include <stdbool.h>
/*#include <stdint.h>*/

void VCP_Init(void);
void VCP_Task(void);

void VCP_OnRxActivity(void);
void VCP_OnTxActivity(void);

bool VCP_IsConnected(void);

#endif /* VCP_MONITOR_H */