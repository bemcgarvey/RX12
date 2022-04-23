/////////////////////////////////////////////////////
// Project: RX12                                   //
// File: adc.c                                     //
// Target: PIC32MKxxxxGPD/E064                     // 
// Compiler: XC32 (Tested with 2.10)               //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: adc functions                      //
/////////////////////////////////////////////////////
#include <xc.h>
#include "adc.h"
#include <sys/attribs.h>
#include "logging.h"

void initADC(void) {
    //initialize ADC calibration setting
    ADC0CFG = DEVADC0;
    ADC1CFG = DEVADC1;
    ADC2CFG = DEVADC2;
    ADC3CFG = DEVADC3;
    ADC4CFG = DEVADC4;
    ADC5CFG = DEVADC5;
    ADC7CFG = DEVADC7;
    ADCCON1bits.FRACT = 0;
    ADCANCONbits.WKUPCLKCNT = 5;
    ADCCON3bits.ADCSEL = 0;
    ADCCON3bits.CONCLKDIV = 1; // Control clock frequency is half of input clock
    ADCCON3bits.VREFSEL = 0;
    ADC5TIMEbits.ADCDIV = 1; // TAD is half of control clock
    ADC5TIMEbits.SAMC = 28; // sampling time = 30 * TAD = 2us
    ADC5TIMEbits.SELRES = 3;
    ADCTRGMODEbits.SH5ALT = 0b11; //AN25
    //Filter
    ADCFLTR1bits.CHNLID = 5; // Use ADC5 as the source
    ADCFLTR1bits.OVRSAM = 0b100; // 32x average
    ADCFLTR1bits.DFMODE = 1; // average mode
    ADCFLTR1bits.AFEN = 1; // Enable filter 1
    //Interrupt
    ADCFLTR1bits.AFGIEN = 1;
    IPC24bits.AD1DF1IP = 5;
    IPC24bits.AD1DF1IS = 3;
    IFS3bits.AD1DF1IF = 0;
    IEC3bits.AD1DF1IE = 1;
    /* Set up the trigger sources */
    ADCTRGSNSbits.LVL0 = 0; // Edge trigger
    ADCTRG2bits.TRGSRC5 = 0b00001; // Set AN5 to software trigger.
    ADCCON1bits.ON = 1;
    while (!ADCCON2bits.BGVRRDY);
    while (ADCCON2bits.REFFLT);
    ADCANCONbits.ANEN5 = 1;
    while (!ADCANCONbits.WKRDY5);
    ADCCON3bits.DIGEN5 = 1;
}

void __ISR(_ADC_DF1_VECTOR, IPL5SOFT) ADDF1Isr(void) {
    unsigned int result;
    result = ADCFLTR1bits.FLTRDATA;
    if (result > currentFlightLog.rxHighVoltage) {
        currentFlightLog.rxHighVoltage = result;
    }
    if (result < currentFlightLog.rxLowVoltage) {
        currentFlightLog.rxLowVoltage = result;
    }
    IFS3bits.AD1DF1IF = 0;
}

void initADCSingleSample(void) {
    //initialize ADC calibration setting
    ADC0CFG = DEVADC0;
    ADC1CFG = DEVADC1;
    ADC2CFG = DEVADC2;
    ADC3CFG = DEVADC3;
    ADC4CFG = DEVADC4;
    ADC5CFG = DEVADC5;
    ADC7CFG = DEVADC7;
    ADCCON1bits.FRACT = 0;
    ADCANCONbits.WKUPCLKCNT = 5;
    ADCCON3bits.ADCSEL = 0;
    ADCCON3bits.CONCLKDIV = 1; // Control clock frequency is half of input clock
    ADCCON3bits.VREFSEL = 0;
    ADC5TIMEbits.ADCDIV = 1; // TAD frequency is half of control clock 
    ADC5TIMEbits.SAMC = 28; //sampling time = 30 * TAD = 2us
    ADC5TIMEbits.SELRES = 3;
    ADCTRGMODEbits.SH5ALT = 0b11; //AN25
    ADCTRG2bits.TRGSRC5 = 0b00001; //trigger from software.
    //Setup filter
    ADCFLTR1bits.CHNLID = 5; // Use ADC5 as the source
    ADCFLTR1bits.OVRSAM = 0b100; // 32x average
    ADCFLTR1bits.DFMODE = 1; // average mode
    ADCFLTR1bits.AFEN = 1; // Enable filter 1
    ADCCON1bits.ON = 1;
    while (!ADCCON2bits.BGVRRDY);
    while (ADCCON2bits.REFFLT);
    ADCANCONbits.ANEN5 = 1;
    while (!ADCANCONbits.WKRDY5);
    ADCCON3bits.DIGEN5 = 1;
}

unsigned int readADC(void) {
    unsigned int result;
    ADCCON3bits.GSWTRG = 1;
    while (ADCFLTR1bits.AFRDY == 0);
    result = ADCFLTR1bits.FLTRDATA;
    return result;
}