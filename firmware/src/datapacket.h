/* 
 * File:   datapacket.h
 * Author: bemcg
 *
 * Created on July 20, 2019, 1:04 PM
 */

#ifndef DATAPACKET_H
#define	DATAPACKET_H

#include "stdint.h"

#ifdef	__cplusplus
extern "C" {
#endif
    
#define PACKET_QUEUE_LENGTH 12
    
    typedef struct {
        union {
            struct {
                uint8_t system;
                uint8_t fades;
                uint16_t servo[7];
            };
            uint32_t data[4];
        };
        unsigned int timeStamp;
    } DataPacket;

    extern volatile DataPacket packetQueue[PACKET_QUEUE_LENGTH];
    extern volatile int packetQueueHead;
    extern volatile int packetQueueTail;
    extern unsigned int packetsReceived;

    void processCurrentPacket(void);

#ifdef	__cplusplus
}
#endif

#endif	/* DATAPACKET_H */

