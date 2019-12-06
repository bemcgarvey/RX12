//TODO add header comments

#include <xc.h>
#include "sbus.h"
#include "output.h"

void initSBus(void) {
    //Setup UART1
    //100000 baud, 8E2S, inverted polarity
    U1MODEbits.CLKSEL = 0b01; //SYSCLOCK
    U1BRG = 75; //100000 baud
    U1MODEbits.BRGH = 0;
    U1MODEbits.PDSEL = 0b01; //8E
    U1MODEbits.STSEL = 1; //2S
    U1STAbits.UTXINV = 1; //invert
    U1STAbits.UTXEN = 1;
    U1STAbits.UTXISEL = 0b01; //interrupt when last char transmitted
    IPC10bits.U1TXIP = 6;
    IPC10bits.U1TXIS = 0;
}

void startSBus(void) {
    IFS1bits.U1TXIF = 0;
    IEC1bits.U1TXIE = 1;
    U1MODEbits.ON = 1;
    
}

void buildSBusPacket(void) {
}
