/////////////////////////////////////////////////////
// Project: RX12                                   //
// File: datapacket.h                              //
// Target: PIC32MKxxxxGPD/E064                     // 
// Compiler: XC32 (Tested with 2.10)               //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: rx data packet processing          //
/////////////////////////////////////////////////////

#ifndef DATAPACKET_H
#define	DATAPACKET_H

#include "stdint.h"
#include "satellite.h"

#ifdef	__cplusplus
extern "C" {
#endif

#define PACKET_QUEUE_LENGTH 12

    typedef struct {
        union {
            struct {
                union {
                    struct {
                        uint8_t system;
                        uint8_t fades;
                    };
                    uint16_t fades16;
                };
                uint16_t servo[7];
            };
            uint32_t data[4];
        };
        unsigned int timeStamp;
        unsigned int satellite;
    } DataPacket;

    extern volatile DataPacket packetQueue[PACKET_QUEUE_LENGTH];
    extern volatile int packetQueueHead;
    extern volatile int packetQueueTail;
    extern unsigned int packetsReceived;

    void processCurrentPacket(void);
    DSMSystemType checkPacketType(unsigned int primary);

#ifdef	__cplusplus
}
#endif

#endif	/* DATAPACKET_H */

