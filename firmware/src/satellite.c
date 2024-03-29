/////////////////////////////////////////////////////
// Project: RX12                                   //
// File: satellite.c                               //
// Target: PIC32MKxxxxGPD/E064                     // 
// Compiler: XC32 (Tested with 2.10)               //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: satellite detection and bind       //
/////////////////////////////////////////////////////
#include <xc.h>
#include "satellite.h"
#include <stdbool.h>
#include "timers.h"
#include "eeprom.h"

bool connectedSatellites[3] = {false, false, false};
unsigned int primarySatellite = SAT1;

void DetectConnectedSatellites(void) {
    CNPDEbits.CNPDE14 = 1;
    CNPDBbits.CNPDB4 = 1;
    CNPDCbits.CNPDC0 = 1;
    delay_us(50000);
    if (PORTEbits.RE14 == 1) {
        connectedSatellites[SAT1] = true;
    }
    if (PORTBbits.RB4 == 1) {
        connectedSatellites[SAT2] = true;
    }
    if (PORTCbits.RC0 == 1) {
        connectedSatellites[SAT3] = true;
    }
    CNPDEbits.CNPDE14 = 0;
    CNPDBbits.CNPDB4 = 0;
    CNPDCbits.CNPDC0 = 0;
    for (int i = SAT1; i <= SAT3; ++i) {
        if (connectedSatellites[i]) {
            primarySatellite = i;
            break;
        }
    }
    writeEEPROM(ADDRESS_PRIMARY_SAT, primarySatellite);
}

void SendBindPulses(BindType type) {
    LATEbits.LATE14 = 1;
    LATBbits.LATB4 = 1;
    LATCbits.LATC0 = 1;
    TRISEbits.TRISE14 = 0;
    TRISBbits.TRISB4 = 0;
    TRISCbits.TRISC0 = 0;
    delay_us(50000);
    int pulseCount;
    if (type == DSMX_11) {
        pulseCount = DSMX_EXTERNAL_11MS;
    } else if (type == DSMX_22) {
        pulseCount = DSMX_EXTERNAL_22MS;
    } else if (type == DSM2_11) {
        pulseCount = DSM2_EXTERNAL_11MS;
    } else if (type == DSM2_22) {
        pulseCount = DSM2_EXTERNAL_22MS;
    } else {
        pulseCount = DSMX_EXTERNAL_11MS;
    }
    for (int i = 0; i < pulseCount; ++i) {
        if (primarySatellite == SAT1) {
            if (i < pulseCount - 1) {
                LATEbits.LATE14 = 0;
            }
        } else {
            LATEbits.LATE14 = 0;
        }
        if (primarySatellite == SAT2) {
            if (i < pulseCount - 1) {
                LATBbits.LATB4 = 0;
            }
        } else {
            LATBbits.LATB4 = 0;
        }
        if (primarySatellite == SAT3) {
            if (i < pulseCount - 1) {
                LATCbits.LATC0 = 0;
            }
        } else {
            LATCbits.LATC0 = 0;
        }
        delay_us(118);
        LATEbits.LATE14 = 1;
        LATBbits.LATB4 = 1;
        LATCbits.LATC0 = 1;
        delay_us(123);
    }
    TRISEbits.TRISE14 = 1;
    TRISBbits.TRISB4 = 1;
    TRISCbits.TRISC0 = 1;
}