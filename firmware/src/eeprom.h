/////////////////////////////////////////////////////
// Project: RX12                                   //
// File: eeprom.h                                  //
// Target: PIC32MKxxxxGPD/E064                     // 
// Compiler: XC32 (Tested with 2.10)               //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: EEPROM functions                   //
/////////////////////////////////////////////////////

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
#define ADDRESS_LOGGING_ACTIVE      0x10
#define ADDRESS_ADC_CALIBRATION1    0x14
#define ADDRESS_ADC_CALIBRATION2    0x18
#define ADDRESS_CURRENT_LOG         0x1c
#define ADDRESS_LOG_SEQUENCE        0x20
#define ADDRESS_PRIMARY_SAT         0x24
#define ADDRESS_OUTPUT_TYPE         0x28
#define ADDRESS_FAILSAFE_VALUES     0x40
#define ADDRESS_LOG_START           0x100

#ifdef	__cplusplus
extern "C" {
#endif

    void initEEPROM(void);
    int readEEPROM(unsigned int address, uint32_t *data);
    int writeEEPROM(unsigned int address, uint32_t data);
    bool readyEEPROM(void);
    void startIntWrite(unsigned int address, uint32_t *data, int count);

#ifdef	__cplusplus
}
#endif

#endif	/* EEPROM_H */

