/* 
 * File:   output.h
 * Author: bemcg
 *
 * Created on July 20, 2019, 8:10 PM
 */

#ifndef OUTPUT_H
#define	OUTPUT_H

#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

    #define MAX_CHANNEL     12
    
    enum {
        THROTTLE = 0, AILERON = 1, ELEVATOR = 2, RUDDER = 3, GEAR = 4, AUX1 = 5,
        AUX2 = 6, AUX3 = 7, AUX4 = 8, AUX5 = 9, AUX6 = 10, AUX7 = 11
    };
    
    extern uint16_t servos[MAX_CHANNEL];
    extern uint32_t outputPulses[MAX_CHANNEL];
    extern const unsigned int startOffsets[MAX_CHANNEL];
    extern const unsigned int pulseOffsets[MAX_CHANNEL];
    extern volatile unsigned int* const pulseRegister[MAX_CHANNEL]; 
    extern bool outputsActivated;
    
    void initOutputs(void);
    void enableActiveOutputs(void);
    void disableThrottle(void);
    void enableThrottle(void);


#ifdef	__cplusplus
}
#endif

#endif	/* OUTPUT_H */

