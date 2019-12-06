//TODO add header comments
#include <xc.h>
#include "ppm.h"
#include "output.h"
#include <sys/attribs.h>

static unsigned int ppmChannelCount = 0;
static unsigned int currentPPMChannel;

void initPPM(void) {
    //Ch 12 (on pins) - Need to change this if PPM channel is changed
    OC7CONbits.OC32 = 1;
    OC7CONbits.OCM = 0b101;
    IPC20bits.OC7IP = 6;
    IPC20bits.OC7IS = 0;
    IFS2bits.OC7IF = 0;
    IEC2bits.OC7IE = 1;
    for (int i = 0; i < MAX_CHANNEL; ++i) {
        outputPulses[i] = 0;
    }
}

void startPPM(void) {
    currentPPMChannel = 0;
    ppmChannelCount = 0;
    for (int i = 0; i < MAX_CHANNEL; ++i) {
        if (servos[i] != 0xffff) {
            ppmChannelCount = i + 1;
        }
    }
    if (ppmChannelCount > MAX_PPM_CHANNEL) {
        ppmChannelCount = MAX_PPM_CHANNEL;
    }
    *startRegister[PPM_OUT - 1] = 0;
    *pulseRegister[PPM_OUT - 1] = PPM_SPACE;
    IFS2bits.OC7IF = 0;
    *OCxCONSETRegister[PPM_OUT - 1] = 0x8000; //ON bit
}

void __ISR(_OUTPUT_COMPARE_7_VECTOR, IPL6SOFT) OC7Isr(void) {
    if (currentPPMChannel >= ppmChannelCount) {
        currentPPMChannel = 0;
        *startRegister[PPM_OUT - 1] = 0;
        *pulseRegister[PPM_OUT - 1] = PPM_SPACE;
    } else {
        unsigned int start;
        if (servos[0] == 0xffff && currentPPMChannel == 0) {
            start = *startRegister[PPM_OUT - 1] + PULSE;
        } else {
            start = *startRegister[PPM_OUT - 1] + outputPulses[currentPPMChannel] + PULSE;
        }
        *startRegister[PPM_OUT - 1] = start;
        *pulseRegister[PPM_OUT - 1] = start + PPM_SPACE;
        ++currentPPMChannel;
    }
    IFS2bits.OC7IF = 0;
}
