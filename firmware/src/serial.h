/////////////////////////////////////////////////////
// Project: RX12                                   //
// File: serial.h                                  //
// Target: PIC32MKxxxxGPD/E064                     // 
// Compiler: XC32 (Tested with 2.10)               //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: serial communication with client   //
/////////////////////////////////////////////////////

#ifndef SERIAL_H
#define	SERIAL_H

#ifdef	__cplusplus
extern "C" {
#endif

    void initSerial(void);

    enum {
        ENABLE_LOGGING = 0x20, DISABLE_LOGGING, GET_SETTINGS,
        SET_FRAME_22, SET_FRAME_11, GET_LOG, CLEAR_LOG, 
        SET_VOLTAGE_CALIBRATION, GET_VOLTAGE, GET_VOLTAGE_CALIBRATION,
        GET_LOG_COUNT, GET_VERSION
    };

#ifdef	__cplusplus
}
#endif

#endif	/* SERIAL_H */

