/////////////////////////////////////////////////////
// Project: RX12                                   //
// File: main.c                                    //
// Target: PIC32MKxxxxGPD/E064                     // 
// Compiler: XC32 (Tested with 2.10)               //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: program entry point and nmi handler//
/////////////////////////////////////////////////////
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
#include "failsafe.h"
#include "logging.h"
#include "serial.h"

static unsigned int lastSat1;
static unsigned int lastSat2;
static unsigned int lastSat3;

int main(void) {
    int result;

    startupMode = START_NORMAL;
    if (RCONbits.POR == 1) {
        //power on reset
        RCONbits.POR = 0;
        RCONbits.BOR = 0;
    } else if (RCONbits.WDTO == 1) {
        startupMode = START_WDTO;
        RCONbits.WDTO = 0;
    } else if (RCONbits.BOR == 1) {
        startupMode = START_WDTO; //Treat a brown-out like a WDT reset - start quickly
        RCONbits.BOR = 0;
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
    } else {
        readEEPROM(ADDRESS_PRIMARY_SAT, &primarySatellite);
        if (primarySatellite > SAT3) {
            primarySatellite = SAT1;
        }
    }
    outputType = OUTPUT_TYPE_PWM;
    unsigned int savedOutputType = 0;
    result = readEEPROM(ADDRESS_OUTPUT_TYPE, &savedOutputType);
    if (result == EEPROM_SUCCESS) {
        if (savedOutputType == OUTPUT_TYPE_PWM || savedOutputType == OUTPUT_TYPE_PPM
                || savedOutputType == OUTPUT_TYPE_SBUS) {
            outputType = savedOutputType;
        } else {
            writeEEPROM(ADDRESS_OUTPUT_TYPE, outputType);
        }
    }
    setPPS();
    if (startupMode == START_SERIAL) {
        LED3On();
        initSerial();
        __builtin_set_isr_state(0);
        __builtin_enable_interrupts();
        while (true);
    }
    FrameMode savedFrameMode = 0;
    result = readEEPROM(ADDRESS_FRAME_RATE, &savedFrameMode);
    if (result == EEPROM_SUCCESS) {
        if (savedFrameMode == FRAME_22MS || savedFrameMode == FRAME_11MS) {
            frameMode = savedFrameMode;
        } else {
            writeEEPROM(ADDRESS_FRAME_RATE, frameMode);
        }
    }
    FailsafeType savedFailsafeType = HOLD_FAILSAFE;
    result = readEEPROM(ADDRESS_FAILSAFE_TYPE, &savedFailsafeType);
    if (result == EEPROM_SUCCESS) {
        if (savedFailsafeType == HOLD_FAILSAFE || savedFailsafeType == PRESET_FAILSAFE) {
            failsafeType = savedFailsafeType;
        } else {
            writeEEPROM(ADDRESS_FAILSAFE_TYPE, failsafeType);
        }
    }
    if (failsafeType == PRESET_FAILSAFE) {
        if (!loadFailsafePresets()) {
            failsafeType = HOLD_FAILSAFE;
        }
    }
    unsigned int blinks = 0;
    if (startupMode != START_WDTO) {
        if (outputType == OUTPUT_TYPE_PWM && frameMode == FRAME_11MS) {
            blinks = 1;
        } else if (outputType == OUTPUT_TYPE_PPM) {
            blinks = 2;
        } else if (outputType == OUTPUT_TYPE_SBUS) {
            blinks = 3;
            if (frameMode == FRAME_11MS) {
                blinks = 4;
            }
        }
    }
    if (startupMode == START_BIND) {
        blinks = 10;
    }
    for (unsigned int i = 0; i < blinks; ++i) {
        LED3On();
        delay_us(200000);
        LED3Off();
        delay_us(200000);
    }
    if (failsafeType == PRESET_FAILSAFE && startupMode != START_WDTO) {
        for (unsigned int i = 0; i < 2; ++i) {
            LED2On();
            delay_us(200000);
            LED2Off();
            delay_us(200000);
        }
    }
    __builtin_set_isr_state(0);
    __builtin_enable_interrupts();
    startSystemTickTimer();
    initOutputs();
    if (frameMode == FRAME_22MS || outputType == OUTPUT_TYPE_PPM) {
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
        checkLoggingActive();
        if (logging) {
            if (startupMode != START_WDTO) {
                for (unsigned int i = 0; i < 4; ++i) {
                    LED1On();
                    delay_us(200000);
                    LED1Off();
                    delay_us(200000);
                }
            }
            startLogging();
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
            lastSat1 = systemTickCount - lastPacket[SAT1];
            lastSat2 = systemTickCount - lastPacket[SAT2];
            lastSat3 = systemTickCount - lastPacket[SAT3];
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
                if (failsafeType == PRESET_FAILSAFE) {
                    for (int i = 0; i < MAX_CHANNEL; ++i) {
                        outputPulses[i] = presetOutputPulses[i];
                    }
                } else {
                    disableThrottle();
                }
                servos[THROTTLE] = 0xffff; //Mark throttle as inactive
                if (outputsActivated) {
                    currentFlightLog.statusFlags |= STATUS_FAILSAFE;
                }
                failsafeEngaged = true;
            }
            if (failsafeEngaged && servos[THROTTLE] != 0xffff) {
                if (failsafeType == HOLD_FAILSAFE) {
                    enableThrottle();
                }
                failsafeEngaged = false;
            }
        }
        WDTCONbits.WDTCLRKEY = 0x5743;
    }
    return ( EXIT_FAILURE);
}

void _nmi_handler(void) {
    int wdtFlag;
    wdtFlag = WDTCONbits.ON;
    WDTCONbits.ON = 0;
    if (RNMICONbits.CF == 1) {
        SYSKEY = 0x00000000;
        SYSKEY = 0xAA996655;
        SYSKEY = 0x556699AA;
        SPLLCONbits.PLLIDIV = 0b000; //according to datasheet PLLIDIV is ignored when FRC is selected
        SPLLCONbits.PLLICLK = 1;
        OSCCONbits.FRCDIV = 0;
        OSCCONbits.NOSC = 0b001;
        OSCCONbits.OSWEN = 1;
        OSCCONbits.CF = 0;
        RNMICONbits.CF = 0;
        SYSKEY = 0x33333333;
        while (OSCCONbits.OSWEN == 1);
        currentFlightLog.statusFlags |= STATUS_CF;
    }
    //Clear BEV flag
    _CP0_BIC_STATUS(_CP0_STATUS_BEV_MASK);
    WDTCONbits.WDTCLRKEY = 0x5743;
    WDTCONbits.ON = wdtFlag;
    __asm__("ERET");
}

/*******************************************************************************
 End of File
 */

