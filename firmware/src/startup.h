/* 
 * File:   startup.h
 * Author: bemcg
 *
 * Created on July 18, 2019, 1:48 PM
 */

#ifndef STARTUP_H
#define	STARTUP_H

#ifdef	__cplusplus
extern "C" {
#endif

    typedef enum {START_NORMAL = 0, START_BIND = 1, START_SERIAL = 2} StartupMode;
    
    void DetectStartupMode(void);
    
    extern StartupMode startupMode;


#ifdef	__cplusplus
}
#endif

#endif	/* STARTUP_H */

