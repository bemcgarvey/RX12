/* 
 * File:   eeprom.h
 * Author: bemcg
 *
 * Created on July 22, 2019, 9:17 AM
 */

#ifndef EEPROM_H
#define	EEPROM_H

#include <stdint.h>
#include <stdbool.h>

//Error codes returned by read/write
#define EEPROM_SUCCESS      0
#define EEPROM_VERIFY_ERROR 1
#define EEPROM_BAD_COMMAND  2
#define EEPROM_BOR_ERROR    3
#define EEPROM_NOT_READY    4

//Addresses
#define EEPROM_SIZE                 0x1000
#define ADDRESS_FRAME_RATE          0x00
#define ADDRESS_DSM_TYPE            0x04
#define ADDRESS_FAILSAFE_TYPE       0x08
#define ADDRESS_FAILSAFE_VALUES     0x0c
#define ADDRESS_LOGGING_ACTIVE      0x10
#define ADDRESS_ADC_CALIBRATION1    0x14
#define ADDRESS_ADC_CALIBRATION2    0x18
#define ADDRESS_RESERVED            0x1c
//Start of log data, reserve 12 words
#define ADDRESS_LOG             (ADDRESS_RESERVED + 12 * 4)

#ifdef	__cplusplus
extern "C" {
#endif

    void initEEPROM(void);
    int readEEPROM(unsigned int address, uint32_t *data);
    int writeEEPROM(unsigned int address, uint32_t data);
    bool readyEEPROM(void);

#ifdef	__cplusplus
}
#endif

#endif	/* EEPROM_H */

