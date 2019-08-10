#ifndef LOGDATA_H
#define LOGDATA_H

#include <stdint.h>

typedef struct {
    uint32_t sequenceNumber;
    uint32_t totalPackets;
    uint32_t rxLowVoltage;
    uint32_t rxHighVoltage;
    uint32_t duration;
    uint32_t fades[3];
    uint32_t statusFlags;
} LogData;

typedef enum {
    STATUS_NONE = 0, STATUS_WDTO = 0x01, STATUS_CF = 0x02, STATUS_FAILSAFE = 0x04
} StatusFlags;

#endif // LOGDATA_H
