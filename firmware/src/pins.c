/////////////////////////////////////////////////////
// Project: RX12                                   //
// File: pins.c                                    //
// Target: PIC32MKxxxxGPD/E064                     // 
// Compiler: XC32 (Tested with 2.10)               //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: I/O pin configuration              //
/////////////////////////////////////////////////////
#include <xc.h>
#include "pins.h"
#include "startup.h"

void initPins(void) {
    //Set all pins to digital output low level
    // except AN25/RB7
    LATA = 0;
    LATB = 0;
    LATC = 0;
    LATD = 0;
    LATE = 0;
    LATF = 0;
    LATG = 0;
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;
    ANSELE = 0;
    ANSELG = 0;
    TRISA = 0;
    TRISB = 0;
    TRISC = 0;
    TRISD = 0;
    TRISE = 0;
    TRISF = 0;
    TRISG = 0;
    //AN25/RB7
    ANSELBbits.ANSB7 = 1;
    TRISBbits.TRISB7 = 1;
    //UART pins should be inputs
    TRISEbits.TRISE14 = 1;
    TRISBbits.TRISB4 = 1;
    TRISCbits.TRISC0 = 1;
}

void setPPS(void) {
    /* unlock system for PPS configuration */
    SYSKEY = 0x00000000;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    CFGCONbits.IOLOCK = 0;

    if (startupMode == START_SERIAL) {
        TRISBbits.TRISB0 = 1;
        U4RXR = 2;  //setup USART4
        RPA1R = 2;
    }
    
    /* PPS Input Remapping */
    U3RXR = 6;
    U5RXR = 8;
    U6RXR = 2;

    /* PPS Output Remapping */
    RPA7R = 5;
    RPB14R = 5;
    RPB15R = 5;
    RPG6R = 6;
    RPB9R = 9;
    RPC6R = 9;
    RPC8R = 8;
    RPD5R = 9;
    RPC9R = 6;
    RPB12R = 6;
    RPB13R = 6;
    RPD6R = 5;
    
    /* Lock back the system after PPS configuration */
    SYSKEY = 0x00000000;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    CFGCONbits.IOLOCK = 1;
}
