/////////////////////////////////////////////////////
// Project: RX12                                   //
// File: uart.c                                    //
// Target: PIC32MKxxxxGPD/E064                     // 
// Compiler: XC32 (Tested with 2.10)               //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: satellite uart functions           //
/////////////////////////////////////////////////////
#include <xc.h>
#include "uart.h"
#include <sys/attribs.h>
#include "led.h"
#include "satellite.h"
#include "timers.h"
#include <stdint.h>
#include <stdbool.h>
#include "datapacket.h"

typedef union {
    uint8_t buffer[16];
    uint32_t packet[4];
} UartPacket;

static volatile UartPacket uart5Packet;
static volatile UartPacket uart6Packet;
static volatile UartPacket uart3Packet;

static volatile int uart5Pos = 0;
static volatile bool uart5PacketGood = true;

static volatile int uart6Pos = 0;
static volatile bool uart6PacketGood = true;

static volatile int uart3Pos = 0;
static volatile bool uart3PacketGood = true;

static volatile unsigned int lastRx[3] = {0, 0, 0};

void initUARTs(void) {
    //SAT1 = UART5
    U5BRG = 129; //115200 baud
    U5MODEbits.BRGH = 1;
    U5STAbits.URXEN = 1;
    IPC17bits.U5RXIP = 3;
    IPC17bits.U5RXIS = 3;
    IFS2bits.U5RXIF = 0;
    IEC2bits.U5RXIE = 1;
    IPC17bits.U5EIP = 4;
    IPC17bits.U5EIS = 3;
    IFS2bits.U5EIF = 0;
    IEC2bits.U5EIE = 1;
    U5STAbits.OERR = 0;

    //SAT2 = UART6
    U6BRG = 129; //115200 baud
    U6MODEbits.BRGH = 1;
    U6STAbits.URXEN = 1;
    IPC41bits.U6RXIP = 3;
    IPC41bits.U6RXIS = 2;
    IFS5bits.U6RXIF = 0;
    IEC5bits.U6RXIE = 1;
    IPC41bits.U6EIP = 4;
    IPC41bits.U6EIS = 2;
    IFS5bits.U6EIF = 0;
    IEC5bits.U6EIE = 1;
    U6STAbits.OERR = 0;

    //SAT3 = UART3
    U3BRG = 129; //115200 baud
    U3MODEbits.BRGH = 1;
    U3STAbits.URXEN = 1;
    IPC15bits.U3RXIP = 3;
    IPC15bits.U3RXIS = 1;
    IFS1bits.U3RXIF = 0;
    IEC1bits.U3RXIE = 1;
    IPC15bits.U3EIP = 4;
    IPC15bits.U3EIS = 1;
    IFS1bits.U3EIF = 0;
    IEC1bits.U3EIE = 1;
    U3STAbits.OERR = 0;

    PRISSbits.PRI3SS = 1;
    U5MODEbits.ON = 1;
    U6MODEbits.ON = 1;
    U3MODEbits.ON = 1;
}

void __ISR(_UART5_RX_VECTOR, IPL3SRS) uart5Isr(void) {
    uint8_t rxByte;
    unsigned int elapsedTime;
    elapsedTime = systemTickCount - lastRx[SAT1];
    lastRx[SAT1] = systemTickCount;
    while (U5STAbits.URXDA == 1) {
        if (elapsedTime > 2) {
            uart5Pos = 0;
            uart5PacketGood = true;
        }
        if (U5STAbits.FERR == 1) {
            uart5PacketGood = false;
        }
        rxByte = U5RXREG;
        if ((uart5Pos & 1) == 0) {
            uart5Packet.buffer[uart5Pos + 1] = rxByte;
        } else {
            uart5Packet.buffer[uart5Pos - 1] = rxByte;
        }
        ++uart5Pos;
        if (uart5Pos == 16) {
            if (uart5PacketGood) {
                for (int i = 0; i < 4; ++i) {
                    packetQueue[packetQueueHead].data[i] = uart5Packet.packet[i]; 
                }
                packetQueue[packetQueueHead].timeStamp = systemTickCount;
                packetQueue[packetQueueHead].satellite = SAT1;
                ++packetQueueHead;
                if (packetQueueHead == PACKET_QUEUE_LENGTH) {
                    packetQueueHead = 0;
                }
            }
            uart5PacketGood = false;
            uart5Pos = 0;
        }
    }
    IFS2bits.U5RXIF = 0;
}

