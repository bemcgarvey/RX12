/* 
 * File:   led.h
 * Author: bemcg
 *
 * Created on July 18, 2019, 5:06 PM
 */

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

