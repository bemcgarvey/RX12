/* 
 * File:   satellite.h
 * Author: bemcg
 *
 * Created on July 18, 2019, 3:04 PM
 */

#ifndef SATELLITE_H
#define	SATELLITE_H

#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

    enum {SAT1 = 0, SAT2 = 1, SAT3 = 2};
    
    extern bool activeSatellites[3];
    
    void DetectConnectedSatellites(void);


#ifdef	__cplusplus
}
#endif

#endif	/* SATELLITE_H */

