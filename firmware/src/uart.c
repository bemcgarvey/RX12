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

UartPacket uart1Packet;
UartPacket uart6Packet;
UartPacket uart3Packet;

int uart1Pos = 0;
volatile bool uart1PacketGood = true;
volatile unsigned int uart1PacketCount = 0;

int uart6Pos = 0;
volatile bool uart6PacketGood = true;
volatile unsigned int uart6PacketCount = 0;

int uart3Pos = 0;
volatile bool uart3PacketGood = true;
volatile unsigned int uart3PacketCount = 0;

void initUARTs(void) {
    //SAT1 = UART1
    U1BRG = 129; //115200 baud
    U1MODEbits.BRGH = 1;
    U1STAbits.URXEN = 1;
    IPC9bits.U1RXIP = 1;
    IPC9bits.U1RXIS = 3;
    IFS1bits.U1RXIF = 0;
    IEC1bits.U1RXIE = 1;
    IPC9bits.U1EIP = 2;
    IPC9bits.U1EIS = 3;
    IFS1bits.U1EIF = 0;
    IEC1bits.U1EIE = 1;
    U1STAbits.OERR = 0;

    //SAT2 = UART6
    U6BRG = 129; //115200 baud
    U6MODEbits.BRGH = 1;
    U6STAbits.URXEN = 1;
    IPC41bits.U6RXIP = 1;
    IPC41bits.U6RXIS = 2;
    IFS5bits.U6RXIF = 0;
    IEC5bits.U6RXIE = 1;
    IPC41bits.U6EIP = 2;
    IPC41bits.U6EIS = 2;
    IFS5bits.U6EIF = 0;
    IEC5bits.U6EIE = 1;
    U6STAbits.OERR = 0;

    //SAT3 = UART3
    U3BRG = 129; //115200 baud
    U3MODEbits.BRGH = 1;
    U3STAbits.URXEN = 1;
    IPC15bits.U3RXIP = 1;
    IPC15bits.U3RXIS = 3;
    IFS1bits.U3RXIF = 0;
    IEC1bits.U3RXIE = 1;
    IPC15bits.U3EIP = 2;
    IPC15bits.U3EIS = 3;
    IFS1bits.U3EIF = 0;
    IEC1bits.U3EIE = 1;
    U3STAbits.OERR = 0;

    U1MODEbits.ON = 1;
    U6MODEbits.ON = 1;
    U3MODEbits.ON = 1;
}

void __ISR(_UART1_RX_VECTOR, IPL1SOFT) uart1Isr(void) {
    uint8_t rxByte;
    unsigned int elapsedTime;
    elapsedTime = systemTickCount - lastRxTime[SAT1];
    lastRxTime[SAT1] = systemTickCount;
    while (U1STAbits.URXDA == 1) {
        if (elapsedTime > 2) {
            uart1Pos = 0;
            uart1PacketGood = true;
        }
        if (U1STAbits.FERR == 1) {
            uart1PacketGood = false;
        }
        rxByte = U1RXREG;
        if ((uart1Pos & 1) == 0) {
            uart1Packet.buffer[uart1Pos + 1] = rxByte;
        } else {
            uart1Packet.buffer[uart1Pos - 1] = rxByte;
        }
        ++uart1Pos;
        if (uart1Pos == 16) {
            if (uart1PacketGood) {
                ++uart1PacketCount;
                for (int i = 0; i < 4; ++i) {
                    packetQueue[packetQueueHead].data[i] = uart1Packet.packet[i]; 
                }
                packetQueue[packetQueueHead].timeStamp = systemTickCount;
                ++packetQueueHead;
                if (packetQueueHead == PACKET_QUEUE_LENGTH) {
                    packetQueueHead = 0;
                }
                //TODO Need to handle queue overrun
//                if (packetQueueHead == packetQueueTail) {
//                    LED3On();
//                }
            }
            uart1PacketGood = false;
            uart1Pos = 0;
        }
    }
    IFS1bits.U1RXIF = 0;
}

void __ISR(_UART1_FAULT_VECTOR, IPL2SOFT) uart1ErrorIsr(void) {
    if (U1STAbits.OERR == 1) {
        U1STAbits.OERR = 0;
        uart1PacketGood = false;
    }
    IFS1bits.U1EIF = 0;
}

void __ISR(_UART6_RX_VECTOR, IPL1SOFT) uart6Isr(void) {
    uint8_t rxByte;
    unsigned int elapsedTime;
    elapsedTime = systemTickCount - lastRxTime[SAT2];
    lastRxTime[SAT2] = systemTickCount;
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
                ++uart6PacketCount;
                for (int i = 0; i < 4; ++i) {
                    packetQueue[packetQueueHead].data[i] = uart6Packet.packet[i]; 
                }
                packetQueue[packetQueueHead].timeStamp = systemTickCount;
                ++packetQueueHead;
                if (packetQueueHead == PACKET_QUEUE_LENGTH) {
                    packetQueueHead = 0;
                }
//                if (packetQueueHead == packetQueueTail) {
//                    LED3On();
//                }
            }
            uart6PacketGood = false;
            uart6Pos = 0;
        }
    }
    IFS5bits.U6RXIF = 0;
}

void __ISR(_UART6_FAULT_VECTOR, IPL2SOFT) uart6ErrorIsr(void) {
    if (U6STAbits.OERR == 1) {
        U6STAbits.OERR = 0;
        uart6PacketGood = false;
    }
    IFS5bits.U6EIF = 0;
}

void __ISR(_UART3_RX_VECTOR, IPL1SOFT) uart3Isr(void) {
    uint8_t rxByte;
    unsigned int elapsedTime;
    elapsedTime = systemTickCount - lastRxTime[SAT3];
    lastRxTime[SAT3] = systemTickCount;
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
                ++uart3PacketCount;
                for (int i = 0; i < 4; ++i) {
                    packetQueue[packetQueueHead].data[i] = uart3Packet.packet[i]; 
                }
                packetQueue[packetQueueHead].timeStamp = systemTickCount;
                ++packetQueueHead;
                if (packetQueueHead == PACKET_QUEUE_LENGTH) {
                    packetQueueHead = 0;
                }
//                if (packetQueueHead == packetQueueTail) {
//                    LED3On();
//                }
            }
            uart3PacketGood = false;
            uart3Pos = 0;
        }
    }
    IFS1bits.U3RXIF = 0;
}

void __ISR(_UART3_FAULT_VECTOR, IPL2SOFT) uart3ErrorIsr(void) {
    if (U3STAbits.OERR == 1) {
        U3STAbits.OERR = 0;
        uart3PacketGood = false;
    }
    IFS1bits.U3EIF = 0;
}