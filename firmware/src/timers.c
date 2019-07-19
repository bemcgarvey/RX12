#include <xc.h>
#include "timers.h"
#include <sys/attribs.h>
#include "led.h"

volatile unsigned int systemTickCount = 0;

void delay_us(unsigned int us) {
    unsigned int delay;
    delay = US_COUNT * us;
    _CP0_SET_COUNT(0);
    while (_CP0_GET_COUNT() < delay);
}


void startSystemTickTimer(void) {
    //Timer 8
    T8CONbits.T32 = 0;
    T8CONbits.TCKPS = 0b010; //1:4
    TMR8 = 0;
    PR8 = MS_COUNT / 4;
    PRISSbits.PRI7SS = 1;
    IPC21bits.T8IP = 7;
    IPC21bits.T8IS = 0;
    IFS2bits.T8IF = 0;
    IEC2bits.T8IE = 1;
    T8CONbits.ON = 1;
}
    
void __ISR(_TIMER_8_VECTOR, IPL7SRS) Timer8Isr(void) {
    ++systemTickCount;
    IFS2bits.T8IF = 0;
}

void startOCTimer(unsigned int period) {
    //Timer 2
    T2CONbits.T32 = 0;
    T2CONbits.TCKPS = 0b110; //1:64
    TMR2 = 0;
    if (period == PERIOD_11MS) {
        PR2 = MS_COUNT * 11 / 64;
    } else {
        PR2 = MS_COUNT * 22 / 64;
    }
    IPC2bits.T2IP = 4;
    IPC2bits.T2IS = 0;
    IFS0bits.T2IF = 0;
    IEC0bits.T2IE = 1;
    T2CONbits.ON = 1;
}

void __ISR(_TIMER_2_VECTOR, IPL4SRS) Timer2Isr(void) {
    IFS0bits.T2IF = 0;
}