/////////////////////////////////////////////////////
// Project: RX12                                   //
// File: ppm.h                                     //
// Target: PIC32MKxxxxGPD/E064                     // 
// Compiler: XC32 (Tested with 2.10)               //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: PPM output code                    //
/////////////////////////////////////////////////////

#ifndef PPM_H
#define	PPM_H

#include "timers.h"

#ifdef	__cplusplus
extern "C" {
#endif

#define MAX_PPM_CHANNEL 8
#define PPM_OUT         12  //Channel for PPM output
#define PPM_SPACE  (300 * US_COUNT) 

    void initPPM(void);
    void startPPM(void);

#ifdef	__cplusplus
}
#endif

#endif	/* PPM_H */

