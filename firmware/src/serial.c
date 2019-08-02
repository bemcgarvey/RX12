#include <xc.h>
#include "serial.h"
#include <sys/attribs.h>
#include <stdint.h>
#include "logging.h"
#include "eeprom.h"
#include "startup.h"
#include "failsafe.h"

static volatile enum {WAIT_COMMAND = 0, RX_DATA = 1} state;
static volatile int command;
static volatile uint8_t *pData;
static volatile int rxCount;
static volatile uint8_t buffer[64]; //TODO adjust this to the best size

static void transmitSettings(void);
static void transmitLog(void);
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
}
    
void processCommand(void) {
    switch (command) {
        case ENABLE_LOGGING:
            enableLogging(true);
            break;
        case DISABLE_LOGGING:
            enableLogging(false);
            break;
        case SET_FRAME_11:
            writeEEPROM(ADDRESS_FRAME_RATE, FRAME_11MS);
            break;
        case SET_FRAME_22:
            writeEEPROM(ADDRESS_FRAME_RATE, FRAME_22MS);
            break;
        case GET_SETTINGS:
            transmitSettings();
            break;
        case SET_VOLTAGE_CALIBRATION:
            pData = buffer;
            rxCount = sizeof(uint32_t);
            state = RX_DATA;
            break;
        case GET_LOG:
            transmitLog();
            break;
        case CLEAR_LOG:
            //TODO mark log as clear?
            break;
        default:
            break;            
    }
}

void postProcessCommand(void) {
    switch (command) {
        case SET_VOLTAGE_CALIBRATION:
            adcCalibration = *(uint32_t *)buffer;
            writeEEPROM(ADDRESS_ADC_CALIBRATION, adcCalibration);
            state = WAIT_COMMAND;
            break;
        default:
            state = WAIT_COMMAND;
            break;
    }
}


void __ISR(_UART4_RX_VECTOR, IPL1SOFT) uart4Isr(void) {
    uint8_t rxByte;
    while (U4STAbits.URXDA == 1) {
        rxByte = U1RXREG;
        if (state == WAIT_COMMAND) {
            command = rxByte;
            processCommand();
        } else if (state == RX_DATA) {
            *pData = rxByte;
            --rxCount;
            if (rxCount == 0) {
                postProcessCommand();
                state = WAIT_COMMAND;
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
    readEEPROM(ADDRESS_ADC_CALIBRATION, (uint32_t *)&buffer[16]);
    transmitData(20);
}

void transmitLog(void) {
    
}