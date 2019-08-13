/////////////////////////////////////////////////////
// Project: RX12                                   //
// File: adc.h                                     //
// Target: PIC32MKxxxxGPD/E064                     // 
// Compiler: XC32 (Tested with 2.10)               //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: adc functions                      //
/////////////////////////////////////////////////////

#ifndef ADC_H
#define	ADC_H

#ifdef	__cplusplus
extern "C" {
#endif

    void initADC(void);
    void initADCSingleSample(void);
    unsigned int readADC(void);


#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */

