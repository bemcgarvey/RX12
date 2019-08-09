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
    T5CONbits.T32 = 0;
    T5CONbits.TCKPS = 0b010; //1:4
    TMR5 = 0;
    PR5 = MS_COUNT / 4;
    IPC6bits.T5IP = 5;
    IPC6bits.T5IS = 0;
    IFS0bits.T5IF = 0;
    IEC0bits.T5IE = 1;
    T5CONbits.ON = 1;
}

void __ISR(_TIMER_5_VECTOR, IPL5SOFT) Timer5Isr(void) {
    ++loggingTimer;
    if (loggingTimer >= LOGGING_INTERVAL) {
        loggingTimer = 0;
        currentFlightLog.duration = systemTickCount;
        currentFlightLog.totalPackets = packetsReceived;
        startIntWrite(logAddress, (unsigned int *)currentFlightLog.words, LOG_WORDS);
    }
    IFS0bits.T5IF = 0;
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