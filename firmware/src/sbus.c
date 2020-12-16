/////////////////////////////////////////////////////
// Project: RX12                                   //
// File: sbus.c                                    //
// Target: PIC32MKxxxxGPD/E064                     // 
// Compiler: XC32 (Tested with 2.10)               //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: S.Bus output code                  //
/////////////////////////////////////////////////////

#include <xc.h>
#include "sbus.h"
#include "output.h"
#include <sys/attribs.h>
#include <stdint.h>
#include "failsafe.h"

#define SBUS_HEADER 0x0f;
#define SBUS_FOOTER 0x00;

volatile SBusPacket sbusPacket;

static volatile int bytesRemaining = 0;
static volatile int currentPos = 0;
static unsigned int activeChannels = 0;

void initSBus(void) {
    //Setup UART1
    //100000 baud, 8E2, inverted polarity
    U1MODEbits.CLKSEL = 0b01; //SYSCLOCK
    U1BRG = 75; //100000 baud
    U1MODEbits.BRGH = 0;
    U1MODEbits.PDSEL = 0b01; //8E
    U1MODEbits.STSEL = 1; //2 stop bits
    U1STAbits.UTXINV = 1; //invert
    U1STAbits.UTXISEL = 0b10; //interrupt when tx buffer empty
    IPC10bits.U1TXIP = 6;
    IPC10bits.U1TXIS = 0;
    //initialize packet;
    for (int i = 0; i < sizeof(SBusPacket); ++i) {
        sbusPacket.bytes[i] = 0;
    }
    sbusPacket.header = SBUS_HEADER;
    sbusPacket.footer = SBUS_FOOTER;
}

void startSBus(void) {
    activeChannels = 0;
    for (int i = 0; i < MAX_CHANNEL; ++i) {
        if (servos[i] != 0xffff) {
            activeChannels = i + 1;
        } else {
            servos[i] = 1024;  //set inactive channels to midpoint value;
        }
    }
    IFS1bits.U1TXIF = 0;
    IEC1bits.U1TXIE = 1;
    U1MODEbits.ON = 1;
    IEC0bits.T2IE = 1; //enable frame timer interrupt
}

void transmitSBusPacket(void) {
    //build packet - Use Futaba AETR order
    uint16_t throttle;
    if (servos[0] == 0xffff) {
        throttle = 0;
    } else {
        throttle = servos[0];
    }
    sbusPacket.channels[0] = servos[1];
    sbusPacket.channels[1] = (servos[1] >> 8) | (servos[2] << 3);
    sbusPacket.channels[2] = (servos[2] >> 5) | (throttle << 6);
    sbusPacket.channels[3] = (throttle >> 2);
    sbusPacket.channels[4] = (throttle >> 10) | (servos[3] << 1);
    sbusPacket.channels[5] = (servos[3] >> 7) | (servos[4] << 4);
    sbusPacket.channels[6] = (servos[4] >> 4) | (servos[5] << 7);
    sbusPacket.channels[7] = (servos[5] >> 1);
    sbusPacket.channels[8] = (servos[5] >> 9) | (servos[6] << 2);
    sbusPacket.channels[9] = (servos[6] >> 6) | (servos[7] << 5);
    sbusPacket.channels[10] = (servos[7] >> 3);
    sbusPacket.channels[11] = servos[8];
    sbusPacket.channels[12] = (servos[8] >> 8) | (servos[9] << 3);
    sbusPacket.channels[13] = (servos[9] >> 5) | (servos[10] << 6);
    sbusPacket.channels[14] = (servos[10] >> 2);
    sbusPacket.channels[15] = (servos[10] >> 10) | (servos[11] << 1);
    sbusPacket.channels[16] = (servos[11] >> 7);
    if (failsafeEngaged) {
        sbusPacket.failsafe = 1;
    } else {
        sbusPacket.failsafe = 0;
    }
    //load first 8 bytes
    for (int i = 0; i < 8; ++i) {
        U1TXREG = sbusPacket.bytes[i];
    }
    U1STAbits.UTXEN = 1;
    IFS1bits.U1TXIF = 0;
    IEC1bits.U1TXIE = 1;
    currentPos = 8;
    bytesRemaining = 17;
}

void __ISR(_UART1_TX_VECTOR, IPL6SOFT) Uart1TxIsr(void) {
    if (bytesRemaining > 8) {
        for (int i = 0; i < 8; ++i) {
            U1TXREG = sbusPacket.bytes[currentPos];
            ++currentPos;
        }
        bytesRemaining -= 8;
    } else {
        IEC1bits.U1TXIE = 0;
        U1TXREG = sbusPacket.bytes[24];
    }
    IFS1bits.U1TXIF = 0;
}
