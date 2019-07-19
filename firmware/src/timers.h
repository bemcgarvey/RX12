/* 
 * File:   timers.h
 * Author: bemcg
 *
 * Created on July 18, 2019, 5:47 PM
 */

#ifndef TIMERS_H
#define	TIMERS_H

#define OSC_FREQ    120000000U
#define US_COUNT    ((OSC_FREQ / 2U) / 1000000U)
#define MS_COUNT    ((OSC_FREQ / 2U) / 1000)

enum {PERIOD_22MS = 0, PERIOD_11MS = 1};

#ifdef	__cplusplus
extern "C" {
#endif
    extern volatile unsigned int systemTickCount;
    
    void startSystemTickTimer(void);
    void startOCTimer(unsigned int period);
    void delay_us(unsigned int us);

#ifdef	__cplusplus
}
#endif

#endif	/* TIMERS_H */

