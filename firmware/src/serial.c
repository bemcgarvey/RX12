/////////////////////////////////////////////////////
// Project: RX12                                   //
// File: serial.c                                  //
// Target: PIC32MKxxxxGPD/E064                     // 
// Compiler: XC32 (Tested with 2.10)               //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: serial communication with client   //
/////////////////////////////////////////////////////
#include <xc.h>
#include "serial.h"
#include <sys/attribs.h>
#include <stdint.h>
#include "logging.h"
#include "eeprom.h"
#include "startup.h"
#include "failsafe.h"
#include "adc.h"
#include "version.h"

static volatile enum {WAIT_COMMAND = 0, RX_DATA = 1} state;
static volatile int command;
static volatile uint8_t *pData;
static volatile int rxCount;
static volatile uint8_t buffer[64];

static void transmitData(int numBytes);
static void transmitSettings(void);
static void transmitLog(unsigned int count);
static void transmitVoltage(void);
static void transmitCalibration(void);
static void transmitLogCount(void);
static void processCommand(void);
static void postProcessCommand(void);

void initSerial(void) {
    U4BRG = 129; //115200 baud
    U4MODEbits.BRGH = 1;
    U4STAbits.URXEN = 1;
    U4STAbits.UTXEN = 1;
    IPC16bits.U4RXIP = 1;
    IPC16bits.U4RXIS = 0;
    IFS2bits.U4RXIF = 0;
    IEC2bits.U4RXIE = 1;
    U4STAbits.OERR = 0;
    state = WAIT_COMMAND;
    U4MODEbits.ON = 1;
    initADCSingleSample();
}
    
void processCommand(void) {
    switch (command) {
        case ENABLE_LOGGING:
            enableLogging(true);
            state = WAIT_COMMAND;
            break;
        case DISABLE_LOGGING:
            enableLogging(false);
            state = WAIT_COMMAND;
            break;
        case SET_FRAME_11:
            writeEEPROM(ADDRESS_FRAME_RATE, FRAME_11MS);
            state = WAIT_COMMAND;
            break;
        case SET_FRAME_22:
            writeEEPROM(ADDRESS_FRAME_RATE, FRAME_22MS);
            state = WAIT_COMMAND;
            break;
        case GET_SETTINGS:
            transmitSettings();
            state = WAIT_COMMAND;
            break;
        case SET_VOLTAGE_CALIBRATION:
            pData = buffer;
            rxCount = 2 * sizeof(uint32_t);
            state = RX_DATA;
            break;
        case GET_LOG:
            pData = buffer;
            rxCount = 1;
            state = RX_DATA;
            break;
        case CLEAR_LOG:
            writeEEPROM(ADDRESS_CURRENT_LOG, ADDRESS_LOG_START - sizeof(LogData));
            writeEEPROM(ADDRESS_LOG_SEQUENCE, 0);
            state = WAIT_COMMAND;
            break;
        case GET_VOLTAGE:
            transmitVoltage();
            state = WAIT_COMMAND;
            break;
        case GET_VOLTAGE_CALIBRATION:
            transmitCalibration();
            state = WAIT_COMMAND;
        case GET_LOG_COUNT:
            transmitLogCount();
            state = WAIT_COMMAND;
        case GET_VERSION:
            *(unsigned int *)buffer = firmwareVersion;
            transmitData(4);
            state = WAIT_COMMAND;
        default:
            state = WAIT_COMMAND;
            break;            
    }
}

void postProcessCommand(void) {
    switch (command) {
        case SET_VOLTAGE_CALIBRATION:
            writeEEPROM(ADDRESS_ADC_CALIBRATION1, *(uint32_t *)buffer);
            writeEEPROM(ADDRESS_ADC_CALIBRATION2, *(uint32_t *)&buffer[4]);
            state = WAIT_COMMAND;
            break;
        case GET_LOG:
            transmitLog(buffer[0]);
            state = WAIT_COMMAND;
        default:
            state = WAIT_COMMAND;
            break;
    }
}


void __ISR(_UART4_RX_VECTOR, IPL1SOFT) uart4Isr(void) {
    uint8_t rxByte;
    while (U4STAbits.URXDA == 1) {
        rxByte = U4RXREG;
        if (state == WAIT_COMMAND) {
            command = rxByte;
            processCommand();
        } else if (state == RX_DATA) {
            *pData = rxByte;
            ++pData;
            --rxCount;
            if (rxCount == 0) {
                postProcessCommand();
            }
        } else {
            state = WAIT_COMMAND;
        }
    }
    IFS2bits.U4RXIF = 0;
}

void transmitData(int numBytes) {
    uint8_t *tx;
    tx = (uint8_t *)buffer;
    while (numBytes > 0) {
        while (U4STAbits.UTXBF);
        U4TXREG = *tx;
        ++tx;
        --numBytes;
    }
}

void transmitSettings(void) {
    readEEPROM(ADDRESS_FRAME_RATE, (uint32_t *)&buffer[0]);
    readEEPROM(ADDRESS_DSM_TYPE, (uint32_t *)&buffer[4]);
    readEEPROM(ADDRESS_FAILSAFE_TYPE, (uint32_t *)&buffer[8]);
    readEEPROM(ADDRESS_LOGGING_ACTIVE, (uint32_t *)&buffer[12]);
    transmitData(16);
    transmitCalibration();
    transmitVoltage();
}

void transmitCalibration(void) {
    readEEPROM(ADDRESS_ADC_CALIBRATION1, (uint32_t *)&buffer[0]);
    readEEPROM(ADDRESS_ADC_CALIBRATION2, (uint32_t *)&buffer[4]);
    transmitData(8);
}
void transmitLog(unsigned int count) {
    unsigned int logAddress = 0;
    readEEPROM(ADDRESS_CURRENT_LOG, &logAddress);
    for (int i = 0; i < count; ++i) {
        loadLogData((LogData *)buffer, logAddress);
        transmitData(sizeof(LogData));
        logAddress -= sizeof(LogData);
        if (logAddress < ADDRESS_LOG_START) {
            logAddress = EEPROM_SIZE - sizeof(LogData) - ((EEPROM_SIZE - ADDRESS_LOG_START) % sizeof(LogData));
        }
    }
}

void transmitVoltage(void) {
    uint32_t sum = 0;
    for (int i = 0; i < 16; ++i) {
        sum += readADC();
    }
    *(uint32_t *)&buffer[0] = sum / 16;
    transmitData(4);
}

#define MAX_COUNT   ((EEPROM_SIZE - ADDRESS_LOG_START) / sizeof(LogData))

void transmitLogCount(void) {
    unsigned int logAddress = 0;
    unsigned int seq = 0;
    unsigned int count = 0;
    readEEPROM(ADDRESS_CURRENT_LOG, &logAddress);
    readEEPROM(ADDRESS_LOG_SEQUENCE, &seq);
    if (seq == 0 || logAddress < ADDRESS_LOG_START) {
        count = 0;
    } else {
        if (seq > MAX_COUNT) {
            count = MAX_COUNT;
        } else {
            count = seq;
        }
    }
    buffer[0] = count;
    transmitData(1);
}