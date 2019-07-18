#include <xc.h>
#include "satellite.h"
#include <stdbool.h>

bool activeSatellites[3] = {false, false, false};

void DetectConnectedSatellites(void) {
    ANSELEbits.ANSE14 = 0;
    ANSELCbits.ANSC0 = 0;
    TRISEbits.TRISE14 = 1;
    TRISBbits.TRISB4 = 1;
    TRISCbits.TRISC0 = 1;
    CNPDEbits.CNPDE14 = 1;
    CNPDBbits.CNPDB4 = 1;
    CNPDCbits.CNPDC0 = 1;
    int i;
    for (i = 0; i < 1000000; ++i);
    if (PORTEbits.RE14 == 1) {
        activeSatellites[SAT1] = true;
    }
    if (PORTBbits.RB4 == 1) {
        activeSatellites[SAT2] = true;
    }
    if (PORTCbits.RC0 == 1) {
        activeSatellites[SAT3] = true;
    }
    CNPDEbits.CNPDE14 = 0;
    CNPDBbits.CNPDB4 = 0;
    CNPDCbits.CNPDC0 = 0;
}