void __ISR(_UART5_FAULT_VECTOR, IPL4SOFT) uart5ErrorIsr(void) {
    if (U5STAbits.OERR == 1) {
        U5STAbits.OERR = 0;
        uart5PacketGood = false;
    }
    IFS2bits.U5EIF = 0;
}

void __ISR(_UART6_RX_VECTOR, IPL3SRS) uart6Isr(void) {
    uint8_t rxByte;
    unsigned int elapsedTime;
    elapsedTime = systemTickCount - lastRx[SAT2];
    lastRx[SAT2] = systemTickCount;
    while (U6STAbits.URXDA == 1) {
        if (elapsedTime > 2) {
            uart6Pos = 0;
            uart6PacketGood = true;
        }
        if (U6STAbits.FERR == 1) {
            uart6PacketGood = false;
        }
        rxByte = U6RXREG;
        if ((uart6Pos & 1) == 0) {
            uart6Packet.buffer[uart6Pos + 1] = rxByte;
        } else {
            uart6Packet.buffer[uart6Pos - 1] = rxByte;
        }
        ++uart6Pos;
        if (uart6Pos == 16) {
            if (uart6PacketGood) {
                for (int i = 0; i < 4; ++i) {
                    packetQueue[packetQueueHead].data[i] = uart6Packet.packet[i]; 
                }
                packetQueue[packetQueueHead].timeStamp = systemTickCount;
                packetQueue[packetQueueHead].satellite = SAT2;
                ++packetQueueHead;
                if (packetQueueHead == PACKET_QUEUE_LENGTH) {
                    packetQueueHead = 0;
                }
            }
            uart6PacketGood = false;
            uart6Pos = 0;
        }
    }
    IFS5bits.U6RXIF = 0;
}

void __ISR(_UART6_FAULT_VECTOR, IPL4SOFT) uart6ErrorIsr(void) {
    if (U6STAbits.OERR == 1) {
        U6STAbits.OERR = 0;
        uart6PacketGood = false;
    }
    IFS5bits.U6EIF = 0;
}

void __ISR(_UART3_RX_VECTOR, IPL3SRS) uart3Isr(void) {
    uint8_t rxByte;
    unsigned int elapsedTime;
    elapsedTime = systemTickCount - lastRx[SAT3];
    lastRx[SAT3] = systemTickCount;
    while (U3STAbits.URXDA == 1) {
        if (elapsedTime > 2) {
            uart3Pos = 0;
            uart3PacketGood = true;
        }
        if (U3STAbits.FERR == 1) {
            uart3PacketGood = false;
        }
        rxByte = U3RXREG;
        if ((uart3Pos & 1) == 0) {
            uart3Packet.buffer[uart3Pos + 1] = rxByte;
        } else {
            uart3Packet.buffer[uart3Pos - 1] = rxByte;
        }
        ++uart3Pos;
        if (uart3Pos == 16) {
            if (uart3PacketGood) {
                for (int i = 0; i < 4; ++i) {
                    packetQueue[packetQueueHead].data[i] = uart3Packet.packet[i]; 
                }
                packetQueue[packetQueueHead].timeStamp = systemTickCount;
                packetQueue[packetQueueHead].satellite = SAT3;
                ++packetQueueHead;
                if (packetQueueHead == PACKET_QUEUE_LENGTH) {
                    packetQueueHead = 0;
                }
            }
            uart3PacketGood = false;
            uart3Pos = 0;
        }
    }
    IFS1bits.U3RXIF = 0;
}

void __ISR(_UART3_FAULT_VECTOR, IPL4SOFT) uart3ErrorIsr(void) {
    if (U3STAbits.OERR == 1) {
        U3STAbits.OERR = 0;
        uart3PacketGood = false;
    }
    IFS1bits.U3EIF = 0;
}