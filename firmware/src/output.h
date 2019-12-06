/////////////////////////////////////////////////////
// Project: RX12                                   //
// File: output.h                                  //
// Target: PIC32MKxxxxGPD/E064                     // 
// Compiler: XC32 (Tested with 2.10)               //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: servo output functions             //
/////////////////////////////////////////////////////

#ifndef OUTPUT_H
#define	OUTPUT_H

#include <stdbool.h>
#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define MAX_CHANNEL     12
#define OFFSET  (2097 * US_COUNT)
#define PULSE   (903 * US_COUNT)

    enum {
        THROTTLE = 0, AILERON = 1, ELEVATOR = 2, RUDDER = 3, GEAR = 4, AUX1 = 5,
        AUX2 = 6, AUX3 = 7, AUX4 = 8, AUX5 = 9, AUX6 = 10, AUX7 = 11
    };

    enum {
        OUTPUT_TYPE_PWM = 0x37, OUTPUT_TYPE_PPM = 0x92, OUTPUT_TYPE_SBUS = 0xa9
    };

    extern unsigned int outputType __attribute__((persistent));
    extern uint16_t servos[MAX_CHANNEL];
    extern uint32_t outputPulses[MAX_CHANNEL];
    extern const unsigned int startOffsets[MAX_CHANNEL];
    extern const unsigned int pulseOffsets[MAX_CHANNEL];
    extern volatile unsigned int* const pulseRegister[MAX_CHANNEL];
    extern volatile unsigned int* const startRegister[MAX_CHANNEL];
    extern volatile unsigned int* const OCxCONSETRegister[MAX_CHANNEL];
    extern bool outputsActivated;

    void initOutputs(void);
    void enableActiveOutputs(void);
    void disableThrottle(void);
    void enableThrottle(void);

#ifdef	__cplusplus
}
#endif

#endif	/* OUTPUT_H */

