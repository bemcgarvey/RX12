/////////////////////////////////////////////////////
// Project: RX12                                   //
// File: timers.c                                  //
// Target: PIC32MKxxxxGPD/E064                     // 
// Compiler: XC32 (Tested with 2.10)               //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: timers for output and system       //
/////////////////////////////////////////////////////
#include <xc.h>
#include "timers.h"
#include <sys/attribs.h>
#include "output.h"
#include "sbus.h"

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
    IPC21bits.T8IP = 7;
    IPC21bits.T8IS = 0;
    IFS2bits.T8IF = 0;
    IEC2bits.T8IE = 1;
    T8CONbits.ON = 1;
}

void __ISR(_TIMER_8_VECTOR, IPL7SOFT) Timer8Isr(void) {
    ++systemTickCount;
    IFS2bits.T8IF = 0;
}

void startOCTimer(unsigned int msPeriod) {
    //Timer 2/3 in 32bit mode
    T2CONbits.ON = 0;
    Nop();
    T2CONbits.T32 = 1; //32 bit
    T2CONbits.TCKPS = 0; //1:1
    TMR2 = 0;
    PR2 = MS_COUNT * msPeriod;
    IPC2bits.T2IP = 6;
    IPC2bits.T2IS = 0;
    IFS0bits.T2IF = 0;
    if (outputType == OUTPUT_TYPE_PWM) {
        IEC0bits.T2IE = 1;
    }
    T2CONbits.ON = 1;
}

void __ISR(_TIMER_2_VECTOR, IPL6SOFT) Timer2Isr(void) {
    if (outputType == OUTPUT_TYPE_PWM) {
        for (int i = 0; i < MAX_CHANNEL; ++i) {
            *pulseRegister[i] = outputPulses[i] + pulseOffsets[i];
        }
    } else if (outputType == OUTPUT_TYPE_SBUS) {
        transmitSBusPacket();
    }
    IFS0bits.T2IF = 0;
}