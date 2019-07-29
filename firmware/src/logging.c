#include <xc.h>
#include "logging.h"
#include "eeprom.h"
#include "timers.h"
#include <sys/attribs.h>

#define LOGGING_MAGIC_NUMBER    0x12398745

LogData __attribute__((persistent)) currentFlightLog;
uint16_t adcCalibration;
bool logging = false;

volatile uint32_t loggingTimer = 0;

void saveLogData(const LogData *data, unsigned int eeAddress) {

}

bool loadLogData(LogData *data, unsigned int eeAddress) {
    uint32_t *p = (uint32_t *) data;
    for (int i = 0; i < sizeof (LogData) / sizeof (uint32_t); ++i) {
        if (!readEEPROM(eeAddress, p)) {
            return false;
        } else {
            ++p;
            eeAddress += sizeof (uint32_t);
        }
    }
    return true;
}

void startLogging(void) {
    //Enable ADC5
    
    //Initialize the loggingData struct
    //If WDTO set flag otherwise start new
   
    //Timer 5
    T5CONbits.T32 = 0;
    T5CONbits.TCKPS = 0b010; //1:4
    TMR5 = 0;
    PR5 = MS_COUNT / 4;
    IPC6bits.T5IP = 3;
    IPC6bits.T5IS = 0;
    IFS0bits.T5IF = 0;
    IEC0bits.T5IE = 1;
    T5CONbits.ON = 1;
}

void __ISR(_TIMER_5_VECTOR, IPL3SOFT) Timer5Isr(void) {
    ++loggingTimer;
    if (loggingTimer >= LOGGING_INTERVAL) {
        loggingTimer = 0;
        //Save log data to EEPROM
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