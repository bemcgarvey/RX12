#include <xc.h>
#include "startup.h"
#include "timers.h"

StartupMode startupMode = START_NORMAL;
FrameMode frameMode = FRAME_22MS;
BindType bindType = DSMX_11;
DSMSystemType systemType = SYSTEM_TYPE_NONE;

void DetectStartupMode(void) {
    ANSELBbits.ANSB0 = 0;
    ANSELAbits.ANSA1 = 0;
    LATAbits.LATA1 = 0;
    LATBbits.LATB0 = 0;
    TRISBbits.TRISB0 = 1;
    CNPDBbits.CNPDB0 = 1; //enable pull-down
    TRISAbits.TRISA1 = 0;
    delay_us(1);
    if (PORTBbits.RB0 == 1) {
        startupMode = START_SERIAL;
    } else {
        CNPDBbits.CNPDB0 = 0;
        CNPUBbits.CNPUB0 = 1; //enable pull-up
        delay_us(1);
        if (PORTBbits.RB0 == 0) {
            startupMode = START_BIND;
            //Check for 11ms frame
            TRISGbits.TRISG6 = 1;
            CNPUGbits.CNPUG6 = 1;
            delay_us(1);
            if (PORTGbits.RG6 == 0) {
                frameMode = FRAME_11MS;
            }
            CNPUGbits.CNPUG6 = 0;
            TRISGbits.TRISG6 = 0;
            //Check for DSM2
            TRISBbits.TRISB13 = 1;
            CNPUBbits.CNPUB13 = 1;
            delay_us(1);
            if (PORTBbits.RB13 == 0) {
                bindType = DSM2_11;
            }
            CNPUBbits.CNPUB13 = 0;
            TRISBbits.TRISB13 = 0;
        }
    }
    CNPDBbits.CNPDB0 = 0; //Make sure pull-ups and pull-downs are off
    CNPUBbits.CNPUB0 = 0;
    TRISBbits.TRISB0 = 0;
    LATBbits.LATB0 = 0;
}
