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
    /* initialize ADC calibration setting */
    ADC0CFG = DEVADC0;
    ADC1CFG = DEVADC1;
    ADC2CFG = DEVADC2;
    ADC3CFG = DEVADC3;
    ADC4CFG = DEVADC4;
    ADC5CFG = DEVADC5;
    ADC7CFG = DEVADC7;
    ADCCON1bits.FRACT = 0; //integer output
    /* Initialize warm up time register */
    ADCANCONbits.WKUPCLKCNT = 5; // Wakeup exponent = 32 * TADx
    /* Clock setting */
    ADCCON3bits.ADCSEL = 0; // Select input clock source
    ADCCON3bits.CONCLKDIV = 1; // Control clock frequency is half of input clock
    ADCCON3bits.VREFSEL = 0; // Select AVDD and AVSS as reference source
    /* Select ADC sample time and conversion clock */
    ADC5TIMEbits.ADCDIV = 1; // ADC5 clock frequency is half of control clock = TAD0
    ADC5TIMEbits.SAMC = 18; // ADC5 sampling time = 20 * TAD
    ADC5TIMEbits.SELRES = 3; // ADC5 resolution is 12 bits
    /* Select analog input for ADC modules, no presync trigger, not sync sampling */
    ADCTRGMODEbits.SH5ALT = 0b11; //AN25
    /* Configure interrupt */
    ADCGIRQEN1bits.AGIEN5 = 1; //Enable AN5 interrupt
    IPC27bits.AD1D5IP = 5;
    IPC27bits.AD1D5IS = 3;
    IFS3bits.AD1D5IF = 0;
    IEC3bits.AD1D5IE = 1;
    /* Set up the trigger sources */
    ADCTRGSNSbits.LVL0 = 0; // Edge trigger
    ADCTRG2bits.TRGSRC5 = 0b00111; // Set AN5 to trigger from TMR5.
    /* Turn the ADC on */
    ADCCON1bits.ON = 1;
    /* Wait for voltage reference to be stable */
    while (!ADCCON2bits.BGVRRDY); // Wait until the reference voltage is ready
    while (ADCCON2bits.REFFLT); // Wait if there is a fault with the reference voltage
    /* Enable clock to analog circuit */
    ADCANCONbits.ANEN5 = 1; // Enable the clock to analog bias
    /* Wait for ADC to be ready */
    while (!ADCANCONbits.WKRDY5); // Wait until ADC5 is ready
    /* Enable the ADC module */
    ADCCON3bits.DIGEN5 = 1; // Enable ADC5
}

void __ISR(_ADC_DATA5_VECTOR, IPL5SOFT) ADC5Isr(void) {
    unsigned int result;
    result = ADCDATA5;
    if (result > currentFlightLog.rxHighVoltage) {
        currentFlightLog.rxHighVoltage = result;
    }
    if (result < currentFlightLog.rxLowVoltage) {
        currentFlightLog.rxLowVoltage = result;
    }
    IFS3bits.AD1D5IF = 0;
}

void initADCSingleSample(void) {
    /* initialize ADC calibration setting */
    ADC0CFG = DEVADC0;
    ADC1CFG = DEVADC1;
    ADC2CFG = DEVADC2;
    ADC3CFG = DEVADC3;
    ADC4CFG = DEVADC4;
    ADC5CFG = DEVADC5;
    ADC7CFG = DEVADC7;
    ADCCON1bits.FRACT = 0; //integer output
    /* Initialize warm up time register */
    ADCANCONbits.WKUPCLKCNT = 5; // Wakeup exponent = 32 * TADx
    /* Clock setting */
    ADCCON3bits.ADCSEL = 0; // Select input clock source
    ADCCON3bits.CONCLKDIV = 1; // Control clock frequency is half of input clock
    ADCCON3bits.VREFSEL = 0; // Select AVDD and AVSS as reference source
    /* Select ADC sample time and conversion clock */
    ADC5TIMEbits.ADCDIV = 1; // ADC5 clock frequency is half of control clock = TAD0
    ADC5TIMEbits.SAMC = 18; // ADC5 sampling time = 20 * TAD5
    ADC5TIMEbits.SELRES = 3; // ADC5 resolution is 12 bits
    /* Select analog input for ADC modules, no presync trigger, not sync sampling */
    ADCTRGMODEbits.SH5ALT = 0b11; //AN25
    /* Set up the trigger sources */
    ADCTRG2bits.TRGSRC5 = 0b00001; // Set AN5 to trigger from software.
    /* Turn the ADC on */
    ADCCON1bits.ON = 1;
    /* Wait for voltage reference to be stable */
    while (!ADCCON2bits.BGVRRDY); // Wait until the reference voltage is ready
    while (ADCCON2bits.REFFLT); // Wait if there is a fault with the reference voltage
    /* Enable clock to analog circuit */
    ADCANCONbits.ANEN5 = 1; // Enable the clock to analog bias
    /* Wait for ADC to be ready */
    while (!ADCANCONbits.WKRDY5); // Wait until ADC5 is ready
    /* Enable the ADC module */
    ADCCON3bits.DIGEN5 = 1; // Enable ADC5
}

unsigned int readADC(void) {
    unsigned int result;
    ADCCON3bits.GSWTRG = 1;
    while (ADCDSTAT1bits.ARDY5 == 0);
    result = ADCDATA5;
    return result;
}