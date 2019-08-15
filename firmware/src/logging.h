/////////////////////////////////////////////////////
// Project: RX12                                   //
// File: logging.h                                 //
// Target: PIC32MKxxxxGPD/E064                     // 
// Compiler: XC32 (Tested with 2.10)               //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: data logging functions             //
/////////////////////////////////////////////////////

#ifndef LOGGING_H
#define	LOGGING_H

#include <stdint.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define LOG_WORDS   9
    typedef union {

        struct {
            uint32_t sequenceNumber;
            uint32_t totalPackets;
            uint32_t rxLowVoltage;
            uint32_t rxHighVoltage;
            uint32_t duration;
            uint32_t fades[3];
            uint32_t statusFlags;
        };
        unsigned int words[LOG_WORDS];
    } LogData;

    typedef enum {
        STATUS_NONE = 0, STATUS_WDTO = 0x01, STATUS_CF = 0x02, STATUS_FAILSAFE = 0x04
    } StatusFlags;

    extern volatile LogData __attribute__((persistent)) currentFlightLog;
    extern bool logging;
    extern unsigned int logAddress;

    bool loadLogData(LogData *data, unsigned int eeAddress);
    void startLogging(void);
    void enableLogging(bool enable);
    void checkLoggingActive(void);

#define LOGGING_INTERVAL    1000

#ifdef	__cplusplus
}
#endif

#endif	/* LOGGING_H */

