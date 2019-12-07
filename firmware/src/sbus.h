/* 
 * File:   sbus.h
 * Author: bemcg
 *
 * Created on December 5, 2019, 10:32 PM
 */

#ifndef SBUS_H
#define	SBUS_H

#ifdef	__cplusplus
extern "C" {
#endif

    typedef union {
        struct {
            uint8_t header;
            uint8_t channels[22];
            struct {
                int channel17 : 1;
                int channel18 : 1;
                int frameLost : 1;
                int failsafe : 1;
                int : 4;
            };
            uint8_t footer;
        };
        uint8_t bytes[25];
    } SBusPacket;

    void initSBus(void);
    void startSBus(void);
    void transmitSBusPacket(void);


#ifdef	__cplusplus
}
#endif

#endif	/* SBUS_H */

