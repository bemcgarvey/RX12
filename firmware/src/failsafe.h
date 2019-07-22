/* 
 * File:   failsafe.h
 * Author: bemcg
 *
 * Created on July 21, 2019, 5:30 PM
 */

#ifndef FAILSAFE_H
#define	FAILSAFE_H

#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

    typedef enum {
        HOLD_FAILSAFE = 0, PRESET_FAILSAFE = 1
    } FailsafeType;
    
    extern FailsafeType failsafeType;
    extern bool failsafeEngaged;


#ifdef	__cplusplus
}
#endif

#endif	/* FAILSAFE_H */

