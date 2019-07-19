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

    enum {
        SAT1 = 0, SAT2 = 1, SAT3 = 2
    };

    enum {
        DSMX_INTERNAL_11MS = 9, DSMX_EXTERNAL_11MS = 10, DSMX_INTERNAL_22MS = 7
        , DSMX_EXTERNAL_22MS = 8, DSM2_INTERNAL_22MS = 3, DSM2_EXTERNAL_22MS = 4
        , DSM2_INTERNAL_11MS = 5, DSM2_EXTERNAL_11MS = 6
    };

    typedef enum {
        DSMX_22 = 1, DSMX_11 = 2, DSM2_22 = 3, DSM2_11 = 3
    } BindType;

    extern bool connectedSatellites[3];
    void DetectConnectedSatellites(void);
    void SendBindPulses(BindType type);

#ifdef	__cplusplus
}
#endif

#endif	/* SATELLITE_H */

