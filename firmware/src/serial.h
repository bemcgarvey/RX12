/* 
 * File:   serial.h
 * Author: bemcg
 *
 * Created on August 2, 2019, 1:39 PM
 */

#ifndef SERIAL_H
#define	SERIAL_H

#ifdef	__cplusplus
extern "C" {
#endif

    void initSerial(void);

    enum {
        ENABLE_LOGGING = 0x20, DISABLE_LOGGING, GET_SETTINGS,
        SET_FRAME_22, SET_FRAME_11, GET_LOG, CLEAR_LOG, 
        SET_VOLTAGE_CALIBRATION, GET_VOLTAGE, GET_VOLTAGE_CALIBRATION
    };

#ifdef	__cplusplus
}
#endif

#endif	/* SERIAL_H */

