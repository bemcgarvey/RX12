#include <xc.h>
#include "datapacket.h"
#include "led.h"
#include "servos.h"

volatile DataPacket packetQueue[PACKET_QUEUE_LENGTH];
volatile int packetQueueHead = 0;
volatile int packetQueueTail = 0;

void processCurrentPacket(void) {
    for (int i = 0; i < 7; ++i) {
        int channel = packetQueue[packetQueueTail].servo[i] >> 11;
        channel &= 0x0000000f;
        if (channel < MAX_CHANNEL) {
            servos[channel] = packetQueue[packetQueueTail].servo[i] & 0x000007ff;
        }
    }
    ++packetQueueTail;
    if (packetQueueTail == PACKET_QUEUE_LENGTH) {
        packetQueueTail = 0;
    }
}
