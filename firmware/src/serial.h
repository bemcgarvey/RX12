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
        ENABLE_LOGGING = 0x20, DISABLE_LOGGING = 0x21, GET_SETTINGS = 0x22,
        SET_FRAME_22 = 0x23, SET_FRAME_11 = 0x24, GET_LOG = 0x25, CLEAR_LOG = 0x26, 
        SET_VOLTAGE_CALIBRATION = 0x27
    };

#ifdef	__cplusplus
}
#endif

#endif	/* SERIAL_H */

