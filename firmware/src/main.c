#include <xc.h>
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include "startup.h"
#include "satellite.h"
#include "led.h"
#include "timers.h"
#include "uart.h"
#include "datapacket.h"
#include "output.h"
#include "pins.h"
#include "failsafe.h"
#include "eeprom.h"

unsigned int lastSat1;
unsigned int lastSat2;
unsigned int lastSat3;

int main(void) {
    startupMode = START_NORMAL;
    if (RCONbits.WDTO == 1) {
        startupMode = START_WDTO;
    }
    SYS_Initialize(NULL);
    initPins();
    initEEPROM();
    if (startupMode != START_WDTO) {
        DetectStartupMode();
    }
    if (startupMode == START_BIND) {
        DetectConnectedSatellites();
        SendBindPulses(bindType);
        writeEEPROM(ADDRESS_FRAME_RATE, frameMode);
    }
    setPPS();
    if (startupMode == START_SERIAL) {
        LED3On();
        while (true);
        //TODO Implement serial main loop
    }
    FrameMode savedFrameMode = 0;
    int result = readEEPROM(ADDRESS_FRAME_RATE, &savedFrameMode);
    if (result == EEPROM_SUCCESS) {
        if (savedFrameMode == FRAME_22MS || savedFrameMode == FRAME_11MS) {
            frameMode = savedFrameMode;
        } else {
            writeEEPROM(ADDRESS_FRAME_RATE, frameMode);
        }
    }
    unsigned int blinks = 0;
    if (frameMode == FRAME_11MS && startupMode != START_WDTO) {
        blinks = 2;
    }
    if (startupMode == START_BIND) {
        blinks = 10;
    }

    //TODO For test purposes.  Remove when tested
    if (startupMode == START_WDTO) {
        blinks = 30;
    }

    for (unsigned int i = 0; i < blinks; ++i) {
        LED3On();
        delay_us(100000);
        LED3Off();
        delay_us(100000);
    }
    __builtin_set_isr_state(0);
    __builtin_enable_interrupts();
    startSystemTickTimer();
    initOutputs();
    if (frameMode == FRAME_22MS) {
        startOCTimer(PERIOD_22MS);
    } else {
        startOCTimer(PERIOD_11MS);
    }
    initUARTs();
    if (startupMode == START_BIND) {
        while (startupMode == START_BIND) {
            if (packetQueueHead != packetQueueTail) {
                systemType = checkPacketType(primarySatellite);
            }
            if (systemType != SYSTEM_TYPE_NONE) {
                startupMode = START_NORMAL;
            }
        }
        writeEEPROM(ADDRESS_DSM_TYPE, systemType);
    } else {
        systemType = SYSTEM_TYPE_DSMX_11;
        DSMSystemType savedSystemType;
        if (readEEPROM(ADDRESS_DSM_TYPE, &savedSystemType) == EEPROM_SUCCESS) {
            if (savedSystemType == SYSTEM_TYPE_DSM2_1024 ||
                    savedSystemType == SYSTEM_TYPE_DSM2_2048 ||
                    savedSystemType == SYSTEM_TYPE_DSMX_11 ||
                    savedSystemType == SYSTEM_TYPE_DSMX_22) {
                systemType = savedSystemType;
            }
        }
    }
    WDTCONbits.WDTCLRKEY = 0x5743;
    RCONbits.WDTO = 0;
    WDTCONbits.ON = 1; //Turn on WDT
    while (true) {
        if (packetQueueHead != packetQueueTail) {
            processCurrentPacket();
        }
        if (!outputsActivated && packetsReceived > 10) {
            enableActiveOutputs();
            outputsActivated = true;
        }
        if (systemTickCount > 100) {
            lastSat1 = systemTickCount - lastRxTime[SAT1];
            lastSat2 = systemTickCount - lastRxTime[SAT2];
            lastSat3 = systemTickCount - lastRxTime[SAT3];
            if (lastSat1 < 100) {
                LED1On();
            } else {
                LED1Off();
            }
            if (lastSat2 < 100) {
                LED2On();
            } else {
                LED2Off();
            }
            if (lastSat3 < 100) {
                LED3On();
            } else {
                LED3Off();
            }
            if (!failsafeEngaged && lastSat1 > 100 && lastSat2 > 100 && lastSat3 > 100) {
                disableThrottle();
                failsafeEngaged = true;
            }
            if (failsafeEngaged && servos[THROTTLE] != 0xffff) {
                enableThrottle();
                failsafeEngaged = false;
            }
        }
        WDTCONbits.WDTCLRKEY = 0x5743;
    }
    return ( EXIT_FAILURE);
}

/*******************************************************************************
 End of File
 */

