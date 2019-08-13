/////////////////////////////////////////////////////
// Project: RX12                                   //
// File: led.h                                     //
// Target: PIC32MKxxxxGPD/E064                     // 
// Compiler: XC32 (Tested with 2.10)               //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: LED macros                         //
/////////////////////////////////////////////////////

#ifndef LED_H
#define	LED_H

    #define LED1On()        (LATEbits.LATE13 = 1)
    #define LED1Off()       (LATEbits.LATE13 = 0)
    #define LED1Toggle()    (LATEbits.LATE13 ^= 1)
    #define LED2On()        (LATAbits.LATA8 = 1)
    #define LED2Off()       (LATAbits.LATA8 = 0)
    #define LED2Toggle()    (LATAbits.LATA8 ^= 1)
    #define LED3On()        (LATCbits.LATC1 = 1)
    #define LED3Off()       (LATCbits.LATC1 = 0)
    #define LED3Toggle()    (LATCbits.LATC1 ^= 1)

#endif	/* LED_H */

