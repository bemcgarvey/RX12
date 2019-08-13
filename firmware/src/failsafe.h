/////////////////////////////////////////////////////
// Project: RX12                                   //
// File: failsafe.c                                //
// Target: PIC32MKxxxxGPD/E064                     // 
// Compiler: XC32 (Tested with 2.10)               //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: failsafe implementation            //
/////////////////////////////////////////////////////

#ifndef FAILSAFE_H
#define	FAILSAFE_H

#include <stdbool.h>
#include "output.h"

#ifdef	__cplusplus
extern "C" {
#endif

    typedef enum {
        HOLD_FAILSAFE = 0, PRESET_FAILSAFE = 1
    } FailsafeType;
    
    extern FailsafeType failsafeType;
    extern bool failsafeEngaged;
    extern uint32_t presetOutputPulses[MAX_CHANNEL];
    
    bool loadFailsafePresets(void);
    void saveFailsafePresets(void);


#ifdef	__cplusplus
}
#endif

#endif	/* FAILSAFE_H */

