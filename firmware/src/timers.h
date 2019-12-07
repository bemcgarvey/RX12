/////////////////////////////////////////////////////
// Project: RX12                                   //
// File: timers.h                                  //
// Target: PIC32MKxxxxGPD/E064                     // 
// Compiler: XC32 (Tested with 2.10)               //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: timers for output and system       //
/////////////////////////////////////////////////////

#ifndef TIMERS_H
#define	TIMERS_H

#define OSC_FREQ    120000000U
#define US_COUNT    ((OSC_FREQ / 2U) / 1000000U)
#define MS_COUNT    ((OSC_FREQ / 2U) / 1000)

enum {PERIOD_22MS = 22, PERIOD_11MS = 11, PERIOD_SBUS = 7};

#ifdef	__cplusplus
extern "C" {
#endif
    extern volatile unsigned int systemTickCount;
    
    void startSystemTickTimer(void);
    void startOCTimer(unsigned int msPeriod);
    void delay_us(unsigned int us);

#ifdef	__cplusplus
}
#endif

#endif	/* TIMERS_H */

