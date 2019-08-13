/////////////////////////////////////////////////////
// Project: RX12                                   //
// File: startup.h                                 //
// Target: PIC32MKxxxxGPD/E064                     // 
// Compiler: XC32 (Tested with 2.10)               //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: startup and detection routines     //
/////////////////////////////////////////////////////

#ifndef STARTUP_H
#define	STARTUP_H

#include "satellite.h"

#ifdef	__cplusplus
extern "C" {
#endif

    typedef enum {
        START_NORMAL = 0, START_BIND = 1, START_SERIAL = 2, START_WDTO = 3
    } StartupMode;

    typedef enum {
        FRAME_22MS = 0x22, FRAME_11MS = 0x11
    } FrameMode;

    void DetectStartupMode(void);

    extern StartupMode startupMode;
    extern FrameMode frameMode;
    extern BindType bindType;
    extern DSMSystemType systemType;

#ifdef	__cplusplus
}
#endif

#endif	/* STARTUP_H */

