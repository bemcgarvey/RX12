#include <xc.h>
#include "led.h"

void LED1On(void) {
    LATEbits.LATE13 = 1;
}

void LED1Off(void) {
    LATEbits.LATE13 = 0;
}

void LED1Toggle(void) {
    LATEbits.LATE13 ^= 1;
}

void LED2On(void) {
    LATAbits.LATA8 = 1;
}

void LED2Off(void) {
    LATAbits.LATA8 = 0;
}

void LED2Toggle(void) {
    LATAbits.LATA8 ^= 1;
}

void LED3On(void) {
    LATCbits.LATC1 = 1;
}

void LED3Off(void) {
    LATCbits.LATC1 = 0;
}

void LED3Toggle(void) {
    LATCbits.LATC1 ^= 1;
}