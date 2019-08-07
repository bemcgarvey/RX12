/* 
 * File:   logging.h
 * Author: bemcg
 *
 * Created on July 27, 2019, 7:33 PM
 */

#ifndef LOGGING_H
#define	LOGGING_H

#include <stdint.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct {
        uint32_t sequenceNumber;
        uint32_t totalPackets;
        uint32_t rxLowVoltage;
        uint32_t rxHighVoltage;
        uint32_t duration;
        uint32_t sat1Fades;
        uint32_t sat2Fades;
        uint32_t sat3Fades;
        uint32_t statusFlags;
        uint32_t reserved;
    } LogData;

    typedef enum {
        STATUS_NONE = 0, STATUS_WDTO = 0x01, STATUS_CF = 0x02, STATUS_FAIL_SAFE = 0x04
    } StatusFlags;

    extern LogData currentFlightLog;
    extern bool logging;

    void saveLogData(const LogData *data, unsigned int eeAddress);
    bool loadLogData(LogData *data, unsigned int eeAddress);
    void startLogging(void);
    void enableLogging(bool enable);
    void checkLoggingActive(void);

#define LOGGING_INTERVAL    5000
    
#ifdef	__cplusplus
}
#endif

#endif	/* LOGGING_H */

