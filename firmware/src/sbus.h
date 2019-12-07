/////////////////////////////////////////////////////
// Project: RX12                                   //
// File: sbus.h                                    //
// Target: PIC32MKxxxxGPD/E064                     // 
// Compiler: XC32 (Tested with 2.10)               //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: S.Bus output code                  //
/////////////////////////////////////////////////////

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

