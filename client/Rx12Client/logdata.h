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
#endif // LOGDATA_H
