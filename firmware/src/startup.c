#include <xc.h>
#include "startup.h"
#include "timers.h"
#include <sys/attribs.h>
#include "failsafe.h"
#include "output.h"
#include "eeprom.h"

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
    if (startupMode == START_BIND) {
        //Assume hold failsafe
        failsafeType = HOLD_FAILSAFE;
        writeEEPROM(ADDRESS_FAILSAFE_TYPE, failsafeType);
        while (!readyEEPROM());
        //setup change notification on RB0 to check for preset failsafe
        CNCONBbits.ON = 1;
        PORTB;
        IPC11bits.CNBIP = 6;
        IPC11bits.CNBIS = 0;
        CNCONBbits.EDGEDETECT = 1;
        CNENBbits.CNIEB0 = 1;
        IFS1bits.CNBIF = 0;
        IEC1bits.CNBIE = 1;
    } else {
        CNPDBbits.CNPDB0 = 0; //Make sure pull-ups and pull-downs are off
        CNPUBbits.CNPUB0 = 0;
        TRISBbits.TRISB0 = 0;
        LATBbits.LATB0 = 0;
    }
}


void __ISR(_CHANGE_NOTICE_B_VECTOR, IPL6SOFT) ChangeBISR(void) {
        int temp = WDTCONbits.ON;
        WDTCONbits.ON = 0;
        PORTB;
        IFS1bits.CNBIF = 0;
        IEC1bits.CNBIE = 0;
        CNCONBbits.ON = 0;
        CNPDBbits.CNPDB0 = 0; //Make sure pull-ups and pull-downs are off
        CNPUBbits.CNPUB0 = 0;
        LATBbits.LATB0 = 0;
        TRISBbits.TRISB0 = 0;
        failsafeType = PRESET_FAILSAFE;
        writeEEPROM(ADDRESS_FAILSAFE_TYPE, failsafeType);
        while (!readyEEPROM());
        for (int i = 0; i < MAX_CHANNEL; ++i) {
            presetOutputPulses[i] = outputPulses[i];
        }
        saveFailsafePresets();
        WDTCONbits.ON = temp;
}