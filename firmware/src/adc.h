/* 
 * File:   adc.h
 * Author: bemcg
 *
 * Created on July 29, 2019, 5:00 PM
 */

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

