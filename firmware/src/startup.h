/* 
 * File:   startup.h
 * Author: bemcg
 *
 * Created on July 18, 2019, 1:48 PM
 */

#ifndef STARTUP_H
#define	STARTUP_H

#include "satellite.h"

#ifdef	__cplusplus
extern "C" {
#endif

    typedef enum {START_NORMAL = 0, START_BIND = 1, START_SERIAL = 2, START_WDTO} StartupMode;
    typedef enum {FRAME_22MS = 0x22, FRAME_11MS = 0x11} FrameMode;
    
    void DetectStartupMode(void);
    
    extern StartupMode startupMode;
    extern FrameMode frameMode;
    extern BindType bindType;
    extern DSMSystemType systemType;

#ifdef	__cplusplus
}
#endif

#endif	/* STARTUP_H */

