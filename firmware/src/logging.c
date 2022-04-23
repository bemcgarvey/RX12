/////////////////////////////////////////////////////
// Project: RX12                                   //
// File: logging.c                                 //
// Target: PIC32MKxxxxGPD/E064                     // 
// Compiler: XC32 (Tested with 2.10)               //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: data logging functions             //
/////////////////////////////////////////////////////
#include <xc.h>
#include "logging.h"
#include "eeprom.h"
#include "timers.h"
#include <sys/attribs.h>
#include "startup.h"
#include "adc.h"
#include "datapacket.h"

#define LOGGING_MAGIC_NUMBER    0x12398745

volatile LogData __attribute__((persistent)) currentFlightLog;
bool logging = false;
unsigned int logAddress = ADDRESS_LOG_START;

volatile uint32_t loggingTimer = 0;

bool loadLogData(LogData *data, unsigned int eeAddress) {
    int result = 0;
    for (int i = 0; i < LOG_WORDS; ++i) {
        result |= readEEPROM(eeAddress, &(data->words[i]));
        eeAddress += 4;
    }
    return result == EEPROM_SUCCESS;
}

void startLogging(void) {
    if (startupMode == START_WDTO) {
        currentFlightLog.statusFlags |= STATUS_WDTO;
    } else {
        unsigned int seq;
        readEEPROM(ADDRESS_LOG_SEQUENCE, &seq);
        ++seq;
        writeEEPROM(ADDRESS_LOG_SEQUENCE, seq);
        currentFlightLog.duration = 0;
        currentFlightLog.fades[0] = 0xffffffff;
        currentFlightLog.fades[1] = 0xffffffff;
        currentFlightLog.fades[2] = 0xffffffff;
        currentFlightLog.statusFlags = 0;
        currentFlightLog.totalPackets = 0;
        currentFlightLog.rxHighVoltage = 0;
        currentFlightLog.rxLowVoltage = 0xffffffff;
        currentFlightLog.sequenceNumber = seq;
        readEEPROM(ADDRESS_CURRENT_LOG, &logAddress);
        logAddress += sizeof(LogData);
        if (logAddress >= EEPROM_SIZE) {
            logAddress = ADDRESS_LOG_START;
        }
        writeEEPROM(ADDRESS_CURRENT_LOG, logAddress);
    }  
    initADC();
    T9CONbits.T32 = 0;
    T9CONbits.TCKPS = 0b010; //1:4
    TMR9 = 0;
    PR9 = MS_COUNT / 4;
    IPC22bits.T9IP = 5;
    IPC22bits.T9IS = 0;
    IFS2bits.T9IF = 0;
    IEC2bits.T9IE = 1;
    T9CONbits.ON = 1;
}

void __ISR(_TIMER_9_VECTOR, IPL5SOFT) Timer9Isr(void) {
    ++loggingTimer;
    ADCCON3bits.GSWTRG = 1;   //Trigger an ADC conversion
    if (loggingTimer >= LOGGING_INTERVAL) {
        loggingTimer = 0;
        currentFlightLog.duration = systemTickCount;
        currentFlightLog.totalPackets = packetsReceived;
        startIntWrite(logAddress, (unsigned int *)currentFlightLog.words, LOG_WORDS);
    }
    IFS2bits.T9IF = 0;
}

void enableLogging(bool enable) {
    if (enable) {
        writeEEPROM(ADDRESS_LOGGING_ACTIVE, LOGGING_MAGIC_NUMBER); 
    } else {
        writeEEPROM(ADDRESS_LOGGING_ACTIVE, 0);
    }
    while (!readyEEPROM());
}

void checkLoggingActive(void) {
    unsigned int num = 0;
    readEEPROM(ADDRESS_LOGGING_ACTIVE, &num);
    if (num == LOGGING_MAGIC_NUMBER) {
        logging = true;
    } else {
        logging = false;
    }
}