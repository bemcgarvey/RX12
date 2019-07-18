#include <xc.h>
#include "startup.h"

StartupMode startupMode = START_NORMAL;

void DetectStartupMode(void) {
    startupMode = START_NORMAL;
    ANSELBbits.ANSB0 = 0;
    ANSELAbits.ANSA8 = 0;
    TRISBbits.TRISB0 = 1;
    CNPDBbits.CNPDB0 = 1; //enable pull-down
    TRISAbits.TRISA1 = 0;
    LATAbits.LATA1 = 0;
    Nop();
    Nop();
    if (PORTBbits.RB0 == 1) {
        startupMode = START_SERIAL;
    } else {
        CNPDBbits.CNPDB0 = 0;
        CNPUBbits.CNPUB0 = 1; //enable pull-up
        int i;
        for (i = 0; i < 100; ++i);
        if (PORTBbits.RB0 == 0) {
            startupMode = START_BIND;
        }
    }
    CNPDBbits.CNPDB0 = 0; //Make sure pull-ups and pull-downs are off
    CNPUBbits.CNPUB0 = 0;
    TRISBbits.TRISB0 = 0;
    LATBbits.LATB0 = 0;
}
