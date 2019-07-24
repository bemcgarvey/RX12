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

    typedef enum {
        DSMX_INTERNAL_11MS = 9, DSMX_EXTERNAL_11MS = 10, DSMX_INTERNAL_22MS = 7
        , DSMX_EXTERNAL_22MS = 8, DSM2_INTERNAL_22MS = 3, DSM2_EXTERNAL_22MS = 4
        , DSM2_INTERNAL_11MS = 5, DSM2_EXTERNAL_11MS = 6
    } BindPulses;

    typedef enum {
        DSMX_22 = 1, DSMX_11 = 2, DSM2_22 = 3, DSM2_11 = 4
    } BindType;

    typedef enum {
        SYSTEM_TYPE_DSM2_1024 = 0x01, SYSTEM_TYPE_DSM2_2048 = 0x12,
        SYSTEM_TYPE_DSMX_22 = 0xa2, SYSTEM_TYPE_DSMX_11 = 0xb2,
        SYSTEM_TYPE_NONE = 0x00
    } DSMSystemType;

    extern bool connectedSatellites[3];
    extern volatile unsigned int lastRxTime[3];
    extern unsigned int primarySatellite;

    void DetectConnectedSatellites(void);
    void SendBindPulses(BindType type);

#ifdef	__cplusplus
}
#endif

#endif	/* SATELLITE_H */

