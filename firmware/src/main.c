
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
    SYS_Initialize(NULL);
    initPins();
    initEEPROM();
    DetectStartupMode();
    DetectConnectedSatellites();
    if (startupMode == START_BIND) {
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
    if (frameMode == FRAME_11MS) {
        blinks = 2;
    }
    if (startupMode == START_BIND) {
        blinks = 10;
    }
    //TODO set startup blinks based on frame rate and DSMX/DSM2?
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
    while (true) {
        if (packetQueueHead != packetQueueTail) {
            processCurrentPacket();
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
        if (!outputsActivated && packetsReceived > 100) {
            enableActiveOutputs();
            outputsActivated = true;
        }
    }
    return ( EXIT_FAILURE);
}

/*******************************************************************************
 End of File
 */

