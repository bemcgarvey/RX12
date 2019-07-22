#include <xc.h>
#include "eeprom.h"

enum {
    EECMDCFG = 0b100, EEBULKERASE = 0b011, EEPAGEERASE = 0b010, EEWORDPGM = 0b001,
    EEWORDREAD = 0b000
};

void executeWrite(unsigned int addr, uint32_t data);

void initEEPROM(void) {
    CFGCON2bits.EEWS = 2;
    EECONbits.ON = 1;
    while (EECONbits.RDY == 0);
    EECONbits.CMD = EECMDCFG;
    executeWrite(0x0000, DEVEE1);
    executeWrite(0x0004, DEVEE2);
    executeWrite(0x0008, DEVEE3);
    EECONbits.WREN = 0;
}

int readEEPROM(unsigned int address, uint32_t *data) {
    if (EECONbits.RDY == 1) {
        if (EECONbits.RW == 0) {
            EEADDR = address & 0xFFC; // Set address on 32-bit boundary
            EECONbits.CMD = EEWORDREAD;
            EECONbits.WREN = 0;
            EECONbits.RW = 1;
            while (EECONbits.RW == 1);
            *data = EEDATA;
            return EECONbits.ERR;
        }
    }
    return EEPROM_NOT_READY;
}

int writeEEPROM(unsigned int address, uint32_t data) {
    if (EECONbits.RDY == 1) {
        if (EECONbits.RW == 0) {
            EECONbits.CMD = EEWORDPGM;
            executeWrite(address, data);
            return EECONbits.ERR;
        }
    }
    return EEPROM_NOT_READY;
}

void executeWrite(unsigned int addr, uint32_t data) {
    EECONbits.WREN = 1;
    EEADDR = addr & 0xffc;
    EEDATA = data;
    EEKEY = 0xEDB7;
    EEKEY = 0x1248;
    EECONSET = _EECON_RW_MASK;
    while (EECONbits.RW == 1);
}