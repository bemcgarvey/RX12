/////////////////////////////////////////////////////
// Project: RX12                                   //
// File: output.c                                  //
// Target: PIC32MKxxxxGPD/E064                     // 
// Compiler: XC32 (Tested with 2.10)               //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: servo output functions             //
/////////////////////////////////////////////////////
#include <xc.h>
#include "output.h"
#include "timers.h"

#define OFFSET  (2097 * US_COUNT)
#define PULSE   (903 * US_COUNT)

uint16_t servos[MAX_CHANNEL] = {0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff};

uint32_t outputPulses[MAX_CHANNEL];

const unsigned int startOffsets[MAX_CHANNEL] = {0, OFFSET, 0, OFFSET, 0, OFFSET,
    0, OFFSET, 2 * OFFSET, 2 * OFFSET, 2 * OFFSET, 2 * OFFSET};

const unsigned int pulseOffsets[MAX_CHANNEL] = {0 + PULSE, OFFSET + PULSE,
    0 + PULSE, OFFSET + PULSE, 0 + PULSE, OFFSET + PULSE, 0 + PULSE, OFFSET + PULSE,
    2 * OFFSET + PULSE, 2 * OFFSET + PULSE, 2 * OFFSET + PULSE, 2 * OFFSET + PULSE};

volatile unsigned int* const pulseRegister[MAX_CHANNEL] = {&OC11RS, &OC10RS, &OC9RS,
    &OC13RS, &OC2RS, &OC6RS, &OC8RS, &OC5RS, &OC4RS, &OC3RS, &OC1RS, &OC7RS};

volatile unsigned int* const startRegister[MAX_CHANNEL] = {&OC11R, &OC10R, &OC9R,
    &OC13R, &OC2R, &OC6R, &OC8R, &OC5R, &OC4R, &OC3R, &OC1R, &OC7R};

volatile unsigned int* const OCxCONSETRegister[MAX_CHANNEL] = {&OC11CONSET, &OC10CONSET, &OC9CONSET,
    &OC13CONSET, &OC2CONSET, &OC6CONSET, &OC8CONSET, &OC5CONSET, &OC4CONSET, &OC3CONSET, &OC1CONSET, &OC7CONSET};

volatile unsigned int* const OCxCONCLRRegister[MAX_CHANNEL] = {&OC11CONCLR, &OC10CONCLR, &OC9CONCLR,
    &OC13CONCLR, &OC2CONCLR, &OC6CONCLR, &OC8CONCLR, &OC5CONCLR, &OC4CONCLR, &OC3CONCLR, &OC1CONCLR, &OC7CONCLR};

bool outputsActivated = false;

void initOutputs(void) {
    //Ch 0
    OC11CONbits.OC32 = 1;
    OC11CONbits.OCM = 0b101;
    //Ch 1
    OC10CONbits.OC32 = 1;
    OC10CONbits.OCM = 0b101;
    //Ch 2
    OC9CONbits.OC32 = 1;
    OC9CONbits.OCM = 0b101;
    //Ch 3
    OC13CONbits.OC32 = 1;
    OC13CONbits.OCM = 0b101;
    //Ch 4
    OC2CONbits.OC32 = 1;
    OC2CONbits.OCM = 0b101;
    //Ch 5
    OC6CONbits.OC32 = 1;
    OC6CONbits.OCM = 0b101;
    //Ch 6
    OC8CONbits.OC32 = 1;
    OC8CONbits.OCM = 0b101;
    //Ch 7
    OC5CONbits.OC32 = 1;
    OC5CONbits.OCM = 0b101;
    //Ch 8
    OC4CONbits.OC32 = 1;
    OC4CONbits.OCM = 0b101;
    //Ch 9
    OC3CONbits.OC32 = 1;
    OC3CONbits.OCM = 0b101;
    //Ch 10
    OC1CONbits.OC32 = 1;
    OC1CONbits.OCM = 0b101;
    //Ch 11
    OC7CONbits.OC32 = 1;
    OC7CONbits.OCM = 0b101;
    for (int i = 0; i < MAX_CHANNEL; ++i) {
        *startRegister[i] = startOffsets[i];
        outputPulses[i] = pulseOffsets[i] + ((1194 * US_COUNT) * 1024) / 2048;  //Mid
        *pulseRegister[i] = outputPulses[i];
    }
}

void enableActiveOutputs(void) {
    for (int i = 0; i < MAX_CHANNEL; ++i) {
        if (servos[i] != 0xffff) {
            *OCxCONSETRegister[i] = 0x8000;  //ON bit
        } 
    }
}

void disableThrottle(void) {
    *OCxCONCLRRegister[THROTTLE] = 0x8000; //clear ON bit
}

void enableThrottle(void) {
    *OCxCONSETRegister[THROTTLE] = 0x8000; //clear ON bit
}
