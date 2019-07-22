/* 
 * File:   eeprom.h
 * Author: bemcg
 *
 * Created on July 22, 2019, 9:17 AM
 */

#ifndef EEPROM_H
#define	EEPROM_H

#include <stdint.h>

#define EEPROM_SUCCESS      0
#define EEPROM_VERIFY_ERROR 1
#define EEPROM_BAD_COMMAND  2
#define EEPROM_BOR_ERROR    3
#define EEPROM_NOT_READY    4

#ifdef	__cplusplus
extern "C" {
#endif

    void initEEPROM(void);
    int readEEPROM(unsigned int address, uint32_t *data);
    int writeEEPROM(unsigned int address, uint32_t data);

#ifdef	__cplusplus
}
#endif

#endif	/* EEPROM_H */

