/* 
 * File:   servos.h
 * Author: bemcg
 *
 * Created on July 20, 2019, 3:01 PM
 */

#ifndef SERVOS_H
#define	SERVOS_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

    enum {
        THROTTLE = 0, AILERON = 1, ELEVATOR = 2, RUDDER = 3, GEAR = 4, AUX1 = 5,
        AUX2 = 6, AUX3 = 7, AUX4 = 8, AUX5 = 9, AUX6 = 10, AUX7 = 11
    };
    
#define MAX_CHANNEL     12
    
    extern uint16_t servos[MAX_CHANNEL];

#ifdef	__cplusplus
}
#endif

#endif	/* SERVOS_H */

