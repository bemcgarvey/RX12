/////////////////////////////////////////////////////
// Project: RX12                                   //
// File: datapacket.c                              //
// Target: PIC32MKxxxxGPD/E064                     // 
// Compiler: XC32 (Tested with 2.10)               //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: rx data packet processing          //
/////////////////////////////////////////////////////
#include <xc.h>
#include "datapacket.h"
#include "led.h"
#include "output.h"
#include "timers.h"
#include "satellite.h"
#include "startup.h"
#include "logging.h"

volatile DataPacket packetQueue[PACKET_QUEUE_LENGTH];
volatile int packetQueueHead = 0;
volatile int packetQueueTail = 0;
unsigned int packetsReceived = 0;

void processCurrentPacket(void) {
    if (logging) {
        unsigned int sat = packetQueue[packetQueueTail].satellite;
        if (sat == primarySatellite) {
            currentFlightLog.fades[sat] = packetQueue[packetQueueTail].fades;
        } else {
            currentFlightLog.fades[sat] = packetQueue[packetQueueTail].fades16;
        }
    }
    for (int i = 0; i < 7; ++i) {
        int channel;
        if (systemType == SYSTEM_TYPE_DSM2_1024) {
            channel = packetQueue[packetQueueTail].servo[i] >> 10;
            if (channel < MAX_CHANNEL) {
                servos[channel] = packetQueue[packetQueueTail].servo[i] & 0x000003ff;
                outputPulses[channel] = pulseOffsets[channel] + ((1194 * US_COUNT) * servos[channel]) / 1024;
            }
        } else {
            channel = packetQueue[packetQueueTail].servo[i] >> 11;
            channel &= 0x0000000f;
            if (channel < MAX_CHANNEL) {
                servos[channel] = packetQueue[packetQueueTail].servo[i] & 0x000007ff;
                outputPulses[channel] = pulseOffsets[channel] + ((1194 * US_COUNT) * servos[channel]) / 2048;
            }
        }
    }
    ++packetQueueTail;
    if (packetQueueTail == PACKET_QUEUE_LENGTH) {
        packetQueueTail = 0;
    }
    ++packetsReceived;
}

DSMSystemType checkPacketType(unsigned int primary) {
    DSMSystemType type = SYSTEM_TYPE_NONE;
    if (packetQueue[packetQueueTail].satellite == primary) {
        if (packetQueue[packetQueueTail].system == SYSTEM_TYPE_DSM2_1024 ||
                packetQueue[packetQueueTail].system == SYSTEM_TYPE_DSM2_2048 ||
                packetQueue[packetQueueTail].system == SYSTEM_TYPE_DSMX_11 ||
                packetQueue[packetQueueTail].system == SYSTEM_TYPE_DSMX_22) {
            type = packetQueue[packetQueueTail].system;
        }
        ++packetQueueTail;
        if (packetQueueTail == PACKET_QUEUE_LENGTH) {
            packetQueueTail = 0;
        }
        ++packetsReceived;
    }
    return type;
}