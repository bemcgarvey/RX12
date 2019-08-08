#include <xc.h>
#include "eeprom.h"
#include <sys/attribs.h>

enum {
    EECMDCFG = 0b100, EEBULKERASE = 0b011, EEPAGEERASE = 0b010, EEWORDPGM = 0b001,
    EEWORDREAD = 0b000
};

static volatile unsigned int eeAddress;
static volatile int remainingWords = 0;
static volatile unsigned int *pData;

void executeWrite(unsigned int addr, uint32_t data);

void initEEPROM(void) {
    CFGCON2bits.EEWS = 2;
    EECONbits.ON = 1;
    while (EECONbits.RDY == 0);
    EECONbits.CMD = EECMDCFG;
    executeWrite(0x0000, DEVEE0);
    executeWrite(0x0004, DEVEE1);
    executeWrite(0x0008, DEVEE2);
    executeWrite(0x000c, DEVEE3);
    EECONbits.WREN = 0;
    IPC46bits.DATAEEIP = 1;
    IPC46bits.DATAEEIS = 0;
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

bool readyEEPROM(void) {
    if (EECONbits.RDY == 0 || EECONbits.RW == 1) {
        return false;
    } else {
        return true;
    }
}

void startIntWrite(unsigned int address, uint32_t *data, int count) {
    if (!readyEEPROM() || remainingWords != 0) {
        return;
    }
    eeAddress = address & 0xffc;
    pData = data;
    remainingWords = count;
    EECONbits.CMD = EEWORDPGM;
    EECONbits.WREN = 1;
    EEADDR = eeAddress;
    EEDATA = *pData;
    __builtin_disable_interrupts();
    EEKEY = 0xEDB7;
    EEKEY = 0x1248;
    EECONSET = _EECON_RW_MASK;
    __builtin_enable_interrupts();
    IFS5bits.DATAEEIF = 0;
    IEC5bits.DATAEEIE = 1;
}

void __ISR(_DATA_EE_VECTOR, IPL1SOFT) EEPROMIsr(void) {
    if (EECONbits.ERR) {
        IEC5bits.DATAEEIE = 0;
        remainingWords = 0;
    } else {
        eeAddress += 4;
        ++pData;
        --remainingWords;
        if (remainingWords != 0) {
            EECONbits.WREN = 1;
            EEADDR = eeAddress;
            EEDATA = *pData;
            __builtin_disable_interrupts();
            EEKEY = 0xEDB7;
            EEKEY = 0x1248;
            EECONSET = _EECON_RW_MASK;
            __builtin_enable_interrupts();
        } else {
            IEC5bits.DATAEEIE = 0;
        }
    }
    IFS5bits.DATAEEIF = 0;
}