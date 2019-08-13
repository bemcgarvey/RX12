/////////////////////////////////////////////////////
// Project: RX12                                   //
// File: failsafe.c                                //
// Target: PIC32MKxxxxGPD/E064                     // 
// Compiler: XC32 (Tested with 2.10)               //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: failsafe implementation            //
/////////////////////////////////////////////////////
#include <xc.h>
#include "failsafe.h"
#include "eeprom.h"

FailsafeType failsafeType = HOLD_FAILSAFE;
bool failsafeEngaged = true;
uint32_t presetOutputPulses[MAX_CHANNEL];

bool loadFailsafePresets(void) {
    bool success = true;
    for (int i = 0; i < MAX_CHANNEL; ++i) {
        if (readEEPROM(ADDRESS_FAILSAFE_VALUES + (4 * i), &presetOutputPulses[i]) != EEPROM_SUCCESS) {
            success = false;
        }
    }
    return success;
}

void saveFailsafePresets(void) {
    for (int i = 0; i < MAX_CHANNEL; ++i) {
        writeEEPROM(ADDRESS_FAILSAFE_VALUES + (4 * i), presetOutputPulses[i]);
        while (!readyEEPROM());
    }
}